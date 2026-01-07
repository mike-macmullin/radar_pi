/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Radar Plugin – ASTERIX CAT-240
 *
 ***************************************************************************
 */

#ifndef _ASTERIX240CONTROLSET_H_
#define _ASTERIX240CONTROLSET_H_

#include "SoftwareControlSet.h"

// ---------------------------------------------------------------------------
// ASTERIX CAT-240 is receive-only
// No active radar controls are supported
// ---------------------------------------------------------------------------

HAVE_CONTROL(
    CT_RANGE, CTD_AUTO_YES, 1000, CTD_MIN_ZERO, 0, CTD_STEP_1, CTD_NUMERIC)
HAVE_CONTROL(
    CT_TIMED_IDLE, CTD_AUTO_NO, CTD_DEF_OFF, 1, 10, CTD_STEP_1, CTD_MINUTES)
HAVE_CONTROL(
    CT_TIMED_RUN, CTD_AUTO_NO, 1, 1, 5, CTD_STEP_1, CTD_MINUTES)

#endif /* _ASTERIX240CONTROLSET_H_ */
