/************************************ Copyright Notice *******************************************
 *																								 
 * This is a source file for the MODBUS Communication Master Driver Stack developed by Sunlux 
 * Technologies Ltd., Bangalore. Sunlux retains ownership of the products and as such exposes the 
 * code to registered customers who have purchased this Source Code Library from Sunlux. The 
 * customer is bound to protect the confidentiality of the source code and also bind all its 
 * employees and other related entities to protect the confidentiality of the source code. The 
 * source code cannot be sold for cost or free to any third party by the end customers. By the 
 * sale of this source code library Sunlux provides the customer license to port this SCL to the 
 * native platform of the customer and use it on one product only. The code is licensed 
 * royalty-free on multiple units of the same product. Use of the library on further products 
 * can be made only if further licenses are purchased by the customer from Sunlux. Infringement 
 * and/or violation of this Copyright in any form is an offence and is punishable under the 
 * Indian Copyright Laws. Jurisdiction: Bangalore, India
 * Sunlux Technologies Ltd. can be contacted at the following address:
 *	Sunlux Technologies Ltd., 497, 6th 'A' Main, H I G Colony, R M V II Stage
 *	Bangalore - 560 094, India
 *	Ph: ++91 80 23417073	Email: info@sunlux-india.com		Web: www.sunlux-india.com
 *
 *************************************************************************************************/

/********************************************************************************************************
*  MBframes.c: This file contains the functions for frame building and parsing.
*			  This file also contains functions for calculating and checking CRC.  	                    
*														
* Software Edition No: 1.0			  										
* -----------------------------------
* Modification history
* -----------------------------------
* SlNo,Date,	By,		  Details
* -----------------------------------													
* 01,04/04/2001,SRIKANTH,  Written
* 02,01/12/2002,RMD,  Reduced the Number of Files. Only Two C files(MBDriver.c & MBFrames.c)
*					  and Three Header files (MBMaster.h, MBDefs.h & MBlinkUser.h) retained.
* 03,01/01/2003,RMD,  Added code for MODBUS over TCP/IP.
* 04,13/10/2003,SAN,  Implemented Construct and Parse functions for Function Codes 
*					  07,08,0B,0C,11,14,15,16,17,18,2B,2B/0D.
* 05,02/08/2006,GHO,  Protected several functions and calls in macros to allow selective inclusion
*					  and exclusion of support modbus functions. Not all functions covered yet
* 06,02/08/2006,GHO,  Added macros CRC_TABLE_IN_RAM, CRC_TABLE_IN_ROM & CRC_TABLE_CREATE_DYNAMIC
*						to enable users to choose placement of CRC table
* 07,02/08/2006,GHO,  Removed all references to g_FrameConstructFunctionArray as this is no longer
*						used in MBDriver.c
* 08,11/11/2006,GHO,  Add all remaining functions under macros to enable inclusion or exclusion of 
*						the functions by setting a value for the macros.
*******************************************************************************************************/


#include "MBMaster.h"
#include "MB_UserPort.h"


#define POLY_CRC16  0xA001	/*Polynomial for CRC calculation*/
static	xdata MBDEFS_LONG InitOK = 0;

extern  xdata MBDEFS_UINT8 MBSendBuffer[];
//extern  xdata MBDEFS_UINT8 MBReceiveBuffer[20];
extern  xdata MBDEFS_UCHAR MBErrNo;

	#if CRC_TABLE_IN_RAM
		static	xdata MBDEFS_UINT8 Table1[256];
		static	xdata MBDEFS_UINT8 Table2[256];
	#elif CRC_TABLE_IN_ROM
		const MBDEFS_UINT8 Table1[256] = {
		0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
		0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
		0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
		0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
		0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
		0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
		0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
		0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
		0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
		0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
		0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
		0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
		0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
		0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
		0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
		0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
		0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
		0x40
		};
	
		const MBDEFS_UINT8 Table2[256] = {
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
		0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
		0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
		0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
		0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
		0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
		0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
		0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
		0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
		0x40
		};
	#endif

/********************************************************************************
 *  FUNCTION	: CrcInit  				     	                                    *
 *																				*
 *  PARAMETERS	: None												            *
 *																				*	                                                                
 *  DESCRIPTION	:                                                                *
 *																				*
 *  RETURNS		: None								                                *
 *																				*
 ********************************************************************************/
#if CRC_TABLE_IN_RAM
MBDEFS_NONE CrcInit(MBDEFS_NONE)
{
	xdata MBDEFS_UINT16 mask, Crc, mem;
	xdata MBDEFS_UINT8 nBit;
   	for(mask = 0; mask < 256; mask++)
    {
    	Crc = mask;
     	for(nBit = 0; nBit < 8; nBit++)
        {
      		mem = Crc & 0x0001;
      		Crc /= 2;
      		if(mem != 0)
			{
      	 		Crc ^= POLY_CRC16;
			}
        }
    	Table2[mask] = Crc & 0xff;
     	Table1[mask] = Crc >> 8; 	 		
    }

 }  
#endif

/************************************************************************************
 * Function		: MakeOrCheckCrc
 * Author		: PSV
 * Description	:  i) Given a MODBUS frame appends a 2 byte CRC to it
 *				  ii) Given a MODBUS frame with 2 byte CRC checks if CRC is good/bad
 * Parameters	: a) IN/OUT Frame   : Frame for which CRC must be appended/whose CRC
 *										must be checked
 *				  b) IN FrameLength : Size of frame (including 2 CRC bytes)
 *				  c) IN Action		: Action to perform MAKE=Make CRC, CHECK=Check CRC
 * Returns		: TRUE if CRC Good, FALSE if CRC Bad (TRUE if Action = MAKE)
 ************************************************************************************/
MBDEFS_BOOL MakeOrCheckCrc(MBDEFS_UINT8* Frame, MBDEFS_UINT16 FrameLength, MBDEFS_UINT8 Action)
{
	xdata MBDEFS_UINT8 car, i;
	xdata MBDEFS_UINT8 sCrc[2];

	#if CRC_TABLE_CREATE_DYNAMIC
		xdata MBDEFS_UINT16 Crc, mem;
		xdata MBDEFS_UINT8 nbit;
	#endif

	sCrc[0] = 0xff;
	sCrc[1] = 0xff;

	if(InitOK == 0)
	{
		#if CRC_TABLE_IN_RAM
			CrcInit();
		#endif
		InitOK = 1;
	}

	for(i=0; i < (FrameLength-2); i++)
	{
		car = Frame[i];
		car ^= sCrc[0];

		#if CRC_TABLE_CREATE_DYNAMIC
			Crc = car;
			for(nbit=0; nbit < 8; nbit++)
			{
				mem = Crc & (MBDEFS_UINT16)0x0001;
				Crc /= 2;
				if(mem != 0)
				{
					Crc ^= (MBDEFS_UINT16)POLY_CRC16;
				}
			}
			sCrc[0] = sCrc[1] ^ (MBDEFS_UINT8)(Crc & 0x00ff);
			sCrc[1] = (MBDEFS_UINT8)(Crc >> 8); 	 		
		#else		
			sCrc[0] = sCrc[1] ^ Table2[car];
			sCrc[1] = Table1[car];
		#endif
	}

	if( Action == MAKE )
	{	/* Append the calculated CRC to the end of the frame */
		Frame[i++] = sCrc[0];
		Frame[i]   = sCrc[1];
	}
	else
	{	/* compare CRC */
		if( ( Frame[i] != sCrc[0] ) || ( Frame[i+1] != sCrc[1] ) )
		{
			#if DEBUGENABLED
				printf("CRC Err\n");
			#endif
			return MBDEFS_FALSE;
		}
	}
	return MBDEFS_TRUE;
}


#if (READ_COILS_SUPPORTED || DISCRETE_INPUTS_SUPPORTED)
/*--------------------------------------------------------------------------*
 * Function to construct frame for function type 1 : Read Coils             *
 *--------------------------------------------------------------------------*/
MBDEFS_BOOL ConstructFC1(MODBUSLINKINFO *SendInfo)	
{
	/*-----*/

	#if DEBUGENABLED
		if(SendInfo->FunctionCode == 1)
		{
			printf("\nFC0x01: Read coils");
		}
		else
		{
			printf("\nFC0x02: Read discrete inputs");
		}
	#endif
	/*Check if number of coils to read is exceeding the maximum value*/
	if(SendInfo->Quantity > MAXCOILSREAD)
	{
		#if DEBUGENABLED
			printf("\nNo. of coils/Registers to read exceeding\n");
		#endif
		MBErrNo = EXCEED_NO_OF_ITEMS;
		return(MBDEFS_FALSE);
	}

	/*Calculate the expected response bytes*/
	SendInfo->ResponseFramelength += ((SendInfo->Quantity-1)/8 + 1) + 1;/*num of bytes for packing coils + One Byte cout*/
	
	/*-----*/


	MBSendBuffer[SLAVEADDRESS] = SendInfo->SlaveInfo.SlaveNo;
    MBSendBuffer[FUNCTION] = SendInfo->FunctionCode;
	MBSendBuffer[STARTADDRESSH] = *((MBDEFS_UINT8 *)(&SendInfo->UserPassedInfo ->VarAddress) + HIGH);
    MBSendBuffer[STARTADDRESSL] = *((MBDEFS_UINT8 *)(&SendInfo->UserPassedInfo ->VarAddress) + LOW);
	MBSendBuffer[STARTADDRESSL +1] =  ((MBDEFS_UINT8 *)(&(SendInfo->Quantity)))[HIGH];
    MBSendBuffer[STARTADDRESSL +2] =  ((MBDEFS_UINT8 *)(&(SendInfo->Quantity)))[LOW];
	#if !(MODBUS_TCP)
		MakeOrCheckCrc( MBSendBuffer, SendInfo->FrameLength, MAKE );
	#endif

	return(MBDEFS_TRUE);
}
#endif	/* READ_COILS_SUPPORTED || DISCRETE_INPUTS_SUPPORTED */

#if DISCRETE_INPUTS_SUPPORTED
/*--------------------------------------------------------------------------*
* Function to construct frame for function type 2 : Read Discrete inputs    *
*--------------------------------------------------------------------------*/
MBDEFS_BOOL ConstructFC2(MODBUSLINKINFO *SendInfo)	
{
	 return (ConstructFC1(SendInfo));
}
#endif /* DISCRETE_INPUTS_SUPPORTED */


#if (READ_HOLDING_REGISTERS_SUPPORTED || INPUT_REGISTERS_SUPPORTED)
/*--------------------------------------------------------------------------*
 * Function to construct frame for function type 3 : Read Holding Registers *
 *--------------------------------------------------------------------------*/
MBDEFS_BOOL ConstructFC3(MODBUSLINKINFO *SendInfo)	
{
	/*-----*/
	#if DEBUGENABLED
		if(SendInfo->FunctionCode == 3)
		{
			printf("\nFC0x03: Read holding registers");
		}
		else
		{
			printf("\nFC0x04: Read input registers");
		}
	#endif

	/*Check if number of registers to read is exceeding the maximum value*/
	if(SendInfo->Quantity > MAXREGSREAD)
	{
		#if DEBUGENABLED
			printf("\nNo. of coils/Registers to read exceeding\n");
		#endif
		MBErrNo = EXCEED_NO_OF_ITEMS;
		return(MBDEFS_FALSE);
	}
	SendInfo->ResponseFramelength += (SendInfo->Quantity * 2) + 1;/*num of registers * 2 + One Byte cout*/
	/*-----*/


	MBSendBuffer[SLAVEADDRESS] = SendInfo->SlaveInfo.SlaveNo;
    MBSendBuffer[FUNCTION] = SendInfo->FunctionCode;
	MBSendBuffer[STARTADDRESSH] = *((MBDEFS_UINT8 *)(&SendInfo->UserPassedInfo ->VarAddress) + HIGH);
    MBSendBuffer[STARTADDRESSL] = *((MBDEFS_UINT8 *)(&SendInfo->UserPassedInfo ->VarAddress) + LOW);
	MBSendBuffer[STARTADDRESSL +1] =  ((MBDEFS_UINT8 *)(&(SendInfo->Quantity)))[HIGH];
    MBSendBuffer[STARTADDRESSL +2] =  ((MBDEFS_UINT8 *)(&(SendInfo->Quantity)))[LOW];
	#if !(MODBUS_TCP)
		MakeOrCheckCrc( MBSendBuffer, SendInfo->FrameLength, MAKE );
	#endif

	return(MBDEFS_TRUE);
}
#endif /* READ_HOLDING_REGISTERS_SUPPORTED || INPUT_REGISTERS_SUPPORTED */

