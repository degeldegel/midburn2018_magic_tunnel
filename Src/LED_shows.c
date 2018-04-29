#include "stm32f4xx_hal.h"
#include "ws2812b_multi_strip_driver.h"
#include "LED_shows.h"
#include "stdlib.h"

/* user interface db, the user fills up this array according to LED strip id and led number,
 * the driver will push the LED strips according to this db */
extern volatile uint8_t LED_strips[MAX_SUPPORTED_NUM_OF_STRIPS][MAX_SUPPORTED_LEDS_IN_STRIP][NUM_OF_CFG_BYTES_PER_LED];
volatile show_db_t shows[NUM_OF_SHOWS];


volatile double percent_of_rgb[3]={0};
volatile uint32_t cycle_cntr=0, startup_cycle_cntr=0;
volatile uint8_t select_new_color = TRUE;
//volatile uint8_t started_coloring = FALSE;

int teddy_bear[MAX_SUPPORTED_NUM_OF_STRIPS][TEDDY_BEAR_SNAKE_LENGTH] =
{
		{0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1},
		{0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,2,2,0,0,0,0,0,2,2,0,1,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,2,2,2,0,0,0,0,1,1,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,2,0,0,0,0,0,2,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,2,2,2,2,2,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1},
		{0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1},
		{0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,2,0,0,0,0,0,1,0,0,0,1},
		{0,0,0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0,2,0,0,0,0,0,0,1,0,0,0,1,1},
		{0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,2,0,0,0,0,0,0,1,0,1,0,0,0,1},
		{0,0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,0,0,2,0,0,0,0,0,0,1,0,0,1,0,0,0,1,0,0},
		{0,0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0,0,2,0,0,0,0,0,1,1,1,0,0,1,1,1},
		{0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,1,0,0,0,2,0,0,0,1,1,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0},
		{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0}
};



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
    //volatile int led_id, strip_id;

    //uint32_t cycle_cntr=0, startup_cycle_cntr=0;
    //double percent_of_rgb[3]={0};
    //uint8_t select_new_color = TRUE;
    //int8_t shut_down_seq_idx;
    //uint16_t new_led_idx;

    //light up green led indication
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    //HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

    srand(SysTick->VAL);

    while (shows[SHOWS_SNAKE].status == SHOW_STATUS_RUNNING)
    {
    	Led_Show_Options(SHOWS_TEDDY_BEAR);
    }

    Led_End_Options(SHOWS_TEDDY_BEAR);

    shows[SHOWS_SNAKE].status = SHOW_STATUS_DISABLED;
    //turn off green led indication
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
}

void Led_Show_Options(show_id_e option)
{
	switch (option)
	{
		case(SHOWS_SNAKE):
			DanielShow();
			break;
		case(SHOWS_TEDDY_BEAR):
			TakoShow();
			break;
		case(SHOWS_RESERVED1):
		case(SHOWS_RESERVED2):
		default:
			break;
	}
}

void Led_End_Options(show_id_e option)
{
	switch (option)
	{
		case(SHOWS_SNAKE):
			DanielEnd();
			break;
		case(SHOWS_TEDDY_BEAR):
			TakoEnd();
			break;
		case(SHOWS_RESERVED1):
		case(SHOWS_RESERVED2):
		default:
			break;
	}
}


