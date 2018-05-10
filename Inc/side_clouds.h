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

/**
  * @brief  side clouds main function
  * @param  void
  * @retval void
  */
void side_clouds_show(void);

#endif /* SIDE_CLOUDS_H_ */