#if INPUT_REGISTERS_SUPPORTED
/*--------------------------------------------------------------------------*
 * Function to construct frame for function type 4 : Read Input registers   *
 *--------------------------------------------------------------------------*/
MBDEFS_BOOL ConstructFC4(MODBUSLINKINFO *SendInfo)	
{
	 return (ConstructFC3(SendInfo));	 
}
#endif /* INPUT_REGISTERS_SUPPORTED */


#if (WRITE_COILS_SUPPORTED || WRITE_HOLDING_REGISTERS_SUPPORTED)
/*--------------------------------------------------------------------------*
 * Function to construct frame for function type 5 : Write single coil      *
 *--------------------------------------------------------------------------*/
MBDEFS_BOOL ConstructFC5(MODBUSLINKINFO *SendInfo)	
{
	/*--*/
	#if DEBUGENABLED
		if(SendInfo->FunctionCode == 5)
		{
			printf("\nFC0x05: Preset single coil");
		}
		else
		{
			printf("\nFC0x06: Preset single register");
		}
	#endif

	SendInfo->Data = (DATA *) SendInfo->UserPassedInfo ->varData;
	/*Calculate the expected response bytes*/
	SendInfo->ResponseFramelength += 4;		
	/*--*/

	if(SendInfo->FunctionCode == FORCESINGLECOIL && SendInfo->Data[0].Register > 0)
	{
		SendInfo->Data[0].Register = 0xff00;
	}
	MBSendBuffer[SLAVEADDRESS] = SendInfo->SlaveInfo.SlaveNo;
    MBSendBuffer[FUNCTION] = SendInfo->FunctionCode;
	MBSendBuffer[STARTADDRESSH] = *((MBDEFS_UINT8 *)(&SendInfo->UserPassedInfo ->VarAddress) + HIGH);
    MBSendBuffer[STARTADDRESSL] = *((MBDEFS_UINT8 *)(&SendInfo->UserPassedInfo ->VarAddress) + LOW);
	MBSendBuffer[STARTADDRESSL +1] =  ((MBDEFS_UINT8 *)(&(SendInfo->Data[0].Register)))[HIGH];
    MBSendBuffer[STARTADDRESSL +2] =  ((MBDEFS_UINT8 *)(&(SendInfo->Data[0].Register)))[LOW];
	#if !(MODBUS_TCP)
		MakeOrCheckCrc( MBSendBuffer, SendInfo->FrameLength, MAKE );
	#endif

	return(MBDEFS_TRUE);
}
#endif /* WRITE_COILS_SUPPORTED || WRITE_HOLDING_REGISTERS_SUPPORTED */


#if WRITE_HOLDING_REGISTERS_SUPPORTED
/*--------------------------------------------------------------------------*
 * Function to construct frame for function type 6 : Write single register  *			    
 *--------------------------------------------------------------------------*/
MBDEFS_BOOL ConstructFC6(MODBUSLINKINFO *SendInfo)	
{
	return(ConstructFC5(SendInfo));
}
#endif /* WRITE_HOLDING_REGISTERS_SUPPORTED */


#if GET_EXCEPTION_COIL_DATA_SUPPORTED
/*--------------------------------------------------------------------------*
 * Function to construct frame for function type 7 : Read Exception status  *
 *--------------------------------------------------------------------------*/
MBDEFS_BOOL ConstructFC7(MODBUSLINKINFO *SendInfo)	
{
	#if DEBUGENABLED
		printf("\nFC0x07: Read Exception status");		
	#endif
#if !(MODBUS_TCP)
	SendInfo->ResponseFramelength += 1; /* Total =6, Sl.Add + FC + 2 Bytes Sub FC + 2 Bytes Data = 6 */

	MBSendBuffer[SLAVEADDRESS] = SendInfo->SlaveInfo.SlaveNo;
    MBSendBuffer[FUNCTION] = SendInfo->FunctionCode;
		
	MakeOrCheckCrc( MBSendBuffer, SendInfo->FrameLength, MAKE );
	
	return(MBDEFS_TRUE);
#else
	#if DEBUGENABLED
		printf("\nFC0x07: Read Exception status not supported by Master");		
	#endif
	MBErrNo = UNKNOWN_REQUEST;
	return(MBDEFS_FALSE);
#endif
}
#endif /* GET_EXCEPTION_COIL_DATA_SUPPORTED */


#if DIAGNOSTICS_SUPPORTED
/*------------------------------------------------------------------------------------------*
 * Function to construct frame for function type 8 : Diagnostics							*
 *------------------------------------------------------------------------------------------*/
MBDEFS_BOOL ConstructFC8(MODBUSLINKINFO *SendInfo)	
{
	#if DEBUGENABLED
		printf("\nFC0x08: Diagnostics");		
	#endif
#if !(MODBUS_TCP)
	SendInfo->DiagInfo =(DIAGNOSTIC_INFO*)calloc(1,sizeof(DIAGNOSTIC_INFO));
	SendInfo->DiagInfo->SubFC =	SendInfo->UserPassedInfo->varData[0];
	SendInfo->DiagInfo->Data  = SendInfo->UserPassedInfo->varData[1];
	SendInfo->ResponseFramelength += 4; /* Total =6, Sl.Add + FC + 2 Bytes Sub FC + 2 Bytes Data Field = 6 */

	MBSendBuffer[SLAVEADDRESS] = SendInfo->SlaveInfo.SlaveNo;
    MBSendBuffer[FUNCTION] = SendInfo->FunctionCode;

	MBSendBuffer[DIAGNOSTICSSUBFCHI]  = ((MBDEFS_UINT8 *)(&(SendInfo->DiagInfo ->SubFC )))[HIGH];
    MBSendBuffer[DIAGNOSTICSSUBFCLOW] = ((MBDEFS_UINT8 *)(&(SendInfo->DiagInfo ->SubFC )))[LOW];

	MBSendBuffer[DIAGNOSTICSDATAHI]  = ((MBDEFS_UINT8 *)(&(SendInfo->DiagInfo ->Data  )))[HIGH];
    MBSendBuffer[DIAGNOSTICSDATALOW] = ((MBDEFS_UINT8 *)(&(SendInfo->DiagInfo ->Data  )))[LOW];
		
	MakeOrCheckCrc( MBSendBuffer, SendInfo->FrameLength, MAKE );
	return(MBDEFS_TRUE);
#else
	#if DEBUGENABLED
		printf("\nFC0x08: Diagnostics not supported by Master");		
	#endif
	MBErrNo = UNKNOWN_REQUEST;
	return(MBDEFS_FALSE);
#endif
}
#endif /* DIAGNOSTICS_SUPPORTED */


#if FETCH_COMM_EVENT_COUNTER_SUPPORTED
/*------------------------------------------------------------------------------------*
 * Function to construct frame for function type 11 : Get Communication event counter *
 *------------------------------------------------------------------------------------*/
MBDEFS_BOOL ConstructFC11(MODBUSLINKINFO *SendInfo)	
{
	#if DEBUGENABLED
		printf("\nFC0x0B: Get Communication event counter");		
	#endif
#if !(MODBUS_TCP)
	SendInfo->ResponseFramelength += 4; /* Total =6, Sl.Add + FC + Status (2 Bytes)
													+ Event Count (2 Bytes) = 6 */
	MBSendBuffer[SLAVEADDRESS]	= SendInfo->SlaveInfo.SlaveNo;
    MBSendBuffer[FUNCTION]		= SendInfo->FunctionCode;
		
	MakeOrCheckCrc( MBSendBuffer, SendInfo->FrameLength, MAKE );
	return(MBDEFS_TRUE);
#else
	#if DEBUGENABLED
		printf("\nFC0x0B: Get Communication event counter not supported by Master");		
	#endif
	MBErrNo = UNKNOWN_REQUEST;
	return(MBDEFS_FALSE);
#endif
} 
#endif /* FETCH_COMM_EVENT_COUNTER_SUPPORTED */

#if FETCH_COMM_EVENT_LOG_SUPPORTED
/*-------------------------------------------------------------------------------*
 * Function to construct frame for function type 12 : Get communication event log* 
 *-------------------------------------------------------------------------------*/
MBDEFS_BOOL ConstructFC12(MODBUSLINKINFO *SendInfo)	
{
	#if DEBUGENABLED
		printf("\nFC0x0C: Get Communication event log");		
	#endif
#if !(MODBUS_TCP)
	SendInfo->ResponseFramelength += 1; /* Total =3, Sl.Add + FC + Byte Count = 3 */

	MBSendBuffer[SLAVEADDRESS]	= SendInfo->SlaveInfo.SlaveNo;
    MBSendBuffer[FUNCTION]		= SendInfo->FunctionCode;
		
	MakeOrCheckCrc( MBSendBuffer, SendInfo->FrameLength, MAKE );
	return(MBDEFS_TRUE);
#else
	#if DEBUGENABLED
		printf("\nFC0x0C: Get Communication event log not supported by Master");		
	#endif
	MBErrNo = UNKNOWN_REQUEST;
	return(MBDEFS_FALSE);
#endif
}
#endif /* FETCH_COMM_EVENT_LOG_SUPPORTED */


#if WRITE_COILS_SUPPORTED
/*--------------------------------------------------------------------------*
 * Function to construct frame for function type 15 : Write Multiple Coils  *
 *--------------------------------------------------------------------------*/
MBDEFS_BOOL ConstructFC15(MODBUSLINKINFO *SendInfo)	
{
    xdata MBDEFS_UINT16 Count, StartByte;
	xdata MBDEFS_UINT8 Mask = 0, Shift;
	
	/*--*/
	#if DEBUGENABLED
		printf("\nFC0x0F: Write multiple coils");		
	#endif
	/*Check if nor of coils to write is exceeding the maximum value*/
	if(SendInfo->Quantity > MAXCOILSWRITE)
	{
		#if DEBUGENABLED
			printf("\nNo. of coils to write exceeding\n");
		#endif
		MBErrNo = EXCEED_NO_OF_ITEMS;
		return(MBDEFS_FALSE);
	}
	SendInfo->NBytes = ((SendInfo->Quantity - 1)/8) + 1 ;
	SendInfo->FrameLength += SendInfo->NBytes;
	SendInfo->Data = (DATA *)SendInfo->UserPassedInfo ->varData;
	/*Calculate the expected response bytes*/
	SendInfo->ResponseFramelength += 4;
	/*--*/

    MBSendBuffer[SLAVEADDRESS]	= SendInfo->SlaveInfo.SlaveNo;
    MBSendBuffer[FUNCTION]		= SendInfo->FunctionCode;
	MBSendBuffer[STARTADDRESSH] = *((MBDEFS_UINT8 *)(&SendInfo->UserPassedInfo ->VarAddress) + HIGH);
    MBSendBuffer[STARTADDRESSL] = *((MBDEFS_UINT8 *)(&SendInfo->UserPassedInfo ->VarAddress) + LOW);
	MBSendBuffer[REQUESTQUANTITY] = ((MBDEFS_UINT8 *)(&(SendInfo->Quantity)))[HIGH];
    MBSendBuffer[REQUESTQUANTITY + 1] = ((MBDEFS_UINT8 *)(&(SendInfo->Quantity)))[LOW];
	MBSendBuffer[BYTECOUNTREQUEST] = SendInfo->NBytes;
	
	StartByte = BYTECOUNTREQUEST+1;
	Shift = 0;
	MBSendBuffer[StartByte] = 0;

	/* Put each value into frame */
	for(Count = 0; Count < SendInfo->Quantity; Count++)
    {
		Mask = (MBDEFS_UINT8)SendInfo->Data[Count].Bit;
	  	Mask <<= Shift++;
	  	MBSendBuffer[StartByte] |= Mask;

	  	if (Shift == 8)
      	{
			StartByte++;
	   		Shift = 0;
			MBSendBuffer[StartByte] = 0;
		}
    }
	#if !(MODBUS_TCP)
		MakeOrCheckCrc( MBSendBuffer, SendInfo->FrameLength, MAKE );
	#endif
	
	return(MBDEFS_TRUE);
}
#endif /* WRITE_COILS_SUPPORTED */

#if WRITE_HOLDING_REGISTERS_SUPPORTED
/*-----------------------------------------------------------------------------*
 * Function to construct frame for function type 16 : Write Multiple registers *
 *-----------------------------------------------------------------------------*/