void TakoShow(void)
{
	for (int strip_id=0; strip_id < MAX_ACTIVE_STRIPS; strip_id++)
	{
		// go over the strip and move every LED one hop according to the direction
		// For regular direction go from last led to the first and move state of led-1 to led_id
		// For reverse direction go from the first to the last and move state of led_id+1 to led_id
		if (shows[SHOWS_SNAKE].direction == REGULAR_DIRECTION)
		{
			for (int led_id=(MAX_LEDS_IN_STRIP-1); led_id!=0; led_id--)
			{
				LED_strips[strip_id][led_id][GREEN] = LED_strips[strip_id][led_id-1][GREEN];
				LED_strips[strip_id][led_id][RED]   = LED_strips[strip_id][led_id-1][RED];
				LED_strips[strip_id][led_id][BLUE]  = LED_strips[strip_id][led_id-1][BLUE];
			}
		}
		else
		{
			for (int led_id=0; led_id<(MAX_LEDS_IN_STRIP-1); led_id++)
			{
				LED_strips[strip_id][led_id][GREEN] = LED_strips[strip_id][led_id+1][GREEN];
				LED_strips[strip_id][led_id][RED]   = LED_strips[strip_id][led_id+1][RED];
				LED_strips[strip_id][led_id][BLUE]  = LED_strips[strip_id][led_id+1][BLUE];
			}
		}

//		// choose new random color for every new snake
//		if (select_new_color)
//		{
//			uint8_t rgb_idx;
//
//			for (rgb_idx=0; rgb_idx<3; rgb_idx++)
//			{
//				percent_of_rgb[rgb_idx] = ((double)(rand()%100))/100;
//			}
//
//			select_new_color = FALSE;
//		}

		// update the first led, the only one that wasn't updated till now
		uint16_t new_led_idx = (shows[SHOWS_SNAKE].direction == REGULAR_DIRECTION) ? 0 : MAX_LEDS_IN_STRIP - 1;

		if (cycle_cntr < TEDDY_BEAR_SNAKE_LENGTH)
		{
			double percent_of_rgb[3]={0};
			//uint8_t power = ((cycle_cntr == 0) || (cycle_cntr == (MAX_TEDDY_BEAR_LENGTH-1))) ? 50 :
			//				((cycle_cntr == 1) || (cycle_cntr == (MAX_TEDDY_BEAR_LENGTH-2))) ? 100 : 200;

			if (teddy_bear[strip_id][cycle_cntr] == 1)
			{
				percent_of_rgb[GREEN] = 0;
				percent_of_rgb[RED] = 1.0f;
				percent_of_rgb[BLUE] = 0.5f;

				//started_coloring = TRUE;
			}
			else if (teddy_bear[strip_id][cycle_cntr] == 2)
			{
				percent_of_rgb[GREEN] = 1.0f;
				percent_of_rgb[RED] = 1.0f;
				percent_of_rgb[BLUE] = 1.0f;

				//started_coloring = TRUE;
			}
			else //if (started_coloring == TRUE)
			{
				//if (teddy_bear[strip_id][cycle_cntr] != 2)
				//{
					percent_of_rgb[GREEN] = 0;
					percent_of_rgb[RED] = 0;
					percent_of_rgb[BLUE] = 0;

//					started_coloring = FALSE;
//				}
//				else
//				{
//					percent_of_rgb[GREEN] = 0;
//					percent_of_rgb[RED] = 1.0f;
//					percent_of_rgb[BLUE] = 0.5f;
//				}
			}

			LED_strips[strip_id][new_led_idx][GREEN] = percent_of_rgb[GREEN] * SET_POWER(SHOWS_SNAKE, 200);
			LED_strips[strip_id][new_led_idx][RED]   = percent_of_rgb[RED]   * SET_POWER(SHOWS_SNAKE, 200);
			LED_strips[strip_id][new_led_idx][BLUE]  = percent_of_rgb[BLUE]  * SET_POWER(SHOWS_SNAKE, 200);
		}
		else
		{
			LED_strips[strip_id][new_led_idx][GREEN] = 0;
			LED_strips[strip_id][new_led_idx][RED] = 0;
			LED_strips[strip_id][new_led_idx][BLUE] = 0;
		}
	}

//	if (SNAKE_SHOW_PERFORM_STRATUP_SEQ && (startup_cycle_cntr < SNAKE_SHOW_STARTUP_SEQ_END_CYCLE))
//	{
//		// startup sequence - run the loop without delay and driving the led strips
//		startup_cycle_cntr++;
//	}
//	else
//	{
		// regular cycle drive leds and wait refresh time
		drive_LED_strips();
		HAL_Delay(SNAKE_SHOW_REFRESH_TIME);
//	}

	if (cycle_cntr == TEDDY_BEAR_CYCLE_LENGTH - 1)
	{
		cycle_cntr = 0;
		//select_new_color = TRUE;
	}
	else
	{
		cycle_cntr++;
	}
}

void DanielShow(void)
{
	for (int strip_id=0; strip_id < MAX_ACTIVE_STRIPS; strip_id++)
	{
		// go over the strip and move every LED one hop according to the direction
		// For regular direction go from last led to the first and move state of led-1 to led_id
		// For reverse direction go from the first to the last and move state of led_id+1 to led_id
		if (shows[SHOWS_SNAKE].direction == REGULAR_DIRECTION)
		{
			for (int led_id=(MAX_LEDS_IN_STRIP-1); led_id!=0; led_id--)
			{
				LED_strips[strip_id][led_id][GREEN] = LED_strips[strip_id][led_id-1][GREEN];
				LED_strips[strip_id][led_id][RED]   = LED_strips[strip_id][led_id-1][RED];
				LED_strips[strip_id][led_id][BLUE]  = LED_strips[strip_id][led_id-1][BLUE];
			}
		}
		else
		{
			for (int led_id=0; led_id<(MAX_LEDS_IN_STRIP-1); led_id++)
			{
				LED_strips[strip_id][led_id][GREEN] = LED_strips[strip_id][led_id+1][GREEN];
				LED_strips[strip_id][led_id][RED]   = LED_strips[strip_id][led_id+1][RED];
				LED_strips[strip_id][led_id][BLUE]  = LED_strips[strip_id][led_id+1][BLUE];
			}
		}

		// choose new random color for every new snake
		if (select_new_color) {
			uint8_t rgb_idx;
			for (rgb_idx=0; rgb_idx<3; rgb_idx++)
			{
				percent_of_rgb[rgb_idx] = ((double)(rand()%100))/100;
			}
			select_new_color = FALSE;
		}

		// update the first led, the only one that wasn't updated till now
		uint16_t new_led_idx = (shows[SHOWS_SNAKE].direction == REGULAR_DIRECTION) ? 0 : MAX_LEDS_IN_STRIP-1;
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
		// startup sequence - run the loop without delay and driving the led strips
		startup_cycle_cntr++;
	}
	else
	{
		// regular cycle drive leds and wait refresh time
		drive_LED_strips();
		HAL_Delay(SNAKE_SHOW_REFRESH_TIME);
	}
	if (cycle_cntr==SNAKE_SHOW_CYCLE_LENGTH - 1)
	{
		cycle_cntr = 0;
		select_new_color = TRUE;
	}
	else
	{
		cycle_cntr++;
	}
}

