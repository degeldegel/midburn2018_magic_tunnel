#include "stm32f4xx_hal.h"
#include "ws2812b_multi_strip_driver.h"
#include "LED_shows.h"
#include "stdio.h"
#include "stdlib.h"
#include "flash.h"


/* user interface db, the user fills up this array according to LED strip id and led number,
 * the driver will push the LED strips according to this db */
extern volatile uint8_t LED_strips[MAX_SUPPORTED_NUM_OF_STRIPS][MAX_SUPPORTED_LEDS_IN_STRIP][NUM_OF_CFG_BYTES_PER_LED];
volatile show_db_t shows[NUM_OF_SHOWS];
volatile flash_show_config_db_t config_db;

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
		shows[i].max_power = DEFAULT_MAX_POWER;
	}
	/* clean Flash! -----> */
//	config_db.magic_word = 0;
//	flashStore((uint32_t*)&config_db, DATA_FLASH_START_ADDR, 0x4);
	/* clean Flash! <----- */
	load_config_from_flash();
	if (config_db.magic_word != FLASH_MAGIC_WORD)
	{
	    /* flash is corrupt or non initialized set default configuration */
	    load_default_configuration();
	    store_config_to_flash();
	}
}

/**
  * @brief  Loads default configuration
  * @param  void
  * @retval void
  */
void load_default_configuration(void)
{
    /* update config db */
    uint8_t snake_id;
    config_db.magic_word = FLASH_MAGIC_WORD;
    for (snake_id=0; snake_id<NUM_OF_SNAKE_SHOWS; snake_id++)
    {
        uint8_t snake_show_id = snake_id; //it looks straight forward but this is done for future moving snakes to different location in the show database
        config_db.snake[snake_id].cycle_length = DEFAULT_SNAKE_SHOW_CYCLE_LENGTH;
        config_db.snake[snake_id].fade_out_steps = DEFAULT_SNAKE_SHOW_FADE_OUT_STEPS;
        config_db.snake[snake_id].perform_startup_seq = DEFAULT_SNAKE_SHOW_PERFORM_STRATUP_SEQ;
        config_db.snake[snake_id].refresh_time = DEFAULT_SNAKE_SHOW_REFRESH_TIME;
        config_db.snake[snake_id].snake_length = DEFAULT_SNAKE_SHOW_SNAKE_LENGTH;
        config_db.snake[snake_id].starup_seq_end_cycle = DEFAULT_SNAKE_SHOW_STARTUP_SEQ_END_CYCLE;
        /* update shows db */
        shows[snake_show_id].direction = REGULAR_DIRECTION;
        shows[snake_show_id].max_power = DEFAULT_MAX_POWER;
    }
}

/**
  * @brief  Stores current configuration to flash
  * @param  void
  * @retval void
  */
void store_config_to_flash(void)
{
    uint8_t status;
    uint8_t show_idx;

    /* Snake shows store */
    for (show_idx=0; show_idx<NUM_OF_SNAKE_SHOWS; show_idx++)
    {
        uint8_t snake_idx = show_idx; //it looks straight forward but this is done for future moving snakes to different location in the show database
        config_db.snake[snake_idx].max_power = shows[show_idx].max_power;
        config_db.snake[snake_idx].direction = shows[show_idx].direction;

    }
    status = flashStore((uint32_t*)&config_db, DATA_FLASH_START_ADDR, sizeof(flash_show_config_db_t));
    if (status == FLASH_FAIL)
    {
        while(1){}
    }
}

/**
  * @brief  Loads configuration from flash
  * @param  void
  * @retval void
  */
void load_config_from_flash(void)
{
    /* read magic word only, if OK proceed with full read */
    flashLoad((uint32_t*)&config_db, DATA_FLASH_START_ADDR, 0x4);
    if (config_db.magic_word == FLASH_MAGIC_WORD)
    {
        uint8_t show_idx;
        flashLoad((uint32_t*)&config_db, DATA_FLASH_START_ADDR, sizeof(flash_show_config_db_t));
        /* Snake shows load */
        for (show_idx=0; show_idx<NUM_OF_SNAKE_SHOWS; show_idx++)
        {
            uint8_t snake_idx = show_idx; //it looks straight forward but this is done for future moving snakes to different location in the show database
            shows[show_idx].max_power = config_db.snake[snake_idx].max_power;
            shows[show_idx].direction = config_db.snake[snake_idx].direction;
        }
    }
}