MBDEFS_BOOL ConstructFC16(MODBUSLINKINFO *SendInfo)	
{
    xdata MBDEFS_UINT16 Count,StartByte;

	/*--*/
	#if DEBUGENABLED
		printf("\nFC0x10: Write multiple registers");		
	#endif
	/*Check if no. of registers to write is exceeding the maximum value*/
	if(SendInfo->Quantity > MAXREGSWRITE)
	{
		#if DEBUGENABLED
			printf("\nNo. of Registers to write exceeding\n");
		#endif
		MBErrNo = EXCEED_NO_OF_ITEMS;
		return(MBDEFS_FALSE);
	}		
	SendInfo->NBytes = SendInfo->Quantity * 2;
	SendInfo->FrameLength += SendInfo->NBytes;
	SendInfo->Data = (DATA *)SendInfo->UserPassedInfo ->varData;
	/*Calculate the expected response bytes*/
	SendInfo->ResponseFramelength += 4;
	/*--*/

	MBSendBuffer[SLAVEADDRESS] = SendInfo->SlaveInfo.SlaveNo;
    MBSendBuffer[FUNCTION] = SendInfo->FunctionCode;
	MBSendBuffer[STARTADDRESSH] = *((MBDEFS_UINT8 *)(&SendInfo->UserPassedInfo ->VarAddress) + HIGH);
    MBSendBuffer[STARTADDRESSL] = *((MBDEFS_UINT8 *)(&SendInfo->UserPassedInfo ->VarAddress) + LOW);
	MBSendBuffer[REQUESTQUANTITY] = (( MBDEFS_UINT8 *)(&(SendInfo->Quantity)))[HIGH];
    MBSendBuffer[REQUESTQUANTITY + 1] = (( MBDEFS_UINT8 *)(&(SendInfo->Quantity)))[LOW];
	MBSendBuffer[BYTECOUNTREQUEST] = SendInfo->NBytes;
	StartByte = BYTECOUNTREQUEST+1;
	
	for(Count = 0; Count < SendInfo->Quantity ; Count++)
    {
		MBSendBuffer[StartByte++] = (( MBDEFS_UINT8 *)(&(SendInfo->Data[Count].Register)))[HIGH];
    	MBSendBuffer[StartByte++] = (( MBDEFS_UINT8 *)(&(SendInfo->Data[Count].Register)))[LOW];		
    }
	#if !(MODBUS_TCP)
		MakeOrCheckCrc( MBSendBuffer, SendInfo->FrameLength, MAKE );
	#endif

	return(MBDEFS_TRUE);
}
#endif /* WRITE_HOLDING_REGISTERS_SUPPORTED */

#if REPORT_SLAVE_ID_SUPPORTED
/*------------------------------------------------------------------------------------------*
 * Function to construct frame for function type 17 : Report Slave ID						*
 *------------------------------------------------------------------------------------------*/
MBDEFS_BOOL ConstructFC17(MODBUSLINKINFO *SendInfo)	
{
	#if DEBUGENABLED
		printf("\nFC0x11: Report slave ID");		
	#endif
#if !(MODBUS_TCP)
	SendInfo->ResponseFramelength += 3; /* Total =5, Sl.Add + FC + Byte Count + Slave Id +
													+ Run Indicator Status = 5 */

	MBSendBuffer[SLAVEADDRESS] = SendInfo->SlaveInfo.SlaveNo;
    MBSendBuffer[FUNCTION] = SendInfo->FunctionCode;
	
	MakeOrCheckCrc( MBSendBuffer, SendInfo->FrameLength, MAKE );
	return(MBDEFS_TRUE);
#else
	#if DEBUGENABLED
		printf("\nFC0x11: Report Slave ID not supported by Master");		
	#endif
	MBErrNo = UNKNOWN_REQUEST;
	return(MBDEFS_FALSE);	
#endif	
}
#endif /* REPORT_SLAVE_ID_SUPPORTED */

#if READ_FILE_RECORD_SUPPORTED
/*--------------------------------------------------------------------------*
 * Function to construct frame for function type 20 : Read File Record		*
 *--------------------------------------------------------------------------*/
MBDEFS_BOOL ConstructFC20(MODBUSLINKINFO *SendInfo)	
{
	xdata MBDEFS_UINT16 Count,StartByte,MaxItems;
	xdata MBDEFS_UINT8 offset;

	/*--*/
	#if DEBUGENABLED
		printf("\nFC0x14: Read file record");		
	#endif
	/* NItems will determine the number of GROUP_REFERENCE_INFO required */
	SendInfo->ExtraInfo  = (GROUP_REFERENCE_INFO*)calloc(SendInfo->Quantity,sizeof(GROUP_REFERENCE_INFO));
	
	SendInfo->NBytes = SendInfo->Quantity * 7; /* 1 GROUP_REFERENCE_INFO = 7 Bytes */
	
	offset = 0;
	for (MaxItems=0; MaxItems<SendInfo->Quantity; MaxItems++)
	{
		SendInfo->ExtraInfo[MaxItems].ReferenceType =	(MBDEFS_UINT8)SendInfo->UserPassedInfo ->varData[0];
		SendInfo->ExtraInfo[MaxItems].Fileno 		=	SendInfo->UserPassedInfo ->varData[1 + offset];	
		SendInfo->ExtraInfo[MaxItems].StartAddress	=	SendInfo->UserPassedInfo ->varData[2 + offset];
		SendInfo->ExtraInfo[MaxItems].Count			=	SendInfo->UserPassedInfo ->varData[3 + offset];
		offset += 4;
	}			

	SendInfo->FrameLength += SendInfo->NBytes;

	/* Calculate the expected response bytes */
	SendInfo->ResponseFramelength += 1;

	/*--*/

	MBSendBuffer[SLAVEADDRESS]		= SendInfo->SlaveInfo.SlaveNo;
    MBSendBuffer[FUNCTION]			= SendInfo->FunctionCode;
	MBSendBuffer[FC2021BYTECOUNTREQ]	= SendInfo->NBytes ;

	StartByte = FC2021BYTECOUNTREQ+1;
	
	/* Copy Ref Type, File No. Record No. and Record Length */
	for(Count = 0; Count < (SendInfo->NBytes/7) ; Count++)
    {
		MBSendBuffer[StartByte++] = SendInfo->ExtraInfo[Count].ReferenceType ;
		MBSendBuffer[StartByte++] = (( MBDEFS_UINT8 *)(&(SendInfo->ExtraInfo[Count].Fileno )))[HIGH];
		MBSendBuffer[StartByte++] = (( MBDEFS_UINT8 *)(&(SendInfo->ExtraInfo[Count].Fileno )))[LOW];
		MBSendBuffer[StartByte++] = (( MBDEFS_UINT8 *)(&(SendInfo->ExtraInfo[Count].StartAddress)))[HIGH];
		MBSendBuffer[StartByte++] = (( MBDEFS_UINT8 *)(&(SendInfo->ExtraInfo[Count].StartAddress)))[LOW];
		MBSendBuffer[StartByte++] = (( MBDEFS_UINT8 *)(&(SendInfo->ExtraInfo[Count].Count )))[HIGH];
		MBSendBuffer[StartByte++] = (( MBDEFS_UINT8 *)(&(SendInfo->ExtraInfo[Count].Count )))[LOW];	
		
    }

	#if !(MODBUS_TCP)
		MakeOrCheckCrc( MBSendBuffer, SendInfo->FrameLength, MAKE );
	#endif
	free(SendInfo->ExtraInfo);

	return(MBDEFS_TRUE);
}
#endif /* READ_FILE_RECORD_SUPPORTED */

#if WRITE_FILE_RECORD_SUPPORTED
/*--------------------------------------------------------------------------*
 * Function to construct frame for function type 21 : Write File Record     *
 *--------------------------------------------------------------------------*/
MBDEFS_BOOL ConstructFC21(MODBUSLINKINFO *SendInfo)	
{
	xdata MBDEFS_UINT16 Count,StartByte,MaxItems;

	/*--*/
	#if DEBUGENABLED
		printf("\nFC0x15: Write file record");		
	#endif
	/* NItems will determine the number of GROUP_REFERENCE_INFO required */
	SendInfo->ExtraInfo  = (GROUP_REFERENCE_INFO*)calloc(1,sizeof(GROUP_REFERENCE_INFO));
	
	SendInfo->NBytes = SendInfo->Quantity * 7; /* 1 GROUP_REFERENCE_INFO = 7 Bytes */
	
	SendInfo->ExtraInfo[0].ReferenceType	=	(MBDEFS_UINT8)SendInfo->UserPassedInfo->varData[0];
	SendInfo->ExtraInfo[0].Fileno 			=	*((MBDEFS_UINT16*)&SendInfo->UserPassedInfo->varData[1]);	
	SendInfo->ExtraInfo[0].StartAddress		=	*((MBDEFS_UINT16*)&SendInfo->UserPassedInfo->varData[2]);
	SendInfo->ExtraInfo[0].Count			=	*((MBDEFS_UINT16*)&SendInfo->UserPassedInfo->varData[3]);

	SendInfo->ExtraInfo->Data   = (MBDEFS_UINT16*)calloc(SendInfo->ExtraInfo[0].Count,sizeof(MBDEFS_UINT16));

	for (MaxItems=0; MaxItems < SendInfo->ExtraInfo[0].Count ; MaxItems++)
	{
		*(SendInfo->ExtraInfo->Data + MaxItems) = SendInfo->UserPassedInfo->varData[4 + MaxItems];
	}

	SendInfo->FrameLength += (SendInfo->ExtraInfo[0].Count * 2) + SendInfo->NBytes;
	
	/* Calculate the expected response bytes */
	SendInfo->ResponseFramelength += 1;

	/*--*/
	MBSendBuffer[SLAVEADDRESS]		= SendInfo->SlaveInfo.SlaveNo;
    MBSendBuffer[FUNCTION]			= SendInfo->FunctionCode;
	/* Total number of Bytes = (Record Length * 2 Bytes) + Ref Type + File No. (2 Bytes) + 
								Record No. (2 Bytes) + Record Length (2 Bytes) */
	MBSendBuffer[FC2021BYTECOUNTREQ] = ( SendInfo->ExtraInfo ->Count * 2 ) + 7 ;

	StartByte = FC2021BYTECOUNTREQ + 1;

	/* Copy Ref Type, File No. Record No. and Record Length */
	MBSendBuffer[StartByte++] = SendInfo->ExtraInfo[0].ReferenceType ;
	MBSendBuffer[StartByte++] = (( MBDEFS_UINT8 *)(&(SendInfo->ExtraInfo[0].Fileno )))[HIGH];
	MBSendBuffer[StartByte++] = (( MBDEFS_UINT8 *)(&(SendInfo->ExtraInfo[0].Fileno )))[LOW];
	MBSendBuffer[StartByte++] = (( MBDEFS_UINT8 *)(&(SendInfo->ExtraInfo[0].StartAddress)))[HIGH];
	MBSendBuffer[StartByte++] = (( MBDEFS_UINT8 *)(&(SendInfo->ExtraInfo[0].StartAddress)))[LOW];
	MBSendBuffer[StartByte++] = (( MBDEFS_UINT8 *)(&(SendInfo->ExtraInfo[0].Count )))[HIGH];
	MBSendBuffer[StartByte++] = (( MBDEFS_UINT8 *)(&(SendInfo->ExtraInfo[0].Count )))[LOW];
	
	/* Copy the data to the frame that has to be written */
	for (Count=0; Count < SendInfo->ExtraInfo ->Count; Count ++) 
	{
		MBSendBuffer[StartByte++] = (( MBDEFS_UINT8 *)(&(SendInfo->ExtraInfo[0].Data[Count] )))[HIGH];
		MBSendBuffer[StartByte++] = (( MBDEFS_UINT8 *)(&(SendInfo->ExtraInfo[0].Data[Count] )))[LOW];	
	}
	
	#if !(MODBUS_TCP)
		MakeOrCheckCrc( MBSendBuffer, SendInfo->FrameLength, MAKE );
	#endif
	
	free(SendInfo->ExtraInfo->Data );
	free(SendInfo->ExtraInfo);

	return(MBDEFS_TRUE);
}
#endif /* WRITE_FILE_RECORD_SUPPORTED */

