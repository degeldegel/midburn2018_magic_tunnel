#include "ws2812b_multi_strip_driver.h"
#include "LED_shows.h"
#include "stdio.h"
#include "stdlib.h"

/* user interface db, the user fills up this array according to LED strip id and led number,
 * the driver will push the LED strips according to this db */
uint8_t  LED_strips[MAX_SUPPORTED_NUM_OF_STRIPS][MAX_SUPPORTED_LEDS_IN_STRIP][NUM_OF_CFG_BYTES_PER_LED] = {0};
systick_emul_t SysTick_s;
systick_emul_t *SysTick;
unsigned led_driver_cntr = 0;
extern volatile show_db_t shows[NUM_OF_SHOWS];

void stop_all_shows()
{
    int i;
    for (i=0; i<NUM_OF_SHOWS; i++)
    {
        if (shows[i].status == SHOW_STATUS_RUNNING)
        {
            shows[i].status = SHOW_STATUS_STOP;
        }
    }
}
/**
  * @brief  main function that drives the strips, it should be called after the LED_strips db is updated with the new output.
  *         all strips are updated on parallel.
  * @param  void
  * @retval void
  */
void drive_LED_strips(void)
{
    #define MAX_LED_DRIVER_CNTR (500)
    FILE *pfile;
    unsigned strip_id, led_id;

    /*START of code writing to file, all the strip info + time */
    SysTick->VAL += 9;
    if (led_driver_cntr==0)
    {
        pfile = fopen("LED_Emul_output.txt", "w");
    }
    else
    {
        pfile = fopen("LED_Emul_output.txt", "a");
    }
    if (pfile==NULL)
    {
        printf("error while openning file LED_Emul_output! exiting!");
        exit(1);
    }

    /* pushing strip data to file */
    fprintf(pfile, "TIME: %u\n", SysTick->VAL);
    for (strip_id=0; strip_id < MAX_ACTIVE_STRIPS; strip_id++)
    {
         for (led_id=0; led_id<MAX_LEDS_IN_STRIP; led_id++)
         {
             fprintf(pfile, "%u %u %u,", LED_strips[strip_id][led_id][GREEN], LED_strips[strip_id][led_id][RED], LED_strips[strip_id][led_id][BLUE]);
         }
         fprintf(pfile, "\n");
    }
    //fprintf(pfile, "\n");
    fclose(pfile);
    /* END of code writing to file */

    if (led_driver_cntr < MAX_LED_DRIVER_CNTR)
    {
        led_driver_cntr++;
    }
    else
    {
        stop_all_shows();
        //printf("Emulator finished!");
    }
}

/* dummy function for emulator */
void HAL_GPIO_WritePin(int port, int pin, int operation)
{
    /* I'm a dummy function :( */
}

/* emulator function of delay */
void HAL_Delay(uint32_t delay)
{
    SysTick->VAL += delay;
}
