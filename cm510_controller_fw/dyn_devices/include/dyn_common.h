#ifndef _DYN_COMMON_H
#define _DYN_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>

#define            MODEL_NUM           ((unsigned char)0x00)
#define            FIRMWARE_VER        ((unsigned char)0x02)
#define            ID                  ((unsigned char)0x03)
#define            BAUD_RATE           ((unsigned char)0x04)
#define            RET_DEL_TIME        ((unsigned char)0x05)
#define            STATUS_RET_LEVEL    ((unsigned char)0x10)

// error definitions
#define         INSTRUCTION_ERROR         0x40
#define         OVERLOAD_ERROR            0x20
#define         CHECKSUM_ERROR            0x10
#define         RANGE_ERROR               0x08
#define         OVERHEATING_ERROR         0x04
#define         ANGLE_LIMIT_ERROR         0x02
#define         INPUT_VOLTAGE_ERROR       0x01

// error mask check functions
#define         IS_INSTRUCTION_ERROR_SET(A)      ((A&INSTRUCTION_ERROR)>>6)
#define         IS_OVERLOAD_ERROR_SET(A)         ((A&OVERLOAD_ERROR)>>5)
#define         IS_CHECKSUM_ERROR_SET(A)         ((A&CHECKSUM_ERROR)>>4)
#define         IS_RANGE_ERROR_SET(A)            ((A&RANGE_ERROR)>>3)
#define         IS_OVERHEATING_ERROR_SET(A)      ((A&OVERHEATING_ERROR)>>2)
#define         IS_ANGLE_LIMIT_ERROR_SET(A)      ((A&ANGLE_LIMIT_ERROR)>>1)
#define         IS_INPUT_VOLTAGE_ERROR_SET(A)    (A&INPUT_VOLTAGE_ERROR)

// servo interface functions EEPROM memory
/**
 * \brief function to get the model number of a given servo
 * 
 * This function return the code corresponding to the model number of the servo 
 * identifier given as a parameter. The correspondence between the returned 
 * codes and the servo models is as follows:
 *
 * * AX-12/ AX-12+/ AX-12A -> 0x000C
 * * AX-18F/ AX-18A        -> 0x0012 
 * * DX-113                -> 0x0071
 * * DX-116                -> 0x0074
 * * DX-117                -> 0x0075
 * * RX-10                 -> 0x000A
 * * RX-24F                -> 0x0018
 * * RX-28                 -> 0x001C
 * * RX-64                 -> 0x0040
 * * EX-106+               -> 0x006B
 * * MX-28                 -> 0x001D
 * 
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 * 
 * \return the code corresponding to the model number of the given servo. 0xFFFF
 *         is returned in case of any error.
 *
 */
uint16_t get_model_number(uint8_t servo_id);
/**
 * \brief function to get the firmware version of a given servo
 *
 * This function return the version of the firmware of the given servo.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \return the current firmware version of the servo or 0xFFFF in case of any 
 *         error.
 */
uint8_t get_firmware_version(uint8_t servo_id);
/**
 * \brief function to change the current identifier of a given servo
 *
 * This function changes the current identifier of a given servo. Care must be 
 * taken when using this function to avoid assigning an identifer that is 
 * already present in the bus. If so, any access to the new identifier will 
 * report an error, since both servos will try to comply at the same time.
 * 
 * \param old_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \param new_id the new identifier that will be assigned to the servo.
 * 
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t set_servo_id(uint8_t old_id, uint8_t new_id);
/**
 * \brief Function to get the current baudrate of the bus
 *
 * This function returns the code corresponding to the baudrate used in the
 * dynamixel bus. See the servo documentation for the correspondence between the
 * code and the actual baudrate.
 *
 * Care must be taken when using this function because when the baudrate of a servo
 * is changed, it will be no longer possible to communicate with it util the host
 * baudrate is also changed.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \return the baudrate code of the given servo or 0xFF in case of any error.
 */
uint8_t get_baudrate(uint8_t servo_id);
/**
 * \brief Function to change the current baudrate of the given servo.
 *
 * This function changes the communication baudrate of the given servo. Once the
 * baudrate is changed, it will be no longer possible to communicate with servo
 * until the host baudrate is also changed.
 *
 * \param servo_id identifier of the desired servo. This number must be a 
 *        non-negative integer between 0x00 and 0xFD. The 0xFE identifier can 
 *        not be passed because it is used as the broadcast identifier.
 *
 * \param baudrate the code for the desired baudrate. This value can be any
 *                 non-negative integer between 1 and 252. Not that not all 
 *                 servo models support the highest baudrates.
 *
 * \return 0 if the function completes correctly, -1 otherwise
 */
uint8_t set_baudrate(uint8_t servo_id, uint8_t baudrate);

#ifdef __cplusplus
}
#endif

#endif
