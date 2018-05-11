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
volatile side_clouds_t side_clouds_db;
uint8_t colors[NUM_OF_COLORS_PALETTE][NUM_OF_CFG_BYTES_PER_LED] =
{
    { 70, 130, 180}, /*steelblue*/
    {230, 230, 250}, /*lavender*/
    {173, 216, 230}, /*lightblue*/
    {135, 206, 250}, /*lightskyblue*/
    {176, 224, 230}, /*powderblue*/
    {  0, 191, 255}, /*deepskyblue*/
    { 72,  61, 139}, /*darkslateblue*/
    {  0,   0, 255}, /*blue*/
    { 95, 158, 160}, /*cadetblue*/
    { 25,  25, 112}, /*midnightblue*/
    {138,  43, 226}, /*blueViolet*/
    { 75,   0, 130}, /*indigo*/
    {148,   0, 211}, /*darkviolet*/
    {128,   0, 128}, /*purple*/
    {218, 112, 214}, /*archid*/
    { 32, 178, 170}, /*lightseagreen*/
    {143, 188, 143}, /*darkseagreen*/
    {219, 112, 147}, /*palevioletred*/
    {199,  21, 133}, /*mediumvioletred*/
    { 65, 105, 255}  /*royalblue*/
};
/**
  * @brief  initialization of side clouds
  * @param  void
  * @retval void
  */
void init_side_clouds_show(void)
{
    uint8_t section_id;
    side_clouds_db.max_active_sections = DEFAULT_MAX_ACTIVE_SECTIONS;
    side_clouds_db.maximum_duration    = DEFAULT_MAX_DURATION;
    side_clouds_db.minimum_duration    = DEFAULT_MIN_DURATION;
    side_clouds_db.num_of_sections     = DEFAULT_MAX_SECTIONS;
    side_clouds_db.refresh_rate        = DEFAULT_REFRESH_RATE_SIDE_CLOUDS;
    /* init flow */
    for (section_id = 0; section_id < side_clouds_db.num_of_sections; section_id++)
    {
        side_clouds_db.section[section_id].count = 0;
    }
}

/**
  * @brief  side clouds main function
  * @param  void
  * @retval void
  */
