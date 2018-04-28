#ifndef _FLASH_H_
#define _FLASH_H_

/* =========================================================================================== */
/* ==  INCLUDES == */
/* =========================================================================================== */

/* =========================================================================================== */
/* ==  DEFINES == */
/* =========================================================================================== */
#define DATA_FLASH_START_ADDR     FLASH_BASE + 0x6000
#define DATA_FLASH_END_ADDR       DATA_FLASH_START_ADDR + 0xfff
#define FLASH_FAIL    (1)
#define FLASH_SUCCESS (0)
/* =========================================================================================== */
/* ==  ENUMERATORS == */
/* =========================================================================================== */

/* =========================================================================================== */
/* ==  STRUCTS == */
/* =========================================================================================== */

/* =========================================================================================== */
/* ==  FUNCTIONS == */
/* =========================================================================================== */
//void init_flash(void);
//void unlock_flash(void);
uint8_t flashStore(uint32_t *pData, uint32_t address, uint16_t sizeInBytes);
void flashLoad(uint32_t *pData, uint32_t address, uint16_t sizeInBytes);

#endif /* FLASH_H_ */
