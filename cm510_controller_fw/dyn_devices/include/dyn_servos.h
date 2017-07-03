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

#ifndef _DYN_SERVOS_H
#define _DYN_SERVOS_H

#include <avr/io.h>
#include "dyn_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Function to get the current angle limits of a given servo.
 * 
 * This function returns the clockwise and the counter-clockwise angle limits 
 * of the given servo. This values correspond to the actual motion range of the
 * servo, which is equal or smaller than the maximum range of the servo. The
 * maximum motion range changes depending on the servo model.
 * 
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 * 
 * \param cw the maximum clockwise position of the servo in servo units. This 
 *           value depend on the servo encoder resolution and motion range, and 
 *           it must be always smaller than the ccw  value.
 *
 * \param ccw the maximum counter-clockwise position of the servo in servo units. 
 *            This value depend on the servo encoder resolution and motion range, 
 *            and it must be always bigger than the cw  value.
 *
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t get_angle_limits(uint8_t servo_id, uint16_t *cw, uint16_t *ccw);
/**
 * \brief Function to set the angle limits of a given servo
 * 
 * This function effectivelly changes the range of motion of a given servo. The value 
 * of the motion limits depend on the maximum range of motion of the given servo and 
 * the encoder resolution, both of which depend on the servo model.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 * 
 * \param cw the maximum clockwise position of the servo in servo units. This 
 *           value depend on the servo encoder resolution and motion range, and 
 *           it must be always smaller than the ccw  value.
 *
 * \param ccw the maximum counter-clockwise position of the servo in servo units. 
 *            This value depend on the servo encoder resolution and motion range, 
 *            and it must be always bigger than the cw  value.
 *
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t set_angle_limits(uint8_t servo_id, uint16_t cw, uint16_t ccw);
/**
 * \brief Function to get the temperature limit of a given servo
 * 
 * This function returns the current temperature limit of the given servo. Temperatures
 * above this one will shutdown the servo until the temperature goes down. The returned 
 * value is given in celsius degrees and the valid range is between 10 and 99.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \return the current temperature limit or 0xFF in case of any error.
 */
uint8_t get_temperature_limit(uint8_t servo_id);
/**
 * \brief Function to change the temperature limit of a given servo
 * 
 * This function changes the current value of the temperature limit of a given
 * servo. When the servo temperature rises over this value it will shutdown to 
 * avoid damage.
 * 
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 * 
 * \param temp the new temperature limit in celsius degrees. The valid range of
 *             this value is between 10 ºC and 99 ºC. 
 *
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t set_temperature_limit(uint8_t servo_id, uint8_t temp);
/**
 * \brief Function to get the operating voltage range of a given servo
 * 
 * This functions returns the current operating voltage range of the given servo.
 * If the supply voltage is outside this range, the servo will not operate 
 * correctly.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \param low the current lowest allowed supply voltage. This value is given in
 *            steps of 0.1V and the valid range varies with the servo model.
 *
 * \param high the current highest allowed supply voltage. This value is given in
 *             steps of 0.1V and the valid range varies with the servo model.
 *
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t get_voltage_limits(uint8_t servo_id, uint8_t *low, uint8_t *high);
/**
 * \brief Function to change the operating voltage range of a given servo
 *
 * This funtion chnages the operating voltage range of the given servo. If the 
 * supply voltage is outside this range, the servo will not operate correctly.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \param low the current lowest allowed supply voltage. This value is given in
 *            steps of 0.1V and the valid range varies with the servo model.
 *
 * \param high the current highest allowed supply voltage. This value is given in
 *             steps of 0.1V and the valid range varies with the servo model.
 *
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t set_voltage_limits(uint8_t servo_id, uint8_t low, uint8_t high);
/**
 * \brief Function to get the maximum servo torque
 *
 * This function returns the maximum torque allowed to the given servo, which must 
 * be always smaller than the maximum servo torque. The value is given by an integer
 * in the range of 0 to 1023, which corresponds to the ratio of the maximum torque 
 * between 0% and 100%.
 *
 * This value is stored in non-volatile memory and it is used at start time
 * to set up the maximum torque. 
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \return the current maximum torque allowed or 0xFFFF in case of any error.
 */
