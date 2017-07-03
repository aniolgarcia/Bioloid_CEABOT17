#ifndef _EXP_BOARD_REG_H
#define _EXP_BOARD_REG_H

// Register Id  EEPROM - declared in dynamixel
#define Hardware_available        0x06   //6 - 0x06 R - Peripherals available with the current configuration
                                         //             bit 7 | bit 6 | bit 5 | bit 4 |  bit 3   |  bit 2   | bit 1 | bit 0
                                         //                                             uart_usb   uart_ttl    DAC    PWM
#define Status_Return_Level       0X10   //16 - 0x10 R/W - Level os reply information 
// EEPROM end at 0x1F
// RAM starts at 0x20
#define ADC_CONTROL               0x20   //32 - 0x20 R/W  ADC control  bit 7 | bit 6 | bit 5 | bit 4 | bit 3 |  bit 2  | bit 1 | bit 0
                                         //                                                                    running   stop    start
#define ADC_NUM_SAMPLES           0x21   //33 - 0x21 R/W  Number of sample to average
#define ADC_SAMPLE_PERIOD         0x22   //34 - 0x22 R/W  ADC sample period in ms
#define GPIO0_config              0x30   //48 - 0x30 R/W  GPIO 0 configuration bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                            mode (0 -> input, 1 -> output)
#define GPIO0_data                0x31   //49 - 0x31 R/W  GPIO 0 data bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                   current input value (input mode)
                                         //                                                                                   output value (output mode)
#define GPIO1_config              0x32   //50 - 0x32 R/W  GPIO 0 configuration bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                            mode (0 -> input, 1 -> output)
#define GPIO1_data                0x33   //51 - 0x33 R/W  GPIO 0 data bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                   current input value (input mode)
                                         //                                                                                   output value (output mode)
#define GPIO2_config              0x34   //52 - 0x34 R/W  GPIO 0 configuration bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                            mode (0 -> input, 1 -> output)
#define GPIO2_data                0x35   //53 - 0x35 R/W  GPIO 0 data bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                   current input value (input mode)
                                         //                                                                                   output value (output mode) 
#define GPIO3_config              0x36   //54 - 0x36 R/W  GPIO 0 configuration bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                            mode (0 -> input, 1 -> output)
#define GPIO3_data                0x37   //55 - 0x37 R/W  GPIO 0 data bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                   current input value (input mode)
                                         //                                                                                   output value (output mode)
#define GPIO4_config              0x38   //56 - 0x38 R/W  GPIO 0 configuration bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                            mode (0 -> input, 1 -> output)
#define GPIO4_data                0x39   //57 - 0x39 R/W  GPIO 0 data bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                   current input value (input mode)
                                         //                                                                                   output value (output mode) 
#define GPIO5_config              0x3A   //58 - 0x3A R/W  GPIO 0 configuration bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                            mode (0 -> input, 1 -> output)
#define GPIO5_data                0x3B   //59 - 0x3B R/W  GPIO 0 data bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                   current input value (input mode)
                                         //                                                                                   output value (output mode) 
#define GPIO6_config              0x3C   //60 - 0x3C R/W  GPIO 0 configuration bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                            mode (0 -> input, 1 -> output)
#define GPIO6_data                0x3D   //61 - 0x3D R/W  GPIO 0 data bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                   current input value (input mode)
                                         //                                                                                   output value (output mode) 
#define GPIO7_config              0x3E   //62 - 0x3E R/W  GPIO 0 configuration bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                            mode (0 -> input, 1 -> output)
#define GPIO7_data                0x3F   //63 - 0x3F R/W  GPIO 0 data bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                   current input value (input mode)
                                         //                                                                                   output value (output mode) 
#define GPIO8_config              0x40   //64 - 0x40 R/W  GPIO 0 configuration bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                            mode (0 -> input, 1 -> output)
#define GPIO8_data                0x41   //65 - 0x41 R/W  GPIO 0 data bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                   current input value (input mode)
                                         //                                                                                   output value (output mode) 
