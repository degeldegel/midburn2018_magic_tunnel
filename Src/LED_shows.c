#include "stm32f4xx_hal.h"
#include "ws2812b_multi_strip_driver.h"
#include "LED_shows.h"
#include "stdlib.h"

/* user interface db, the user fills up this array according to LED strip id and led number,
 * the driver will push the LED strips according to this db */
extern volatile uint8_t LED_strips[MAX_SUPPORTED_NUM_OF_STRIPS][MAX_SUPPORTED_LEDS_IN_STRIP][NUM_OF_CFG_BYTES_PER_LED];
volatile show_db_t shows[NUM_OF_SHOWS];

/**
  * @brief  initialize shows database.
  * @param  void
  * @retval void
  */
void init_shows(void)
{
	uint8_t i;
	for (i=0; i<NUM_OF_SHOWS; i++)
	{
		shows[i].status = SHOW_STATUS_DISABLED;
		shows[i].direction = REGULAR_DIRECTION;
		shows[i].max_power = 60;
	}
}

/**
  * @brief  snake show, runs colorful snakes through the LED strips.
  * @param  void
  * @retval void
  */
void snake_show(void)
{
    volatile int led_id, strip_id;

    uint32_t cycle_cntr=0, startup_cycle_cntr=0;
    double percent_of_rgb[3]={0};
    uint8_t select_new_color = TRUE;
    int8_t shut_down_seq_idx;
    uint16_t new_led_idx;

    //light up green led indication
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    //HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

    srand(SysTick->VAL);
    while (shows[SHOWS_SNAKE].status == SHOW_STATUS_RUNNING)
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
                    LED_strips[strip_id][led_id][GREEN] = LED_strips[strip_id][led_id-1][GREEN];
                    LED_strips[strip_id][led_id][RED]   = LED_strips[strip_id][led_id-1][RED];
                    LED_strips[strip_id][led_id][BLUE]  = LED_strips[strip_id][led_id-1][BLUE];
                }
            }
            else
            {
                for (led_id=0; led_id<(MAX_LEDS_IN_STRIP-1); led_id++)
                {
                    LED_strips[strip_id][led_id][GREEN] = LED_strips[strip_id][led_id+1][GREEN];
                    LED_strips[strip_id][led_id][RED]   = LED_strips[strip_id][led_id+1][RED];
                    LED_strips[strip_id][led_id][BLUE]  = LED_strips[strip_id][led_id+1][BLUE];
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
            if (cycle_cntr<SNAKE_SHOW_SNAKE_LENGTH)
            {
                uint8_t power = ((cycle_cntr == 0) || (cycle_cntr == (SNAKE_SHOW_SNAKE_LENGTH-1))) ? 50 :
                                ((cycle_cntr == 1) || (cycle_cntr == (SNAKE_SHOW_SNAKE_LENGTH-2))) ? 100 : 200;
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
        if (SNAKE_SHOW_PERFORM_STRATUP_SEQ && (startup_cycle_cntr < SNAKE_SHOW_STARTUP_SEQ_END_CYCLE))
        {
            /* startup sequence - run the loop without delay and driving the led strips */
            startup_cycle_cntr++;
        }
        else
        {
            /* regular cycle drive leds and wait refresh time */
            drive_LED_strips();
            HAL_Delay(SNAKE_SHOW_REFRESH_TIME);
        }
        if (cycle_cntr==SNAKE_SHOW_CYCLE_LENGTH)
        {
            cycle_cntr = 0;
            select_new_color = TRUE;
        }
        else
        {
            cycle_cntr++;
        }
    }

    /* snake shut down sequence */
    for (shut_down_seq_idx=SNAKE_SHOW_NUM_OF_DIM_STEPS; shut_down_seq_idx>=0; shut_down_seq_idx--)
    {
        for (strip_id=0; strip_id < MAX_ACTIVE_STRIPS; strip_id++)
        {
            /* go over the strip and move every LED one hop according to the direction + perform dimming out */
            /* For regular direction go from last led to the first and move state of led-1 to led_id*/
            /* For reverse direction go from the first to the last and move state of led_id+1 to led_id*/
            double dim_percentage = (double)shut_down_seq_idx / SNAKE_SHOW_NUM_OF_DIM_STEPS;
            if (shows[SHOWS_SNAKE].direction == REGULAR_DIRECTION)
            {
                for (led_id=(MAX_LEDS_IN_STRIP-1); led_id!=0; led_id--)
                {
                    LED_strips[strip_id][led_id][GREEN] = LED_strips[strip_id][led_id-1][GREEN] * dim_percentage;
                    LED_strips[strip_id][led_id][RED]   = LED_strips[strip_id][led_id-1][RED]   * dim_percentage;
                    LED_strips[strip_id][led_id][BLUE]  = LED_strips[strip_id][led_id-1][BLUE]  * dim_percentage;
                }
            }
            else
            {
                for (led_id=0; led_id<(MAX_LEDS_IN_STRIP-1); led_id++)
                {
                    LED_strips[strip_id][led_id][GREEN] = LED_strips[strip_id][led_id+1][GREEN] * dim_percentage;
                    LED_strips[strip_id][led_id][RED]   = LED_strips[strip_id][led_id+1][RED]   * dim_percentage;
                    LED_strips[strip_id][led_id][BLUE]  = LED_strips[strip_id][led_id+1][BLUE]  * dim_percentage;
                }
            }
            new_led_idx = (shows[SHOWS_SNAKE].direction == REGULAR_DIRECTION) ? 0 : MAX_LEDS_IN_STRIP-1;
            LED_strips[strip_id][new_led_idx][GREEN] = 0;
            LED_strips[strip_id][new_led_idx][RED] = 0;
            LED_strips[strip_id][new_led_idx][BLUE] = 0;
        }
        drive_LED_strips();
        HAL_Delay(SNAKE_SHOW_REFRESH_TIME);
    }
    shows[SHOWS_SNAKE].status = SHOW_STATUS_DISABLED;
    //turn off green led indication
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
}

