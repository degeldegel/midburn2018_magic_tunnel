#ifndef _WS2812B_MULTI_STRIP_DRIVER_H
#define _WS2812B_MULTI_STRIP_DRIVER_H

/* =========================================================================================== */
/* ==  DEFINES == */
/* =========================================================================================== */

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

/* typedefs for emulator */
typedef unsigned uint8_t;
typedef unsigned uint16_t;
typedef unsigned uint32_t;
typedef int int8_t;
typedef int int16_t;
typedef int int32_t;

/* emulator of systick */
typedef struct _systick_emul {
        unsigned VAL;
        unsigned ZIBI;
} systick_emul_t;

/* dummy defines for emulator */
#define DUMMY_VAL      0xdeadbeef
#define LD2_GPIO_Port  DUMMY_VAL
#define LD2_Pin        DUMMY_VAL
#define GPIO_PIN_SET   DUMMY_VAL
#define GPIO_PIN_RESET DUMMY_VAL

/* =========================================================================================== */
/* ==  ENUMS == */
/* =========================================================================================== */

enum {
	GREEN 	= 0,
	RED 	= 1,
	BLUE 	= 2
};

/* =========================================================================================== */
/* ==  MACROS == */
/* =========================================================================================== */

#define GET_STRIP_PORT(strip) (strip_ports[strip])
#define GET_STRIP_GPIO(strip) (strip_GPIOs[strip])

/* =========================================================================================== */
/* ==  EXTERNAL FUNCTIONS== */
/* =========================================================================================== */

/**
  * @brief  main function that drives the strips, it should be called after the LED_strips db is updated with the new output.
  *         all strips are updated on parallel.
  * @param  void
  * @retval void
  */
void drive_LED_strips(void);

/* emulator function of delay */
void HAL_Delay(uint32_t delay);

/* dummy function for emulator */
void HAL_GPIO_WritePin(int port, int pin, int operation);

#endif  /* _WS2812B_MULTI_STRIP_DRIVER_H */

