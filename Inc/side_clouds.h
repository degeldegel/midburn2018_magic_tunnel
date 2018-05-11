/*
 * side_clouds.h
 *
 *  Created on: 11 May 2018
 *      Author: danielg
 */

#ifndef SIDE_CLOUDS_H_
#define SIDE_CLOUDS_H_

#define SIDE_CLOUD_MAX_POWER (50)
#define SIDE_CLOUD_SET_POWER(power) (uint8_t)(((double)(SIDE_CLOUD_MAX_POWER)/100)*power)
#define False (0)
#define True (1)

#define MAX_NUM_OF_SECTIONS (96)
#define NUM_OF_SIDE_CLOUD_STRIPS (8)

/* color scheme */
#define NUM_OF_COLORS_PALETTE (20)

/* default defines */
#define DEFAULT_MAX_SECTIONS        (24)
#define DEFAULT_MAX_ACTIVE_SECTIONS (8)
#define DEFAULT_MAX_DURATION        (500)
#define DEFAULT_MIN_DURATION        (300)
#define DEFAULT_REFRESH_RATE_SIDE_CLOUDS (30)

typedef struct _side_cloud_section
{
    uint8_t count;
    uint8_t duration;
    uint8_t color[NUM_OF_CFG_BYTES_PER_LED];
} side_cloud_section_t;

typedef struct _side_clouds
{
    uint8_t num_of_sections;
    uint8_t max_active_sections;
    uint8_t refresh_rate;
    uint16_t maximum_duration;
    uint16_t minimum_duration;
    side_cloud_section_t section[MAX_NUM_OF_SECTIONS];
} side_clouds_t;

/**
  * @brief  side clouds main function
  * @param  void
  * @retval void
  */
void side_clouds_show(void);

/**
  * @brief  initialization of side clouds
  * @param  void
  * @retval void
  */
void init_side_clouds_show(void);

#endif /* SIDE_CLOUDS_H_ */