void side_clouds_show(void)
{
    volatile int led_id;

    //double percent_of_rgb[3]={0};

    uint32_t leds_per_section = NUM_OF_SIDE_CLOUD_STRIPS * MAX_LEDS_IN_STRIP / side_clouds_db.num_of_sections;
    uint8_t active_sections = 0;
    uint8_t section_id, section_strip, section_first_led;
    //uint8_t rgb_idx;

    HAL_Delay(3);
    srand(SysTick->VAL);
//    for (rgb_idx=0; rgb_idx<3; rgb_idx++)
//    {
//        percent_of_rgb[rgb_idx] = ((double)(rand()%100))/100;
//    }

    while (1)
    {
        // Check if we need to activate a new section
        if ( (active_sections < side_clouds_db.max_active_sections) && ((rand() % 100) < 10))
        {
            uint8_t found_new_section = False;
            uint8_t rand_color;
            /* choose new show */
            while (!found_new_section)
            { /* loop till find new random show that wasn't run yet */
                uint8_t rand_section_id = rand() % side_clouds_db.num_of_sections;
                if (side_clouds_db.section[rand_section_id].count == 0)
                {
                    section_id = rand_section_id;
                    found_new_section = True;
                }
            }

            // Select color for the section
            rand_color = rand() % NUM_OF_COLORS_PALETTE;
            side_clouds_db.section[section_id].color[RED]   = colors[rand_color][RED];
            side_clouds_db.section[section_id].color[GREEN] = colors[rand_color][GREEN];
            side_clouds_db.section[section_id].color[BLUE]  = colors[rand_color][BLUE];
            side_clouds_db.section[section_id].duration = (rand() % (side_clouds_db.maximum_duration - side_clouds_db.minimum_duration)) + side_clouds_db.minimum_duration;
            side_clouds_db.section[section_id].count = side_clouds_db.section[section_id].duration;
            active_sections++;
        }

        // Light Active Sections
        for (section_id = 0; section_id < side_clouds_db.num_of_sections; section_id++)
        {
            if (side_clouds_db.section[section_id].count > 0)
            {
                uint16_t section_duration = side_clouds_db.section[section_id].duration;
                // This is an active section
                side_clouds_db.section[section_id].count--;// = sections[section_id] - 1;

                section_strip = section_id * leds_per_section / MAX_LEDS_IN_STRIP;
                section_first_led = (section_id * leds_per_section) % MAX_LEDS_IN_STRIP;

                if (side_clouds_db.section[section_id].count > (section_duration * 0.9f))
                {
                    // Fade In

                    double fade_in_percentage = ((double)(section_duration - side_clouds_db.section[section_id].count)) / ((double)section_duration * 0.1f);
                    uint8_t color_r = SIDE_CLOUD_SET_POWER(side_clouds_db.section[section_id].color[RED]);//percent_of_rgb[RED] *   SIDE_CLOUD_SET_POWER(200);
                    uint8_t color_g = SIDE_CLOUD_SET_POWER(side_clouds_db.section[section_id].color[GREEN]);//percent_of_rgb[GREEN] * SIDE_CLOUD_SET_POWER(200);
                    uint8_t color_b = SIDE_CLOUD_SET_POWER(side_clouds_db.section[section_id].color[BLUE]);//percent_of_rgb[BLUE] *  SIDE_CLOUD_SET_POWER(200);

                    for (led_id = section_first_led; (led_id < section_first_led + leds_per_section) && (led_id < MAX_LEDS_IN_STRIP); led_id++)
                    {
                        LED_strips[section_strip][led_id][RED]   = color_r * fade_in_percentage;
                        LED_strips[section_strip][led_id][GREEN] = color_g * fade_in_percentage;
                        LED_strips[section_strip][led_id][BLUE]  = color_b * fade_in_percentage;
                    }

                }
                else if (side_clouds_db.section[section_id].count < section_duration * 0.1f)
                {
                    // Fade Out
                    double dim_percentage = (double)side_clouds_db.section[section_id].count / (section_duration * 0.1f);
                    for (led_id = section_first_led; (led_id < section_first_led + leds_per_section) && (led_id < MAX_LEDS_IN_STRIP); led_id++)
                    {
                        LED_strips[section_strip][led_id][RED]   = LED_strips[section_strip][led_id][RED] * dim_percentage;
                        LED_strips[section_strip][led_id][GREEN] = LED_strips[section_strip][led_id][GREEN] * dim_percentage;
                        LED_strips[section_strip][led_id][BLUE]  = LED_strips[section_strip][led_id][BLUE] * dim_percentage;
                    }
                }
                else
                {
                    // Show
                    uint8_t color_r = SIDE_CLOUD_SET_POWER(side_clouds_db.section[section_id].color[RED]);//percent_of_rgb[RED] *   SIDE_CLOUD_SET_POWER(200);
                    uint8_t color_g = SIDE_CLOUD_SET_POWER(side_clouds_db.section[section_id].color[GREEN]);//percent_of_rgb[GREEN] * SIDE_CLOUD_SET_POWER(200);
                    uint8_t color_b = SIDE_CLOUD_SET_POWER(side_clouds_db.section[section_id].color[BLUE]);//percent_of_rgb[BLUE] *  SIDE_CLOUD_SET_POWER(200);

                    for (led_id = section_first_led; (led_id < section_first_led + leds_per_section) && (led_id < MAX_LEDS_IN_STRIP); led_id++)
                    {
                        LED_strips[section_strip][led_id][RED]   = color_r;
                        LED_strips[section_strip][led_id][GREEN] = color_g;
                        LED_strips[section_strip][led_id][BLUE]  = color_b;
                    }
                }

                if (side_clouds_db.section[section_id].count == 0)
                {
                    active_sections--;
                }
            }
        }

        drive_LED_strips();
        HAL_Delay(side_clouds_db.refresh_rate);
    }
}

