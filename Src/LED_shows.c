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
    uint8_t current_color[NUM_OF_CFG_BYTES_PER_LED];
    uint8_t select_new_color = TRUE;
    int8_t shut_down_seq_idx;
    uint16_t new_led_idx;

    //light up green led indication
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    //HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

    // Calculate the new led index based on the direction
    new_led_idx = (shows[SHOWS_SNAKE].direction == REGULAR_DIRECTION) ? 0 : MAX_LEDS_IN_STRIP - 1;

    srand(SysTick->VAL);
    while (shows[SHOWS_SNAKE].status == SHOW_STATUS_RUNNING)
    {
        /* Choose new random color for every new snake */
        if (select_new_color) {
            aux_LED_randomize(&current_color);
            select_new_color = FALSE;
        }

        /* Update frame */
        for (strip_id = 0; strip_id < MAX_ACTIVE_STRIPS; strip_id++)
        {
            /* Push all colors */
            aux_STRIPE_push(&LED_strips[strip_id], shows[SHOWS_SNAKE].direction);
            
            /* Update the first led, the only one that wasn't updated till now */
            if (cycle_cntr < SNAKE_SHOW_SNAKE_LENGTH)
            {
                double power_factor = ((cycle_cntr == 0) || (cycle_cntr == (SNAKE_SHOW_SNAKE_LENGTH-1))) ? 0.2 :
                                      ((cycle_cntr == 1) || (cycle_cntr == (SNAKE_SHOW_SNAKE_LENGTH-2))) ? 0.4 : 0.8;
                double power = GET_POWER(SHOWS_SNAKE, power_factor);
                
                aux_LED_copy(&current_color, &LED_strips[strip_id][new_led_idx], power);
            }
            else
            {
                aux_LED_clear(&LED_strips[strip_id][new_led_idx]);
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
        
        if (cycle_cntr == SNAKE_SHOW_CYCLE_LENGTH)
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
    for (shut_down_seq_idx = SNAKE_SHOW_NUM_OF_DIM_STEPS; shut_down_seq_idx >= 0; shut_down_seq_idx--)
    {
        for (strip_id = 0; strip_id < MAX_ACTIVE_STRIPS; strip_id++)
        {
            /* go over the strip and move every LED one hop according to the direction + perform dimming out */
            double dim_percentage = (double)shut_down_seq_idx / SNAKE_SHOW_NUM_OF_DIM_STEPS;
            aux_STRIPE_push(&LED_strips[strip_id], shows[SHOWS_SNAKE].direction, dim_percentage);

            aux_LED_clear(&LED_strips[strip_id][new_led_idx]);
        }
        drive_LED_strips();
        HAL_Delay(SNAKE_SHOW_REFRESH_TIME);
    }
    shows[SHOWS_SNAKE].status = SHOW_STATUS_DISABLED;
    //turn off green led indication
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
}

/*************************
  *  Auxiliary Functions *
  ************************/

/**
  * @brief  Clear the given led
  * @retval void
  */
void aux_LED_randomize(uint8_t *LED_target[NUM_OF_CFG_BYTES_PER_LED])
{
    (*LED_target)[RED]   = ((double)(rand()%100))/100 * 255;
    (*LED_target)[GREEN] = ((double)(rand()%100))/100 * 255;
    (*LED_target)[BLUE]  = ((double)(rand()%100))/100 * 255;
}

/**
  * @brief  Clear the given led
  * @retval void
  */
void aux_LED_clear(uint8_t *LED_target[NUM_OF_CFG_BYTES_PER_LED])
{
    (*LED_target)[RED]   = 0;
    (*LED_target)[GREEN] = 0;
    (*LED_target)[BLUE]  = 0;
}

/**
  * @brief  Copies the color from LED_from to LED_to
  * @retval void
  */
void aux_LED_copy(uint8_t *LED_from[NUM_OF_CFG_BYTES_PER_LED], uint8_t *LED_to[NUM_OF_CFG_BYTES_PER_LED])
{
    aux_LED_copy(LED_from, LED_to, 1);
}

void aux_LED_copy(uint8_t *LED_from[NUM_OF_CFG_BYTES_PER_LED], uint8_t *LED_to[NUM_OF_CFG_BYTES_PER_LED], double dim_percentage)
{
    (*LED_to)[RED]   = (*LED_from)[RED] * dim_percentage;
    (*LED_to)[GREEN] = (*LED_from)[GREEN] * dim_percentage;
    (*LED_to)[BLUE]  = (*LED_from)[BLUE] * dim_percentage;
}

/**
  * @brief  Go over the strip and move every LED one hop according to the direction 
  *         For regular direction go from last led to the first and move state of led-1 to led_id
  *         For reverse direction go from the first to the last and move state of led_id+1 to led_id
  * @retval void
  */
void aux_STRIPE_push(uint8_t *LED_strip[MAX_SUPPORTED_LEDS_IN_STRIP][NUM_OF_CFG_BYTES_PER_LED], uint8_t direction)
{
    aux_STRIPE_push(LED_strip, direction, 1);
}

void aux_STRIPE_push(uint8_t *LED_strip[MAX_SUPPORTED_LEDS_IN_STRIP][NUM_OF_CFG_BYTES_PER_LED], uint8_t direction, double dim_percentage)
{
    volatile int led_id;

    if (direction == REGULAR_DIRECTION)
    {
        for (led_id = (MAX_LEDS_IN_STRIP - 1); led_id > 0; led_id--)
        {
            aux_LED_copy(&LED_strip[led_id - 1], &LED_strip[led_id], dim_percentage);
        }
    }
    else
    {
        for (led_id = 0; led_id < (MAX_LEDS_IN_STRIP - 1); led_id++)
        {
            aux_LED_copy(&LED_strip[led_id + 1], &LED_strip[led_id], dim_percentage);
        }
    }
}
