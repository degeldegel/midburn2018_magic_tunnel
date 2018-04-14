#ifndef _LED_SHOWS_H
#define _LED_SHOWS_H

typedef struct shows_db
{
    uint8_t direction;
    uint8_t max_power;
} show_db_t;

//typedef struct shows_db
//{
//    showdb_t show[NUM_OF_SHOWS];
//    uint8_t direction;
//} shows_db_t;
#define TRUE  (1)
#define FALSE (0)

#define REGULAR_DIRECTION (0) //From LED #0 to end of strip
#define REVERSE_DIRECTION (1) //From end of strip to LED #0
#define NUM_OF_SHOWS      (4)

typedef enum _shows
{
    SHOWS_SNAKE     = 0,
    SHOWS_RESERVED0 = 1,
    SHOWS_RESERVED1 = 2,
    SHOWS_RESERVED2 = 3
} show_id;

#define SET_POWER(show, power) (uint8_t)(((double)(shows[show].max_power)/100)*power)
/**
  * @brief  snake show, runs colorful snakes through the LED strips.
  * @param  void
  * @retval void
  */
void snake_show(void);

#endif  /* _LED_SHOWS_H */