#if MASK_WRITE_REGISTER_SUPPORTED
/*--------------------------------------------------------------------------------------*
 * Function to construct frame for function type 22 : Mask Write Register				*
 *--------------------------------------------------------------------------------------*/
MBDEFS_BOOL ConstructFC22(MODBUSLINKINFO *SendInfo)	
{
	/*--*/
	#if DEBUGENABLED
		printf("\nFC0x16: Mask write register");		
	#endif
	SendInfo->MASKInfo  = (MASK_INFO *)calloc(1,sizeof(MASK_INFO));
	SendInfo->MASKInfo[0].RegAdd  = SendInfo->UserPassedInfo ->VarAddress;
	SendInfo->MASKInfo[0].ANDMask =	SendInfo->UserPassedInfo ->varData[0];
	SendInfo->MASKInfo[0].ORMask  =	SendInfo->UserPassedInfo ->varData[1];

	/*Calculate the expected response bytes*/
	SendInfo->ResponseFramelength += 6;

	/*--*/

	MBSendBuffer[SLAVEADDRESS] = SendInfo->SlaveInfo.SlaveNo;
    MBSendBuffer[FUNCTION] = SendInfo->FunctionCode;
	MBSendBuffer[STARTADDRESSH] = (( MBDEFS_UINT8 *)(&(SendInfo->MASKInfo [0].RegAdd )))[HIGH];
    MBSendBuffer[STARTADDRESSL] = (( MBDEFS_UINT8 *)(&(SendInfo->MASKInfo [0].RegAdd )))[LOW];
	
	
	MBSendBuffer[ANDMASKHI]	= (( MBDEFS_UINT8 *)(&(SendInfo->MASKInfo [0].ANDMask )))[HIGH];
   	MBSendBuffer[ANDMASKLOW]= (( MBDEFS_UINT8 *)(&(SendInfo->MASKInfo [0].ANDMask )))[LOW];
	MBSendBuffer[ORMASKHI]	= (( MBDEFS_UINT8 *)(&(SendInfo->MASKInfo [0].ORMask  )))[HIGH];
   	MBSendBuffer[ORMASKLOW] = (( MBDEFS_UINT8 *)(&(SendInfo->MASKInfo [0].ORMask  )))[LOW];
	
	
	#if (DEBUGENABLED)
		printf("\nsending AND Mask:%x%x  & OR Mask: %x%x",MBSendBuffer[ANDMASKHI],MBSendBuffer[ANDMASKLOW],
			MBSendBuffer[ORMASKHI], MBSendBuffer[ORMASKLOW]);
	#endif

	#if !(MODBUS_TCP)
		MakeOrCheckCrc( MBSendBuffer, SendInfo->FrameLength, MAKE );
	#endif
	free(SendInfo->MASKInfo);

	return(MBDEFS_TRUE);
}  
#endif /* MASK_WRITE_REGISTER_SUPPORTED */

#if READ_WRITE_REGISTERS_SUPPORTED
/*----------------------------------------------------------------------------------*
 * Function to construct frame for function type 23 : Read/Write Multiple Registers *
 *----------------------------------------------------------------------------------*/
MBDEFS_BOOL ConstructFC23(MODBUSLINKINFO *SendInfo)	
{
	xdata MBDEFS_UINT16 Count,StartByte;
	/*--*/
	#if DEBUGENABLED
		MBDEFS_UINT16 TempAdd;
		printf("\nFC0x17: Read/Write multiple registers");		
	#endif
	/* Read Info */
	SendInfo->ReadRegInfo = (READ_REG_INFO*)calloc(1,sizeof(READ_REG_INFO));
	SendInfo->ReadRegInfo ->StartAdd = SendInfo->UserPassedInfo ->VarAddress;
	SendInfo->ReadRegInfo ->RegQuantity = SendInfo->Quantity ;

	SendInfo->ResponseFramelength += (SendInfo->Quantity * 2) + 1; /*num of registers * 2 + One Byte cout*/

	/* Write Info */
	SendInfo->WriteRegInfo = (WRITE_REG_INFO*)calloc(1,sizeof(WRITE_REG_INFO));
	SendInfo->WriteRegInfo ->StartAdd = SendInfo->UserPassedInfo ->varData[0];
	SendInfo->WriteRegInfo ->RegQuantity = SendInfo->UserPassedInfo ->varData[1];
	SendInfo->WriteRegInfo ->ByteCount = SendInfo->WriteRegInfo ->RegQuantity * 2;
	
	SendInfo->WriteRegInfo ->RegVals =(MBDEFS_UINT16*)calloc(SendInfo->WriteRegInfo ->ByteCount ,sizeof(MBDEFS_UINT16));
//	memcpy(SendInfo->WriteRegInfo ->RegVals, &SendInfo->UserPassedInfo ->varData[2], SendInfo->WriteRegInfo ->ByteCount );			

	SendInfo->FrameLength += SendInfo->WriteRegInfo ->ByteCount;
	/*--*/

	/* Frame Header */
	MBSendBuffer[SLAVEADDRESS] = SendInfo->SlaveInfo.SlaveNo;
    MBSendBuffer[FUNCTION] = SendInfo->FunctionCode;
	
	/* Read Registers */
	MBSendBuffer[STARTADDRESSH] = ((MBDEFS_UINT8 *)(&(SendInfo->ReadRegInfo ->StartAdd)))[HIGH];
    MBSendBuffer[STARTADDRESSL] = ((MBDEFS_UINT8 *)(&(SendInfo->ReadRegInfo ->StartAdd)))[LOW];
	MBSendBuffer[STARTADDRESSL +1] =  ((MBDEFS_UINT8 *)(&(SendInfo->ReadRegInfo ->RegQuantity)))[HIGH];
    MBSendBuffer[STARTADDRESSL +2] =  ((MBDEFS_UINT8 *)(&(SendInfo->ReadRegInfo ->RegQuantity)))[LOW];
	
	/* Write Registers */ 
	MBSendBuffer[STARTADDRESSL +3] = (( MBDEFS_UINT8 *)(&(SendInfo->WriteRegInfo ->StartAdd )))[HIGH];
    MBSendBuffer[STARTADDRESSL +4] = (( MBDEFS_UINT8 *)(&(SendInfo->WriteRegInfo ->StartAdd )))[LOW];
	MBSendBuffer[STARTADDRESSL +5] = (( MBDEFS_UINT8 *)(&(SendInfo->WriteRegInfo ->RegQuantity)))[HIGH];
    MBSendBuffer[STARTADDRESSL +6] = (( MBDEFS_UINT8 *)(&(SendInfo->WriteRegInfo ->RegQuantity)))[LOW];
	MBSendBuffer[STARTADDRESSL +7] = SendInfo->WriteRegInfo ->ByteCount;
	StartByte = STARTADDRESSL +8;
	
	#if (DEBUGENABLED)
		printf("\nData to write: Address\t  Data");
		TempAdd = SendInfo->WriteRegInfo ->StartAdd;
	#endif
	for(Count = 0; Count < SendInfo->WriteRegInfo ->RegQuantity ; Count++)
    {
		#if (DEBUGENABLED)
			printf("\n\t\t%4x\t%6x",TempAdd,SendInfo->WriteRegInfo->RegVals[Count]);
			TempAdd++;
		#endif

		MBSendBuffer[StartByte++] = (( MBDEFS_UINT8 *)(SendInfo->WriteRegInfo ->RegVals + Count))[HIGH];
    	MBSendBuffer[StartByte++] = (( MBDEFS_UINT8 *)(SendInfo->WriteRegInfo ->RegVals + Count))[LOW];
				
    }

	/* CRC */
	#if !(MODBUS_TCP)
		MakeOrCheckCrc( MBSendBuffer, SendInfo->FrameLength, MAKE );
	#endif
	
	/* free the memory */
	free(SendInfo->ReadRegInfo );
	free(SendInfo->WriteRegInfo ->RegVals );
	free(SendInfo->WriteRegInfo );

	return(MBDEFS_TRUE);
}
#endif /* READ_WRITE_REGISTERS_SUPPORTED */

#if READ_FIFO_QUEUE_SUPPORTED
/*--------------------------------------------------------------------------*
 * Function to construct frame for function type 24 : Read FIFO Queue       *
 *--------------------------------------------------------------------------*/
MBDEFS_BOOL ConstructFC24(MODBUSLINKINFO *SendInfo)	
{
	/*--*/
	#if DEBUGENABLED
		printf("\nFC0x18: Read FIFO Queue");		
	#endif
	/* Read Info */
	SendInfo->FIFOQueueInfo = (FIFOQUEUE_INFO*)calloc(1,sizeof(FIFOQUEUE_INFO));
	SendInfo->FIFOQueueInfo ->FIFOPntr  = SendInfo->UserPassedInfo ->VarAddress;
	
	SendInfo->ResponseFramelength +=  2; /* Sl Add + FC +  2 Byte Bytecount = 4 */			

	/*--*/

	MBSendBuffer[SLAVEADDRESS] = SendInfo->SlaveInfo.SlaveNo;
    MBSendBuffer[FUNCTION] = SendInfo->FunctionCode;
	MBSendBuffer[STARTADDRESSH] = (( MBDEFS_UINT8 *)(&(SendInfo->FIFOQueueInfo ->FIFOPntr )))[HIGH];
    MBSendBuffer[STARTADDRESSL] = (( MBDEFS_UINT8 *)(&(SendInfo->FIFOQueueInfo ->FIFOPntr )))[LOW];
	
	
	#if (DEBUGENABLED)
		printf("\nFIFO Pntr Address:%x%x",MBSendBuffer[STARTADDRESSH], MBSendBuffer[STARTADDRESSL]);
	#endif

	#if !(MODBUS_TCP)
		MakeOrCheckCrc( MBSendBuffer, SendInfo->FrameLength, MAKE );
	#endif
	free(SendInfo->FIFOQueueInfo );

	return(MBDEFS_TRUE);
}
#endif /* READ_FIFO_QUEUE_SUPPORTED */

#if ENCAPSULATED_INTERFACE_TRANSPORT_SUPPORTED
/*--------------------------------------------------------------------------------------------*
 * Function to construct frame for function type 43 : Encapsulated Interface Transport        *
 *--------------------------------------------------------------------------------------------*/
MBDEFS_BOOL ConstructFC43(MODBUSLINKINFO *SendInfo)		
{
	/*--*/
	xdata MBDEFS_UINT8 NBytes;

	#if DEBUGENABLED
		printf("\nFC0x2B: Encapsulated Interface Transport");		
	#endif
	/* Check whether the limit of data bytes in the frame is exceeding		*/
	if(SendInfo->Quantity  > MEITYPESPECIFICDATA) /* NItems = MEI Type + Following Bytes */
	{								 /* varData[0] => MEI Type					*/
		#if DEBUGENABLED
			printf("\nMEI Type Specific Data exceeding\n");
		#endif
		MBErrNo = EXCEED_NO_OF_ITEMS;
		return(MBDEFS_FALSE);
	}

	if((SendInfo->MEIInfo  = (MEI_INFO *)calloc(1,sizeof(MEI_INFO))) == MBDEFS_NULL)
	{
		#if DEBUGENABLED
			printf("\nError in Memory Allocation\n");
		#endif
		MBErrNo = NOMEMORY;
		return(MBDEFS_FALSE);
	}

	if( (SendInfo->MEIInfo ->MEIData  = (MBDEFS_UCHAR *)calloc(SendInfo->Quantity,sizeof(MBDEFS_UCHAR))) == MBDEFS_NULL )
	{
		#if DEBUGENABLED
			printf("\nError in Memory Allocation\n");
		#endif
		free (SendInfo->MEIInfo );
		SendInfo->MEIInfo = MBDEFS_NULL;
		MBErrNo = NOMEMORY;
		return(MBDEFS_FALSE);
	}

	SendInfo->MEIInfo ->MEIType = (MBDEFS_UINT8) SendInfo->UserPassedInfo ->varData[0];
	for( NBytes=1; NBytes<SendInfo->Quantity; NBytes++)
	{
		*(SendInfo->MEIInfo ->MEIData + NBytes - 1) = (MBDEFS_UINT8) SendInfo->UserPassedInfo ->varData[NBytes];
	}
	SendInfo->FrameLength += SendInfo->Quantity;
	/*Calculate the expected response bytes -- Sl.Add + FC + MEI Type = 3 Bytes*/
	SendInfo->ResponseFramelength += 1; /* Total = 3 */

	/*--*/

	MBSendBuffer[SLAVEADDRESS] = SendInfo->SlaveInfo.SlaveNo;
    MBSendBuffer[FUNCTION] = SendInfo->FunctionCode;
	switch (SendInfo->MEIInfo ->MEIType )
	{
	case READDEVICEIDENTIFICATION:
	{
			#if DEBUGENABLED
				printf("\nSub function: Read device identification ");		
			#endif
			MBSendBuffer[MEITYPE]		= SendInfo->MEIInfo ->MEIType;
			MBSendBuffer[READDEVIDCODE] = *(SendInfo->MEIInfo ->MEIData);
			MBSendBuffer[MBOBJECTID]		= *(SendInfo->MEIInfo ->MEIData + 1);

	}
	break;
	default:
		{
		}
		break;
	}
	
	#if !(MODBUS_TCP)
		MakeOrCheckCrc( MBSendBuffer, SendInfo->FrameLength, MAKE );
	#endif

	return(MBDEFS_TRUE);	
}
#endif /* ENCAPSULATED_INTERFACE_TRANSPORT_SUPPORTED */


