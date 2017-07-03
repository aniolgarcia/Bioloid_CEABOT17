#ifndef _ACTION_H
#define _ACTION_H

#ifdef __cplusplus
extern "C" {
#endif

// basic motion interface
uint8_t action_set_page(uint8_t page_id);
void action_start_page(void);
void action_stop_page(void);
uint8_t is_action_running(void);

#ifdef __cplusplus
}
#endif

#endif