#define GPIO9_config              0x41   //66 - 0x42 R/W  GPIO 0 configuration bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                            mode (0 -> input, 1 -> output)
#define GPIO9_data                0x43   //67 - 0x43 R/W  GPIO 0 data bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                   current input value (input mode)
                                         //                                                                                   output value (output mode) 
#define GPIO10_config             0x44   //68 - 0x44 R/W  GPIO 0 configuration bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                            mode (0 -> input, 1 -> output)
#define GPIO10_data               0x45   //69 - 0x45 R/W  GPIO 0 data bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                   current input value (input mode)
                                         //                                                                                   output value (output mode) 
#define GPIO11_config             0x46   //70 - 0x46 R/W  GPIO 0 configuration bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                            mode (0 -> input, 1 -> output)
#define GPIO11_data               0x47   //71 - 0x47 R/W  GPIO 0 data bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                   current input value (input mode)
                                         //                                                                                   output value (output mode) 
#define GPIO12_config             0x48   //72 - 0x48 R/W  GPIO 0 configuration bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                            mode (0 -> input, 1 -> output)
#define GPIO12_data               0x49   //73 - 0x49 R/W  GPIO 0 data bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                   current input value (input mode)
                                         //                                                                                   output value (output mode) 
#define GPIO13_config             0x4A   //74 - 0x4A R/W  GPIO 0 configuration bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                            mode (0 -> input, 1 -> output)
#define GPIO13_data               0x4B   //75 - 0x4B R/W  GPIO 0 data bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                   current input value (input mode)
                                         //                                                                                   output value (output mode)
#define GPIO14_config             0x4C   //76 - 0x4C R/W  GPIO 0 configuration bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                            mode (0 -> input, 1 -> output)
#define GPIO14_data               0x4D   //77 - 0x4D R/W  GPIO 0 data bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                   current input value (input mode)
                                         //                                                                                   output value (output mode) 
#define GPIO15_config             0x4E   //78 - 0x4E R/W  GPIO 0 configuration bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                            mode (0 -> input, 1 -> output)
#define GPIO15_data               0x4F   //79 - 0x4F R/W  GPIO 0 data bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                   current input value (input mode)
                                         //                                                                                   output value (output mode) 
#define GPIO16_config             0x50   //80 - 0x50 R/W  GPIO 0 configuration bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                            mode (0 -> input, 1 -> output)
#define GPIO16_data               0x51   //81 - 0x51 R/W  GPIO 0 data bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                   current input value (input mode)
                                         //                                                                                   output value (output mode) 
#define GPIO17_config             0x52   //82 - 0x52 R/W  GPIO 0 configuration bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                            mode (0 -> input, 1 -> output)
#define GPIO17_data               0x53   //83 - 0x53 R/W  GPIO 0 data bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                   current input value (input mode)
                                         //                                                                                   output value (output mode) 
#define GPIO18_config             0x54   //84 - 0x54 R/W  GPIO 0 configuration bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                            mode (0 -> input, 1 -> output)
#define GPIO18_data               0x55   //85 - 0x55 R/W  GPIO 0 data bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                   current input value (input mode)
                                         //                                                                                   output value (output mode) 
#define LED                       0x56   //86 - 0x56 R/W  Auxiliary LED bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                     current value
#define SWITCHES                  0x57   //87 - 0x57 R    Auxiliary Switches bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0
                                         //                                                                                  current switches value
