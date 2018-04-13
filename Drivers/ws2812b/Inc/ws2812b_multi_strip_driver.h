#ifndef _WS2812B_MULTI_STRIP_DRIVER_H
#define _WS2812B_MULTI_STRIP_DRIVER_H

#define MAX_SUPPORTED_NUM_OF_STRIPS (20)
#define MAX_SUPPORTED_LEDS_IN_STRIP (300)
#define NUM_OF_CFG_BYTES_PER_LED (3)
#define BITS_TO_CONFIGURE_ONE_LED (24)
#define MAX_LEDS_IN_STRIP (300)
#define MAX_ACTIVE_STRIPS (4)
#define BITS_IN_BYTE (8)
#define GPIOB_PORT 0
#define GPIOC_PORT 1
#define LED_MAX_POWER (150)

enum {
	GREEN 	= 0,
	RED 	= 1,
	BLUE 	= 2
};

#define GET_STRIP_PORT(strip) ((strip<4) ? GPIOB_PORT : GPIOC_PORT)
#define GET_STRIP_GPIO(strip) (strip)

void drive_port_strips(void);
void update_driver_mask(int port_idx);
void update_GPIO_all_strips_mask(uint16_t update_mask);
#endif  /* _WS2812B_MULTI_STRIP_DRIVER_H */
