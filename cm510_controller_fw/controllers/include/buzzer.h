#ifndef _BUZZER_H
#define _BUZZER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

// buzzer interface
typedef enum {NOTE_DO=0,NOTE_RE=1,NOTE_MI=2,NOTE_FA=3,NOTE_SOL=4,NOTE_LA=5,NOTE_SI=6} note_t;
#define     NUM_NOTES                7

/**
 * \brief Function to initialize the buzzed module
 *
 * This function initializes the buzzer module of the CM510 controller. The buzzer 
 * uses a single GPIO pin (bit 5 of port B) which is a PWM output and can be used
 * to generate different frequencies. The PWM module uses the timer 1.
 *
 * This function also initializes the table with the correspondence between the
 * notes and the actual value that has to be loaded to the PWM module.
 *
 * \return this function always returns 0
 */
void init_buzzer(void);
/**
 * \brief Function to start playing a note with the buzzer
 *
 * This function is used to start playing the given note with the buzzer. The
 * note is identified with the macros defined above. Once the buzzer module
 * is started, it will continue playing the same note until the buzzer_stop()
 * or the buzzer_chnage_note() functions are called.
 *
 * \param note identifier of the note to be played.
 * 
 * \return 0 if the function completes successfully and 0xFF otherwise.
 */
void buzzer_start(note_t note,uint16_t on_time_100ms,uint16_t off_time_100ms);
/**
 * \brief Function to change the note is being played
 *
 * This function changes the note that is being played if the buzzer is
 * started. Otherwise it has no effect. The note is identified with one of 
 * the macors defined above.
 * 
 * \param note identifier of the note to be played.
 * 
 * \return 0 if the function completes successfully and 0xFF otherwise.
 */
void buzzer_change_note(note_t note);
void buzzer_change_on_time(uint16_t time_100ms);
void buzzer_change_off_time(uint16_t time_100ms);
/**
 * \brief Function to stop the buzzer module
 *
 * This functions stops playing any note that is being played immediatelly.
 *
 * \return this function always returns 0
 */
void buzzer_stop(void);

uint8_t buzzer_is_playing(void);
uint8_t buzzer_is_playing_alarm(void);

#ifdef __cplusplus
}
#endif

#endif
