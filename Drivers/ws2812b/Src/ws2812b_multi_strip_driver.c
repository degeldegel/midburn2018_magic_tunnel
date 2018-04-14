#include "stm32f4xx_hal.h"
#include "ws2812b_multi_strip_driver.h"

/*contains masks of all bits that are zero in PortB/C*/
uint16_t GPIO_strips_mask[MAX_SUPPORTED_NUM_OF_PORTS][MAX_SUPPORTED_LEDS_IN_STRIP * BITS_TO_CONFIGURE_ONE_LED];

/*active GPIOs in the port 0 - portB, 1 - portC */
uint16_t GPIO_all_strips_mask[MAX_SUPPORTED_NUM_OF_PORTS];

/* user interface db, the user fills up this array according to LED strip id and led number,
 * the driver will push the LED strips according to this db */
uint8_t  LED_strips[MAX_SUPPORTED_NUM_OF_STRIPS][MAX_SUPPORTED_LEDS_IN_STRIP][NUM_OF_CFG_BYTES_PER_LED];

uint8_t strip_ports[MAX_SUPPORTED_NUM_OF_STRIPS] =
       /* Strip #0     Strip #1     Strip #2     Strip #3     Strip #4     Strip #5     Strip #6     Strip #7  */
       {GPIO_PORT_C, GPIO_PORT_C, GPIO_PORT_C, GPIO_PORT_C, GPIO_PORT_B, GPIO_PORT_B, GPIO_PORT_B, GPIO_PORT_B,
       /* Strip #8     Strip #9     Strip #10    Strip #11    Strip #12    Strip #13    Strip #14    Strip #15 */
        GPIO_PRT_NA, GPIO_PRT_NA, GPIO_PRT_NA, GPIO_PRT_NA, GPIO_PRT_NA, GPIO_PRT_NA, GPIO_PRT_NA, GPIO_PRT_NA,
       /* Strip #16    Strip #17    Strip #18    Strip #19 */
        GPIO_PRT_NA, GPIO_PRT_NA, GPIO_PRT_NA, GPIO_PRT_NA};
uint16_t strip_GPIOs[MAX_SUPPORTED_NUM_OF_STRIPS] =
       /* Strip #0     Strip #1     Strip #2     Strip #3     Strip #4     Strip #5     Strip #6     Strip #7  */
       { GPIO_PIN_0,  GPIO_PIN_1,  GPIO_PIN_2,  GPIO_PIN_3, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15,
       /* Strip #8     Strip #9     Strip #10    Strip #11    Strip #12    Strip #13    Strip #14    Strip #15 */
        GPIO_PIN_NA, GPIO_PIN_NA, GPIO_PIN_NA, GPIO_PIN_NA, GPIO_PIN_NA, GPIO_PIN_NA, GPIO_PIN_NA, GPIO_PIN_NA,
       /* Strip #16    Strip #17    Strip #18    Strip #19 */
        GPIO_PIN_NA, GPIO_PIN_NA, GPIO_PIN_NA, GPIO_PIN_NA};

/**
  * @brief  main function that drives the strips, it should be called after the LED_strips db is updated with the new output.
  * @param  void
  * @retval void
  */
void drive_port_strips(void)
{
	volatile int idx;
	int i, curr_led_bit_idx;
	uint16_t curr_zero_mask[2];
	//volatile uint32_t systick_val[4];
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
	for (curr_led_bit_idx=0; curr_led_bit_idx < MAX_LEDS_IN_STRIP * BITS_TO_CONFIGURE_ONE_LED; curr_led_bit_idx++)
	{
		//raise all Strips up
	    //systick_val[0] = SysTick->VAL;
		GPIOB->ODR |=  GPIO_all_strips_mask[GPIO_PORT_B];
		GPIOC->ODR |=  GPIO_all_strips_mask[GPIO_PORT_C];
		//wait for strips with bit value zero
		for (i=0; i < 7; i++) {idx=i;}
		//lower all strips with bit value zero
		curr_zero_mask[GPIO_PORT_B] = GPIO_strips_mask[GPIO_PORT_B][curr_led_bit_idx];
		curr_zero_mask[GPIO_PORT_C] = GPIO_strips_mask[GPIO_PORT_C][curr_led_bit_idx];
		//systick_val[1] = SysTick->VAL;
		GPIOB->ODR &= ~curr_zero_mask[GPIO_PORT_B];
		GPIOC->ODR &= ~curr_zero_mask[GPIO_PORT_C];
		//wait for strips with bit value one
		for (i=0; i < 13; i++) {idx=i;}
		//lower all strips with bit value zero
		//systick_val[2] = SysTick->VAL;
		GPIOB->ODR &= ~(curr_zero_mask[GPIO_PORT_B] ^ GPIO_all_strips_mask[GPIO_PORT_B]);
		GPIOC->ODR &= ~(curr_zero_mask[GPIO_PORT_C] ^ GPIO_all_strips_mask[GPIO_PORT_C]);
		//finish bit configuration cycle ~1.25 msec
		for (i=0; i < 7; i++) {idx=i;}
		//systick_val[3] = SysTick->VAL;
	}
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}

