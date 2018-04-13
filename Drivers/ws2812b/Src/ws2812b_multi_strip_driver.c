#include "stm32f4xx_hal.h"
#include "ws2812b_multi_strip_driver.h"

uint16_t GPIO_strips_mask[MAX_SUPPORTED_LEDS_IN_STRIP * BITS_TO_CONFIGURE_ONE_LED]; /*contains masks of all bits that are zero*/
uint16_t GPIO_strips_mask_2[MAX_SUPPORTED_LEDS_IN_STRIP * BITS_TO_CONFIGURE_ONE_LED]; /*contains masks of all bits that are zero*/
uint16_t GPIO_all_strips_mask[2];
uint8_t  LED_strips[MAX_SUPPORTED_NUM_OF_STRIPS][MAX_SUPPORTED_LEDS_IN_STRIP][NUM_OF_CFG_BYTES_PER_LED];

void drive_port_strips(void)
{
	volatile int idx;
	int i, curr_led_bit_idx;
	uint16_t curr_zero_mask[2];
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
	for (curr_led_bit_idx=0; curr_led_bit_idx < MAX_LEDS_IN_STRIP * BITS_TO_CONFIGURE_ONE_LED; curr_led_bit_idx++)
	{
		//raise all Strips up
		GPIOB->ODR |=  GPIO_all_strips_mask[0];
		GPIOC->ODR |=  GPIO_all_strips_mask[1];
		//wait for strips with bit value zero
		for (i=0; i < 7; i++) {idx=i;}
		//lower all strips with bit value zero
		curr_zero_mask[0] = GPIO_strips_mask[curr_led_bit_idx];
		curr_zero_mask[1] = GPIO_strips_mask_2[curr_led_bit_idx];
		GPIOB->ODR &= ~curr_zero_mask[0];
		GPIOC->ODR &= ~curr_zero_mask[1];
		//wait for strips with bit value one
		for (i=0; i < 16; i++) {idx=i;}
		//lower all strips with bit value zero
		GPIOB->ODR &= ~(curr_zero_mask[0] ^ GPIO_all_strips_mask[0]);
		GPIOC->ODR &= ~(curr_zero_mask[1] ^ GPIO_all_strips_mask[1]);
		//finish bit configuration cycle ~1.25 msec
		for (i=0; i < 7; i++) {idx=i;}
	}
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}

void update_GPIO_all_strips_mask(uint16_t update_mask)
{
	GPIO_all_strips_mask[0] = update_mask;
	GPIO_all_strips_mask[1] = update_mask;
}

void zero_all_driver_masks(void)
{
	int curr_led_bit_idx;
	for (curr_led_bit_idx=0; curr_led_bit_idx < MAX_LEDS_IN_STRIP * BITS_TO_CONFIGURE_ONE_LED; curr_led_bit_idx++)
	{
		GPIO_strips_mask[curr_led_bit_idx] = 0;
		GPIO_strips_mask_2[curr_led_bit_idx] = 0;
	}
}

void update_driver_mask(int port_idx)
{
	//TODO - need to add hash table to convert LED_STRIP index to actual GPIO port and number
	int led_idx, rgb_idx, strip_idx;
	zero_all_driver_masks();
	for (strip_idx=0; strip_idx < MAX_ACTIVE_STRIPS; strip_idx++)
	{
		int curr_strip_port, curr_strip_gpio_idx;
		curr_strip_port = GET_STRIP_PORT(strip_idx);
		if (curr_strip_port!=port_idx) continue;
		curr_strip_gpio_idx = GET_STRIP_GPIO(strip_idx);
		for (led_idx=0; led_idx < MAX_LEDS_IN_STRIP; led_idx ++)
		{
			for (rgb_idx=0; rgb_idx < NUM_OF_CFG_BYTES_PER_LED; rgb_idx ++)
			{
				uint8_t cur_rgb_led;
				cur_rgb_led = LED_strips[strip_idx][led_idx][rgb_idx];
				int base_bit_idx = led_idx*BITS_TO_CONFIGURE_ONE_LED + rgb_idx*BITS_IN_BYTE;
				GPIO_strips_mask[base_bit_idx + 0] |= ((cur_rgb_led>>7) & 0x1) ? 0 : 1 << curr_strip_gpio_idx;
				GPIO_strips_mask[base_bit_idx + 1] |= ((cur_rgb_led>>6) & 0x1) ? 0 : 1 << curr_strip_gpio_idx;
				GPIO_strips_mask[base_bit_idx + 2] |= ((cur_rgb_led>>5) & 0x1) ? 0 : 1 << curr_strip_gpio_idx;
				GPIO_strips_mask[base_bit_idx + 3] |= ((cur_rgb_led>>4) & 0x1) ? 0 : 1 << curr_strip_gpio_idx;
				GPIO_strips_mask[base_bit_idx + 4] |= ((cur_rgb_led>>3) & 0x1) ? 0 : 1 << curr_strip_gpio_idx;
				GPIO_strips_mask[base_bit_idx + 5] |= ((cur_rgb_led>>2) & 0x1) ? 0 : 1 << curr_strip_gpio_idx;
				GPIO_strips_mask[base_bit_idx + 6] |= ((cur_rgb_led>>1) & 0x1) ? 0 : 1 << curr_strip_gpio_idx;
				GPIO_strips_mask[base_bit_idx + 7] |= ((cur_rgb_led>>0) & 0x1) ? 0 : 1 << curr_strip_gpio_idx;

				GPIO_strips_mask_2[base_bit_idx + 0] |= ((cur_rgb_led>>7) & 0x1) ? 0 : 1 << curr_strip_gpio_idx;
				GPIO_strips_mask_2[base_bit_idx + 1] |= ((cur_rgb_led>>6) & 0x1) ? 0 : 1 << curr_strip_gpio_idx;
				GPIO_strips_mask_2[base_bit_idx + 2] |= ((cur_rgb_led>>5) & 0x1) ? 0 : 1 << curr_strip_gpio_idx;
				GPIO_strips_mask_2[base_bit_idx + 3] |= ((cur_rgb_led>>4) & 0x1) ? 0 : 1 << curr_strip_gpio_idx;
				GPIO_strips_mask_2[base_bit_idx + 4] |= ((cur_rgb_led>>3) & 0x1) ? 0 : 1 << curr_strip_gpio_idx;
				GPIO_strips_mask_2[base_bit_idx + 5] |= ((cur_rgb_led>>2) & 0x1) ? 0 : 1 << curr_strip_gpio_idx;
				GPIO_strips_mask_2[base_bit_idx + 6] |= ((cur_rgb_led>>1) & 0x1) ? 0 : 1 << curr_strip_gpio_idx;
				GPIO_strips_mask_2[base_bit_idx + 7] |= ((cur_rgb_led>>0) & 0x1) ? 0 : 1 << curr_strip_gpio_idx;
			}
		}
	}
}