#if (READ_COILS_SUPPORTED || DISCRETE_INPUTS_SUPPORTED)
/*-------------------------------------------------------------------------------------------*
 * Function to parse Response frame  for function type 1 : Read Coils						 *
 *-------------------------------------------------------------------------------------------*/
MBDEFS_UINT8 ParseFC1(MODBUSLINKINFO *SendInfo,MODBUSLINKINFO *RecvInfo,MBDEFS_UINT16 *varData)
{   
   	 xdata MBDEFS_UINT8 Shift, Byte;
   	 xdata MBDEFS_UINT16 NBytes;
	 xdata MBDEFS_UINT16 i;
#if DEBUGENABLED
	 MBDEFS_UINT16 Add = SendInfo->UserPassedInfo->VarAddress;
#endif
		
 #if !(MODBUS_TCP)
	 MBDEFS_UINT8 Success;

   	 if ((Success = MakeOrCheckCrc( MBReceiveBuffer, RecvInfo->FrameLength, CHECK)))
     {
   		 #if (DEBUGENABLED)
   			printf("\nCRC Success");
   		 #endif /*(DEBUGENABLED)*/
 #endif /*(MODBUS_TCP)*/
   		 if (MBReceiveBuffer[FUNCTION] & 0x80)
         {
		 	 #if (DEBUGENABLED)
		 	 printf("\n Exception Response %x",MBReceiveBuffer[EXCEPTIONCODE]);
//		 	 fflush(stdout);
		  	 #endif
		  	 return(MBReceiveBuffer[EXCEPTIONCODE]);
		 }

   		 /*RecvInfo->Data = (DATA *)calloc(RecvInfo->Quantity,sizeof(DATA));*/
		 RecvInfo->FunctionCode = MBReceiveBuffer[FUNCTION];
   		 RecvInfo->SlaveInfo.SlaveNo = MBReceiveBuffer[SLAVEADDRESS];
   		 Byte = MBReceiveBuffer[DATAOFFSETRESPONSE];
   		 Shift = 0;
  		 NBytes = 0;
  		 for(i = 0; i < SendInfo->Quantity; i++)
  		 {
			  if ((Byte >> Shift++) & 0x01)
              {
				   	varData[i] = 0x01; 
              }
		 	  else
              {

		 		    varData[i] = 0x00; 
              }

			  if(Shift == 8)
			  {
			  		Shift = 0;
					NBytes++;
					Byte = MBReceiveBuffer[DATAOFFSETRESPONSE+NBytes];
			  }
		 }
		 #if DEBUGENABLED
			if( MBReceiveBuffer[FUNCTION] == 1)
		   		printf("\nCoils Data:     Address\t\tValue\n");
			else
				printf("\nDiscrete Inputs:Address\t\tValue\n");
			for(i = 0; i< RecvInfo->Quantity ; i++)
			{
				Add++;
				printf("\t\t%4x\t\t%4x  ",Add,varData[i]);
				printf("\n");				 
			}
		 #endif
		 return(SUCCESS);
 #if !(MODBUS_TCP)
     }
	 else
     {
		#if (DEBUGENABLED)
		printf("\nCRC Error");
		#endif
		return(CRCERROR);
	 }
 #endif /*(MODBUS_TCP)*/
}
#endif /* (READ_COILS_SUPPORTED || DISCRETE_INPUTS_SUPPORTED) */

#if DISCRETE_INPUTS_SUPPORTED
/*------------------------------------------------------------------------------*
 * Function to parse Response frame  for function type 2 : Read Discrete inputs *
 *------------------------------------------------------------------------------*/
MBDEFS_UINT8 ParseFC2(MODBUSLINKINFO *SendInfo,MODBUSLINKINFO *RecvInfo,MBDEFS_UINT16 *varData)
{	 
	return(ParseFC1(SendInfo,RecvInfo,varData));
}
#endif
 
#if (READ_HOLDING_REGISTERS_SUPPORTED || INPUT_REGISTERS_SUPPORTED)
/*--------------------------------------------------------------------------------*
 * Function to parse Response frame  for function type 3 : Read Holding Registers *
 *--------------------------------------------------------------------------------*/
 MBDEFS_UINT8 ParseFC3(MODBUSLINKINFO *SendInfo,MODBUSLINKINFO *RecvInfo,MBDEFS_UINT16 *varData)
 {
	 xdata MBDEFS_UINT8 StartByte;
   	 xdata MBDEFS_UINT16 i;
#if DEBUGENABLED
	 MBDEFS_UINT16 Add = SendInfo->UserPassedInfo->VarAddress;
#endif

#if !(MODBUS_TCP)
	 xdata MBDEFS_UINT8 Success;

   	 if ((Success = MakeOrCheckCrc( MBReceiveBuffer, RecvInfo->FrameLength, CHECK)))
     {
         #if DEBUGENABLED
   		 	printf("\nCRC Success");
   		 #endif /*(DEBUGENABLED)*/
#endif /*(MODBUS_TCP)*/
   		 if (MBReceiveBuffer[FUNCTION] & 0x80)
         {
		 	return(MBReceiveBuffer[EXCEPTIONCODE]);
		  
         }

   		 /*RecvInfo->Data = (DATA *)calloc(RecvInfo->Quantity,sizeof(DATA));*/
		 RecvInfo->FunctionCode = MBReceiveBuffer[FUNCTION];
   		 RecvInfo->SlaveInfo.SlaveNo = MBReceiveBuffer[SLAVEADDRESS];
   		 StartByte = DATAOFFSETRESPONSE;
		 #if DEBUGENABLED
		   	printf("\nRegister Data:  Address\t\tValue\n");
		 #endif
		 for(i = 0; i< RecvInfo->Quantity ; i++)
         {
			 ((MBDEFS_UINT8 *)(&(varData[i])))[HIGH] = MBReceiveBuffer[StartByte++];
			 ((MBDEFS_UINT8 *)(&(varData[i])))[LOW] = MBReceiveBuffer[StartByte++];
			 #if DEBUGENABLED
				Add++;
				printf("\t\t%4x\t\t%4x  ",Add,varData[i]);
				printf("\n");
			 #endif

         }
		 return(SUCCESS);
 #if !(MODBUS_TCP)
     }
	 else
     {
	  	#if DEBUGENABLED
	 		printf("\nCRC Error");
//	  		fflush(stdout);
	  	#endif
	  	return(CRCERROR);
     }
 #endif /*(MODBUS_TCP)*/

 }
#endif /* (READ_HOLDING_REGISTERS_SUPPORTED || INPUT_REGISTERS_SUPPORTED) */
 
#if INPUT_REGISTERS_SUPPORTED
/*------------------------------------------------------------------------------*
 * Function to parse Response frame  for function type 4 : Read Input registers *
 *------------------------------------------------------------------------------*/
MBDEFS_UINT8 ParseFC4(MODBUSLINKINFO *SendInfo,MODBUSLINKINFO *RecvInfo,MBDEFS_UINT16 *varData)
{
	return(ParseFC3(SendInfo,RecvInfo,varData));
}
#endif


#if (WRITE_COILS_SUPPORTED || WRITE_HOLDING_REGISTERS_SUPPORTED)
/*---------------------------------------------------------------------------*
 * Function to parse Response frame  for function type 5 : Write single coil *
 *---------------------------------------------------------------------------*/
 MBDEFS_UINT8 ParseFC5(MODBUSLINKINFO *SendInfo,MODBUSLINKINFO *RecvInfo,MBDEFS_UINT16 *varData)
 {
	 
 #if !(MODBUS_TCP)
	 xdata MBDEFS_UINT8 Success;

   	 if ((Success = MakeOrCheckCrc( MBReceiveBuffer, RecvInfo->FrameLength, CHECK)))
     {
		#if DEBUGENABLED
   		 	printf("\nCRC Success");
   		#endif
 #endif /*(MODBUS_TCP)*/
   		if (MBReceiveBuffer[FUNCTION] & 0x80)
        {
		  return(MBReceiveBuffer[EXCEPTIONCODE]);
		}

		RecvInfo->FunctionCode = MBReceiveBuffer[FUNCTION];
   		RecvInfo->SlaveInfo.SlaveNo = MBReceiveBuffer[SLAVEADDRESS];

		return(SUCCESS);
 #if !(MODBUS_TCP)
     }
	 else
     {
	  	#if DEBUGENABLED
	 		printf("\nCRC Error");
//	  		fflush(stdout);
	  	#endif
	  	return(CRCERROR);
     }
 #endif /*(MODBUS_TCP)*/
 }
#endif	/* (WRITE_COILS_SUPPORTED || WRITE_HOLDING_REGISTERS_SUPPORTED) */


#if WRITE_HOLDING_REGISTERS_SUPPORTED
/*--------------------------------------------------------------------------------*
 * Function to parse Response frame  for function type 6 : Write single registers *
 *--------------------------------------------------------------------------------*/
MBDEFS_UINT8 ParseFC6(MODBUSLINKINFO *SendInfo,MODBUSLINKINFO *RecvInfo,MBDEFS_UINT16 *varData)
{
	return(ParseFC5(SendInfo,RecvInfo,varData));
}
#endif


#if GET_EXCEPTION_COIL_DATA_SUPPORTED
/*-------------------------------------------------------------------------------*
 * Function to parse Response frame  for function type 7 : Read Exception status *
 *-------------------------------------------------------------------------------*/
 MBDEFS_UINT8 ParseFC7(MODBUSLINKINFO *SendInfo,MODBUSLINKINFO *RecvInfo,MBDEFS_UINT16 *varData)
 {
#if !(MODBUS_TCP)
	 xdata MBDEFS_UINT8 Success;
 	 
   	 if ((Success = MakeOrCheckCrc( MBReceiveBuffer, RecvInfo->FrameLength, CHECK)))
     {
		#if DEBUGENABLED
   		 	printf("\nCRC Success");
   		#endif

   		if (MBReceiveBuffer[FUNCTION] & 0x80)
        {
		  return(MBReceiveBuffer[EXCEPTIONCODE]);
		}

		 RecvInfo->FunctionCode			= MBReceiveBuffer[FUNCTION];
		 RecvInfo->SlaveInfo.SlaveNo	= MBReceiveBuffer[SLAVEADDRESS];
	
		 varData[0] =*((MBDEFS_UINT16 *) &MBReceiveBuffer[EXCEPTIONSTATUS]);
		 #if DEBUGENABLED
		   	printf("\nException Coil Status: %x --",*(MBDEFS_UINT8 *) &varData[0]);
			for(Success = 0; Success < 8; Success++)
			{
				printf(" %d",((*(MBDEFS_UINT8 *) &varData[0] << Success) & 0x80) >> 7);
			}
		 #endif
	
		 return(SUCCESS);
     }
	 else
     {
	  	#if DEBUGENABLED
	 		printf("\nCRC Error");
//	  		fflush(stdout);
	  	#endif
	  	return(CRCERROR);
     }
#endif /*(MODBUS_TCP)*/

	return(MBDEFS_FALSE);
 }
#endif /* GET_EXCEPTION_COIL_DATA_SUPPORTED */


