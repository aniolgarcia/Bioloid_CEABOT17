/*
 *  Copyright (c) 2011 Humanoid Lab
 * 
 *  This file is part of iri-libbioloid. iri-libbioloid is free software: you
 *  can redistribute it and/or modify it under the terms of the GNU Lesser
 *  Public License as published by the Free Software Foundation, either 
 *  version 3 of the License, or (at your option) any later version.
 * 
 *  iri-libbioloid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 * 
 *  You should have received a copy of the GNU Lesser Public License
 *  along with iri-libbioloid.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _CM510_H
#define _CM510_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cont_cfg.h"
#include "motion_cfg.h"
#include "comm_cfg.h"
#include "gpio.h"
#include "adc.h"
#include "buzzer.h"
#include "motion_manager.h"
#include "action.h"
#include "user_time.h"
#include "serial_console.h"
#include "exp_board.h"

// general interface
/**
 * \brief Function to initialize all the cm510 modules
 * 
 * This function initializes the leds, buttons, analog to digital, buzzer and
 * timer modules of the cm510 controller. See the documentation on the other
 * init functions for more details.
 *
 * The controller also uses an internal timer as a time base, which is used by
 * other modules to perform peridic updates or actions. The timer 2 is used to
 * generate a 1ms periodic interrupt. The interrupt service routine performs 
 * the following actions:
 * 
 * * Checks the buttons for any change and calls the corresponding callback 
 *   function.
 * 
 * After all the initializations are done, the global interrupts are enabled.
 *
 * \return 0 if all the initializations complete successfully and 0xFF otherwise.
 */
void init_cm510(void);

#ifdef __cplusplus
}
#endif

#endif
