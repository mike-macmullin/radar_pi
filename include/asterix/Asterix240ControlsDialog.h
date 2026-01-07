/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Radar Plugin – ASTERIX CAT-240
 *
 ***************************************************************************
 */

#ifndef _ASTERIX240CONTROLSDIALOG_H_
#define _ASTERIX240CONTROLSDIALOG_H_

#include "ControlsDialog.h"

PLUGIN_BEGIN_NAMESPACE

//----------------------------------------------------------------------------
//    Radar Control Dialog Specification
//----------------------------------------------------------------------------
class Asterix240ControlsDialog : public ControlsDialog {
public:
    Asterix240ControlsDialog();
    ~Asterix240ControlsDialog() {};
};

PLUGIN_END_NAMESPACE

#endif /* _ASTERIX240CONTROLSDIALOG_H_ */