void DanielEnd(void)
{
	/* snake shut down sequence */
	for (int8_t shut_down_seq_idx=SNAKE_SHOW_NUM_OF_DIM_STEPS; shut_down_seq_idx>=0; shut_down_seq_idx--)
	{
		for (int strip_id=0; strip_id < MAX_ACTIVE_STRIPS; strip_id++)
		{
			/* go over the strip and move every LED one hop according to the direction + perform dimming out */
			/* For regular direction go from last led to the first and move state of led-1 to led_id*/
			/* For reverse direction go from the first to the last and move state of led_id+1 to led_id*/
			double dim_percentage = (double)shut_down_seq_idx / SNAKE_SHOW_NUM_OF_DIM_STEPS;
			if (shows[SHOWS_SNAKE].direction == REGULAR_DIRECTION)
			{
				for (int led_id=(MAX_LEDS_IN_STRIP-1); led_id!=0; led_id--)
				{
					LED_strips[strip_id][led_id][GREEN] = LED_strips[strip_id][led_id-1][GREEN] * dim_percentage;
					LED_strips[strip_id][led_id][RED]   = LED_strips[strip_id][led_id-1][RED]   * dim_percentage;
					LED_strips[strip_id][led_id][BLUE]  = LED_strips[strip_id][led_id-1][BLUE]  * dim_percentage;
				}
			}
			else
			{
				for (int led_id=0; led_id<(MAX_LEDS_IN_STRIP-1); led_id++)
				{
					LED_strips[strip_id][led_id][GREEN] = LED_strips[strip_id][led_id+1][GREEN] * dim_percentage;
					LED_strips[strip_id][led_id][RED]   = LED_strips[strip_id][led_id+1][RED]   * dim_percentage;
					LED_strips[strip_id][led_id][BLUE]  = LED_strips[strip_id][led_id+1][BLUE]  * dim_percentage;
				}
			}

			uint16_t new_led_idx = (shows[SHOWS_SNAKE].direction == REGULAR_DIRECTION) ? 0 : MAX_LEDS_IN_STRIP-1;
			LED_strips[strip_id][new_led_idx][GREEN] = 0;
			LED_strips[strip_id][new_led_idx][RED] = 0;
			LED_strips[strip_id][new_led_idx][BLUE] = 0;
		}
		drive_LED_strips();
		HAL_Delay(SNAKE_SHOW_REFRESH_TIME);
	}
}

void TakoEnd(void)
{
	/* snake shut down sequence */
	for (int8_t shut_down_seq_idx=SNAKE_SHOW_NUM_OF_DIM_STEPS; shut_down_seq_idx>=0; shut_down_seq_idx--)
	{
		for (int strip_id=0; strip_id < MAX_ACTIVE_STRIPS; strip_id++)
		{
			/* go over the strip and move every LED one hop according to the direction + perform dimming out */
			/* For regular direction go from last led to the first and move state of led-1 to led_id*/
			/* For reverse direction go from the first to the last and move state of led_id+1 to led_id*/
			double dim_percentage = (double)shut_down_seq_idx / SNAKE_SHOW_NUM_OF_DIM_STEPS;
			if (shows[SHOWS_SNAKE].direction == REGULAR_DIRECTION)
			{
				for (int led_id=(MAX_LEDS_IN_STRIP-1); led_id!=0; led_id--)
				{
					LED_strips[strip_id][led_id][GREEN] = LED_strips[strip_id][led_id-1][GREEN] * dim_percentage;
					LED_strips[strip_id][led_id][RED]   = LED_strips[strip_id][led_id-1][RED]   * dim_percentage;
					LED_strips[strip_id][led_id][BLUE]  = LED_strips[strip_id][led_id-1][BLUE]  * dim_percentage;
				}
			}
			else
			{
				for (int led_id=0; led_id<(MAX_LEDS_IN_STRIP-1); led_id++)
				{
					LED_strips[strip_id][led_id][GREEN] = LED_strips[strip_id][led_id+1][GREEN] * dim_percentage;
					LED_strips[strip_id][led_id][RED]   = LED_strips[strip_id][led_id+1][RED]   * dim_percentage;
					LED_strips[strip_id][led_id][BLUE]  = LED_strips[strip_id][led_id+1][BLUE]  * dim_percentage;
				}
			}

			uint16_t new_led_idx = (shows[SHOWS_SNAKE].direction == REGULAR_DIRECTION) ? 0 : MAX_LEDS_IN_STRIP-1;
			LED_strips[strip_id][new_led_idx][GREEN] = 0;
			LED_strips[strip_id][new_led_idx][RED] = 0;
			LED_strips[strip_id][new_led_idx][BLUE] = 0;
		}
		drive_LED_strips();
		HAL_Delay(SNAKE_SHOW_REFRESH_TIME);
	}
}