uint16_t get_max_torque(uint8_t servo_id);
/**
 * \brief Function to change the maximum allowed torque
 *
 * This function changes the maximum output of the servo from 0% to the 100% 
 * (the maximum servo torique). The value is given by an integer in the range 
 * of 0 to 1023, which corresponds to the ratio of the maximum torque between 
 * 0% and 100%.
 * 
 * This value is stored in non-volatile memory and it is used at start time
 * to set up the maximum torque. 
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \param torque the maximum allowed torque as an integer between 0 and 1023,
 *               where 1023 is the maximum servo torque.
 *               
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t set_max_torque(uint8_t servo_id, uint16_t torque);
/**
 * \brief function to get the current led alarm configuration
 *
 * This function returns the mask of errors that start blinking the servo LED.
 * The possible causes of error, and the corresponding bit mask, are listed 
 * below:
 * 
 * * Instruction error   -> 0x40
 * * Overload error      -> 0x20
 * * Checksum error      -> 0x10
 * * Range error         -> 0x08
 * * Overheating error   -> 0x04
 * * Angle limit error   -> 0x02
 * * Input voltage error -> 0x01
 * 
 * Use the IS_<type of error>_SET macros to easily check which errors are set.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 * 
 * \return the current led alarm mask or 0xFF in case of any error
 */
uint8_t get_led_alarm(uint8_t servo_id);
/**
 * \brief Function to set the led alarm configuration
 *
 * This function will set which errors will make the servo LED to start 
 * blinking. The possible causes of error, and the corresponding bit mask, 
 * are listed below:
 *
 * * Instruction error   -> 0x40
 * * Overload error      -> 0x20
 * * Checksum error      -> 0x10
 * * Range error         -> 0x08
 * * Overheating error   -> 0x04
 * * Angle limit error   -> 0x02
 * * Input voltage error -> 0x01
 *
 * Use the error macros or'ed to create the desired mask.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 * 
 * \param alarms The new error mask that will make the servo led to start
 *               blinking.
 *
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t set_led_alarm(uint8_t servo_id, uint8_t alarms);
/**
 * \brief function to get the current shutdown alarm configuration
 *
 * This function returns the mask of errors that will shutdow the servo.
 * The possible causes of error, and the corresponding bit mask, are listed 
 * below:
 * 
 * * Instruction error   -> 0x40
 * * Overload error      -> 0x20
 * * Checksum error      -> 0x10
 * * Range error         -> 0x08
 * * Overheating error   -> 0x04
 * * Angle limit error   -> 0x02
 * * Input voltage error -> 0x01
 * 
 * Use the IS_<type of error>_SET macros to easily check which errors are set.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 * 
 * \return the current led alarm mask or 0xFF in case of any error
 */
uint8_t get_shd_alarm(uint8_t servo_id);
/**
 * \brief Function to set the led alarm configuration
 *
 * This function will set which errors will make the servo to shutdown. The 
 * possible causes of error, and the corresponding bit mask, are listed below:
 *
 * * Instruction error   -> 0x40
 * * Overload error      -> 0x20
 * * Checksum error      -> 0x10
 * * Range error         -> 0x08
 * * Overheating error   -> 0x04
 * * Angle limit error   -> 0x02
 * * Input voltage error -> 0x01
 *
 * Use the error macros or'ed to create the desired mask.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 * 
 * \param alarms The new error mask that will make the servo to shutdown.
 *
 * \return 0 if the function completes correctly, -1 otherwise

 */
uint8_t set_shd_alarm(uint8_t servo_id, uint8_t alarms);

// servo interface functions RAM memory
/**
 * \brief Function to set the wheel mode
 *
 * This function configures the servo in the continuous rotation mode. In this
 * mode the position commands have no effect, and the servo uses a torque
 * control to move. The set_target_speed() function is used to set the desired
 * torque ratio and the direction of rotation.
 * 
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t set_wheel_mode(uint8_t servo_id);
/**
 * \brief Function to set the joint mode
 *
 * This function configures the servo in the normal joint operation, where the
 * servo can move at the desired speed between the maximum and minimum angles.
 * In this case the set_target_position() and set_target_speed() functions are
 * used to set up the desired motion of the servo.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t set_joint_mode(uint8_t servo_id);
/**
 * \brief Function to check the current servo mode of a given servo
 *
 * This function checks wheteher the servo is working in the joint mode or
 * in the wheel mode.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \return 0 if the servo is in joint mode, 0 if the servo is in wheel mode and
 *         0xFF in case of any error.
 */