#if DIAGNOSTICS_SUPPORTED
/*--------------------------------------------------------------------------------*
 * Function to parse Response frame  for function type 8 : Diagnostics			  *
 *--------------------------------------------------------------------------------*/
 MBDEFS_UINT8 ParseFC8(MODBUSLINKINFO *SendInfo,MODBUSLINKINFO *RecvInfo,MBDEFS_UINT16 *varData)
 {
#if !(MODBUS_TCP)
	 xdata MBDEFS_UINT8 Success;
 	 
   	 if ((Success = MakeOrCheckCrc( MBReceiveBuffer, RecvInfo->FrameLength, CHECK)))
     {
		#if DEBUGENABLED
   		 	printf("\nCRC Success");
   		#endif

   		if (MBReceiveBuffer[FUNCTION] & 0x80)
        {
		  return(MBReceiveBuffer[EXCEPTIONCODE]);
		}

		 RecvInfo->FunctionCode			= MBReceiveBuffer[FUNCTION];
		 RecvInfo->SlaveInfo.SlaveNo	= MBReceiveBuffer[SLAVEADDRESS];
	
		 *( ((MBDEFS_UINT8 *) &varData[0]) + HIGH ) = MBReceiveBuffer[DIAGNOSTICSSUBFCHI];
		 *( ((MBDEFS_UINT8 *) &varData[0]) + LOW )  = MBReceiveBuffer[DIAGNOSTICSSUBFCHI + 1];
		 *( ((MBDEFS_UINT8 *) &varData[1]) + HIGH ) = MBReceiveBuffer[DIAGNOSTICSDATAHI];
		 *( ((MBDEFS_UINT8 *) &varData[1]) + LOW )  = MBReceiveBuffer[DIAGNOSTICSDATAHI + 1];
		 

		 #if DEBUGENABLED
		   	printf("\nDiagnostic Sub FC: 0x%x",varData[0]);
			printf("\nDiagnostic Data: 0x%x",varData[1]);
		 #endif
	
		 return(SUCCESS);
     }
	 else
     {
	  	#if DEBUGENABLED
	 		printf("\nCRC Error");
//	  		fflush(stdout);
	  	#endif
	  	return(CRCERROR);
     }
#endif /*(MODBUS_TCP)*/

	return(MBDEFS_FALSE);

 }
#endif /* DIAGNOSTICS_SUPPORTED */

#if FETCH_COMM_EVENT_COUNTER_SUPPORTED
/*------------------------------------------------------------------------------------------*
 * Function to parse Response frame  for function type 11 : Get Communication event counter *
 *------------------------------------------------------------------------------------------*/
 MBDEFS_UINT8 ParseFC11(MODBUSLINKINFO *SendInfo,MODBUSLINKINFO *RecvInfo,MBDEFS_UINT16 *varData)
 {
#if !(MODBUS_TCP)
	 xdata MBDEFS_UINT8 Success;
 	 
   	 if ((Success = MakeOrCheckCrc( MBReceiveBuffer, RecvInfo->FrameLength, CHECK)))
     {
		#if DEBUGENABLED
   		 	printf("\nCRC Success");
   		#endif

   		if (MBReceiveBuffer[FUNCTION] & 0x80)
        {
		  return(MBReceiveBuffer[EXCEPTIONCODE]);
		}

		 RecvInfo->FunctionCode			= MBReceiveBuffer[FUNCTION];
		 RecvInfo->SlaveInfo.SlaveNo	= MBReceiveBuffer[SLAVEADDRESS];
	
		 ((MBDEFS_UINT8 *)(&(varData[0])))[HIGH] = MBReceiveBuffer[FC11STATUSHI];
		 ((MBDEFS_UINT8 *)(&(varData[0])))[LOW]  = MBReceiveBuffer[FC11STATUSLOW];
		 ((MBDEFS_UINT8 *)(&(varData[1])))[HIGH] = MBReceiveBuffer[FC11EVENTCOUNTHI];
		 ((MBDEFS_UINT8 *)(&(varData[1])))[LOW]  = MBReceiveBuffer[FC11EVENTCOUNTLOW];

		 #if DEBUGENABLED
		   	printf("\nEvent Count: %d",varData[1]);
			if(varData[0] == 0xffff)
				printf("\nSlave Status: Busy");
			else
				printf("\nSlave Status: Not Busy");
		 #endif	
		
		 return(SUCCESS);
     }
	 else
     {
	  	#if DEBUGENABLED
	 		printf("\nCRC Error");
//	  		fflush(stdout);
	  	#endif
	  	return(CRCERROR);
     }
#endif /*(MODBUS_TCP)*/

	return(MBDEFS_FALSE);

 }
#endif /* FETCH_COMM_EVENT_COUNTER_SUPPORTED */

#if FETCH_COMM_EVENT_LOG_SUPPORTED
/*--------------------------------------------------------------------------------------*
 * Function to parse Response frame  for function type 12 : Get Communication event log *
 *--------------------------------------------------------------------------------------*/
 MBDEFS_UINT8 ParseFC12(MODBUSLINKINFO *SendInfo,MODBUSLINKINFO *RecvInfo,MBDEFS_UINT16 *varData)
 {
#if !(MODBUS_TCP)
	 xdata MBDEFS_UINT8 Success,i, StartByte;
 	 
   	 if ((Success = MakeOrCheckCrc( MBReceiveBuffer, RecvInfo->FrameLength, CHECK)))
     {
		#if DEBUGENABLED
   		 	printf("\nCRC Success");
   		#endif

   		if (MBReceiveBuffer[FUNCTION] & 0x80)
        {
		  return(MBReceiveBuffer[EXCEPTIONCODE]);
		}

		 RecvInfo->FunctionCode			= MBReceiveBuffer[FUNCTION];
		 RecvInfo->SlaveInfo.SlaveNo	= MBReceiveBuffer[SLAVEADDRESS];
	
		 /* Status */
		 ((MBDEFS_UINT8 *)(&(varData[0])))[HIGH] = MBReceiveBuffer[FC12STATUSHI];
		 ((MBDEFS_UINT8 *)(&(varData[0])))[LOW]  = MBReceiveBuffer[FC12STATUSLOW];

		 /* Event Count */
		 ((MBDEFS_UINT8 *)(&(varData[1])))[HIGH] = MBReceiveBuffer[FC12EVENTCOUNTHI];
		 ((MBDEFS_UINT8 *)(&(varData[1])))[LOW]  = MBReceiveBuffer[FC12EVENTCOUNTLOW];

		 /* Message Count */
		 ((MBDEFS_UINT8 *)(&(varData[2])))[HIGH] = MBReceiveBuffer[FC12MSGCOUNTHI];
		 ((MBDEFS_UINT8 *)(&(varData[2])))[LOW]  = MBReceiveBuffer[FC12MSGCOUNTLOW];
		 #if DEBUGENABLED
			printf("\nEvent Count: %d",varData[1]);
			printf("\nMessage Count: %d",varData[2]);
			if(varData[0] == 0xffff)
				printf("\nSlave Status: Busy");
			else
				printf("\nSlave Status: Not Busy");
		 #endif
		
		 /* Events */
		 StartByte = FC12EVENTDATAOFFSETRESP ;
		 #if DEBUGENABLED
		   	printf("\nData:");
		 #endif

		 ((MBDEFS_UINT8 *)(&(varData[3])))[HIGH] = 0;
		 ((MBDEFS_UINT8 *)(&(varData[3])))[LOW]  = MBReceiveBuffer[BYTECOUNTRESPONSE] - 6;
		 for(i = 0; i < MBReceiveBuffer[BYTECOUNTRESPONSE] - 6 ; i++)
         {
		   varData[4 + i] = MBReceiveBuffer[StartByte++];
		   #if DEBUGENABLED
		   		printf("\t %x", varData[4 + i]);
		   #endif
         }

		 return(SUCCESS);
     }
	 else
     {
	  	#if DEBUGENABLED
	 		printf("\n CRC Error");
//	  		fflush(stdout);
	  	#endif
	  	return(CRCERROR);
     }
#endif /*(MODBUS_TCP)*/


	return(MBDEFS_FALSE);
 }
#endif /* FETCH_COMM_EVENT_LOG_SUPPORTED */


#if WRITE_COILS_SUPPORTED
/*-------------------------------------------------------------------------------*
 * Function to parse Response frame  for function type 15 : Write Multiple Coils *
 *-------------------------------------------------------------------------------*/ 
 MBDEFS_UINT8 ParseFC15(MODBUSLINKINFO *SendInfo,MODBUSLINKINFO *RecvInfo,MBDEFS_UINT16 *varData)
 {
	
 #if !(MODBUS_TCP)	
	 xdata MBDEFS_UINT8 Success;	

   	 if ((Success = MakeOrCheckCrc( MBReceiveBuffer, RecvInfo->FrameLength, CHECK)))
     {
   		 #if DEBUGENABLED
   		 	printf("\nCRC Success");
   		 #endif
 #endif /*(MODBUS_TCP)*/
   		 if (MBReceiveBuffer[FUNCTION] & 0x80)
         {
		  return(MBReceiveBuffer[EXCEPTIONCODE]);
         }
		 RecvInfo->FunctionCode			= MBReceiveBuffer[FUNCTION];
		 RecvInfo->SlaveInfo.SlaveNo	= MBReceiveBuffer[SLAVEADDRESS];
	
		 return(SUCCESS);
 #if !(MODBUS_TCP)     
	 }
	 return(CRCERROR);
 #endif /*(MODBUS_TCP)*/
 }
#endif /* WRITE_COILS_SUPPORTED */

#if WRITE_HOLDING_REGISTERS_SUPPORTED
/*------------------------------------------------------------------------------------*
 * Function to parse Response frame  for function type 16 : Write Multiple registers *
 *-------------------------------------------------------------------------------------*/ 
 MBDEFS_UINT8 ParseFC16(MODBUSLINKINFO *SendInfo,MODBUSLINKINFO *RecvInfo,MBDEFS_UINT16 *varData)
 {
#if !(MODBUS_TCP)	
	 xdata MBDEFS_UINT8 Success;

   	 if ((Success = MakeOrCheckCrc( MBReceiveBuffer, RecvInfo->FrameLength, CHECK)))
     {
   		 #if DEBUGENABLED
   		 	printf("\nCRC Success");
   		 #endif
 #endif /*(MODBUS_TCP)*/
   		 if (MBReceiveBuffer[FUNCTION] & 0x80)
         {
		  return(MBReceiveBuffer[EXCEPTIONCODE]);
		  
         }
		 RecvInfo->FunctionCode			= MBReceiveBuffer[FUNCTION];
		 RecvInfo->SlaveInfo.SlaveNo	= MBReceiveBuffer[SLAVEADDRESS];
	
		 return(SUCCESS);
 #if !(MODBUS_TCP)
     }
	 return(CRCERROR);
 #endif /*(MODBUS_TCP)*/
 }
#endif /* WRITE_HOLDING_REGISTERS_SUPPORTED */

#if REPORT_SLAVE_ID_SUPPORTED
/*--------------------------------------------------------------------------*
 * Function to parse Response frame  for function type 17 : Report Slave ID *
 *--------------------------------------------------------------------------*/
 MBDEFS_UINT8 ParseFC17(MODBUSLINKINFO *SendInfo,MODBUSLINKINFO *RecvInfo,MBDEFS_UINT16 *varData)
 {
 #if !(MODBUS_TCP)

	 xdata MBDEFS_UINT8 Success, *Buffer, i;
 	
   	 if ((Success = MakeOrCheckCrc( MBReceiveBuffer, RecvInfo->FrameLength, CHECK)))
     {
   		 #if DEBUGENABLED
   		 	printf("\nCRC Success");
   		 #endif
 
   		 if (MBReceiveBuffer[FUNCTION] & 0x80)
         {
		  return(MBReceiveBuffer[EXCEPTIONCODE]);
         }

		 RecvInfo->FunctionCode			= MBReceiveBuffer[FUNCTION];
		 RecvInfo->SlaveInfo.SlaveNo	= MBReceiveBuffer[SLAVEADDRESS];
	
		 ((MBDEFS_UINT8 *)(&(varData[0])))[HIGH] = MBReceiveBuffer[BYTECOUNTRESPONSE];
		 ((MBDEFS_UINT8 *)(&(varData[0])))[LOW]  = MBReceiveBuffer[FC17SLAVEID];
		 ((MBDEFS_UINT8 *)(&(varData[1])))[HIGH] = MBReceiveBuffer[FC17RUNINDICATORSTS];
		 ((MBDEFS_UINT8 *)(&(varData[1])))[LOW]  = MBReceiveBuffer[FC17RUNINDICATORSTS + 1];
		 #if DEBUGENABLED
		   	printf("\n Slave Id: %x, Run Indicator Status: %x \n ",((MBDEFS_UINT8 *)(&(varData[0])))[LOW] ,
																	((MBDEFS_UINT8 *)(&(varData[1])))[HIGH]);
			printf("\n Additional Data: %x  ",((MBDEFS_UINT8 *)(&(varData[1])))[LOW]);
		 #endif	

		/* Now copy Device Specific Additional data */
		 Buffer = ((MBDEFS_UINT8 *)(&(varData[2])));
		 for(i = 3; i< MBReceiveBuffer[BYTECOUNTRESPONSE]  ; i++)
         {
			   *Buffer = MBReceiveBuffer[FC17SLAVEID + i];
			   #if DEBUGENABLED
			   		printf("%x  ",*Buffer);
			   #endif
			   Buffer++;
		 }
		 #if DEBUGENABLED
			   	printf(" \n");
		 #endif

		 return(SUCCESS);
 	 }
	 return(CRCERROR);

 #endif /*(!MODBUS_TCP)*/

	return(MBDEFS_FALSE);
 }
