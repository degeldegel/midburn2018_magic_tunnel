/**
*****************************************************************************
**
**  File        : main.c
**
**  Abstract    : main function.
**
**  Functions   : main
**
**  Distribution: The file is distributed "as is", without any warranty
**                of any kind.
**
**  (c)Copyright Atollic AB.
**  You may use this file as-is or modify it according to the needs of your
**  project. Distribution of this file (unmodified or modified) is not
**  permitted. Atollic AB permit registered Atollic TrueSTUDIO(R) users the
**  rights to distribute the assembled, compiled & linked contents of this
**  file as part of an application binary file, provided that it is built
**  using the Atollic TrueSTUDIO(R) toolchain.
**
**
*****************************************************************************
*/

#include "stm32f4xx_hal.h"
#include "ws2812b_multi_strip_driver.h"
#include "LED_shows.h"
#include <stdio.h>

extern volatile show_db_t shows[NUM_OF_SHOWS];

int main(void){

    SysTick = &SysTick_s;/* for code consistency of emulation and operatinal code */
    printf("LED emulator program started!\n");
    printf("LED emulator is generating data, this might take a minute, please wait...\n");
    fflush(stdout);

    init_shows();
    shows[SHOWS_SNAKE].max_power = 100; /*to see it beter in the emulator*/
    //shows[SHOWS_SNAKE].status = SHOW_STATUS_RUNNING;
    //snake_show();

    shows[SHOWS_METEOR].max_power = 200; /*to see it beter in the emulator*/
    shows[SHOWS_METEOR].status = SHOW_STATUS_RUNNING;
    MeteorShow();
    printf("LED emulator program finished successfully!\n");
	return 0;
}
