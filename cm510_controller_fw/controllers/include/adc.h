#ifndef _ADC_H
#define _ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

// ADC channel identifiers
typedef enum {ADC_VCC=0,ADC_PORT_1=1,ADC_PORT_2=2,ADC_PORT_3=3,ADC_PORT_4=4,ADC_PORT_5=5,ADC_PORT_6=6} adc_t;
#define         NUM_ADC               7

/**
 * \brief Function to initialize the ADC module
 *
 * This function initializes the ADC module of the CM510 controller. This includes 
 * configuring the portA as output to sequentially enable each of the ADC channels
 * before starting the conversion, configuring the operation of the ADC module of
 * the micro and start the first conversion.
 *
 * Since the power supply to the external device is disabled by default, and only 
 * activated when the conversion is going to start, it is necessary to wait for a while
 * before actually starting the conversion to enssure the correct operation of the 
 * device (12 us in this case).
 * 
 * A part from the ADC module itself, this module also uses most of the PORT A of the
 * microcontroller (the 6 most significant bits).
 *
 * \return this function always returns 0
 */
void init_adc(void);
/**
 * \brief Function to get the last conversion 
 *
 * This functions returns the digital codification in raw binary of the analog
 * voltage present at the given channel. In Asynchronous mode, this function
 * returns immediatelly with the last caonverted value of the desired channel, 
 * which can be up to 0.5 ms old in the worst case.
 * 
 * In synchronous mode, the conversion is started when this function is called,
 * and it does not return until the conversion id done, which takes around
 * 125 us to complete.
 * 
 * \param channel_id identifier of the ADC channel to be converted. It must be
 *                   one of the macros defined above.
 * 
 * \return 0 if the functions is successfull and 0xFF in case of any error.
 */
uint16_t get_adc_channel(adc_t channel_id);
uint16_t get_adc_avg_channel(adc_t channel_id);

#ifdef __cplusplus
}
#endif

#endif
