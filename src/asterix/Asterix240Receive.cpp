/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Radar Plugin – ASTERIX CAT-240
 *
 ***************************************************************************
 */

#include "Asterix240Receive.h"
#include "RadarReceive.h"
#include "RadarFactory.h"
#include "socketutil.h"

#include <cmath>
#include <cstring>

PLUGIN_BEGIN_NAMESPACE

#define MILLIS_PER_SELECT 250
#define MAX_ASTERIX240_SPOKES 65536

// ---------------------------------------------------------------------------
// Entry – receive thread
// ---------------------------------------------------------------------------
void* Asterix240Receive::Entry() {
    LOG_VERBOSE(wxT("Asterix240Receive thread starting"));

    NetworkAddress fake(127, 0, 0, 10, 3333);

    if (m_ri) {
        m_ri->DetectedRadar(fake, fake);  // enables transmit data
    }

    SOCKET udp_socket = GetNewReportSocket();
    if (udp_socket == INVALID_SOCKET) {
        SetInfoStatus(_("Unable to open CAT-240 socket"));
        return nullptr;
    }

    uint8_t buffer[9000];

    while (!TestDestroy()) {

        fd_set fdin;
        FD_ZERO(&fdin);

        if (udp_socket != INVALID_SOCKET)
            FD_SET(udp_socket, &fdin);

        if (m_receive_socket != INVALID_SOCKET)
            FD_SET(m_receive_socket, &fdin);

        SOCKET maxfd = udp_socket;
        if (m_receive_socket != INVALID_SOCKET && m_receive_socket > maxfd)
            maxfd = m_receive_socket;
        maxfd += 1;

        struct timeval tv = {0, MILLIS_PER_SELECT * 1000};

        int r = select(static_cast<int>(maxfd),
                       &fdin, nullptr, nullptr, &tv);

        if (r > 0) {

            // Shutdown / wakeup
            if (m_receive_socket != INVALID_SOCKET &&
                FD_ISSET(m_receive_socket, &fdin)) {
                break;
            }

            // CAT-240 UDP data
            if (udp_socket != INVALID_SOCKET &&
                FD_ISSET(udp_socket, &fdin)) {

                sockaddr_storage src_addr{};
                socklen_t addrlen = sizeof(src_addr);

                int n = recvfrom(
                    udp_socket,
                    reinterpret_cast<char*>(buffer),
                    sizeof(buffer),
                    0,
                    reinterpret_cast<sockaddr*>(&src_addr),
                    &addrlen
                );

                if (n > 0 && m_ri) {
                    // Only update stats and process spokes if real data received
                    ProcessReport(buffer, static_cast<size_t>(n));
                }
            }
        }
    }

    closesocket(udp_socket);
    LOG_VERBOSE(wxT("Asterix240Receive thread stopping"));
    return nullptr;
}

// ---------------------------------------------------------------------------
// Create UDP socket for CAT-240 multicast receive
// ---------------------------------------------------------------------------
SOCKET Asterix240Receive::GetNewReportSocket() {
    wxString error;

    if (m_radar_interface_addr.addr.s_addr == 0)
        return INVALID_SOCKET;

    SOCKET sock =
        startUDPMulticastReceiveSocket(
            m_radar_interface_addr,   // local NIC
            m_report_addr,            // multicast group + port
            error
        );

    if (sock != INVALID_SOCKET && m_ri) {
        LOG_RECEIVE(wxT("%s listening for CAT-240 on %s via %s"),
                    m_ri->m_name.c_str(),
                    m_report_addr.FormatNetworkAddressPort().c_str(),
                    m_radar_interface_addr.FormatNetworkAddress().c_str());
        SetInfoStatus(_("Listening for CAT-240 data"));
    } else {
        SetInfoStatus(error);
        wxLogError(wxT("CAT-240 socket error: %s"), error.c_str());
    }

    return sock;
}

// ---------------------------------------------------------------------------
// Stop thread and cleanup
// ---------------------------------------------------------------------------
void Asterix240Receive::Shutdown() {
    // Wake select() thread
    if (m_send_socket != INVALID_SOCKET) {
        char c = 0;
        send(m_send_socket, &c, 1, 0);
    }
}

// ---------------------------------------------------------------------------
// Status string
// ---------------------------------------------------------------------------
wxString Asterix240Receive::GetInfoStatus() {
    return _("ASTERIX CAT-240 receiver active");
}