/**
  * @brief  used to update the mask of the active GPIOs of the strips
  * @param  which GPIO port should be updated
  * @param  mask of all active strips belong to the GPIO port
  *
  * @retval void
  */
void update_GPIO_all_strips_mask(uint8_t port, uint16_t update_mask)
{
	GPIO_all_strips_mask[port] = update_mask;
}

/**
  * @brief  inner function used to clear old GPIO strips masks, before the update takes effect.
  * @param  void
  * @retval void
  */
void zero_all_driver_masks(void)
{
	int curr_led_bit_idx, curr_GPIO_port_id;
	for (curr_GPIO_port_id=0; curr_GPIO_port_id<MAX_SUPPORTED_NUM_OF_PORTS; curr_GPIO_port_id++)
	{
        for (curr_led_bit_idx=0; curr_led_bit_idx < MAX_LEDS_IN_STRIP * BITS_TO_CONFIGURE_ONE_LED; curr_led_bit_idx++)
        {
            GPIO_strips_mask[curr_GPIO_port_id][curr_led_bit_idx] = 0;
        }
	}
}

/**
  * @brief  This function performs the conversion between the user interface AKA LED_strips and the driver db
  * @param  void
  * @retval void
  */
void update_driver_mask(void)
{
	int led_idx, rgb_idx, strip_idx;
	zero_all_driver_masks();
	for (strip_idx=0; strip_idx < MAX_ACTIVE_STRIPS; strip_idx++)
	{
		int curr_strip_port, curr_strip_gpio_msk;
		curr_strip_port = GET_STRIP_PORT(strip_idx);
		curr_strip_gpio_msk = GET_STRIP_GPIO(strip_idx);
		for (led_idx=0; led_idx < MAX_LEDS_IN_STRIP; led_idx ++)
		{
			for (rgb_idx=0; rgb_idx < NUM_OF_CFG_BYTES_PER_LED; rgb_idx ++)
			{
				uint8_t cur_rgb_led;
				cur_rgb_led = LED_strips[strip_idx][led_idx][rgb_idx];
				int base_bit_idx = led_idx*BITS_TO_CONFIGURE_ONE_LED + rgb_idx*BITS_IN_BYTE;
				GPIO_strips_mask[curr_strip_port][base_bit_idx + 0] |= ((cur_rgb_led>>7) & 0x1) ? 0 : curr_strip_gpio_msk;
				GPIO_strips_mask[curr_strip_port][base_bit_idx + 1] |= ((cur_rgb_led>>6) & 0x1) ? 0 : curr_strip_gpio_msk;
				GPIO_strips_mask[curr_strip_port][base_bit_idx + 2] |= ((cur_rgb_led>>5) & 0x1) ? 0 : curr_strip_gpio_msk;
				GPIO_strips_mask[curr_strip_port][base_bit_idx + 3] |= ((cur_rgb_led>>4) & 0x1) ? 0 : curr_strip_gpio_msk;
				GPIO_strips_mask[curr_strip_port][base_bit_idx + 4] |= ((cur_rgb_led>>3) & 0x1) ? 0 : curr_strip_gpio_msk;
				GPIO_strips_mask[curr_strip_port][base_bit_idx + 5] |= ((cur_rgb_led>>2) & 0x1) ? 0 : curr_strip_gpio_msk;
				GPIO_strips_mask[curr_strip_port][base_bit_idx + 6] |= ((cur_rgb_led>>1) & 0x1) ? 0 : curr_strip_gpio_msk;
				GPIO_strips_mask[curr_strip_port][base_bit_idx + 7] |= ((cur_rgb_led>>0) & 0x1) ? 0 : curr_strip_gpio_msk;
			}
		}
	}
}