#define ADC_CHANNEL0_L            0x58
#define ADC_CHANNEL0_H            0x59
#define ADC_CHANNEL1_L            0x5A
#define ADC_CHANNEL1_H            0x5B
#define ADC_CHANNEL2_L            0x5C
#define ADC_CHANNEL2_H            0x5D
#define ADC_CHANNEL3_L            0x5E
#define ADC_CHANNEL3_H            0x5F
#define ADC_CHANNEL4_L            0x60
#define ADC_CHANNEL4_H            0x61
#define ADC_CHANNEL5_L            0x62
#define ADC_CHANNEL5_H            0x63
#define ADC_CHANNEL6_L            0x64
#define ADC_CHANNEL6_H            0x65
#define ADC_CHANNEL7_L            0x66
#define ADC_CHANNEL7_H            0x67
#define ADC_AVG_CHANNEL0_L        0x68
#define ADC_AVG_CHANNEL0_H        0x69
#define ADC_AVG_CHANNEL1_L        0x6A
#define ADC_AVG_CHANNEL1_H        0x6B
#define ADC_AVG_CHANNEL2_L        0x6C
#define ADC_AVG_CHANNEL2_H        0x6D
#define ADC_AVG_CHANNEL3_L        0x6E
#define ADC_AVG_CHANNEL3_H        0x6F
#define ADC_AVG_CHANNEL4_L        0x70
#define ADC_AVG_CHANNEL4_H        0x71
#define ADC_AVG_CHANNEL5_L        0x72
#define ADC_AVG_CHANNEL5_H        0x73
#define ADC_AVG_CHANNEL6_L        0x74
#define ADC_AVG_CHANNEL6_H        0x75
#define ADC_AVG_CHANNEL7_L        0x76
#define ADC_AVG_CHANNEL7_H        0x77
#define PWM_CONTROL               0x78   //     bit 7   |    bit 6    |    bit 5    |     bit 4   | bit 3  | bit 2   | bit 1  | bit 0
                                         // ch3_enabled   ch2_enabled   ch1_enabled   ch0_enabled            running   stop     start
#define PWM_ENABLE                0x79   //  bit 7  |  bit 6  |  bit 5  |  bit 4  | bit 3  | bit 2  | bit 1  | bit 0
                                         // dis_ch3   dis_ch2   dis_ch1   dis_ch0   en_ch3   en_ch2   en_ch1   en_ch0
#define PWM_FREQ_L                0x7A
#define PWM_FREQ_H                0x7B
#define PWM_CH0_DUTY              0x7C
#define PWM_CH1_DUTY              0x7D
#define PWM_CH2_DUTY              0x7E
#define PWM_CH3_DUTY              0x7F
#define DAC_CONTROL               0x80   //    bit 7    |    bit 6    | bit 5 |  bit 4  | bit 3  | bit 2  | bit 1 | bit 0
                                         // ch1_enabled   ch0_enabled           running                     stop    start
#define DAC_ENABLE                0x81   //    bit 7    |    bit 6    | bit 5 |  bit 4  | bit 3   | bit 2   | bit 1  | bit 0
#define DAC_CH0_VOLTAGE_L         0x82   //                                               dis_ch1   dis_ch0   en_ch1   en_ch0      
#define DAC_CH0_VOLTAGE_H         0x83
#define DAC_CH1_VOLTAGE_L         0x84
#define DAC_CH1_VOLTAGE_H         0x85
#define COMPASS_CONTROL           0x86   //     bit 7   |  bit 6  | bit 5 | bit 4  |  bit 3   |   bit 2   | bit 1 | bit 0
                                         // calibrating   running                    stop cal   start cal   stop    start
#define COMPASS_HEADING_VAL_L     0x87
#define COMPASS_HEADING_VAL_H     0x88
#define COMPASS_AVG_HEADING_VAL_L 0x89
#define COMPASS_AVG_HEADING_VAL_H 0x8A
#define COMPASS_NUM_SAMPLES       0x8B
#define UART_TTL_CONTROL          0x8C   //      bit 7     |  bit 6  |  bit 5  | bit 4  |  bit 3   |   bit 2   | bit 1 | bit 0
                                         // data_available   sending   running                         send      stop    start