// ---------------------------------------------------------------------------
// Process incoming CAT-240 packet
// ---------------------------------------------------------------------------
bool Asterix240Receive::ProcessReport(uint8_t* data, size_t len) {
    if (!data || len < 32 || !m_ri)
        return false;

    wxCriticalSectionLocker lock(m_ri->m_exclusive);

    size_t offset = 0;

    // --- Fixed header (matches your encoder exactly) ---
    offset += 1;  // Category
    offset += 2;  // Message length
    offset += 2;  // FSPEC
    offset += 1;  // SAC
    offset += 1;  // SIC
    offset += 1;  // Message type
    offset += 4;  // Message sequence ID

    // --- Azimuths ---
    if (offset + 4 > len)
        return false;

    uint16_t startAz =
        (uint16_t(data[offset]) << 8) | data[offset + 1];
    offset += 2;

    uint16_t endAz =
        (uint16_t(data[offset]) << 8) | data[offset + 1];
    offset += 2;

    // --- Start range (meters *not* used because encoder sets 0) ---
    if (offset + 4 > len)
        return false;

    uint32_t startRange =
        (uint32_t(data[offset]) << 24) |
        (uint32_t(data[offset + 1]) << 16) |
        (uint32_t(data[offset + 2]) << 8) |
        (uint32_t(data[offset + 3]));
    offset += 4;

    // --- Cell duration (femtoseconds) ---
    if (offset + 4 > len)
        return false;

    uint32_t cellDuration_fs =
        (uint32_t(data[offset]) << 24) |
        (uint32_t(data[offset + 1]) << 16) |
        (uint32_t(data[offset + 2]) << 8) |
        (uint32_t(data[offset + 3]));
    offset += 4;

    // --- Video format fields ---
    offset += 1;  // Compression indicator
    offset += 1;  // Video resolution

    if (offset + 2 > len)
        return false;

    uint16_t videoOctets =
        (uint16_t(data[offset]) << 8) | data[offset + 1];
    offset += 2;

    offset += 3;  // Video range cell count
    offset += 1;  // Video block count

    // --- Video data ---
    if (offset + videoOctets > len)
        return false;

    uint8_t* video = data + offset;
    size_t video_len = videoOctets;

    // --- Clamp video length manually ---
    size_t max_spoke_len = m_ri->m_spoke_len_max;
    size_t video_len_clamped = video_len;
    if (video_len_clamped > max_spoke_len)
        video_len_clamped = max_spoke_len;
    if (video_len_clamped < 0)
        video_len_clamped = 0;

    // --- Convert cell duration → meters per bin ---
    // distance = c * time / 2
    const double C = 299792458.0;
    double meters_per_bin =
        (cellDuration_fs * 1e-15) * C * 0.5;

    // --- True range in meters ---
    int range_meters =
        int((startRange + video_len_clamped) * meters_per_bin);

    // --- Azimuth scaling ---
    uint16_t spoke_start =
        (startAz * m_ri->m_spokes) / MAX_ASTERIX240_SPOKES;
    uint16_t spoke_end =
        (endAz * m_ri->m_spokes) / MAX_ASTERIX240_SPOKES;

    if (spoke_end < spoke_start)
        spoke_end += m_ri->m_spokes;

    double heading = m_pi->GetHeadingTrue();
    if (std::isnan(heading))
        heading = 0.0;

    int heading_offset =
        int((heading / 360.0) * m_ri->m_spokes);

    SpokeBearing a = MOD_SPOKES(spoke_start + heading_offset);
    SpokeBearing b = MOD_SPOKES(spoke_end + heading_offset);

    wxLongLong time_rec = wxGetUTCTimeMillis();
    time_t now = time(nullptr);

    // --- Update state ---
    m_ri->m_statistics.spokes++;
    m_ri->m_state.Update(RADAR_TRANSMIT);
    m_ri->m_radar_timeout = now + WATCHDOG_TIMEOUT;
    m_ri->m_data_timeout  = now + DATA_TIMEOUT;

    // --- Feed OpenCPN ---
    m_ri->ProcessRadarSpoke(
        a,
        b,
        video,
        video_len_clamped,
        range_meters,
        time_rec
    );

    return true;
}

PLUGIN_END_NAMESPACE