void snake_show_0(void)
{
    snake_show(0);
}

void snake_show_1(void)
{
    snake_show(1);
}

void snake_show_2(void)
{
    snake_show(2);
}

/**
  * @brief  snake show, runs colorful snakes through the LED strips.
  * @param  void
  * @retval void
  */
void snake_show(uint8_t snake_id)
{
    volatile int led_id, strip_id;

    uint32_t cycle_cntr=0, startup_cycle_cntr=0;
    double percent_of_rgb[3]={0};
    uint8_t select_new_color = TRUE;
    int8_t shut_down_seq_idx;
    uint16_t new_led_idx;
    uint8_t snake_show_id = snake_id; //this was added as preparation for future option of snake id not aligned with show id

    //light up green led indication
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    //HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

    srand(SysTick->VAL);
    while (shows[snake_show_id].status == SHOW_STATUS_RUNNING)
    {
        for (strip_id=0; strip_id < MAX_ACTIVE_STRIPS; strip_id++)
        {
            /* go over the strip and move every LED one hop according to the direction */
            /* For regular direction go from last led to the first and move state of led-1 to led_id*/
            /* For reverse direction go from the first to the last and move state of led_id+1 to led_id*/
            if (shows[snake_show_id].direction == REGULAR_DIRECTION)
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
            new_led_idx = (shows[snake_show_id].direction == REGULAR_DIRECTION) ? 0 : MAX_LEDS_IN_STRIP-1;
            if (cycle_cntr < config_db.snake[snake_id].snake_length)
            {
                uint8_t power = ((cycle_cntr == 0) || (cycle_cntr == (config_db.snake[snake_id].snake_length-1))) ? 50 :
                                ((cycle_cntr == 1) || (cycle_cntr == (config_db.snake[snake_id].snake_length-2))) ? 100 : 200;
                LED_strips[strip_id][new_led_idx][GREEN] = percent_of_rgb[GREEN] * SET_POWER(snake_show_id, power);
                LED_strips[strip_id][new_led_idx][RED]   = percent_of_rgb[RED]   * SET_POWER(snake_show_id, power);
                LED_strips[strip_id][new_led_idx][BLUE]  = percent_of_rgb[BLUE]  * SET_POWER(snake_show_id, power);
            }
            else
            {
                LED_strips[strip_id][new_led_idx][GREEN] = 0;
                LED_strips[strip_id][new_led_idx][RED] = 0;
                LED_strips[strip_id][new_led_idx][BLUE] = 0;
            }
        }
        if (config_db.snake[snake_id].perform_startup_seq && (startup_cycle_cntr < config_db.snake[snake_id].starup_seq_end_cycle))
        {
            /* startup sequence - run the loop without delay and driving the led strips */
            startup_cycle_cntr++;
        }
        else
        {
            /* regular cycle drive leds and wait refresh time */
            drive_LED_strips();
            HAL_Delay(config_db.snake[snake_id].refresh_time);
        }
        if (cycle_cntr >= (config_db.snake[snake_id].cycle_length-1))
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
    for (shut_down_seq_idx=config_db.snake[snake_id].fade_out_steps; shut_down_seq_idx>=0; shut_down_seq_idx--)
    {
        for (strip_id=0; strip_id < MAX_ACTIVE_STRIPS; strip_id++)
        {
            /* go over the strip and move every LED one hop according to the direction + perform dimming out */
            /* For regular direction go from last led to the first and move state of led-1 to led_id*/
            /* For reverse direction go from the first to the last and move state of led_id+1 to led_id*/
            double dim_percentage = (double)shut_down_seq_idx / config_db.snake[snake_id].fade_out_steps;
            if (shows[snake_show_id].direction == REGULAR_DIRECTION)
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
            new_led_idx = (shows[snake_show_id].direction == REGULAR_DIRECTION) ? 0 : MAX_LEDS_IN_STRIP-1;
            LED_strips[strip_id][new_led_idx][GREEN] = 0;
            LED_strips[strip_id][new_led_idx][RED] = 0;
            LED_strips[strip_id][new_led_idx][BLUE] = 0;
        }
        drive_LED_strips();
        HAL_Delay(config_db.snake[snake_id].refresh_time);
    }
    //turn off green led indication
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
}

void teady_bear(void)
{
    volatile uint32_t cycle_cntr=0;

    while (shows[SHOWS_TEDDY_BEAR].status == SHOW_STATUS_RUNNING)
    {
        for (int strip_id=0; strip_id < MAX_ACTIVE_STRIPS; strip_id++)
        {
            // go over the strip and move every LED one hop according to the direction
            // For regular direction go from last led to the first and move state of led-1 to led_id
            // For reverse direction go from the first to the last and move state of led_id+1 to led_id
            if (shows[SHOWS_TEDDY_BEAR].direction == REGULAR_DIRECTION)
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

            // update the first led, the only one that wasn't updated till now
            uint16_t new_led_idx = (shows[SHOWS_TEDDY_BEAR].direction == REGULAR_DIRECTION) ? 0 : MAX_LEDS_IN_STRIP - 1;

            if (cycle_cntr < TEDDY_BEAR_SNAKE_LENGTH)
            {
                double percent_of_rgb[3]={0};

                if (teddy_bear[strip_id][cycle_cntr] == 1)
                {
                    percent_of_rgb[GREEN] = 0;
                    percent_of_rgb[RED] = 1.0f;
                    percent_of_rgb[BLUE] = 0.5f;

                }
                else if (teddy_bear[strip_id][cycle_cntr] == 2)
                {
                    percent_of_rgb[GREEN] = 1.0f;
                    percent_of_rgb[RED] = 1.0f;
                    percent_of_rgb[BLUE] = 1.0f;

                }
                else //if (started_coloring == TRUE)
                {
                    percent_of_rgb[GREEN] = 0;
                    percent_of_rgb[RED] = 0;
                    percent_of_rgb[BLUE] = 0;
                }

                LED_strips[strip_id][new_led_idx][GREEN] = percent_of_rgb[GREEN] * SET_POWER(SHOWS_TEDDY_BEAR, 200);
                LED_strips[strip_id][new_led_idx][RED]   = percent_of_rgb[RED]   * SET_POWER(SHOWS_TEDDY_BEAR, 200);
                LED_strips[strip_id][new_led_idx][BLUE]  = percent_of_rgb[BLUE]  * SET_POWER(SHOWS_TEDDY_BEAR, 200);
            }
            else
            {
                LED_strips[strip_id][new_led_idx][GREEN] = 0;
                LED_strips[strip_id][new_led_idx][RED] = 0;
                LED_strips[strip_id][new_led_idx][BLUE] = 0;
            }
        }

        drive_LED_strips();
        HAL_Delay(10); //TODO add to be configurable

        if (cycle_cntr == TEDDY_BEAR_CYCLE_LENGTH - 1)
        {
            cycle_cntr = 0;
        }
        else
        {
            cycle_cntr++;
        }
    }
}

void CometShow(void)
{
	TwinklingStars();
//	CometDrop();
//	CometExplosion();

	//turn off green led indication
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
	shows[SHOWS_COMET].status = SHOW_STATUS_DISABLED;
}

void TwinklingStars(void)
{
    volatile int led_id, strip_id;
    int8_t shut_down_seq_idx;
    // twinkle cycle time
    volatile int cycle_length = 20;

    uint8_t step_size = shows[SHOWS_COMET].max_power * 2 / cycle_length;
    // probability for each led to start the twinkle
    volatile double twinkle_probabilty = 5;

    //light up green led indication
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

    srand(SysTick->VAL);
    while (shows[SHOWS_COMET].status == SHOW_STATUS_RUNNING)
    {
    	for (strip_id=0; strip_id < MAX_ACTIVE_STRIPS; strip_id++)
    	{
    		for (led_id=0; led_id<(MAX_LEDS_IN_STRIP-1); led_id++)
    		{
    			if (LED_strips[strip_id][led_id][GREEN] == 0)  // if the led is off
    			{
    				if (twinkle_probabilty >= (double)(rand()%100000))  // start shining
    				{
    					LED_strips[strip_id][led_id][GREEN] = step_size + 1;
						LED_strips[strip_id][led_id][RED] =   step_size;
						LED_strips[strip_id][led_id][BLUE] =  step_size;
    				}
    			}
    			else
    			{
    				if ((LED_strips[strip_id][led_id][GREEN] > LED_strips[strip_id][led_id][RED]) && (LED_strips[strip_id][led_id][GREEN] < shows[SHOWS_COMET].max_power)) // rising
    				{
    					LED_strips[strip_id][led_id][GREEN] +=  step_size;
						LED_strips[strip_id][led_id][RED]   +=  step_size;
						LED_strips[strip_id][led_id][BLUE]  +=  step_size;
    				}
    				else if ((LED_strips[strip_id][led_id][GREEN] > LED_strips[strip_id][led_id][RED]) && (LED_strips[strip_id][led_id][GREEN] >= shows[SHOWS_COMET].max_power)) //at max
    				{
    					LED_strips[strip_id][led_id][GREEN] -= (step_size + 1);
						LED_strips[strip_id][led_id][RED]   -= step_size;
						LED_strips[strip_id][led_id][BLUE]  -= step_size;
    				}
    				else  //falling
    				{
    					LED_strips[strip_id][led_id][GREEN] = (LED_strips[strip_id][led_id][GREEN] > step_size) ? (LED_strips[strip_id][led_id][GREEN] - step_size) : 0;
						LED_strips[strip_id][led_id][RED] =   (LED_strips[strip_id][led_id][RED] > step_size) ? (LED_strips[strip_id][led_id][RED] - step_size) : 0;
						LED_strips[strip_id][led_id][BLUE] =  (LED_strips[strip_id][led_id][BLUE] > step_size) ? (LED_strips[strip_id][led_id][BLUE] - step_size) : 0;
    				}
    			}
    		}
    	}
    drive_LED_strips();
    HAL_Delay(SNAKE_SHOW_REFRESH_TIME);
    }
    // stars shut down sequence
    for (shut_down_seq_idx=SNAKE_SHOW_NUM_OF_DIM_STEPS; shut_down_seq_idx>=0; shut_down_seq_idx--)
    {
        for (strip_id=0; strip_id < MAX_ACTIVE_STRIPS; strip_id++)
        {
            /* go over the strip and move every LED one hop according to the direction + perform dimming out */
            /* For regular direction go from last led to the first and move state of led-1 to led_id*/
            /* For reverse direction go from the first to the last and move state of led_id+1 to led_id*/
            double dim_percentage = (double)shut_down_seq_idx / SNAKE_SHOW_NUM_OF_DIM_STEPS;
            for (led_id=(MAX_LEDS_IN_STRIP-1); led_id!=0; led_id--)
            {
                LED_strips[strip_id][led_id][GREEN] = LED_strips[strip_id][led_id][GREEN] * dim_percentage;
                LED_strips[strip_id][led_id][RED]   = LED_strips[strip_id][led_id][RED]   * dim_percentage;
                LED_strips[strip_id][led_id][BLUE]  = LED_strips[strip_id][led_id][BLUE]  * dim_percentage;
            }
            LED_strips[strip_id][0][GREEN] = 0;
            LED_strips[strip_id][0][RED] = 0;
            LED_strips[strip_id][0][BLUE] = 0;
        }
        drive_LED_strips();
        HAL_Delay(SNAKE_SHOW_REFRESH_TIME);
    }
}