uint8_t is_joint_mode(uint8_t servo_id); 
/**
 * \brief Function to enable the given servo
 *
 * This function enables the given servo, allowing it to move when a new command
 * is loaded. 
 * 
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 * 
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t enable_servo(uint8_t servo_id);
/**
 * \brief function to disable the given servo
 *
 * This function disables the given servo. After calling this function, the 
 * servo will ignore any motion command.

 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 * 
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t disable_servo(uint8_t servo_id);
/**
 * \brief function to check wether the servo is enabled or not
 *
 * This function checks wether the servo is enabled or not.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 * 
 * \return 0 if the servo is disabled, 1 if the servo is enabled and 0xFF in
 *         case of any error.
 */
uint8_t is_servo_enabled(uint8_t servo_id);
/**
 * \brief Function to turn the servo led on
 * 
 * This function turns the servo LED on. This function will override any led
 * alarm currently active.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t turn_servo_led_on(uint8_t servo_id);
/**
 * \brief Function to turn the servo led off
 *
 * This function turns the servo LED off. This function will override any led
 * alarm currently active.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t turn_servo_led_off(uint8_t servo_id);
/**
 * \brief Function to check wether the servo led is on or not
 *
 * This function checks wether the servo LED is turned on or not.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \return 0 if the LED is turnwed off, 1 if the led is turned on and 0xFF in case
 *         of any error.
 */
uint8_t is_servo_led_on(uint8_t servo_id);
/**
 * \brief Function to get the current compliance margin of a given servo
 *
 * This function return the size of the dead zone arround the desired target position 
 * in both directions (clockwise and counter-clockwise). The compliance margin in both 
 * directions can be any integer value between 0 and 255.
 *
 * Some servo models do not use this type of control, and use a PID standard control
 * instead. Check the servo documentation to know which control is used in the given
 * servo.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \param cw the current clockwise compliance margin in error counts.
 *
 * \param ccw the current counter-clockwise compliance margin in error counts.
 *
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t get_compliance_margin(uint8_t servo_id, uint8_t *cw, uint8_t *ccw);
/**
 * \brief Function to set the desired compliance margin of a given servo
 *
 * This function sets the size of the dead zone arround the desired target position 
 * in both directions (clockwise and counter-clockwise). The compliance margin in both 
 * directions can be any integer value between 0 and 255.
 *
 * Some servo models do not use this type of control, and use a PID standard control
 * instead. Check the servo documentation to know which control is used in the given
 * servo.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \param cw the desired clockwise compliance margin in error counts.
 *
 * \param ccw the desired counter-clockwise compliance margin in error counts.
 *
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t set_compliance_margin(uint8_t servo_id, uint8_t cw, uint8_t ccw);
/**
 * \brief Function to get the current compliance slope of a given servo
 *
 * This functions returns the gain of the controller in both directions (clockwise and counter-
 * clockwise). The compliance slope can only be set in discrete steps as shown below:
 *
 * 0x00 - 0x03 -> 0x02 
 * 0x04 - 0x07 -> 0x04 
 * 0x08 - 0x0F -> 0x08 
 * 0x10 - 0x1F -> 0x10 
 * 0x20 - 0x3F -> 0x20 
 * 0x40 - 0x7F -> 0x40 
 * 0x80 - 0xFF -> 0x80 
 *
 * This value sets the actual window in which the set point is proportional to the error. The
 * actual gain (slope) will also depend on the punch value and the compliace margin.
 *
 * Some servo models do not use this type of control, and use a PID standard control
 * instead. Check the servo documentation to know which control is used in the given
 * servo.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \param cw the current clockwise compliance slope in error counts
 *
 * \param ccw the current counter-clockwise compliance slope in error counts.
 * 
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t get_compliance_slope(uint8_t servo_id, uint8_t *cw, uint8_t *ccw);
/**
 * \brief Function to set the desired compliance slope of a given servo
 *
 * This functions sets the gain of the controller in both directions (clockwise and counter-
 * clockwise). The compliance slope can only be set in discrete steps as shown below:
 *
 * 0x00 - 0x03 -> 0x02 
 * 0x04 - 0x07 -> 0x04 
 * 0x08 - 0x0F -> 0x08 
 * 0x10 - 0x1F -> 0x10 
 * 0x20 - 0x3F -> 0x20 
 * 0x40 - 0x7F -> 0x40 
 * 0x80 - 0xFF -> 0x80 
 *
 * This value sets the actual window in which the set point is proportional to the error. The
 * actual gain (slope) will also depend on the punch value and the compliace margin.
 *
 * Some servo models do not use this type of control, and use a PID standard control
 * instead. Check the servo documentation to know which control is used in the given
 * servo.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \param cw the desired clockwise compliance slope in error counts
 *
 * \param ccw the desired counter-clockwise compliance slope in error counts.
 * 
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t set_compliance_slope(uint8_t servo_id, uint8_t cw, uint8_t ccw);
/**
 * \brief Function to get the PID controller parameters of a given servo
 *
 * Not yet implemented
 */
