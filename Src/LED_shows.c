#include "stm32f4xx_hal.h"
#include "ws2812b_multi_strip_driver.h"
#include "LED_shows.h"
#include "stdlib.h"
#include "flash.h"

/* user interface db, the user fills up this array according to LED strip id and led number,
 * the driver will push the LED strips according to this db */
extern volatile uint8_t LED_strips[MAX_SUPPORTED_NUM_OF_STRIPS][MAX_SUPPORTED_LEDS_IN_STRIP][NUM_OF_CFG_BYTES_PER_LED];
volatile show_db_t shows[NUM_OF_SHOWS];
volatile flash_show_config_db_t config_db;

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
	load_config_from_flash();
	if (config_db.magic_word != FLASH_MAGIC_WORD)
	{
	    /* flash is corrupt or non initialized set default configuration */
	    load_default_configuration();
	    flashStore((uint32_t*)&config_db, DATA_FLASH_START_ADDR, sizeof(flash_show_config_db_t));
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
    config_db.magic_word = FLASH_MAGIC_WORD;
    config_db.snake.cycle_length = DEFAULT_SNAKE_SHOW_CYCLE_LENGTH;
    config_db.snake.fade_out_steps = DEFAULT_SNAKE_SHOW_FADE_OUT_STEPS;
    config_db.snake.perform_startup_seq = DEFAULT_SNAKE_SHOW_PERFORM_STRATUP_SEQ;
    config_db.snake.refresh_time = DEFAULT_SNAKE_SHOW_REFRESH_TIME;
    config_db.snake.snake_length = DEFAULT_SNAKE_SHOW_SNAKE_LENGTH;
    config_db.snake.starup_seq_end_cycle = DEFAULT_SNAKE_SHOW_STARTUP_SEQ_END_CYCLE;
    /* update shows db */
    shows[SHOWS_SNAKE].direction = REGULAR_DIRECTION;
    shows[SHOWS_SNAKE].max_power = DEFAULT_MAX_POWER;
}

/**
  * @brief  Stores current configuration to flash
  * @param  void
  * @retval void
  */
void store_config_to_flash(void)
{
    uint8_t status;
    config_db.snake.max_power = shows[SHOWS_SNAKE].max_power;
    config_db.snake.direction = shows[SHOWS_SNAKE].direction;
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
        flashLoad((uint32_t*)&config_db, DATA_FLASH_START_ADDR, sizeof(flash_show_config_db_t));
        shows[SHOWS_SNAKE].max_power = config_db.snake.max_power;
        shows[SHOWS_SNAKE].direction = config_db.snake.direction;
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
            if (cycle_cntr < config_db.snake.snake_length)
            {
                uint8_t power = ((cycle_cntr == 0) || (cycle_cntr == (config_db.snake.snake_length-1))) ? 50 :
                                ((cycle_cntr == 1) || (cycle_cntr == (config_db.snake.snake_length-2))) ? 100 : 200;
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
        if (config_db.snake.perform_startup_seq && (startup_cycle_cntr < config_db.snake.starup_seq_end_cycle))
        {
            /* startup sequence - run the loop without delay and driving the led strips */
            startup_cycle_cntr++;
        }
        else
        {
            /* regular cycle drive leds and wait refresh time */
            drive_LED_strips();
            HAL_Delay(config_db.snake.refresh_time);
        }
        if (cycle_cntr >= (config_db.snake.cycle_length-1))
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
    for (shut_down_seq_idx=config_db.snake.fade_out_steps; shut_down_seq_idx>=0; shut_down_seq_idx--)
    {
        for (strip_id=0; strip_id < MAX_ACTIVE_STRIPS; strip_id++)
        {
            /* go over the strip and move every LED one hop according to the direction + perform dimming out */
            /* For regular direction go from last led to the first and move state of led-1 to led_id*/
            /* For reverse direction go from the first to the last and move state of led_id+1 to led_id*/
            double dim_percentage = (double)shut_down_seq_idx / config_db.snake.fade_out_steps;
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
        HAL_Delay(config_db.snake.refresh_time);
    }
    shows[SHOWS_SNAKE].status = SHOW_STATUS_DISABLED;
    //turn off green led indication
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
}

