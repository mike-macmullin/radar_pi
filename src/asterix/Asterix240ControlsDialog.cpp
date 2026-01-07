/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Radar Plugin – ASTERIX CAT-240
 *
 ***************************************************************************
 */

#include "Asterix240ControlsDialog.h"

#include "Arpa.h"
#include "RadarPanel.h"

PLUGIN_BEGIN_NAMESPACE

Asterix240ControlsDialog::Asterix240ControlsDialog() {
#include "asterix/Asterix240ControlSet.h"
}

PLUGIN_END_NAMESPACE
