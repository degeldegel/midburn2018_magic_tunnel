#ifndef SCHEDULER_H_
#define SCHEDULER_H_

/* =========================================================================================== */
/* ==  DEFINES == */
/* =========================================================================================== */

#define SCHEDULER_NUM_OF_SHOW (12) //Have to be in even number for flash reasons
#define SCHDLR_SHOW_DIDNT_PLAY (0)
#define SCHDLR_SHOW_PLAYED     (1)

/* =========================================================================================== */
/* ==  STRUCTS == */
/* =========================================================================================== */
typedef struct scheduler_show_db
{
    uint8_t showId;
    uint8_t played;
} scheduler_show_db_t;

typedef struct scheduler_db
{
    scheduler_show_db_t show[SCHEDULER_NUM_OF_SHOW];
    uint8_t num_of_shows;
    uint8_t minimum_time_run; //in seconds
    uint8_t maximum_time_run; //in seconds
    uint8_t current_show;
} scheduler_db_t;

/* =========================================================================================== */
/* ==  MACROS == */
/* =========================================================================================== */

/* =========================================================================================== */
/* ==  EXTERNAL FUNCTIONS== */
/* =========================================================================================== */

uint16_t scheduler_timer_handler(void);
void init_scheduler(void);

#endif /* SCHEDULER_H_ */