#define UART_TTL_TX_DATA          0x8D
#define UART_TTL_RX_DATA          0x8E
#define UART_TTL_BAUDRATE_L       0x8F
#define UART_TTL_BAUDRATE_H       0x90
#define UART_USB_CONTROL          0x91   //      bit 7     |  bit 6  |  bit 5  | bit 4  |  bit 3   |   bit 2   | bit 1 | bit 0
                                         // data_available   sending   running                         send      stop    start
#define UART_USB_TX_DATA          0x92
#define UART_USB_RX_DATA          0x93
#define UART_USB_BAUDRATE_L       0x94
#define UART_USB_BAUDRATE_H       0x95

// block read of all the expansion board data
#define BLOCK_START_ADDRESS             0x96
#define BLOCK_LENGTH                    55

#define GPIO0_data_block                0x96   
#define GPIO1_data_block                0x97   
#define GPIO2_data_block                0x98   
#define GPIO3_data_block                0x99   
#define GPIO4_data_block                0x9A   
#define GPIO5_data_block                0x9B   
#define GPIO6_data_block                0x9C   
#define GPIO7_data_block                0x9D   
#define GPIO8_data_block                0x9E   
#define GPIO9_data_block                0x9F   
#define GPIO10_data_block               0xA0   
#define GPIO11_data_block               0xA1   
#define GPIO12_data_block               0xA2   
#define GPIO13_data_block               0xA3   
#define GPIO14_data_block               0xA4   
#define GPIO15_data_block               0xA5   
#define GPIO16_data_block               0xA6   
#define GPIO17_data_block               0xA7   
#define GPIO18_data_block               0xA8   
#define ADC_CHANNEL0_L_block            0xA9
#define ADC_CHANNEL0_H_block            0xAA
#define ADC_CHANNEL1_L_block            0xAB
#define ADC_CHANNEL1_H_block            0xAC
#define ADC_CHANNEL2_L_block            0xAD
#define ADC_CHANNEL2_H_block            0xAE
#define ADC_CHANNEL3_L_block            0xAF
#define ADC_CHANNEL3_H_block            0xB0
#define ADC_CHANNEL4_L_block            0xB1
#define ADC_CHANNEL4_H_block            0xB2
#define ADC_CHANNEL5_L_block            0xB3
#define ADC_CHANNEL5_H_block            0xB4
#define ADC_CHANNEL6_L_block            0xB5
#define ADC_CHANNEL6_H_block            0xB6
#define ADC_CHANNEL7_L_block            0xB7
#define ADC_CHANNEL7_H_block            0xB8
#define ADC_AVG_CHANNEL0_L_block        0xB9
#define ADC_AVG_CHANNEL0_H_block        0xBA
#define ADC_AVG_CHANNEL1_L_block        0xBB
#define ADC_AVG_CHANNEL1_H_block        0xBC
#define ADC_AVG_CHANNEL2_L_block        0xBD
#define ADC_AVG_CHANNEL2_H_block        0xBE
#define ADC_AVG_CHANNEL3_L_block        0xBF
#define ADC_AVG_CHANNEL3_H_block        0xC0
#define ADC_AVG_CHANNEL4_L_block        0xC1
#define ADC_AVG_CHANNEL4_H_block        0xC2
#define ADC_AVG_CHANNEL5_L_block        0xC3
#define ADC_AVG_CHANNEL5_H_block        0xC4
#define ADC_AVG_CHANNEL6_L_block        0xC5
#define ADC_AVG_CHANNEL6_H_block        0xC6
#define ADC_AVG_CHANNEL7_L_block        0xC7
#define ADC_AVG_CHANNEL7_H_block        0xC8
#define COMPASS_HEADING_VAL_L_block     0xC9
#define COMPASS_HEADING_VAL_H_block     0xCA
#define COMPASS_AVG_HEADING_VAL_L_block 0xCB
#define COMPASS_AVG_HEADING_VAL_H_block 0xCC
#define NUM_PWM_SERVOS                  0xCD

#define RAM_SIZE                  206

#endif
