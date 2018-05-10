#include "stm32f4xx_hal.h"
#include "scheduler.h"
#include "LED_shows.h"
#include "stdlib.h"

extern volatile show_db_t shows[NUM_OF_SHOWS];
extern TIM_HandleTypeDef htim3;

volatile scheduler_db_t schdlr_db;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &htim3)
  {
      uint16_t show_time = scheduler_timer_handler();
      __HAL_TIM_SET_AUTORELOAD(&htim3, show_time);
  }
}

uint16_t scheduler_timer_handler(void)
{

    uint8_t curr_showID = schdlr_db.show[schdlr_db.current_show].showId;
    uint8_t found_new_show = FALSE;
    uint8_t all_shows_finished = TRUE;
    uint8_t new_time_sec = 10;
    uint8_t i;

    /* stop current running show */
    shows[curr_showID].status = SHOW_STATUS_STOP;

    /* check if not all shows run already */
    for (i=0; i<schdlr_db.num_of_shows; i++)
    {
        if (schdlr_db.show[i].played == SCHDLR_SHOW_DIDNT_PLAY)
        {
            all_shows_finished = FALSE;
        }
    }
    /* if all shows run, zero the database */
    if (all_shows_finished)
    {
        for (i=0; i<schdlr_db.num_of_shows; i++)
        {
            schdlr_db.show[i].played = SCHDLR_SHOW_DIDNT_PLAY;
        }
    }
    /* choose new show */
    srand(SysTick->VAL);
    while (!found_new_show)
    { /* loop till find new random show that wasn't run yet */
        uint8_t rand_new_show_idx = rand()%schdlr_db.num_of_shows;
        if (schdlr_db.show[rand_new_show_idx].played == SCHDLR_SHOW_DIDNT_PLAY)
        {
            schdlr_db.current_show = rand_new_show_idx;
            schdlr_db.show[rand_new_show_idx].played = SCHDLR_SHOW_PLAYED;
            found_new_show = TRUE;
        }
    }
    /* activate next show */
    curr_showID = schdlr_db.show[schdlr_db.current_show].showId;
    shows[curr_showID].status = SHOW_STATUS_START;
    /* choose new random run time */
    new_time_sec = (rand()%(schdlr_db.maximum_time_run - schdlr_db.minimum_time_run)) + schdlr_db.minimum_time_run;
    /* return new time in ticks of timer3 */
    return new_time_sec*500;
}

void init_scheduler(void)
{
    uint8_t show_idx;
    schdlr_db.current_show = 0;
    schdlr_db.num_of_shows = 4;
    schdlr_db.maximum_time_run = 40;
    schdlr_db.minimum_time_run = 20;
    for (show_idx=0; show_idx<schdlr_db.num_of_shows; show_idx++)
    {
        schdlr_db.show[show_idx].showId = show_idx;
        schdlr_db.show[show_idx].played = SCHDLR_SHOW_DIDNT_PLAY;
    }
    HAL_TIM_Base_Start_IT(&htim3);
}
