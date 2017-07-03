#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "motion_cfg.h"
#include "action.h"
#include "motion_pages.h"
#include "motion_manager.h"
#include "serial_console.h"

/**************************************
 * Section             /----\
 *                    /|    |\
 *        /+---------/ |    | \
 *       / |        |  |    |  \
 * -----/  |        |  |    |   \----
 *      PRE  MAIN   PRE MAIN POST PAUSE
 ***************************************/
typedef enum{ PRE_SECTION, MAIN_SECTION, POST_SECTION, PAUSE_SECTION } TSection;
typedef enum{ ZERO_FINISH, NONE_ZERO_FINISH} TStopMode;

#define       SPEED_BASE_SCHEDULE      0x00
#define       TIME_BASE_SCHEDULE       0x0a
#define       INVALID_BIT_MASK         0x4000

// external variables
extern uint8_t manager_num_servos;
extern TServoInfo manager_servos[MANAGER_MAX_NUM_SERVOS];

// private variables
uint8_t action_finished;
uint8_t action_stop;
uint8_t action_next_index;
uint8_t action_current_index;
uint8_t action_step_count;

TActionPage action_next_page;
TActionPage action_current_page;

// motion variables
uint16_t wpStartAngle1024[PAGE_MAX_NUM_SERVOS]; 
uint16_t wpTargetAngle1024[PAGE_MAX_NUM_SERVOS];
int16_t ipMovingAngle1024[PAGE_MAX_NUM_SERVOS];
int16_t ipMainAngle1024[PAGE_MAX_NUM_SERVOS]; 
int16_t ipAccelAngle1024[PAGE_MAX_NUM_SERVOS];
int16_t ipMainSpeed1024[PAGE_MAX_NUM_SERVOS];
int16_t ipLastOutSpeed1024[PAGE_MAX_NUM_SERVOS];
int16_t ipGoalSpeed1024[PAGE_MAX_NUM_SERVOS]; 
uint8_t bpFinishType[PAGE_MAX_NUM_SERVOS]; 
uint16_t wUnitTimeCount;
uint16_t wUnitTimeNum;
uint16_t wPauseTime;
uint16_t wUnitTimeTotalNum;
uint16_t wAccelStep;
uint8_t bSection;
uint8_t bPlayRepeatCount;
uint16_t step_angles[PAGE_MAX_NUM_SERVOS];
uint8_t action_is_running;

// public functions
void action_init(void)
{
  action_finished=0x00;
  action_stop=0x00;
  action_next_index=0xFF;
  action_current_index=0xFF;
  action_step_count=0xFF;
  action_is_running=0x00;
}

uint8_t action_set_page(uint8_t page_id)
{
  uint8_t error;

  error=load_page_info(page_id,&action_current_page);
  if(error==0)
    action_current_index=page_id;
  return error;
}

void action_start_page(void)
{
  uint8_t i;

  action_finished=0x00;// reset the finished flag
  wUnitTimeCount = 0;
  wUnitTimeNum = 0;
  wPauseTime = 0;
  bSection = PAUSE_SECTION;
  action_step_count = 0;
  bPlayRepeatCount = action_current_page.header.repeat;
  action_next_index = 0x00;

  for(i=0;i<MANAGER_MAX_NUM_SERVOS;i++)
  {
    if(manager_servos[i].module==MM_ACTION)
    {
      wpTargetAngle1024[i] = manager_servos[i].current_value;
      ipLastOutSpeed1024[i] = 0;
      ipMovingAngle1024[i] = 0;
      ipGoalSpeed1024[i] = 0;
    }
  }
  action_is_running=0x01;
}

void action_stop_page(void)
{
  action_stop=0x01;
}

uint8_t is_action_running(void)
{
  uint8_t running=1; 

  running = action_is_running;

  return running;
}

