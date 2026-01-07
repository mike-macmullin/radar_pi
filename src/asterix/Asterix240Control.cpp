/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Radar Plugin – ASTERIX CAT-240
 *
 ***************************************************************************
 */

#include "Asterix240Control.h"

PLUGIN_BEGIN_NAMESPACE

Asterix240Control::Asterix240Control() {
    m_pi = nullptr;
    m_ri = nullptr;
    m_name = wxT("ASTERIX CAT-240");
}

Asterix240Control::~Asterix240Control() {}

bool Asterix240Control::Init(radar_pi* pi, RadarInfo* ri,
                            NetworkAddress&,
                            NetworkAddress&) {
    m_pi = pi;
    m_ri = ri;
    m_name = ri->m_name;

    return true;
}

void Asterix240Control::RadarTxOff() {
    if (m_ri)
        m_ri->m_state.Update(RADAR_STANDBY);
}

void Asterix240Control::RadarTxOn() {
    if (m_ri)
        m_ri->m_state.Update(RADAR_TRANSMIT);
}

bool Asterix240Control::RadarStayAlive() {
    return true;
}

// ---------------------------------------------------------------------------
// Range is receive-only ignore safely
// ---------------------------------------------------------------------------
bool Asterix240Control::SetRange(int meters) {
    if (m_ri)
        m_ri->m_range.Update(meters);
        return true;
    return false;
}

bool Asterix240Control::SetControlValue(ControlType,
                                       RadarControlItem&,
                                       RadarControlButton*) {
    return false;
}

PLUGIN_END_NAMESPACE
