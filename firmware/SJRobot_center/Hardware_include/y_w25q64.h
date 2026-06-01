#ifndef __Y_W25Q64_H__
#define __Y_W25Q64_H__

#include "main.h"
#include "stm32f4xx.h"


/*******SPI快捷指令�?*******/
#define	SPI_FLASH_CS(x)			GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)x)

/*******W25Q系列芯片ID*******/
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16

/*******W25Q64芯片变量宏定�?*******/
#define W25Q64_SECTOR_SIZE	4096		//4K
#define W25Q64_SECTOR_NUM		2048		//8*1024/4 = 2048

/*******W25Q64芯片地址存储�?*******/
#define FLASH_ASC16_ADDRESS                 0
#define FLASH_HZK16_ADDRESS                 0x1000

#define FLASH_SYSTEM_CONFIG_ADDRESS         0x43000

#define FLASH_BITMAP1_SIZE_ADDRESS	        0x50000
#define FLASH_BITMAP2_SIZE_ADDRESS	        FLASH_BITMAP1_SIZE_ADDRESS+0x28000
#define FLASH_BITMAP3_SIZE_ADDRESS	        FLASH_BITMAP2_SIZE_ADDRESS+0x28000
#define FLASH_BITMAP4_SIZE_ADDRESS	        FLASH_BITMAP3_SIZE_ADDRESS+0x28000
#define FLASH_BITMAP5_SIZE_ADDRESS	        FLASH_BITMAP4_SIZE_ADDRESS+0x28000	
#define FLASH_BITMAP6_SIZE_ADDRESS	        FLASH_BITMAP5_SIZE_ADDRESS+0x28000

#define FLASH_BITMAPMAIN_SIZE_ADDRESS       FLASH_BITMAP6_SIZE_ADDRESS+0x28000
#define FLASH_BITMAPDS1302_SIZE_ADDRESS     FLASH_BITMAPMAIN_SIZE_ADDRESS+0x28000
#define FLASH_BITMAPDS18B20_SIZE_ADDRESS    FLASH_BITMAPDS1302_SIZE_ADDRESS+0x28000
#define FLASH_BITMAPBLUETOOTH_SIZE_ADDRESS  FLASH_BITMAPDS18B20_SIZE_ADDRESS+0x28000

/*******FLASH相关指令�?*******/
#define W25X_WriteEnable			0x06 
#define W25X_WriteDisable			0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData					0x03 
#define W25X_FastReadData			0x0B 
#define W25X_FastReadDual			0x3B 
#define W25X_PageProgram			0x02 
#define W25X_SectorErase			0x20 
#define W25X_BlockErase				0xD8 
#define W25X_ChipErase				0xC7 
#define W25X_PowerDown				0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID					0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 

/*******FLASH相关函数声明*******/
void	SpiFlashInit(void);
void    spi_set_speed(uint16_t SpeedSet);
u8		SpiWriteRead(u8 d);
u16		SpiFlashReadID(void);  																								//读取FLASH ID
u8		SpiFlashReadSR(void);  																								//读取状态寄存器 
void 	SpiFlashWriteSR(u8 sr);  																							//写状态寄存器
void 	SpiFlashWriteEnable(void);    																				//写使�? 
void 	SpiFlashWriteDisable(void);																						//写禁�?
char 	SpiFlashReadChar(u32 ReadAddr);																				//读取flash
void 	SpiFlashRead(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   					//读取flash
void 	SpiFlashWrite(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);					//写入flash
void 	SpiFlashWriteS(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//
void    SpiFlashWriteChar(char tmp,u32 WriteAddr);														//单字节写入flash
void 	SpiFlashWritePage(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);			//单页写入flash
void 	SpiFlashWriteSector(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);		//单扇区写入flash
void 	SpiFlashWriteNoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);		//无检验写入flash
void 	SpiFlashEraseSector(u32 Dst_Addr);																		//扇区擦除
void 	SpiFlashEraseChip(void);    	  																			//整片擦除
void 	SpiFlashWaitBusy(void);           																		//等待空闲
void 	SpiFlashPowerDown(void);      																			  //进入掉电模式
void	SpiFlashWakeUp(void);	
void    setup_w25q64(void);	//初始化存储器W25Q64

#define PRE_CMD_SIZE 128
#define DJ_NUM 8

//typedef struct {
//	uint8_t 	valid;//有效 TODO	
//	uint16_t 	aim;	//执行目标
//	uint16_t 	time;	//执行时间		
//	float 		cur;	//当前�?
//	float 		inc;	//增量	
//}duoji_t;
//extern duoji_t duoji_doing[DJ_NUM];





#define w25x_init() SpiFlashInit()
#define w25x_readId() SpiFlashReadID()
#define w25x_read(buf, addr, len) SpiFlashRead(buf, addr, len)
#define w25x_write(buf, addr, len) SpiFlashWriteNoCheck(buf, addr, len)
#define w25x_writeS(buf, addr, len) SpiFlashWriteS(buf, addr, len)
#define w25x_erase_sector(addr) SpiFlashEraseSector(addr)
#define w25x_wait_busy() SpiFlashWaitBusy()

#endif