uint8_t get_pid_params(uint8_t servo_id,uint8_t *P,uint8_t *I, uint8_t *D);
/**
 * \brief Function to set the PID controller parameters of a given servo
 *
 * Not yet implemented
 */
uint8_t set_pid_params(uint8_t servo_id,uint8_t P,uint8_t I, uint8_t D);
/**
 * \brief Function to get the current target position
 *
 * This function return the last position command sent to the servo. The position is given
 * as an integer between 0 and the maximum encoder resolution, which depends on the servo
 * model.
 *
 * The value returned by this function may or may not coincide with the actual position of
 * the servo. To get the actual position of the servo use the get_current_position() 
 * function.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \return the current target position of the servo between 0 and the maximum encoder 
 *         resolution or 0xFFFF in case of any error.
 */
uint16_t get_target_position(uint8_t servo_id);
/**
 * \brief Function to set the desired target position
 * 
 * This function sends the desired position to the servo. If the servo is enabled, it
 * starts moving immediatelly after calling this function at the speed set by the
 * set_target_speed() function or the default speed of the servo. The position is given
 * as an integer between 0 and the maximum encoder resolution, which depends on the servo
 * model.
 * 
 * In the wheel mode, this function has no effect.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \param position the desired target position of the servo between 0 and the maximum
 *                 encoder resolution.
 *
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t set_target_position(uint8_t serov_id, uint16_t position);
/**
 * \brief Function to get the current target speed
 *
 * This function returns the last speed command sent to the servo. In joint mode, the 
 * speed is given as an integer between 0 and 1023, whatever the servo used. The actual 
 * speed depends linearly on the maximum speed of the servo.
 *
 * In wheel mode, the speed range is between 0 and 2047, where the most significant bit 
 * forces the direction of rotation. In this case, the value corresponds to the ratio
 * of the maximum torque used, but not the actual speed of the servo.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \return the current speed command and 0xFFFF in cas of any error. 
 */
uint16_t get_target_speed(uint8_t servo_id);
/**
 * \brief Function to set teh desired target speed
 *
 * This function sends the desired tagret speed to the servo. In joint mode, this 
 * function will chnage the current speed of the servo if it is moving. Otherwise, 
 * it will have no effect until a new position command is loaded.
 * 
 * In wheel mode, this function immediately changes the current speed of the servo.
 * In this case, the value corresponds to the ratio of the maximum torque used, but 
 * not the actual speed of the servo.
 * 
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 * 
 * \param speed the desired speed in joint mode, or the desired torque ratio in
 *              wheel mode.
 * 
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t set_target_speed(uint8_t servo_id, uint16_t speed);
/**
 * \brief Function to set the maximum torque for a given servo
 * 
 * This function returns the current maximum torque allowed. Right after power
 * up, this function returns the value stored in non-volatile memory, that can
 * be accessed and modified with the get_max_torque() and set_max_torque()
 * functions.
 * 
 * After power up, this function will return the last value set by the 
 * set_torque_limit() function.
 *
 * The value returned by this function is netween 0 and 1023 and corresponds
 * to the maximum torque ratio allowed, being 1023 the 100% of the maximum 
 * torque.
 *
 * If this value is set to 0 in any operation mode, the servo will not move.
 * When any error shuts down the servo, this value is set to 0, so it must 
 * be reset in order to set the servo back to normal operation.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \return the maximum torque allowed or 0xFFFF in case of any error.
 */
uint16_t get_torque_limit(uint8_t servo_id);
/**
 * \brief Function to set the maximum torque for a given servo
 *
 * This function sets the desired maximum torque alloed. Right after power up,
 * the value for the maximum torque allowed is read from the non-volatile 
 * memory. After that, calls to this function will modify the current maximum
 * torque allowed without modifying the default value in non-volatile memory.
 *
 * To change the default value used, use the set_max_torque() function.
 * If this value is set to 0 in any operation mode, the servo will not move.
 * When any error shuts down the servo, this value is set to 0, so it must 
 * be reset in order to set the servo back to normal operation.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \param torque the desired maximum torque allowed to the servo.
 *
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t set_torque_limit(uint8_t servo_id, uint16_t torque);
/**
 * \brief Function to return the current position of the given servo
 *
 * This function returns the current position of the given servo. The 
 * range of the returned value is from 0 to the maximum encoder resolution
 * of the servo, which depends on the model.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 * 
 * \return the current servo position and 0xFFFF in case of any error.
 */
