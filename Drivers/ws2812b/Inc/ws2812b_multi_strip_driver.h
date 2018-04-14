#ifndef _WS2812B_MULTI_STRIP_DRIVER_H
#define _WS2812B_MULTI_STRIP_DRIVER_H

#define MAX_SUPPORTED_NUM_OF_STRIPS (20)
#define MAX_SUPPORTED_LEDS_IN_STRIP (300)
#define MAX_SUPPORTED_NUM_OF_PORTS (2)
#define NUM_OF_CFG_BYTES_PER_LED (3)
#define BITS_TO_CONFIGURE_ONE_LED (24)
#define MAX_LEDS_IN_STRIP (300)
#define MAX_ACTIVE_STRIPS (8)
#define BITS_IN_BYTE (8)
#define GPIO_PORT_B (0)
#define GPIO_PORT_C (1)
#define GPIO_PRT_NA (255)
#define GPIO_PIN_NA ((uint16_t)0x0000)
#define LED_MAX_POWER (150)

enum {
	GREEN 	= 0,
	RED 	= 1,
	BLUE 	= 2
};

#define GET_STRIP_PORT(strip) (strip_ports[strip])
#define GET_STRIP_GPIO(strip) (strip_GPIOs[strip])

void drive_port_strips(void);
void update_driver_mask(void);
void update_GPIO_all_strips_mask(uint8_t port, uint16_t update_mask);
#endif  /* _WS2812B_MULTI_STRIP_DRIVER_H */
