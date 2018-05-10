/*
 * side_clouds.c
 *
 *  Created on: 11 May 2018
 *      Author: danielg
 */
#include "stm32f4xx_hal.h"
#include "ws2812b_multi_strip_driver.h"
#include "stdlib.h"
#include "flash.h"
#include "side_clouds.h"

extern volatile uint8_t LED_strips[MAX_SUPPORTED_NUM_OF_STRIPS][MAX_SUPPORTED_LEDS_IN_STRIP][NUM_OF_CFG_BYTES_PER_LED];

/**
  * @brief  side clouds main function
  * @param  void
  * @retval void
  */
void side_clouds_show(void)
{
    volatile int led_id;  // , strip_id;

//    uint32_t cycle_cntr=0, startup_cycle_cntr=0;
    double percent_of_rgb[3]={0};
//    uint8_t select_new_color = TRUE;
//    int8_t shut_down_seq_idx;
//    uint16_t new_led_idx;
    //uint8_t snake_show_id = 3; //this was added as preparation for future option of snake id not aligned with show id

    //light up green led indication
    //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    //HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    #define NUM_OF_SECTIONS (24)
    #define NUM_OF_SIDE_CLOUD_STRIPS (8)
    #define REFRESH_RATE_SIDE_CLOUDS (30)
    uint8_t section_show_duration = 100;
    uint8_t num_of_sections = NUM_OF_SECTIONS;
    uint8_t sections[NUM_OF_SECTIONS];
    uint32_t leds_per_section = NUM_OF_SIDE_CLOUD_STRIPS * MAX_LEDS_IN_STRIP / num_of_sections;
    uint8_t active_sections = 0;
    uint8_t max_active_sections = 5;
    uint8_t section_id, section_strip, section_first_led;
    uint8_t rgb_idx;

    /* init flow */
    for (section_id = 0; section_id < num_of_sections; section_id++)
    {
        sections[section_id] = 0;
    }
    srand(SysTick->VAL);
    for (rgb_idx=0; rgb_idx<3; rgb_idx++)
    {
        percent_of_rgb[rgb_idx] = ((double)(rand()%100))/100;
    }

    while (1)
    {
        // Check if we need to activate a new section
        if ( (active_sections < max_active_sections) && ((rand() % 100) < 10))
        {
            uint8_t found_new_section = False;
            /* choose new show */
            while (!found_new_section)
            { /* loop till find new random show that wasn't run yet */
                uint8_t rand_section_id = rand() % NUM_OF_SECTIONS;
                if (sections[rand_section_id] == 0)
                {
                    section_id = rand_section_id;
                    found_new_section = True;
                }
            }

            // Select color for the section
//            sections_colors[section_id] = random color;
            sections[section_id] = section_show_duration;
            active_sections++;
        }

        // Light Active Sections
        for (section_id = 0; section_id < num_of_sections; section_id++)
        {
            if (sections[section_id] > 0)
            {
                // This is an active section
                sections[section_id] = sections[section_id] - 1;

                section_strip = section_id * leds_per_section / MAX_LEDS_IN_STRIP;
                section_first_led = (section_id * leds_per_section) % MAX_LEDS_IN_STRIP;

                if (sections[section_id] > section_show_duration * 0.9f)
                {
                    // Fade In
                }
                if (sections[section_id] < section_show_duration * 0.1f)
                {
                    // Fade Out
                    for (led_id = section_first_led; (led_id < section_first_led + leds_per_section) && (led_id < MAX_LEDS_IN_STRIP); led_id++)
                   {
                       LED_strips[section_strip][led_id][RED]   = 0;
                       LED_strips[section_strip][led_id][GREEN] = 0;
                       LED_strips[section_strip][led_id][BLUE]  = 0;
                   }

                }
                else
                {
                    // Show
                    uint8_t color_r = percent_of_rgb[RED] *   SIDE_CLOUD_SET_POWER(200);
                    uint8_t color_g = percent_of_rgb[GREEN] * SIDE_CLOUD_SET_POWER(200);
                    uint8_t color_b = percent_of_rgb[BLUE] *  SIDE_CLOUD_SET_POWER(200);

                    for (led_id = section_first_led; (led_id < section_first_led + leds_per_section) && (led_id < MAX_LEDS_IN_STRIP); led_id++)
                    {
                        LED_strips[section_strip][led_id][RED]   = color_r;
                        LED_strips[section_strip][led_id][GREEN] = color_g;
                        LED_strips[section_strip][led_id][BLUE]  = color_b;
                    }
                }

                if (sections[section_id] == 0)
                {
                    active_sections--;
                }
            }
        }

        drive_LED_strips();
        HAL_Delay(REFRESH_RATE_SIDE_CLOUDS);
    }
}