uint16_t get_current_position(uint8_t servo_id);
/**
 * \brief Function to return the current speed of the given servo
 *
 * This function returns the current speed of the given servo. In the joint 
 * mode the range of this value is between 0 and 1023, being 1023 the maximum 
 * speed of the servo which depends on the servo model.
 * 
 * In the wheel mode, the range of the returned value is between 0 and 2047,
 * where the most significant indicates the direction of rotarion, and the 
 * rest indicates the current torque ratio used.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 * 
 * \return the current servo speed and 0xFFFF in case of any error.
 */
uint16_t get_current_speed(uint8_t servo_id);
/**
 * \brief Function to return the current load of the given servo
 * 
 * This function returns the current torque ratio used by the servo in Joint
 * mode. In wheel mode, the value returned by this function is equivalent to
 * the value returned by the get_current_speed() function.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 * 
 * \return the current torque ratio and 0xFFFF in case of any error.
 */
uint16_t get_current_load(uint8_t servo_id);
/**
 * \brief Function to get the current supply voltage of a given servo
 * 
 * This function returns the current supply voltage in 0.1 volts steps. This value
 * must be between the lower and upper limits set by the set_voltage_limits()
 * function.
 *
 * If the supply voltage is out of range, and the corresponding bit is set
 * in the led alarm or the shutdown alarm registers, the LED will blink and/or
 * the servo will be shut down.
 * 
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 * 
 * \return the current supply voltage and 0xFF in case of any error.
 */
uint8_t get_current_voltage(uint8_t servo_id);
/**
 * \brief Function to get the current temperature of a given servo
 *
 * This function returns the current temperature celsius degrees. This value
 * must be between the lower and upper limits set by the set_voltage_limits()
 * function.
 *
 * If the temperature is out of range, and the corresponding bit is set
 * in the led alarm or the shutdown alarm registers, the LED will blink and/or
 * the servo will be shut down.
 * 
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 * 
 * \return the current temperature and 0xFF in case of any error.
 */
uint8_t get_current_temperature(uint8_t servo_id);
/**
 * \brief Function to check whether the servo is moving or not
 *
 * This functions checks whether the servo is moving or not in both
 * joint and wheel modes. 
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \return 0 if the servo is stopped, 1 if the servo is moving and 0xFF in 
 *         case of any error.
 */
uint8_t is_moving(uint8_t servo_id);
/**
 * \brief Function to check whether the servo is locked or not
 *
 * This functions checks whether the servo is locked or not. When the servo is 
 * locked, it is impossible to make any changes to the servo. To unlock the 
 * servo, it is necessary to turn the servo off.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \return 0 if the servo is unlocked, 1 if the servo is locked and 0xFF in 
 *         case of any error.
 */
uint8_t is_locked(uint8_t servo_id);
/**
 * \brief Function to lock the servo
 *
 * This function locks the servo. After this function is called, it will
 * be no loonger possible to make any changes to the servo configuration.
 * To unlock the servo, it is necessary to turn it off.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t lock(uint8_t servo_id);
/**
 * \brief Function to get the current punch value of a given servo.
 *
 * This function returns the current value of the minimum gain used in the
 * control of the servo in both clockwise and counter-clockwise directions.
 * The punch can be any value between 0 and 1023.
 *
 * Some servo models do not use this type of control, and use a PID standard control
 * instead. Check the servo documentation to know which control is used in the given
 * servo.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \return the current punch value and 0xFFFF in case of any error.
 */
uint16_t get_punch(uint8_t servo_id);
/**
 * \brief Function to set the desired punch value of a given servo.
 *
 * This function sets the desired value of the minimum gain used in the 
 * control of the servo in both clockwise and counter-clockwise directions.
 * The punch can be any value between 0 and 1023.
 * 
 * Some servo models do not use this type of control, and use a PID standard control
 * instead. Check the servo documentation to know which control is used in the given
 * servo.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \param punch the desired punch value between 0 and 1023.
 *
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t set_punch(uint8_t servo_id, uint16_t punch);

#ifdef __cplusplus
}
#endif

#endif
