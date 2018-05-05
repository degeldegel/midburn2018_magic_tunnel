#include "stm32f4xx_hal.h"
#include <flash.h>

uint8_t flashStore(uint32_t *pData, uint32_t address, uint16_t sizeInBytes)
{
    HAL_StatusTypeDef res = HAL_OK;
    uint8_t wrIdx;
    uint16_t sizeInDW = sizeInBytes>>2;
    HAL_StatusTypeDef status = HAL_FLASH_Unlock();
    while (status != HAL_OK)
    {
        status = HAL_FLASH_Unlock();
    }
    /* erase the whole sector */
    FLASH_Erase_Sector(FLASH_SECTOR_3, VOLTAGE_RANGE_3);
    /* program flash with new data */
    for (wrIdx=0 ; wrIdx < sizeInDW; wrIdx++) {
        res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, *(pData+wrIdx));
        address = address + 4;
        if (res != HAL_OK)
        {
            status = HAL_FLASH_Lock();
            while (status != HAL_OK)
            {
                status = HAL_FLASH_Lock();
            }
            return FLASH_FAIL;
        }
    }
    status = HAL_FLASH_Lock();
    while (status != HAL_OK)
    {
        status = HAL_FLASH_Lock();
    }
    return FLASH_SUCCESS;
}

void flashLoad(uint32_t *pData, uint32_t address, uint16_t sizeInBytes)
{
    uint8_t rdIdx;
    uint16_t sizeInDW = sizeInBytes>>2;

    for (rdIdx=0 ; rdIdx < sizeInDW; rdIdx++) {
        pData[rdIdx] = *(__IO uint32_t*)address;
        address = address + 4;
    }

}