// motion manager interface
void action_process(void)
{
  //////////////////// Áö¿ª º¯¼ö
  uint8_t bID;
  uint64_t ulTotalTime256T;
  uint64_t ulPreSectionTime256T;
  uint64_t ulMainTime256T;
  int64_t lStartSpeed1024_PreTime_256T;
  int64_t lMovingAngle_Speed1024Scale_256T_2T;
  int64_t lDivider1,lDivider2;
  uint16_t wMaxSpeed256;
  uint16_t wPrevTargetAngle; // Start position
  uint16_t wCurrentTargetAngle; // Target position
  uint16_t wNextTargetAngle; // Next target position
  uint8_t bDirectionChanged;
  int16_t iSpeedN;

  if(action_is_running == 0x00 )
    return;

  if( wUnitTimeCount >= wUnitTimeNum ) // ÇöÀç SectionÀÌ ¿Ï·áµÇ¾ú´Ù¸é
  {
    wUnitTimeCount = 0;

    for(bID=0;bID<MANAGER_MAX_NUM_SERVOS;bID++)
    {
      if(manager_servos[bID].module==MM_ACTION)
      {
        wpStartAngle1024[bID] = manager_servos[bID].current_value;
        ipLastOutSpeed1024[bID] = ipGoalSpeed1024[bID];
      }
    }

    // Section ¾÷µ¥ÀÌÆ® ( PRE -> MAIN -> POST -> (PAUSE or PRE) ... )
    if( bSection == PRE_SECTION )
    {
      // MAIN Section ÁØºñ
      bSection = MAIN_SECTION;
      wUnitTimeNum =  wUnitTimeTotalNum - (wAccelStep << 1);
      for(bID=0;bID<MANAGER_MAX_NUM_SERVOS;bID++)
      {
        if(manager_servos[bID].module==MM_ACTION)
        {
          if( bpFinishType[bID] == NONE_ZERO_FINISH )
          {
            if( (wUnitTimeTotalNum - wAccelStep) == 0 ) // µî¼Ó ±¸°£ÀÌ ÀüÇô ¾ø´Ù¸é
              ipMainAngle1024[bID] = 0;
            else
              ipMainAngle1024[bID] = (short)((((long)(ipMovingAngle1024[bID] - ipAccelAngle1024[bID])) * wUnitTimeNum) / (wUnitTimeTotalNum - wAccelStep));
          }
          else // ZERO_FINISH
            ipMainAngle1024[bID] = ipMovingAngle1024[bID] - ipAccelAngle1024[bID] - (short int)((((long)ipMainSpeed1024[bID] * wAccelStep * 12) / 5) >> 8);
        }
      }
    }
    else if( bSection == MAIN_SECTION )
    {
      // POST Section ÁØºñ
      bSection = POST_SECTION;
      wUnitTimeNum = wAccelStep;

      for(bID=0;bID<MANAGER_MAX_NUM_SERVOS;bID++)
        if(manager_servos[bID].module==MM_ACTION)
          ipMainAngle1024[bID] = ipMovingAngle1024[bID] - ipMainAngle1024[bID] - ipAccelAngle1024[bID];
    }
    else if( bSection == POST_SECTION )
    {
      // Pause time À¯¹«¿¡µû¶ó ´Þ¶óÁü
      if( wPauseTime )
      {
        bSection = PAUSE_SECTION;
        wUnitTimeNum = wPauseTime;
      }
      else
      {
        bSection = PRE_SECTION;
      }
    }
    else if( bSection == PAUSE_SECTION )
    {
      // PRE Section ÁØºñ
      bSection = PRE_SECTION;

      for(bID=0;bID<MANAGER_MAX_NUM_SERVOS;bID++)
        if(manager_servos[bID].module==MM_ACTION)
          ipLastOutSpeed1024[bID] = 0;
    }

    // PRE Section½Ã¿¡ ¸ðµç ÁØºñ¸¦ ÇÑ´Ù.
    if( bSection == PRE_SECTION )
    {
      if( action_finished == 0x01 ) // ¸ð¼ÇÀÌ ³¡³µ´Ù¸é
      {
        action_is_running=0x00;
        action_stop = 0x00;
        return;
      }
      action_step_count++;

      if( action_step_count > action_current_page.header.stepnum ) // ÇöÀç ÆäÀÌÁö Àç»ýÀÌ ³¡³µ´Ù¸é
      {
        action_current_page = action_next_page;
        
        if( action_current_index != action_next_index )
          bPlayRepeatCount = action_current_page.header.repeat;
        action_step_count = 1;
        action_current_index = action_next_index;
      }
      if( action_step_count == action_current_page.header.stepnum ) // ¸¶Áö¸· ½ºÅÜÀÌ¶ó¸é
      {
        // ´ÙÀ½ ÆäÀÌÁö ·Îµù
        if( action_stop == 0x01 ) // ¸ð¼Ç Á¤Áö ¸í·ÉÀÌ ÀÖ´Ù¸é
          action_next_index = action_current_page.header.exit; // ´ÙÀ½ ÆäÀÌÁö´Â Exit ÆäÀÌÁö·Î
        else
        {
          bPlayRepeatCount--;
          if( bPlayRepeatCount > 0 ) // ¹Ýº¹ È½¼ö°¡ ³²¾Ò´Ù¸é
            action_next_index = action_current_index; // ´ÙÀ½ ÆäÀÌÁö´Â ÇöÀç ÆäÀÌÁö·Î
          else // ¹Ýº¹À» ´ÙÇß´Ù¸é
            action_next_index = action_current_page.header.next; // ´ÙÀ½ ÆäÀÌÁö´Â Next ÆäÀÌÁö·Î
        }

        if( action_next_index == 0x00 ) // Àç»ýÇÒ ´ÙÀ½ ÆäÀÌÁö°¡ ¾ø´Ù¸é ÇöÀç ½ºÅÜ±îÁöÇÏ°í Á¾·á
          action_finished=0x01;
        else
        {
          // ´ÙÀ½ÆäÀÌÁö ·Îµù(°°À¸¸é ¸Þ¸ð¸® º¹»ç, ´Ù¸£¸é ÆÄÀÏ ÀÐ±â)
          if( action_current_index != action_next_index )
            load_page_info(action_next_index,&action_next_page);
          else
            action_next_page = action_current_page;

          // Àç»ýÇÒ Á¤º¸°¡ ¾ø´Ù¸é ÇöÀç ½ºÅÜ±îÁöÇÏ°í Á¾·á
          if( action_next_page.header.repeat == 0 || action_next_page.header.stepnum == 0 )
            action_finished = 0x01;
        }
      }
      //////// Step ÆÄ¶ó¹ÌÅÍ °è»ê
      //wPauseTime = (((unsigned short)action_current_page.steps[action_step_count-1].pause) << 5) / (action_current_page.header.speed);
      wPauseTime = ((unsigned short)action_current_page.steps[action_step_count-1].pause * (unsigned short)action_current_page.header.speed) >> 5;
      wMaxSpeed256 = ((unsigned short)action_current_page.steps[action_step_count-1].time * (unsigned short)action_current_page.header.speed) >> 5;
      // wMaxSpeed256 = (((unsigned short)action_current_page.steps[action_step_count-1].time) << 5) / action_current_page.header.speed;
      if( wMaxSpeed256 == 0 )
        wMaxSpeed256 = 1;
        //  wMaxAngle1024 = 0;

      ////////// Jointº° ÆÄ¶ó¹ÌÅÍ °è»ê
      for(bID=0;bID<MANAGER_MAX_NUM_SERVOS;bID++)
      {
        if(manager_servos[bID].module==MM_ACTION)
        {
          // ÀÌÀü, ÇöÀç, ¹Ì·¡¸¦ ¹ÙÅÁÀ¸·Î ±ËÀûÀ» °è»ê
          ipAccelAngle1024[bID] = 0;

          // Find current target angle
          if( action_current_page.steps[action_step_count-1].position[bID] & INVALID_BIT_MASK )
            wCurrentTargetAngle = wpTargetAngle1024[bID];
          else
            wCurrentTargetAngle = action_current_page.steps[action_step_count-1].position[bID];

          // Update start, prev_target, curr_target
          wpStartAngle1024[bID] = wpTargetAngle1024[bID];
          wPrevTargetAngle = wpTargetAngle1024[bID];
          wpTargetAngle1024[bID] = wCurrentTargetAngle;

          // Find Moving offset
          ipMovingAngle1024[bID] = (int)(wpTargetAngle1024[bID] - wpStartAngle1024[bID]);

          // Find Next target angle
          if( action_step_count == action_current_page.header.stepnum ) // ÇöÀç ½ºÅÜÀÌ ¸¶Áö¸·ÀÌ¶ó¸é
          {
            if( action_finished == 0x01 ) // ³¡³¯ ¿¹Á¤ÀÌ¶ó¸é
              wNextTargetAngle = wCurrentTargetAngle;
            else
            {
              if( action_next_page.steps[0].position[bID] & INVALID_BIT_MASK )
                wNextTargetAngle = wCurrentTargetAngle;
              else
                wNextTargetAngle = action_next_page.steps[0].position[bID];
            }
          }
          else
          {
            if( action_current_page.steps[action_step_count].position[bID] & INVALID_BIT_MASK )
              wNextTargetAngle = wCurrentTargetAngle;
            else
              wNextTargetAngle = action_current_page.steps[action_step_count].position[bID];
          }

          // Find direction change
          if( ((wPrevTargetAngle < wCurrentTargetAngle) && (wCurrentTargetAngle < wNextTargetAngle))
              || ((wPrevTargetAngle > wCurrentTargetAngle) && (wCurrentTargetAngle > wNextTargetAngle)) )
          {
            // °è¼Ó Áõ°¡ÇÏ°Å³ª °¨¼ÒÇÏ°í, È¤Àº °°´Ù¸é(Áï, ºÒ¿¬¼Ó Á¡ÀÌ ¾ø´Ù¸é)
            bDirectionChanged = 0;
          }
          else
          {
            bDirectionChanged = 1;
          }

          // Find finish type
          if( bDirectionChanged || wPauseTime || action_finished == 0x01 )
          {
            bpFinishType[bID] = ZERO_FINISH;
          }
          else
          {
            bpFinishType[bID] = NONE_ZERO_FINISH;
          }
        }
      }
      //½Ã°£À» °è»êÇØ¼­ ´Ù½Ã 7.8msec·Î ³ª´©´Ù(<<7)-±×½Ã°£µ¿¾È¿¡ 7.8msec°¡ ¸î°³µé¾î°¡´ÂÁö °è»êÇÑ °Í
      //´ÜÀ§ º¯È¯µÚ¿¡ °¢/¼Óµµ¸¦ ±¸ÇÏ°í(½Ã°£)±× ½Ã°£¿¡ ´Ù½Ã 7.8msec°¡ ¸î°³µé¾î°¡ÀÖ´ÂÁö °è»ê
      //´ÜÀ§ º¯È¯ ---  °¢ :1024°è->300µµ°è,  ¼Óµµ: 256°è ->720°è
      //wUnitTimeNum = ((wMaxAngle1024*300/1024) /(wMaxSpeed256 * 720/256)) /7.8msec;
      //             = ((128*wMaxAngle1024*300/1024) /(wMaxSpeed256 * 720/256)) ;    (/7.8msec == *128)
      //             = (wMaxAngle1024*40) /(wMaxSpeed256 *3);
      wUnitTimeTotalNum  = wMaxSpeed256; //TIME BASE 051025

      wAccelStep = action_current_page.header.accel;
      if( wUnitTimeTotalNum <= (wAccelStep << 1) )
      {
        if( wUnitTimeTotalNum == 0 )
          wAccelStep = 0;
        else
        {
          wAccelStep = (wUnitTimeTotalNum - 1) >> 1;
          if( wAccelStep == 0 )
            wUnitTimeTotalNum = 0; //¿òÁ÷ÀÌ·Á¸é Àû¾îµµ °¡¼Ó,µî¼ÓÀÌ ÇÑ ½ºÅÜÀÌ»ó¾¿Àº Á¸ÀçÇØ¾ß
        }
      }

      ulTotalTime256T = ((unsigned long)wUnitTimeTotalNum) << 1;// /128 * 256
      ulPreSectionTime256T = ((unsigned long)wAccelStep) << 1;// /128 * 256
      ulMainTime256T = ulTotalTime256T - ulPreSectionTime256T;
      lDivider1 = ulPreSectionTime256T + (ulMainTime256T << 1);
      lDivider2 = (ulMainTime256T << 1);
      if(lDivider1 == 0)
        lDivider1 = 1;

      if(lDivider2 == 0)
        lDivider2 = 1;
      for(bID=0;bID<MANAGER_MAX_NUM_SERVOS;bID++)
      {
        if(manager_servos[bID].module==MM_ACTION)
        {
          lStartSpeed1024_PreTime_256T = (long)ipLastOutSpeed1024[bID] * ulPreSectionTime256T; //  *300/1024 * 1024/720 * 256 * 2
          lMovingAngle_Speed1024Scale_256T_2T = (((long)ipMovingAngle1024[bID]) * 2560L) / 12;

          if( bpFinishType[bID] == ZERO_FINISH )
            ipMainSpeed1024[bID] = (short int)((lMovingAngle_Speed1024Scale_256T_2T - lStartSpeed1024_PreTime_256T) / lDivider2);
          else
            ipMainSpeed1024[bID] = (short int)((lMovingAngle_Speed1024Scale_256T_2T - lStartSpeed1024_PreTime_256T) / lDivider1);

          if( ipMainSpeed1024[bID] > 1023 )
            ipMainSpeed1024[bID] = 1023;
          if( ipMainSpeed1024[bID] < -1023 )
            ipMainSpeed1024[bID] = -1023;
        }
      }
      wUnitTimeNum = wAccelStep; //PreSection
    }
  }
  if( wUnitTimeCount < wUnitTimeNum ) // ÇöÀç ÁøÇàÁßÀÌ¶ó¸é
  {
    wUnitTimeCount++;
    if( bSection == PAUSE_SECTION )
    {
    }
    else
    {
      for(bID=0;bID<MANAGER_MAX_NUM_SERVOS;bID++)
      {
        if(manager_servos[bID].module==MM_ACTION)
        {
          if( ipMovingAngle1024[bID] == 0 )
            manager_servos[bID].current_value=wpStartAngle1024[bID];
          else
          {
            if( bSection == PRE_SECTION )
            {
              iSpeedN = (short)(((long)(ipMainSpeed1024[bID] - ipLastOutSpeed1024[bID]) * wUnitTimeCount) / wUnitTimeNum);
              ipGoalSpeed1024[bID] = ipLastOutSpeed1024[bID] + iSpeedN;
              ipAccelAngle1024[bID] =  (short)((((long)(ipLastOutSpeed1024[bID] + (iSpeedN >> 1)) * wUnitTimeCount * 144) / 15) >> 9);
              manager_servos[bID].current_value=wpStartAngle1024[bID] + ipAccelAngle1024[bID];
            }
            else if( bSection == MAIN_SECTION )
            {
              manager_servos[bID].current_value=wpStartAngle1024[bID] + (short int)(((long)(ipMainAngle1024[bID])*wUnitTimeCount) / wUnitTimeNum);
              ipGoalSpeed1024[bID] = ipMainSpeed1024[bID];
            }
            else // POST_SECTION
            {
              if( wUnitTimeCount == (wUnitTimeNum-1) )
              {
                // ½ºÅÜ ¸¶Áö¸· ¿ÀÂ÷¸¦ ÁÙÀÌ±âÀ§ÇØ ±×³É ¸ñÇ¥ À§Ä¡ °ªÀ» »ç¿ë
                manager_servos[bID].current_value=wpTargetAngle1024[bID];
              }
              else
              {
                if( bpFinishType[bID] == ZERO_FINISH )
                {
                  iSpeedN = (short int)(((long)(0 - ipLastOutSpeed1024[bID]) * wUnitTimeCount) / wUnitTimeNum);
                  ipGoalSpeed1024[bID] = ipLastOutSpeed1024[bID] + iSpeedN;
                  manager_servos[bID].current_value=wpStartAngle1024[bID] +  (short)((((long)(ipLastOutSpeed1024[bID] + (iSpeedN>>1)) * wUnitTimeCount * 144) / 15) >> 9);
                }
                else // NONE_ZERO_FINISH
                {
                  // MAIN Section°ú µ¿ÀÏÇÏ°Ô ÀÛµ¿-µ¿ÀÏ
                  // step¿¡¼­ ¾î¶²¼­º¸´Â °¡°í ¾î¶² ¼­º¸´Â ¼­¾ßÇÏ´Â »óÈ²ÀÌ ¹ß»ýÇÒ ¼ö ÀÖÀ¸¹Ç·Î ÀÌ·¸°Ô ÇÒ ¼ö¹Û¿¡ ¾øÀ½
                  manager_servos[bID].current_value=wpStartAngle1024[bID] + (short int)(((long)(ipMainAngle1024[bID]) * wUnitTimeCount) / wUnitTimeNum);
                  ipGoalSpeed1024[bID] = ipMainSpeed1024[bID];
                }
              }
            }
          }
          //set slopes
          manager_servos[bID].slope=action_current_page.header.slope[bID];
        }
      }
    }
  }
}
