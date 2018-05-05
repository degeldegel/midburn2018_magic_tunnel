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

















//------------------------- OLD file should be cleaned before commit!!! -------------------------------

///******************************************************************************
// * Private types, enums and structure declarations / variables definitions
// *****************************************************************************/
//__IO HAL_StatusTypeDef FLASHStatus = HAL_OK;
//__IO TestStatus DataMemoryProgramStatus = PASSED;
//uint32_t NbrOfPage = 0, j = 0, Address = 0;
//flash_header_t pHdr;
//
//
///**************************************************************************//**
// * @brief   		unlock_flash
// *
// * @Description		After Reset, the FLASH PECR register and Data EEPROM memory are locked.
// *					To unlock it, the HAL_FLASHEx_DATAEEPROM_Unlock function is used.
// *
// * @Author			oolinky
// *
// * @param   		void
// *
// * @return  		void
// *
// *****************************************************************************/
//void unlock_flash(void)
//{
//	/* Unlock the FLASH */
//	HAL_FLASHEx_DATAEEPROM_Unlock();
//
//	/* Clear all pending flags */
//	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
//	                | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
//}
//
//
///****************************************************************************
// * @brief   		init_flash
// *
// * @Description		After Reset, the FLASH PECR register and Data EEPROM memory are locked.
// *					To unlock it, the HAL_FLASHEx_DATAEEPROM_Unlock function is used.
// *
// *					Before programming the desired data EEPROM memory addresses, an erase operation is performed
// *					using the Data EEPROM erase data word (HAL_FLASHEx_DATAEEPROM_Erase with type FLASH_TYPEERASEDATA_WORD).
// *
// *					The erase procedure starts with the calculation of the number of pages to be used.
// *					Then all these word will be erased one by one by calling HAL_FLASHEx_DATAEEPROM_Erase
// *					function. The erase is then checked to see all values are 0.
// *
// * @Author			oolinky
// *
// * @param   		void
// *
// * @return  		void
// *
// *****************************************************************************/
//void init_flash(void)
//{
//	/*  Data EEPROM Fast Word program of FAST_DATA_32 at addresses defined by
//	    DATA_EEPROM_START_ADDR and DATA_EEPROM_END_ADDR */
//	Address = DATA_EEPROM_START_ADDR;
//
//	NbrOfPage = ((DATA_EEPROM_END_ADDR - Address) + 1 ) >> 2;
//
//	DEBUG(FLASH_CHANNEL,"Erase the Data EEPROM Memory pages by Word (32-bit)\n\r");
//	/* Erase the Data EEPROM Memory pages by Word (32-bit) */
//	for(j = 0; j < NbrOfPage; j++)
//	{
//	  FLASHStatus = HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEERASEDATA_WORD,Address + (4 * j));
//	}
//
//	DEBUG(FLASH_CHANNEL,"Check the correctness of written data -> should be 0\n\r");
//	/* Check the correctness of written data */
//	while(Address < DATA_EEPROM_END_ADDR)
//	{
//	  if(*(__IO uint32_t*)Address != 0x0)
//	  {
//	    DataMemoryProgramStatus = FAILED;
//	    DEBUG(FLASH_CHANNEL,ANSI_COLOR_F_RED"DataMemoryProgramStatus = FAILED\n\r"ANSI_COLOR_RESET);
//	  }
//	  DEBUG(FLASH_CHANNEL,"Address = 0x%x : Data = 0x%x\n\r",(uint)Address,(uint)*(__IO uint32_t*)Address);
//	  Address = Address + 4;
//	}
//}
//
///*****************************************************************************
// * @brief   		Store2Flash
// *
// * @Description		After Reset, the FLASH PECR register and Data EEPROM memory are locked.
// * 					To unlock it, the HAL_FLASHEx_DATAEEPROM_Unlock function is used.
// * 					Once this operation is finished, the data EEPROM memory programming operation will
// * 					be performed using the HAL_FLASHEx_DATAEEPROM_Program function word by word.
// * 					The written data are then checked and the result of the programming
// * 					operation is stored into the DataMemoryProgramStatus variable.
// *
// * @Author			oolinky
// *
// * @param   		pData - pointer to the data
// *
// * @param   		StartAddress - start address
// *
// * @param   		SizeInBytes - size of the written data in bytes - if not a division of 4 -> will round up
// *
// * @return  		DataMemoryProgramStatus - FAILED = 0, PASSED != FAILED
// *
// *****************************************************************************/
//void Store2Flash(uint32_t *pData, uint32_t StartAddress, uint16_t SizeInBytes)
//{
//	uint32_t Address = StartAddress;
////	uint16_t SizeInWords = ceil(SizeInBytes / 4); //TODO: not rounding-up...
//	uint32_t index = 0;
//
//	while(Address < (StartAddress + SizeInBytes))
//	{
//		FLASHStatus = HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, Address, pData[index]);
//
//		if(FLASHStatus == HAL_OK)
//		{
//			Address = Address + 4;
//			index++;
//			DEBUG(FLASH_CHANNEL,"FLASHStatus == HAL_OK\n\r");
//		}
//		else
//		{
//			__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
//							   | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR);
//			ERROR(FLASH_CHANNEL,ANSI_COLOR_F_RED"FLASHStatus != HAL_OK\n\r"ANSI_COLOR_RESET);
//		}
//	}
//
//	Address = StartAddress;
//	index = 0;
//
//	DEBUG(FLASH_CHANNEL,"Check the correctness of written data\n\r");
//	/* Check the correctness of written data */
//	while(Address < (StartAddress + SizeInBytes))
//	{
//	    if(*(__IO uint32_t*)Address != pData[index])
//	    {
//	      DataMemoryProgramStatus = FAILED;
//	    }
//		DEBUG(FLASH_CHANNEL,"Address = 0x%x : Data = 0x%x\n\r",(uint)Address,(uint)*(__IO uint32_t*)Address);
//		Address = Address + 4;
//		index++;
//	}
//
//	if (DataMemoryProgramStatus != PASSED)
//	{
//		ERROR(FLASH_CHANNEL,ANSI_COLOR_F_RED"Flash store failure!\n\r"ANSI_COLOR_RESET);
//	}
//	else
//	{
//		DEBUG(FLASH_CHANNEL,ANSI_COLOR_F_GREEN"Flash store success!\n\r"ANSI_COLOR_RESET);
//	}
//}
//
///*****************************************************************************
// * @brief   		LoadFrFlash
// *
// * @Description		After Reset, the FLASH PECR register and Data EEPROM memory are locked.
// * 					To unlock it, the HAL_FLASHEx_DATAEEPROM_Unlock function is used.
// * 					Once this operation is finished, the data EEPROM memory programming operation will
// * 					be performed using the HAL_FLASHEx_DATAEEPROM_Program function word by word.
// * 					The written data are then checked and the result of the programming
// * 					operation is stored into the DataMemoryProgramStatus variable.
// *
// * @Author			oolinky
// *
// * @param   		pData - pointer to the data
// *
// * @param   		StartAddress - start address
// *
// * @param   		SizeInBytes - size of the read data in bytes - if not a division of 4 -> will round up
// *
// * @return  		void
// *
// *****************************************************************************/
//void LoadFrFlash(uint32_t *pData, uint32_t StartAddress, uint16_t SizeInBytes)
//{
//	uint8_t  i = 0;
//	uint32_t Address = StartAddress;
//	uint16_t SizeInWords = (SizeInBytes < 4) ? 1 : (uint16_t)ceil((double)SizeInBytes / (double)4) ; //TODO: not rounding-up...
//
////	/* Unlock the FLASH */
////	unlock_flash();
//
//	DEBUG(FLASH_CHANNEL,"loading from the flash...\n\r");
//	for (i=0; i<SizeInWords; i++)
//	{
//		pData[i] = *(__IO uint32_t*)Address;
//		DEBUG(FLASH_CHANNEL,"Address = 0x%x : Data = 0x%x\n\r",(uint)Address,(uint)pData[i]);
//		Address = Address + 4;
//	}
//
////	/* Lock the FLASH */
////	lock_flash();
//}
//
///**************************************************************************//**
// * @brief   		check_if_data_from_flash_valid
// *
// * @Description		Load header from flash - word by word, & check that
// * 					the magic-word is correct & if the data is valid.
// *
// * @Author			oolinky
// *
// * @param   		StartAddress - start address
// *
// * @return  		FlashHeaderStatus Status - FLASH_BAD_MAGIC_WORD / FLASH_VALID_DATA / FLASH_INVALID_DATA
// *
// *****************************************************************************/
//FlashHeaderStatus check_if_data_from_flash_valid(uint32_t StartAddress)
//{
//	uint16_t numBytes = sizeof(flash_header_t);
//	FlashHeaderStatus Status;
//
//	DEBUG(FLASH_CHANNEL,"Load header from flash\n\r");
//	LoadFrFlash((uint32_t *)&pHdr, StartAddress, numBytes);
//
//	if(pHdr.magic_word != DATA_EEPROM_MAGIC_WORD)
//	{
//		Status = FLASH_BAD_MAGIC_WORD;
//		WARNING(FLASH_CHANNEL,"\n\rBad MAGICWORD in address 0x%x : MAGICWORD = 0x%x\n\r",(uint)StartAddress,(uint)pHdr.magic_word);
//	}
//	else if(pHdr.valid == DATA_EEPROM_VALID_WORD)
//	{
//		Status = FLASH_VALID_DATA;
//		DEBUG(FLASH_CHANNEL,"\n\rVALID data in address 0x%x\n\r",(uint)StartAddress);
//	}
//	else
//	{
//		Status = FLASH_INVALID_DATA;
//		DEBUG(FLASH_CHANNEL,"\n\rINVALID data in address 0x%x\n\r",(uint)StartAddress);
//	}
//
//	return Status;
//}
//
///**************************************************************************//**
// * @brief   		invalidate_flash_data
// *
// * @Description		write 0x0 to the valid word in the flash header
// *
// * @Author			oolinky
// *
// * @param   		Address
// *
// * @return  		void
// *
// *****************************************************************************/
//void invalidate_flash_data(uint32_t Address)
//{
//	uint16_t numBytes = NUM_OF_BYTES_IN_WORD;
//	uint32_t Buf = 0x0;
//
//	DEBUG(FLASH_CHANNEL,"Invalidate flash data in address 0x%x\n\r",(uint)Address);
//	Store2Flash(&Buf, Address, numBytes);
//}