#endif /* REPORT_SLAVE_ID_SUPPORTED */

#if READ_FILE_RECORD_SUPPORTED
/*---------------------------------------------------------------------------*
 * Function to parse Response frame  for function type 20 : Read File Record *
 *----------------------------------------------------------------------------*/
 MBDEFS_UINT8 ParseFC20(MODBUSLINKINFO *SendInfo,MODBUSLINKINFO *RecvInfo,MBDEFS_UINT16 *varData)
 {
	 xdata MBDEFS_UINT8 Count, i, j, offset;
 
 #if !(MODBUS_TCP)	
	 xdata MBDEFS_UINT8 Success;

   	 if ((Success = MakeOrCheckCrc( MBReceiveBuffer, RecvInfo->FrameLength, CHECK)))
     {
   		 #if DEBUGENABLED
   		 	printf("\nCRC Success");
   		 #endif
 #endif /*(MODBUS_TCP)*/
   		 if (MBReceiveBuffer[FUNCTION] & 0x80)
         {
		  return(MBReceiveBuffer[EXCEPTIONCODE]);
         }

		 RecvInfo->SlaveInfo.SlaveNo	= MBReceiveBuffer[SLAVEADDRESS];
		 RecvInfo->FunctionCode			= MBReceiveBuffer[FUNCTION];
		 
	
		 /* Copy the data */
		 Count=0;
		 offset = BYTECOUNTRESPONSE + 1;
		 while(offset < (MBReceiveBuffer[BYTECOUNTRESPONSE] + BYTECOUNTRESPONSE) )
		 {
			varData[Count++] = MBReceiveBuffer[offset++];	/* File Resp. Length	*/
			#if DEBUGENABLED
   				printf("\n\nFile Resp Length: 0x%x",varData[Count-1]);
			#endif	

			varData[Count++] = MBReceiveBuffer[offset++];	/* Reference Type		*/
			#if DEBUGENABLED
   				printf("\nReference Type  : 0x%x",varData[Count-1]);
			#endif	

			#if DEBUGENABLED
   				printf("\nData\t\t:");
			#endif
			j = (varData[Count-2]/2);
			for( i=0;  i< j ; i++ )
			{
				((MBDEFS_UINT8 *)&varData[Count])[HIGH]  = MBReceiveBuffer[offset++];	/* Data	*/
				((MBDEFS_UINT8 *)&varData[Count++])[LOW] = MBReceiveBuffer[offset++];

				#if DEBUGENABLED
   					printf(" 0x%x  ",varData[Count-1]);
				#endif	
			}
		 }
		 #if DEBUGENABLED
			printf("\n");
		 #endif	

		 
	 	 return(SUCCESS);

		 
 #if !(MODBUS_TCP)     
	 }
	 return(CRCERROR);
 #endif /*(MODBUS_TCP)*/

	return(MBDEFS_FALSE);
	
 }
#endif /* READ_FILE_RECORD_SUPPORTED */

#if WRITE_FILE_RECORD_SUPPORTED
/*----------------------------------------------------------------------------*
 * Function to parse Response frame  for function type 21 : Write File Record *
 *----------------------------------------------------------------------------*/
 MBDEFS_UINT8 ParseFC21(MODBUSLINKINFO *SendInfo,MODBUSLINKINFO *RecvInfo,MBDEFS_UINT16 *varData)
 {
	 xdata MBDEFS_UINT8 Count, offset, i, j;
 
 #if !(MODBUS_TCP)	
	 xdata MBDEFS_UINT8 Success;

   	 if ((Success = MakeOrCheckCrc( MBReceiveBuffer, RecvInfo->FrameLength, CHECK)))
     {
   		 #if DEBUGENABLED
   		 	printf("\nCRC Success");
   		 #endif
 #endif /*(MODBUS_TCP)*/
   		 if (MBReceiveBuffer[FUNCTION] & 0x80)
         {
		  return(MBReceiveBuffer[EXCEPTIONCODE]);
         }

		 RecvInfo->SlaveInfo.SlaveNo	= MBReceiveBuffer[SLAVEADDRESS];
		 RecvInfo->FunctionCode			= MBReceiveBuffer[FUNCTION];
		 
	 /* Copy the data */
		 Count=0;
		 offset = BYTECOUNTRESPONSE + 1;
		 while(offset < (MBReceiveBuffer[BYTECOUNTRESPONSE] + BYTECOUNTRESPONSE) )
		 {
			varData[Count++]		= MBReceiveBuffer[offset++];	/* Reference Type	*/
			#if DEBUGENABLED
   				printf("\nReference Type: 0x%x",varData[Count-1]);
			#endif	

			((MBDEFS_UINT8 *)&varData[Count])[HIGH]	= MBReceiveBuffer[offset++];	/* File Number	*/
			((MBDEFS_UINT8 *)&varData[Count++])[LOW]	= MBReceiveBuffer[offset++];
			#if DEBUGENABLED
   				printf("\nFile Number   : 0x%x",varData[Count-1]);
			#endif	

			((MBDEFS_UINT8 *)&varData[Count])[HIGH]	= MBReceiveBuffer[offset++];	/* Record Number*/
			((MBDEFS_UINT8 *)&varData[Count++])[LOW]	= MBReceiveBuffer[offset++];
			#if DEBUGENABLED
   				printf("\nRecord Number : 0x%x",varData[Count-1]);
			#endif	

			((MBDEFS_UINT8 *)&varData[Count])[HIGH]	= MBReceiveBuffer[offset++];	/* Record Length*/
			((MBDEFS_UINT8 *)&varData[Count++])[LOW]	= MBReceiveBuffer[offset++];
			#if DEBUGENABLED
   				printf("\nRecord Length : 0x%x",varData[Count-1]);
			#endif

			#if DEBUGENABLED
   				printf("\nData          :");
			#endif
			j = (MBReceiveBuffer[BYTECOUNTRESPONSE] - 7) / 2;

			for( i=0;  i< j ; i++ )
			{
				((MBDEFS_UINT8 *)&varData[Count])[HIGH]	= MBReceiveBuffer[offset++];	/* Data	*/
				((MBDEFS_UINT8 *)&varData[Count++])[LOW]	= MBReceiveBuffer[offset++];

				#if DEBUGENABLED
   					printf("\t0x%x",varData[Count-1]);
				#endif	
			}
		 }
		 #if DEBUGENABLED
			printf("\n");
		 #endif	

		 return(SUCCESS);
 #if !(MODBUS_TCP)     
	 }
	 return(CRCERROR);
 #endif /*(MODBUS_TCP)*/

	return(MBDEFS_FALSE);
 }
#endif /* WRITE_FILE_RECORD_SUPPORTED */

#if MASK_WRITE_REGISTER_SUPPORTED
/*------------------------------------------------------------------------------*
 * Function to parse Response frame  for function type 22 : Mask Write Register *
 *------------------------------------------------------------------------------*/
 MBDEFS_UINT8 ParseFC22(MODBUSLINKINFO *SendInfo,MODBUSLINKINFO *RecvInfo,MBDEFS_UINT16 *varData)
 {
  	 
#if !(MODBUS_TCP)
	 xdata MBDEFS_UINT8 Success;

   	 if ((Success = MakeOrCheckCrc( MBReceiveBuffer, RecvInfo->FrameLength, CHECK)))
     {
         #if DEBUGENABLED
   		 	printf("\nCRC Success");
   		 #endif /*(DEBUGENABLED)*/
#endif /*(MODBUS_TCP)*/
   		 if (MBReceiveBuffer[FUNCTION] & 0x80)
         {
		 	return(MBReceiveBuffer[EXCEPTIONCODE]);
		  
         }

   		 /*RecvInfo->Data = (DATA *)calloc(RecvInfo->Quantity,sizeof(DATA));*/
		 RecvInfo->FunctionCode = MBReceiveBuffer[FUNCTION];
   		 RecvInfo->SlaveInfo.SlaveNo = MBReceiveBuffer[SLAVEADDRESS];
   	
		 ((MBDEFS_UINT8 *)(&(varData[0])))[HIGH]	= MBReceiveBuffer[ANDMASKHI];
		 ((MBDEFS_UINT8 *)(&(varData[0])))[LOW]	= MBReceiveBuffer[ANDMASKLOW];
		 ((MBDEFS_UINT8 *)(&(varData[1])))[HIGH]	= MBReceiveBuffer[ORMASKHI];
		 ((MBDEFS_UINT8 *)(&(varData[1])))[LOW]	= MBReceiveBuffer[ORMASKLOW];

		  #if DEBUGENABLED
		   	printf("\nRecvd AND Mask: %x   OR Mask: %x ",varData[0],varData[1]);
		  #endif
         
		 return(SUCCESS);
 #if !(MODBUS_TCP)
     }
	 else
     {
	  	#if DEBUGENABLED
	 		printf("\n CRC Error");
//	  		fflush(stdout);
	  	#endif
	  	return(CRCERROR);
     }
 #endif /*(MODBUS_TCP)*/
 }
#endif

#if READ_WRITE_REGISTERS_SUPPORTED
 /*---------------------------------------------------------------------------------------*
 * Function to parse Response frame  for function type 23 : Read/Write Multiple Registers*
 *---------------------------------------------------------------------------------------*/
 MBDEFS_UINT8 ParseFC23(MODBUSLINKINFO *SendInfo,MODBUSLINKINFO *RecvInfo,MBDEFS_UINT16 *varData)
 {
 	 xdata MBDEFS_UINT8 StartByte;
   	 xdata MBDEFS_UINT16 i;
	 #if DEBUGENABLED
		xdata MBDEFS_UINT16 TempAdd;
	 #endif
#if !(MODBUS_TCP)
	 xdata MBDEFS_UINT8 Success;

   	 if ((Success = MakeOrCheckCrc( MBReceiveBuffer, RecvInfo->FrameLength, CHECK)))
     {
         #if DEBUGENABLED
   		 	printf("\nCRC Success");
   		 #endif /*(DEBUGENABLED)*/

#endif /*(MODBUS_TCP)*/

   		 if (MBReceiveBuffer[FUNCTION] & 0x80)
         {
		 	return(MBReceiveBuffer[EXCEPTIONCODE]);
		  
         }

   		 RecvInfo->FunctionCode = MBReceiveBuffer[FUNCTION];
   		 RecvInfo->SlaveInfo.SlaveNo = MBReceiveBuffer[SLAVEADDRESS];
   		 StartByte = DATAOFFSETRESPONSE;

		 #if DEBUGENABLED
		   		printf("\nData read: Address\tData");
				TempAdd = SendInfo->UserPassedInfo->VarAddress;
		 #endif
		 for(i = 0; i< (MBReceiveBuffer[BYTECOUNTRESPONSE] / 2); i++)
         {
		   ((MBDEFS_UINT8 *)(&(varData[i])))[HIGH] = MBReceiveBuffer[StartByte++];
		   ((MBDEFS_UINT8 *)(&(varData[i])))[LOW]	= MBReceiveBuffer[StartByte++];
		   
			#if DEBUGENABLED
			    TempAdd++;
		   		printf("\n\t%6x\t\t%4x",TempAdd,varData[i]);
			#endif
         }
		 return(SUCCESS);

 #if !(MODBUS_TCP)
     }
	 else
     {
	  	#if DEBUGENABLED
	 		printf("\nCRC Error");
//	  		fflush(stdout);
	  	#endif
	  	return(CRCERROR);
     }
 #endif /*(MODBUS_TCP)*/

	return(MBDEFS_FALSE);
 }
