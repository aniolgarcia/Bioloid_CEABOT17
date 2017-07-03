#ifndef _BALANCE_H
#define _BALANCE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {robot_standing,robot_face_down,robot_face_up} fallen_t;

// public functions
void balance_init(void);
/**
 * \brief Function to check if the robot has fallen to the ground
 * 
 * This function returns:
 *   0x00 : if the robot is standing.
 *   0x01 : if the robot has fallen face-to-ground 
 *   0x02 : if the robot has fallen back-to-ground
 * After reading the value, the internal state is automatically set
 * back to 0x00
 */
fallen_t balance_robot_has_fallen(void);
void balance_reset_fall_state(void);
uint8_t balance_calibrate_gyro(void);
/**
 * \brief Function to enable the internal gyros
 * 
 * If the gyroscopes have been detected and calibrated in the init_motion() function,
 * this function enbales them to be used. When enabled they perform two tasks: first
 * balance the robot while moving to make it more stable and also detect when the 
 * robot falls.
 */
void balance_enable_gyro(void);
/**
 * \brief Function to disable the internal gyros
 *
 * If the gyroscopes have been detected and calibrated in the init_motion() function,
 * this function disales them. When disabled it is no longer possible to detect when 
 * robot falls. gyro
 *
 * This function may be used to avoid cancelling the standing up action due to fast 
 * motions associated with this motion. After the robot is standing up, the gyros may
 * be enabled again.
 */
void balance_disable_gyro(void);
/**
 * \brief Function to check whether the gyros are enabled or not
 *
 * This function checks wether the gyros are currently enabled or not.
 *
 * \return 0x01 if the gyros are enabled and 0x00 otherwise.
 */
uint8_t balance_is_gyro_enabled(void);

#ifdef __cplusplus
}
#endif

#endif
