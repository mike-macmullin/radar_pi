/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Radar Plugin – ASTERIX CAT-240
 *
 ***************************************************************************
 */

#ifndef _ASTERIX240CONTROL_H_
#define _ASTERIX240CONTROL_H_

#include "RadarInfo.h"
#include "RadarControl.h"
#include "pi_common.h"
#include "socketutil.h"

PLUGIN_BEGIN_NAMESPACE

class Asterix240Control : public RadarControl {
public:
    Asterix240Control();
    ~Asterix240Control();

    bool Init(radar_pi* pi, RadarInfo* ri,
              NetworkAddress& interfaceAddress,
              NetworkAddress& radarAddress);

    void RadarTxOff();
    void RadarTxOn();
    bool RadarStayAlive();

    bool SetRange(int meters);

    bool SetControlValue(ControlType controlType,
                         RadarControlItem& item,
                         RadarControlButton* button);

private:
    radar_pi*  m_pi;
    RadarInfo* m_ri;
    wxString   m_name;
};

PLUGIN_END_NAMESPACE

#endif /* _ASTERIX240CONTROL_H_ */
