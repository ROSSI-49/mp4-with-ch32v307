/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "headfile.h"   /* 添加总的头文件，包含了文件管理部份的Fatfs */

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

//检测disk外设的类型
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	//int result;

//	由于存储设备只用到了SD卡，因此只检测DEV——MMC一种状态即可，其他直接break
	switch (pdrv) {
	case DEV_RAM :
		// result = RAM_disk_status();

		// translate the reslut code here

		// return stat;

		break;

	case DEV_MMC :
		// result = MMC_disk_status();

		// translate the reslut code here

		stat = 0;
		return stat;

	case DEV_USB :
		// result = USB_disk_status();

		// translate the reslut code here

		// return stat;
		break;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

//对磁盘进行初始化
//由于只需要检测SD卡,其他卡就直接当作未初始化,需要使用时再进行检测
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	//int result;

	switch (pdrv) {
	case DEV_RAM :
		// result = RAM_disk_initialize();

		// translate the reslut code here

		// return stat;
		break;

	case DEV_MMC :
		// result = MMC_disk_initialize();

		// translate the reslut code here
		stat = SD_Init();
		return stat;

	case DEV_USB :
		// result = USB_disk_initialize();

		// translate the reslut code here

		// return stat;
		break;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

//读取磁盘中的数据,参考SDIO中的SD_read()
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	//int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		// result = RAM_disk_read(buff, sector, count);

		// translate the reslut code here

		// return res;

	    break;

	case DEV_MMC :
		// translate the arguments here

		// result = MMC_disk_read(buff, sector, count);

		// translate the reslut code here

	    res = SD_ReadDisk(buff, sector, count);

		return res;

	case DEV_USB :
		// translate the arguments here

		// result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		// return res;

	    break;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	//int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		// result = RAM_disk_write(buff, sector, count);

		// translate the reslut code here

		// return res;

	    break;

	case DEV_MMC :
		// translate the arguments here

		// result = MMC_disk_write(buff, sector, count);

		// translate the reslut code here

	    res = SD_WriteDisk(buff, sector, count);

		return res;

	case DEV_USB :
		// translate the arguments here

		// result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		// return res;

	    break;
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

// 用来获取磁盘的各种信息
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
  /*
    GET_SECTOR_SIZE     获取磁盘扇区大小
    GET_BLOCK_SIZE      获取指定擦除的最小单位
    GET_SECTOR_COUNT    获取扇区个数
    RK: 原本的SD卡的定义中,快的大小是比扇区要小的,但是在FatFs中的定义是恰恰相反的
  */

	DRESULT res;
	//int result;

	switch (pdrv) {
	case DEV_RAM :

		// Process of the command for the RAM drive

		// return res;

	    break;

	case DEV_MMC :

		// Process of the command for the MMC/SD card

	    switch(cmd)
	    {
	        case GET_SECTOR_SIZE:
	            *(WORD*) buff = SDCardInfo.CardBlockSize;
	            break;
	        case GET_BLOCK_SIZE:
	            *(DWORD*) buff = SDCardInfo.CardBlockSize;
	            break;
	        case GET_SECTOR_COUNT:
	            *(DWORD*) buff = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;
	            break;
	        case CTRL_SYNC:
	            break;
	    }

		return res;

	case DEV_USB :

		// Process of the command the USB drive

		// return res;

	    break;
	}

	return RES_PARERR;
}

DWORD get_fattime (void)
{
    return 0;
}
