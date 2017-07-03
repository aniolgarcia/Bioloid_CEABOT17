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

#ifndef _DYN_SERVOS_REG_H
#define _DYN_SERVOS_REG_H

// EEPROM memory
#define            CW_ANGLE_LIM        ((unsigned char)0x06)
#define            CCW_ANGLE_LIM       ((unsigned char)0x08)
#define            TEMP_LIM            ((unsigned char)0x0B)
#define            LOW_VOLTAGE_LIM     ((unsigned char)0x0C)
#define            HIGH_VOLTAGE_LIM    ((unsigned char)0x0D)
#define            MAX_TORQUE          ((unsigned char)0x0E)
#define            LED_ALARM           ((unsigned char)0x11)
#define            SHD_ALARM           ((unsigned char)0x12)
// RAM memory
#define            TORQUE_EN           ((unsigned char)0x18)
#define            SERVO_LED           ((unsigned char)0x19)
#define            CW_COMP_MARGIN      ((unsigned char)0x1A)
#define            CCW_COMP_MARGIN     ((unsigned char)0x1B)
#define            CW_COMP_SLOPE       ((unsigned char)0x1C)
#define            CCW_COMP_SLOPE      ((unsigned char)0x1D)
#define            GOAL_POS            ((unsigned char)0x1E)
#define            GOAL_SPEED          ((unsigned char)0x20)
#define            TORQUE_LIM          ((unsigned char)0x22)
#define            CURRENT_POS         ((unsigned char)0x24)
#define            CURRENT_SPEED       ((unsigned char)0x26)
#define            CURRENT_LOAD        ((unsigned char)0x28)
#define            CURRENT_VOLTAGE     ((unsigned char)0x2A)
#define            CURRENT_TEMP        ((unsigned char)0x2B)
#define            REGISTERED          ((unsigned char)0x2C)
#define            MOVING              ((unsigned char)0x2D)
#define            LOCK                ((unsigned char)0x2E)
#define            PUNCH               ((unsigned char)0x30)

#endif
