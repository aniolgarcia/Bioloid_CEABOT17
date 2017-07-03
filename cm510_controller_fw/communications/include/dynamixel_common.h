#ifndef _DYNAMIXEL_COMMON_H
#define _DYNAMIXEL_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#define     MAX_DATA_LENGTH     255

#define     MAX_HEADER_LENGTH   6

// errors
#define     DYN_SUCCESS         0
#define     DYN_BAD_FORMAT      1
#define     DYN_NO_DEVICE       2
#define     DYN_TIMEOUT         3
#define     DYN_COMM_ERROR      4

typedef struct{
  unsigned char *data_addr;
}TWriteData;

typedef enum{no_return=0x00,
             return_only_read=0x01,
             return_all=0x02} return_level_t;

// possible packet types
typedef enum{DYN_PING=0x01,
             DYN_READ=0x02,
             DYN_WRITE=0x03,
             DYN_REG_WRITE=0x04,
             DYN_ACTION=0x05,
             DYN_RESET=0x06,
             DYN_SYNC_READ=0x82,
             DYN_SYNC_WRITE=0x83,
             DYN_BULK_READ=0x92,
             DYN_BULK_WRITE=0x93} TDynInstruction;

// boradcast ID
#define     DYN_BROADCAST_ID    0xFE

// status packet
typedef enum{DYN_NO_ERROR=0x00,
             DYN_INST_ERROR=0x40,
             DYN_OVERLOAD_ERROR=0x20,
             DYN_CHECKSUM_ERROR=0x10,
             DYN_RANGE_ERROR=0x08,
             DYN_OVERTEMP_ERROR=0x04,
             DYN_ANGLE_ERROR=0x02,
             DYN_VOLTAGE_ERROR=0x01} TDynError;

typedef enum{DYN_VER1=0x01,DYN_VER2=0x02} TDynVersion;

#ifdef __cplusplus
}
#endif

#endif
