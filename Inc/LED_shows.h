#ifndef _LED_SHOWS_H
#define _LED_SHOWS_H

/* =========================================================================================== */
/* ==  DEFINES == */
/* =========================================================================================== */

#define TRUE  (1)
#define FALSE (0)

#define REGULAR_DIRECTION (0) //From LED #0 to end of strip
#define REVERSE_DIRECTION (1) //From end of strip to LED #0
#define NUM_OF_SHOWS      (4)
#define SNAKE_SHOW_NUM_OF_DIM_STEPS (10) //in how many steps the snakes should fade out (for shut down sequence)
#define SNAKE_SHOW_REFRESH_TIME (30) //period time of refresh rate in milisec
#define SNAKE_SHOW_CYCLE_LENGTH (17) //this is the length of the cycle of one snake (one snake length + gap to next snake)
#define SNAKE_SHOW_SNAKE_LENGTH (15)
#define SNAKE_SHOW_PERFORM_STRATUP_SEQ   (1)
#define SNAKE_SHOW_STARTUP_SEQ_END_CYCLE (300)

/* =========================================================================================== */
/* ==  ENUMS == */
/* =========================================================================================== */

typedef enum _shows
{
    SHOWS_SNAKE     = 0,
    SHOWS_RESERVED0 = 1,
    SHOWS_RESERVED1 = 2,
    SHOWS_RESERVED2 = 3
} show_id_e;

typedef enum _show_status
{
    SHOW_STATUS_DISABLED = 0, //show is not running
    SHOW_STATUS_START    = 1, //show received start running command from interrupt
    SHOW_STATUS_RUNNING  = 2, //show is running
    SHOW_STATUS_STOP     = 3  //show received stop running command from interrupt
} show_status_e;

/* =========================================================================================== */
/* ==  STRUCTS == */
/* =========================================================================================== */

typedef struct show_db
{
	show_status_e status;
    uint8_t direction;
    uint8_t max_power;
} show_db_t;

/* =========================================================================================== */
/* ==  MACROS == */
/* =========================================================================================== */

#define GET_POWER(show, power) (uint8_t)(((double)(shows[show].max_power)/100)*power)

/* =========================================================================================== */
/* ==  EXTERNAL FUNCTIONS== */
/* =========================================================================================== */

/**
  * @brief  show callback function, all shows functions should be of this type.
  * @param  void
  * @retval void
  */
typedef void (*show_cb_function)(void);

/**
  * @brief  snake show, runs colorful snakes through the LED strips.
  * @param  void
  * @retval void
  */
void snake_show(void);

/**
  * @brief  initialize shows database.
  * @param  void
  * @retval void
  */
void init_shows(void);

#endif  /* _LED_SHOWS_H */
