/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Radar Plugin – ASTERIX CAT-240
 *
 ***************************************************************************
 */

#ifndef _ASTERIX240RECEIVE_H_
#define _ASTERIX240RECEIVE_H_

#include "RadarReceive.h"
#include "socketutil.h"

PLUGIN_BEGIN_NAMESPACE

class Asterix240Receive : public RadarReceive {
public:
    // Keep the original constructor signature
    Asterix240Receive(radar_pi* pi, RadarInfo* ri,
                      const NetworkAddress& report_addr,
                      const NetworkAddress& radar_interface_addr)
        : RadarReceive(pi, ri),
          m_report_addr(report_addr),
          m_radar_interface_addr(radar_interface_addr)
    {
        m_receive_socket = GetLocalhostServerTCPSocket();
        m_send_socket = GetLocalhostSendTCPSocket(m_receive_socket);

        SetInfoStatus(wxString::Format(
            wxT("%s: %s"), m_ri->m_name.c_str(), _("Initializing")));
    }

    virtual ~Asterix240Receive() {
        Shutdown();
    }

    // wxThread entry point
    void* Entry() override;

    // Stop thread and cleanup
    void Shutdown() override;

    // Status shown in Radar UI
    wxString GetInfoStatus() override;

protected:
    // Process incoming CAT-240 packet
    bool ProcessReport(uint8_t* data, size_t len);

private:
    SOCKET GetNewReportSocket();

    NetworkAddress m_radar_interface_addr;
    NetworkAddress m_report_addr;

    SOCKET m_receive_socket; // Where we listen for message from m_send_socket
    SOCKET m_send_socket; // A message to this socket will interrupt select()
                          // and allow immediate shutdown

    wxString m_status;  // Info/status string
};

PLUGIN_END_NAMESPACE

#endif /* _ASTERIX240RECEIVE_H_ */