#endif


#if READ_FIFO_QUEUE_SUPPORTED
/*--------------------------------------------------------------------------*
 * Function to parse Response frame  for function type 24 : Read FIFO Queue *
 *--------------------------------------------------------------------------*/
 MBDEFS_UINT8 ParseFC24(MODBUSLINKINFO *SendInfo,MODBUSLINKINFO *RecvInfo,MBDEFS_UINT16 *varData)
 {
	 xdata MBDEFS_UINT8 StartByte;
   	 xdata MBDEFS_UINT16 i,K;

#if !(MODBUS_TCP)
	 xdata MBDEFS_UINT8 Success;

   	 if ((Success = MakeOrCheckCrc( MBReceiveBuffer, RecvInfo->FrameLength, CHECK)))
     {
         #if DEBUGENABLED
   		 	printf("\nCRC Success");
   		 #endif /*(DEBUGENABLED)*/

#endif /*(MODBUS_TCP)*/

   		 if (MBReceiveBuffer[FUNCTION] & 0x80)
         {
		 	return(MBReceiveBuffer[EXCEPTIONCODE]);		  
         }

		 RecvInfo->SlaveInfo.SlaveNo = MBReceiveBuffer[SLAVEADDRESS];
   		 RecvInfo->FunctionCode = MBReceiveBuffer[FUNCTION];
		 *((MBDEFS_UINT8 *)&K + HIGH) = MBReceiveBuffer [FUNCTION + 1];
		 *((MBDEFS_UINT8 *)&K + LOW) = MBReceiveBuffer [FUNCTION + 2];
   		 
   		 StartByte = FC24FIFOOFFSETRESP;
		 #if DEBUGENABLED
		   		printf("\nData: ");
		#endif
		 for(i = 0; i< ((K/2)-1); i++)
         {
		    *((MBDEFS_UINT8 *)&varData[i] + HIGH) = MBReceiveBuffer[StartByte++];
			*((MBDEFS_UINT8 *)&varData[i] + LOW ) = MBReceiveBuffer[StartByte++];
		  		   
			#if DEBUGENABLED
		   		printf("%x\t",varData[i]);
			#endif
         }
		 return(SUCCESS);

 #if !(MODBUS_TCP)
     }
	 else
     {
	  	#if DEBUGENABLED
	 		printf("\nCRC Error");
//	  		fflush(stdout);
	  	#endif
	  	return(CRCERROR);
     }
 #endif /*(MODBUS_TCP)*/

	return(MBDEFS_FALSE);
 }
#endif /* READ_FIFO_QUEUE_SUPPORTED */

#if ENCAPSULATED_INTERFACE_TRANSPORT_SUPPORTED
/*--------------------------------------------------------------------------------------------*
 * Function to parse Response frame  for function type 43 : Encapsulated Interface Transport  *
 *--------------------------------------------------------------------------------------------*/
 MBDEFS_UINT8 ParseFC43(MODBUSLINKINFO *SendInfo,MODBUSLINKINFO *RecvInfo,MBDEFS_UINT16 *varData)
 {
   	 xdata MBDEFS_UINT16 i;
	 #if DEBUGENABLED
	 	 MBDEFS_UINT16 NumOfObjs, ObjLength, Ctr;
	 #endif

 #if !(MODBUS_TCP)
	 MBDEFS_UINT8 Success;

   	 if ((Success = MakeOrCheckCrc( MBReceiveBuffer, RecvInfo->FrameLength, CHECK)))
     {
         #if DEBUGENABLED
   		 	printf("\nCRC Success");
   		 #endif /*(DEBUGENABLED)*/
 #endif /*(MODBUS_TCP)*/
   		 if (MBReceiveBuffer[FUNCTION] & 0x80)
         {
		 	return(MBReceiveBuffer[MEIEXCEPTIONCODE]);
		  
         }
		
		/*  Copy the Receive Buffer Data */
		RecvInfo->FunctionCode = MBReceiveBuffer[FUNCTION];
		RecvInfo->SlaveInfo.SlaveNo = MBReceiveBuffer[SLAVEADDRESS];
		RecvInfo->Quantity = (RecvInfo->FrameLength - MEITYPE - 1);


		if((RecvInfo->MEIInfo  = (MEI_INFO *)calloc(1,sizeof(MEI_INFO))) == MBDEFS_NULL)
		{
			#if DEBUGENABLED
				printf("\nError in Memory Allocation\n");
			#endif
			MBErrNo = NOMEMORY;
			return(MBDEFS_FALSE);
		}

		if( (RecvInfo->MEIInfo ->MEIData  = (MBDEFS_UCHAR *)calloc(RecvInfo->Quantity,sizeof(MBDEFS_UCHAR))) == MBDEFS_NULL )
		{
			#if DEBUGENABLED
				printf("\nError in Memory Allocation\n");
			#endif
			free(RecvInfo->MEIInfo );
			RecvInfo->MEIInfo = MBDEFS_NULL;
			MBErrNo = NOMEMORY;
			return(MBDEFS_FALSE);
		}
		
		/* Copy the received MEI Type */
		RecvInfo->MEIInfo ->MEIType = MBReceiveBuffer[MEITYPE]; 


		switch( MBReceiveBuffer[MEITYPE] )
		{
			 case READDEVICEIDENTIFICATION: /* MEI Type = 14 */
				 {
					  /*first data byte should be the total number of valid data in buffer*/
					 *((MBDEFS_UINT8 *)(varData)) = (MBDEFS_UINT8)RecvInfo->Quantity;
					 #if DEBUGENABLED
				    /* Total number of valid bytes in the buffer will be available as a first byte in the buffer, the following bytes are
				       the received bytes */
				  		printf("\nTotal number of bytes: %d", RecvInfo->Quantity);
				 	   	printf("\nData: ");
					 #endif
					 /* Copy all the bytes after MEI Type */
					 for(i = 1; i<= RecvInfo->Quantity ; i++)
				     {
						   *((MBDEFS_UINT8 *)(varData)+i) = MBReceiveBuffer[MEITYPE + i];
						   *(RecvInfo->MEIInfo ->MEIData + (i-1)) = MBReceiveBuffer[MEITYPE + i];
					 }
					 #if DEBUGENABLED
						 printf("\nMEI(Read device identification) Data: ");
						 printf("\nRead device id code: %x",*(((MBDEFS_UINT8 *)varData) + 1));
						 printf("\nConformity level   : %x",*(((MBDEFS_UINT8 *)varData) + 2));
						 printf("\nMore follows       : %x",*(((MBDEFS_UINT8 *)varData) + 3));
						 printf("\nNext object id     : %x",*(((MBDEFS_UINT8 *)varData) + 4));
						 printf("\nNumber of objects  : %x",*(((MBDEFS_UINT8 *)varData) + 5));
						 Ctr = 6;
						 for(NumOfObjs = 0; NumOfObjs < *(((MBDEFS_UINT8 *)varData) + 5); NumOfObjs++)
						 {
							 printf("\nObject Id and Name : %x  ",*(((MBDEFS_UINT8 *)varData) + Ctr));
							 Ctr++;
							 for(ObjLength = 1; ObjLength <= *(((MBDEFS_UINT8 *)varData) + Ctr); ObjLength++)
							 {
								 printf("%c",*(((MBDEFS_UINT8 *)varData) + Ctr + ObjLength));
							 }
							 Ctr += ObjLength;
						 }						 
					 #endif
				 }
				 break;
			 default:
				 break;

		}
		return(SUCCESS);

 #if !(MODBUS_TCP)
     }
	 else
     {
	  	#if DEBUGENABLED
	 		printf("\n CRC Error");
//	  		fflush(stdout);
	  	#endif
	  	return(CRCERROR);
     }
 #endif /*(MODBUS_TCP)*/
	
 }
#endif /* ENCAPSULATED_INTERFACE_TRANSPORT_SUPPORTED */

/*--------------------------------------------------------------------------*
 * Function to Exception Response frame 1                                   *
 *--------------------------------------------------------------------------*/
 MBDEFS_NONE HandleException1(MODBUSLINKINFO *SendInfo)
 {
	  #if (DEBUGENABLED)
	  	printf("\n...>  Slave No: %d Exception response : ILLEGAL FUNCTION  0x%x",SendInfo->SlaveInfo.SlaveNo,SendInfo->FunctionCode);
	  #endif
 }

/*--------------------------------------------------------------------------*
 * Function to Exception Response frame 2                                   *
 *--------------------------------------------------------------------------*/
 MBDEFS_NONE HandleException2(MODBUSLINKINFO *SendInfo)
 {
	  #if (DEBUGENABLED)
	  	printf("\nException response : ILLEGAL START ADDRESS, Slave Addr:%d  FC0x%x",SendInfo->SlaveInfo.SlaveNo,SendInfo->FunctionCode);
	  #endif
 }

/*--------------------------------------------------------------------------*
 * Function to Exception Response frame 3                                   *
 *--------------------------------------------------------------------------*/
 MBDEFS_NONE HandleException3(MODBUSLINKINFO *SendInfo)
 {
	  #if (DEBUGENABLED)
	  	printf("\nException response : ILLEGAL DATA VALUE, Slave Addr:%d  FC0x%x",SendInfo->SlaveInfo.SlaveNo,SendInfo->FunctionCode);
	  #endif 
 }

/*--------------------------------------------------------------------------*
 * Function to Exception Response frame 4                                   *
 *--------------------------------------------------------------------------*/
 MBDEFS_NONE HandleException4(MODBUSLINKINFO *SendInfo)
 {
	  #if (DEBUGENABLED)
	  	printf("\nException response : SLAVE DEVICE FAILURE, Slave Addr:%d  FC0x%x",SendInfo->SlaveInfo.SlaveNo,SendInfo->FunctionCode);
	  #endif
 }

/*--------------------------------------------------------------------------*
 * Function to Exception Response frame 5                                   *
 *--------------------------------------------------------------------------*/
 MBDEFS_NONE HandleException5(MODBUSLINKINFO *SendInfo)
 {
	  #if (DEBUGENABLED)
	  	printf("\nException response : ACKNOWLEDGE, Slave Addr:%d  FC0x%x",SendInfo->SlaveInfo.SlaveNo,SendInfo->FunctionCode);
	  #endif
 }

/*--------------------------------------------------------------------------*
 * Function to Exception Response frame 6                                   *
 *--------------------------------------------------------------------------*/
 MBDEFS_NONE HandleException6(MODBUSLINKINFO *SendInfo)
 {
	  #if (DEBUGENABLED)
	  	printf("\nException response : SLAVE DEVICEBUSY, Slave Addr:%d  FC0x%x",SendInfo->SlaveInfo.SlaveNo,SendInfo->FunctionCode);
	  #endif
 }

/*--------------------------------------------------------------------------*
 * Function to Exception Response frame 7                                   *
 *--------------------------------------------------------------------------*/
MBDEFS_NONE HandleException7(MODBUSLINKINFO *SendInfo)
 {
	  #if (DEBUGENABLED)
	  	printf("\nException response : NEGATIVE ACKNOWLEDGE, Slave Addr:%d  FC0x%x",SendInfo->SlaveInfo.SlaveNo,SendInfo->FunctionCode);
	  #endif
 }

/*--------------------------------------------------------------------------*
 * Function to Exception Response frame 8                                   *
 *--------------------------------------------------------------------------*/
MBDEFS_NONE HandleException8(MODBUSLINKINFO *SendInfo)
{
	  #if (DEBUGENABLED)
	  	printf("\nException response : MEMORY PARITY ERROR, Slave Addr:%d  FC0x%x",SendInfo->SlaveInfo.SlaveNo,SendInfo->FunctionCode);
	  #endif
}

/*--------------------------------------------------------------------------*
 * Entry function for database storing on parsing success                   *
 *--------------------------------------------------------------------------*/
MBDEFS_NONE  HandleSuccess(MODBUSLINKINFO *RecvInfo)
{
	#if DEBUGENABLED
	printf("\nFC0x%x Success ",RecvInfo->FunctionCode);
//	fflush(stdout);
	#endif
}

/*--------------------------------------------------------------------------*
 * Handle CRC error                                                         *
 *--------------------------------------------------------------------------*/
MBDEFS_NONE HandleCRCEror(MODBUSLINKINFO *RecvInfo)
{
	 #if DEBUGENABLED
	 	printf("\nCRC Error ");
//	 	fflush(stdout);
  	 #endif
}


