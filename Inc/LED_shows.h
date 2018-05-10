#ifndef _LED_SHOWS_H
#define _LED_SHOWS_H

/* =========================================================================================== */
/* ==  DEFINES == */
/* =========================================================================================== */

#define TRUE  (1)
#define FALSE (0)

#define REGULAR_DIRECTION (0) //From LED #0 to end of strip
#define REVERSE_DIRECTION (1) //From end of strip to LED #0
#define NUM_OF_SHOWS      (5)
#define DEFAULT_MAX_POWER (60)
#define NUM_OF_SNAKE_SHOWS (3)
#define DEFAULT_SNAKE_SHOW_FADE_OUT_STEPS (10) //in how many steps the snakes should fade out (for shut down sequence)
#define DEFAULT_SNAKE_SHOW_REFRESH_TIME (30) //period time of refresh rate in milisec
#define DEFAULT_SNAKE_SHOW_CYCLE_LENGTH (17) //this is the length of the cycle of one snake (one snake length + gap to next snake)
#define DEFAULT_SNAKE_SHOW_SNAKE_LENGTH (15)
#define DEFAULT_SNAKE_SHOW_PERFORM_STRATUP_SEQ   (1)
#define DEFAULT_SNAKE_SHOW_STARTUP_SEQ_END_CYCLE (300)

#define TEDDY_BEAR_CYCLE_LENGTH (40)
#define TEDDY_BEAR_SNAKE_LENGTH (37)

#define METEOR_LENGTH (61)
#define TOTAL_EXPLOSION_TIME (100)
#define EXPLOSION_PHASES (2)
#define EXPLOSION_PHASE1_TIME (30)
#define EXPLOSION_PHASE2_TIME (70)
/* =========================================================================================== */
/* ==  ENUMS == */
/* =========================================================================================== */

typedef enum _shows
{
    SHOWS_SNAKE_0    = 0,
    SHOWS_SNAKE_1    = 1,
    SHOWS_SNAKE_2    = 2,
    SHOWS_TEDDY_BEAR = 3,
    SHOWS_METEOR     = 4
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

typedef struct snake_show_db
{ /* for flash purposes needs to be 32 bit aligned */
    uint8_t refresh_time; /* refresh rate in cycle time [milisec] */
    uint8_t fade_out_steps; /* how many fade out steps to perform on shut down */
    uint8_t perform_startup_seq; /* 0x0 - no startup, snakes starting to run from beginning of strip; 0x1 - startup performed and whole strip is filled with snakes at the beginning */
    uint8_t snake_length; /* length of snake in a cycle --> cycle_length=100 and snake_length=50 --> on a 300 led strip there will be three snakes of 50 leds each */
    uint16_t cycle_length; /* length of snake and turned off area --> if cycle_length equals 100 in a 300 led strip it means only three snake will appear (max value = 255) */
    uint16_t starup_seq_end_cycle; /* for how long to run the startup sequence before starting regular */
    //uint32_t reserved;
    uint8_t direction;
    uint8_t max_power;
    uint8_t reserved0;
    uint8_t reserved1;
} snake_show_db_t;

typedef struct flash_show_config_db
{ /* for flash purposes needs to be 32 bit aligned */
    uint32_t magic_word;
    snake_show_db_t snake[NUM_OF_SNAKE_SHOWS];
} flash_show_config_db_t;

/* =========================================================================================== */
/* ==  MACROS == */
/* =========================================================================================== */

#define SET_POWER(show, power) (uint8_t)(((double)(shows[show].max_power)/100)*power)

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
  * @brief  wrapper function for snake show #0
  * @param  void
  * @retval void
  */
void snake_show_0(void);

/**
  * @brief  wrapper function for snake show #1
  * @param  void
  * @retval void
  */
void snake_show_1(void);

/**
  * @brief  wrapper function for snake show #2
  * @param  void
  * @retval void
  */
void snake_show_2(void);

/**
  * @brief  snake show, runs colorful snakes through the LED strips.
  * @param  void
  * @retval void
  */
void snake_show(uint8_t snake_id);

/**
  * @brief  initialize shows database.
  * @param  void
  * @retval void
  */
void init_shows(void);

/**
  * @brief  Stores current configuration to flash
  * @param  void
  * @retval void
  */
void store_config_to_flash(void);

/**
  * @brief  Loads default configuration
  * @param  void
  * @retval void
  */
void load_default_configuration(void);

/**
  * @brief  Loads configuration from flash
  * @param  void
  * @retval void
  */
void load_config_from_flash(void);

void teady_bear(void);

/**
  * @brief  comet show, stars, comet and explosion.
  * @param  void
  * @retval void
  */
void MeteorShow(void);

/**
  * @brief  TwinklingStars - stars shining and dimming down.
  * @param  void
  * @retval void
  */
void TwinklingStars(void);

/**
  * @brief  MeteorDrop - Meteor falling.
  * @param  void
  * @retval void
  */
void MeteorDrop(void);

/**
  * @brief  MeteorExplosion - Boom!.
  * @param  void
  * @retval void
  */
void MeteorExplosion(void);

#endif  /* _LED_SHOWS_H */
