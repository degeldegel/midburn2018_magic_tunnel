#include "stm32f4xx_hal.h"
#include "ws2812b_multi_strip_driver.h"
#include "LED_shows.h"
#include "stdlib.h"

/* user interface db, the user fills up this array according to LED strip id and led number,
 * the driver will push the LED strips according to this db */
extern volatile uint8_t LED_strips[MAX_SUPPORTED_NUM_OF_STRIPS][MAX_SUPPORTED_LEDS_IN_STRIP][NUM_OF_CFG_BYTES_PER_LED];
volatile show_db_t shows[NUM_OF_SHOWS];

/**
  * @brief  snake show, runs colorful snakes through the LED strips.
  * @param  void
  * @retval void
  */
void snake_show(void)
{
    volatile int led_id, strip_id;

    uint32_t cycle_cntr=0;
    double percent_of_rgb[3]={0};
    uint8_t select_new_color = TRUE;
    uint16_t new_led_idx;
    LD2_GPIO_Port->ODR |= LD2_Pin;
    while (1)
    {
        for (strip_id=0; strip_id < MAX_ACTIVE_STRIPS; strip_id++)
        {
            /* go over the strip and move every LED one hop according to the direction */
            /* For regular direction go from last led to the first and move state of led-1 to led_id*/
            /* For reverse direction go from the first to the last and move state of led_id+1 to led_id*/
            if (shows[SHOWS_SNAKE].direction == REGULAR_DIRECTION)
            {
                for (led_id=(MAX_LEDS_IN_STRIP-1); led_id!=0; led_id--)
                {
                    LED_strips[strip_id][led_id][0] = LED_strips[0][led_id-1][0];
                    LED_strips[strip_id][led_id][1] = LED_strips[0][led_id-1][1];
                    LED_strips[strip_id][led_id][2] = LED_strips[0][led_id-1][2];
                }
            }
            else
            {
                for (led_id=0; led_id!=(MAX_LEDS_IN_STRIP-1); led_id++)
                {
                    LED_strips[strip_id][led_id][0] = LED_strips[0][led_id+1][0];
                    LED_strips[strip_id][led_id][1] = LED_strips[0][led_id+1][1];
                    LED_strips[strip_id][led_id][2] = LED_strips[0][led_id+1][2];
                }
            }
            /* choose new random color for every new snake */
            if (select_new_color) {
                uint8_t rgb_idx;
                for (rgb_idx=0; rgb_idx<3; rgb_idx++)
                {
                    percent_of_rgb[rgb_idx] = ((double)(rand()%100))/100;
                }
                select_new_color = FALSE;
            }
            /* update the first led, the only one that wasn't updated till now */
            new_led_idx = (shows[SHOWS_SNAKE].direction == REGULAR_DIRECTION) ? 0 : MAX_LEDS_IN_STRIP-1;
            if (cycle_cntr<8)
            {
                uint8_t power = ((cycle_cntr == 0) || (cycle_cntr == 7)) ? 50 :
                                ((cycle_cntr == 1) || (cycle_cntr == 6)) ? 100 : 200;
                LED_strips[strip_id][new_led_idx][GREEN] = percent_of_rgb[GREEN] * SET_POWER(SHOWS_SNAKE, power);
                LED_strips[strip_id][new_led_idx][RED]   = percent_of_rgb[RED]   * SET_POWER(SHOWS_SNAKE, power);
                LED_strips[strip_id][new_led_idx][BLUE]  = percent_of_rgb[BLUE]  * SET_POWER(SHOWS_SNAKE, power);
            }
            else
            {
                LED_strips[strip_id][new_led_idx][GREEN] = 0;
                LED_strips[strip_id][new_led_idx][RED] = 0;
                LED_strips[strip_id][new_led_idx][BLUE] = 0;
            }
        }
        update_driver_mask();
        drive_port_strips();
        HAL_Delay(30);
        if (cycle_cntr==40)
        {
            cycle_cntr = 0;
            select_new_color = TRUE;
        }
        else
        {
            cycle_cntr++;
        }
    }
}

