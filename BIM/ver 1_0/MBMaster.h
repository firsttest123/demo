#ifndef _MBMASTER_H
#define	_MBMASTER_H
/*** BeginHeader */

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

/********************************************************************************************
* MBMaster.h:  					This file contains the macros, structures  
* 								and function declarations
* -----------------------------------
* Modification history
* -----------------------------------
* SlNo,Date,	By,		  Details
* -----------------------------------
* 01,04/04/2000,SRIKANTH,  Written
* 02,01/12/2000,RMD,  Reduced the Number of Files. Only Two C files(MBDriver.c & MBFrames.c)
*					  and Three Header files (MBMaster.h, MBDefs.h & MBlinkUser.h) retained.
* 03,01/06/2001,RMD,  Added code for MODBUS over TCP/IP
* 04,10/12/2001,RMD,  'BAD_SEQUENCE'Macro Added. 					   
* 05,30/09/2003,SAN,  Added macros related to Encapsulated Interface Transport Function Code.
* 06,13/10/2003,SAN,  Added macros and definitions related to Function codes 07,08,0B,0C,11,
*					  14,15,16,17,18,2B,2B/0D.
* 07,14/10/2003,RMD,  Removed Function declaration of MBWrite as its definition is removed.
*					  Function MBRead renamed as MBSendRequest.
* 08,18/11/2003,SAN,  'MB_USER_PASSED_INFO' structure included for user information entries.
* 09,01/07/2006,SGO,  Added macros 'UNKNOWN_EXCEPTION' & 'NO_OF_EXCEPTIONS'.
**********************************************************************************************/

#include "MBdefs.h"
#include <stdio.h>
#include <stdlib.h>
//#include <memory.h>
#include "MB_UserPort.h"

#define NO_OF_EXCEPTIONS	   9

/*************************************************************************************************/
/*									 CRC creation options										 */
/*************************************************************************************************/
#define MAKE	0
#define CHECK	1

/****************************************************************/
/*				Modbus Error Code defnitions					*/
/****************************************************************/
#define NOERROR					0 
#define NOMEMORY			   10
#define UNKNOWN_REQUEST		   11
#define INVALID_ADDRESS		   12
#define EXCEED_NO_OF_ITEMS     13

#define TIMEOUT		    	   14
#define CHECKSUM_ERROR		   15
#define SLAVEADD_MISMATCH	   16
#define NETWORK_WRITE_ERR      17
#define NETWORK_READ_ERR       18
#define INVALID_SLAVE_NO	   19
#define TRANSACT_ID_MISMATCH   20
#define BAD_SEQUENCE		   21	/*Functions needs to be called in a predefined sequence. 
									--1. MBDriver_InitSerialDriver
									  2. MBSendRequest
									  3. MBDriver_DeInit. 	
									If this sequence not followed, this error will be generated */
#define UNKNOWN_EXCEPTION	   22

/****************************************************************/
/*				Modbus Exception Response Codes					*/
/****************************************************************/
#define ILLEGALFUNCTION			1
#define ILLEGALDATAADDRESS	   	2
#define ILLEGALDATAVALUE		3
#define SLAVEDEVICEFAILURE		4
#define ACKNOWLEDGE				5
#define SLAVEDEVICEBUSY			6
#define NEGATIVEACKNOWLEDGE		7
#define MEMORYPARITYERROR		8

/********************************************************************/
/*				MODBUS Function Codes								*/
/********************************************************************/
#define READCOILSTATUS 								1	/* 0x01 */
#define READINPUTSTATUS 							2	/* 0x02 */
#define READHOLDINGREGISTERS 						3	/* 0x03 */
#define READINPUTREGISTERS 							4	/* 0x04 */
#define FORCESINGLECOIL								5	/* 0x05 */
#define PRESETSINGLEREGISTER						6	/* 0x06 */
#define FORCEMULTIPLECOILS							15	/* 0x0F */			   
#define PRESETMULTIPLEREGISTERS						16	/* 0x10 */
#define ENCAPSULATEDINTERFACETRANSPORT				43	/* 0x2B */ 
#define READFILERECORD								20	/* 0x14 */
#define WRITEFILERECORD								21	/* 0x15 */
#define MASKWRITEREGISTER							22	/* 0x16 */
#define READWRITEMULTIPLEREGISTERS					23	/* 0X17 */
#define READFIFOQUEUE								24	/* 0x18 */
#if !MODBUS_TCP
	#define READEXCEPTIONSTATUS							7	/* 0X07 */
	#define DIAGNOSTICS									8	/* 0X08 */
	#define GETCOMMEVENTCOUNTER 						11	/* 0x0B */
	#define GETCOMMEVENTLOG								12	/* 0X0C */
	#define REPORTSLAVEID								17	/* 0X11 */
#endif

/* Function Codes comes under Encapsulated Interface Transport -- FC 43 */
#define READDEVICEIDENTIFICATION					14	/* 0x0E */

/********************************************************************/
/*				MODBUS Frame Field Index							*/
/********************************************************************/
#if MODBUS_TCP
	#define SLAVEADDRESS			 6
	#define	FUNCTION				 7
	#define STARTADDRESSH			 8
	#define STARTADDRESSL			 9
	#define EXCEPTIONCODE			 8
	#define BYTECOUNTRESPONSE		 8
	#define BYTECOUNTREQUEST		12
	#define DATAOFFSETRESPONSE		 9
	#define DATAOFFSETREQUEST		13
	#define REQUESTQUANTITY			10
	#define MEITYPE					 8	
	#define READDEVIDCODE			 9
	#define MBOBJECTID				10
	#define CONFORMITYLEVEL			10
	#define MOREFOLLOWS				11
	#define NEXTOBJECTID			12
	#define NUMBEROFOBJECTS			13
	#define MEIEXCEPTIONCODE		11
	#define	ANDMASKHI				10
	#define	ANDMASKLOW				11
	#define	ORMASKHI				12
	#define	ORMASKLOW				13
	#define FC24FIFOOFFSETRESP		12
	#define FC2021BYTECOUNTREQ		 8
#else
	#define SLAVEADDRESS			0
	#define	FUNCTION				1
	#define STARTADDRESSH			2
	#define STARTADDRESSL			3
	#define EXCEPTIONCODE			2
	#define BYTECOUNTRESPONSE		2
	#define BYTECOUNTREQUEST		6
	#define DATAOFFSETRESPONSE		3
	#define DATAOFFSETREQUEST		7
	#define REQUESTQUANTITY			4
	#define MEITYPE					2
	#define READDEVIDCODE			3
	#define MBOBJECTID				4
	#define CONFORMITYLEVEL			4
	#define MOREFOLLOWS				5
	#define NEXTOBJECTID			6
	#define NUMBEROFOBJECTS			7
	#define MEIEXCEPTIONCODE		3
	#define EXCEPTIONSTATUS			2
	#define DIAGNOSTICSSUBFCHI		2
	#define DIAGNOSTICSSUBFCLOW		3
	#define DIAGNOSTICSDATAHI		4
	#define DIAGNOSTICSDATALOW		5
	#define FC11STATUSHI			2				
	#define FC11STATUSLOW			3
	#define FC11EVENTCOUNTHI		4
	#define FC11EVENTCOUNTLOW		5
	#define FC12STATUSHI			3				
	#define FC12STATUSLOW			4
	#define FC12EVENTCOUNTHI		5
	#define FC12EVENTCOUNTLOW		6
	#define FC12MSGCOUNTHI			7
	#define FC12MSGCOUNTLOW			8
	#define FC12EVENTDATAOFFSETRESP	9
	#define	ANDMASKHI				4
	#define	ANDMASKLOW				5
	#define	ORMASKHI				6
	#define	ORMASKLOW				7
	#define FC17SLAVEID				3
	#define FC17RUNINDICATORSTS		4
	#define FC24FIFOOFFSETRESP		6
	#define FC2021BYTECOUNTREQ		2
#endif		/*(MODBUS_TCP)*/
	

#define TRANSACT_ID		256	/*Trasaction ID for MODBUS over TCP/IP */
#define PROTOCOL_ID		  0	/*Protocol identifier, which is 0 for MODBUS over TCP/IP*/
#define SUCCESS			  0
#define CRCERROR		  9
#define NOBYTES			  2
#define INITERR			  0


#if (LITTLE_ENDIAN)		/*In Little-Endian processor high-byte of a word  */ 
	#define HIGH 1		/*  is stored in second memory location           */
	#define LOW  0
#else
	#define HIGH 0		/*In Big-Endian processor high-byte of a word */
	#define LOW  1		/* is stored in first memory location         */
#endif


/******************************************************************/
/*				NETWORK STRUCTURE DECLARATIONS					  */
/******************************************************************/
typedef struct 						/* Structure for Network Info */
{
	MBDEFS_ULONG NetworkId; 		/*Network path ID*/
	
}NETWORK_INFO;

/****************************************************************************/
/*							SESSION STRUCTURE DECLARATIONS					*/
/****************************************************************************/
typedef struct 
{
   MBDEFS_UINT8		SlaveNo;
   MBDEFS_UINT16	SessionNo;
}SESSIONINFO;

typedef union 
{
  MBDEFS_UINT16 Register;
  MBDEFS_UINT16  Bit :1;
 
} DATA;

typedef	struct					/* Info required fro function code 20 & 21 - Read & Write File Record */ 
{
	MBDEFS_UINT8	ReferenceType;		/* Reference Type		*/
	MBDEFS_UINT16	Fileno;				/* File Number			*/
	MBDEFS_UINT16	StartAddress;		/* Record Number		*/
	MBDEFS_UINT16	Count;				/* Record Length		*/
	MBDEFS_UINT16*	Data;				/* Data to be written	*/
 
} GROUP_REFERENCE_INFO;


typedef struct					/* Info required for function code 43 - Encapsulated Interface */
{
	MBDEFS_UINT8	MEIType;	/* MEI Type					*/
	MBDEFS_UCHAR*	MEIData;	/* MEI Type Specific Data	*/
	
} MEI_INFO;

typedef struct					/* Info required for function code 22 - Mask Write Register */
{
	MBDEFS_UINT16	RegAdd;		/* Register Address */
	MBDEFS_UINT16	ANDMask;	/* AND Mask			*/
	MBDEFS_UINT16	ORMask;		/* AND Mask			*/
	
} MASK_INFO;

typedef struct					/* Info required for function code 23 - Read/Write Register */
{
	MBDEFS_UINT16	StartAdd;		/* Starting Register Address		*/
	MBDEFS_UINT16	RegQuantity;	/* Quantity of Registers to Read	*/
		
} READ_REG_INFO;

typedef struct					/* Info required for function code 23 - Read/Write Register */
{
	MBDEFS_UINT16	StartAdd;		/* Starting Register Address		*/
	MBDEFS_UINT16	RegQuantity;	/* Quantity of Registers to Read	*/
	MBDEFS_UINT8	ByteCount;		/* Byte Count						*/
	MBDEFS_UINT16*	RegVals;		/* Register Values					*/

} WRITE_REG_INFO;

typedef struct					/* Info required for function code 8 - Diagnostics */
{
	MBDEFS_UINT16	SubFC;			/* Sub Function Code */
	MBDEFS_UINT16	Data ;			/* Data				 */

} DIAGNOSTIC_INFO;

typedef struct					/* Info required for function code 8 - Diagnostics */
{
	MBDEFS_UINT16	FIFOPntr;	/* FIFO Pointer Address */

} FIFOQUEUE_INFO;

typedef struct
{
/*	MBDEFS_UINT8		SessionNo;
	MBDEFS_UINT8		FunctionCode;
*/	MBDEFS_UINT16		VarAddress;
/*	MBDEFS_UINT16		NItems;
	MBDEFS_ULONG		TimeOut;
	MBDEFS_UINT8		Retries;
*/	MBDEFS_UINT16*		varData;			/* pointer to user buffer */

}MB_USER_PASSED_INFO;

typedef struct
{
	SESSIONINFO		SlaveInfo;
	MBDEFS_UINT8	FunctionCode;
	MBDEFS_UINT16	Quantity;
	MBDEFS_UINT16	FrameLength;
	MBDEFS_UINT8	NBytes;
	MBDEFS_UINT8	GroupCount;
	MBDEFS_UINT16	ResponseFramelength;
	GROUP_REFERENCE_INFO*	ExtraInfo;
	DATA* Data;
	MEI_INFO*		MEIInfo;		/* Used by FC 43 */
	MASK_INFO*		MASKInfo;		/* Used by FC 22 */
	READ_REG_INFO*  ReadRegInfo;	/* Used by FC 23 */
	WRITE_REG_INFO* WriteRegInfo;	/* Used by FC 23 */
	FIFOQUEUE_INFO* FIFOQueueInfo;	/* Used by FC 24 */
	DIAGNOSTIC_INFO* DiagInfo;		/* Used by FC 8  */
	
	MB_USER_PASSED_INFO* UserPassedInfo;	/* pointer to user info structure */
  
} MODBUSLINKINFO;


/*structure of function pointers for frame constructing & parsing functions*/
typedef struct
{
  MBDEFS_BOOL (*ConstructFrame)(MODBUSLINKINFO *);
  MBDEFS_UINT8 (*ParseFrame)(MODBUSLINKINFO *,MODBUSLINKINFO * ,MBDEFS_UINT16*);
  MBDEFS_UINT16 FrameMinSize;
 
} FRAMEFUNCTIONPTR;

/*structure of function pointers for exception handling functions*/
typedef struct
{
  MBDEFS_NONE (*ExceptionHandler)(MODBUSLINKINFO *);

} EXCEPTIONFUNCTIONPTR;


/************************************************************************/
/*					Function Prototypes									*/
/************************************************************************/
MBDEFS_BOOL ConstructFC1(MODBUSLINKINFO *)	;
MBDEFS_UINT8 ParseFC1(MODBUSLINKINFO *,MODBUSLINKINFO *,MBDEFS_UINT16 *);
MBDEFS_BOOL ConstructFC2(MODBUSLINKINFO *);
MBDEFS_UINT8 ParseFC2(MODBUSLINKINFO *,MODBUSLINKINFO *,MBDEFS_UINT16 *);
MBDEFS_BOOL ConstructFC3(MODBUSLINKINFO *);
MBDEFS_UINT8 ParseFC3(MODBUSLINKINFO *,MODBUSLINKINFO *,MBDEFS_UINT16 *);
MBDEFS_BOOL ConstructFC4(MODBUSLINKINFO *);
MBDEFS_UINT8 ParseFC4(MODBUSLINKINFO *,MODBUSLINKINFO *,MBDEFS_UINT16 *);
MBDEFS_BOOL ConstructFC5(MODBUSLINKINFO *);
MBDEFS_UINT8 ParseFC5(MODBUSLINKINFO *,MODBUSLINKINFO *,MBDEFS_UINT16 *);
MBDEFS_BOOL ConstructFC6(MODBUSLINKINFO *);
MBDEFS_UINT8 ParseFC6(MODBUSLINKINFO *,MODBUSLINKINFO *,MBDEFS_UINT16 *);
MBDEFS_BOOL ConstructFC7(MODBUSLINKINFO *);
MBDEFS_UINT8 ParseFC7(MODBUSLINKINFO *,MODBUSLINKINFO *,MBDEFS_UINT16 *);
MBDEFS_BOOL ConstructFC8(MODBUSLINKINFO *);
MBDEFS_UINT8 ParseFC8(MODBUSLINKINFO *,MODBUSLINKINFO *,MBDEFS_UINT16 *);
MBDEFS_BOOL ConstructFC11(MODBUSLINKINFO *);
MBDEFS_UINT8 ParseFC11(MODBUSLINKINFO *,MODBUSLINKINFO *,MBDEFS_UINT16 *);													
MBDEFS_BOOL ConstructFC12(MODBUSLINKINFO *);
MBDEFS_UINT8 ParseFC12(MODBUSLINKINFO *,MODBUSLINKINFO *,MBDEFS_UINT16 *);													
MBDEFS_BOOL ConstructFC15(MODBUSLINKINFO *);
MBDEFS_UINT8 ParseFC15(MODBUSLINKINFO *,MODBUSLINKINFO *,MBDEFS_UINT16 *);													
MBDEFS_BOOL ConstructFC16(MODBUSLINKINFO *);
MBDEFS_UINT8 ParseFC16(MODBUSLINKINFO *,MODBUSLINKINFO *,MBDEFS_UINT16 *);													
MBDEFS_BOOL ConstructFC17(MODBUSLINKINFO *);
MBDEFS_UINT8 ParseFC17(MODBUSLINKINFO *,MODBUSLINKINFO *,MBDEFS_UINT16 *);													
MBDEFS_BOOL ConstructFC20(MODBUSLINKINFO *);
MBDEFS_UINT8 ParseFC20(MODBUSLINKINFO *,MODBUSLINKINFO *,MBDEFS_UINT16 *);													
MBDEFS_BOOL ConstructFC21(MODBUSLINKINFO *);
MBDEFS_UINT8 ParseFC21(MODBUSLINKINFO *,MODBUSLINKINFO *,MBDEFS_UINT16 *);													
MBDEFS_BOOL ConstructFC22(MODBUSLINKINFO *);	
MBDEFS_UINT8 ParseFC22(MODBUSLINKINFO *,MODBUSLINKINFO *,MBDEFS_UINT16 *);	   
MBDEFS_BOOL ConstructFC23(MODBUSLINKINFO *);
MBDEFS_UINT8 ParseFC23(MODBUSLINKINFO *,MODBUSLINKINFO *,MBDEFS_UINT16 *);													
MBDEFS_BOOL ConstructFC24(MODBUSLINKINFO *);
MBDEFS_UINT8 ParseFC24(MODBUSLINKINFO *,MODBUSLINKINFO *,MBDEFS_UINT16 *);												
MBDEFS_BOOL ConstructFC43(MODBUSLINKINFO *);
MBDEFS_UINT8 ParseFC43(MODBUSLINKINFO *,MODBUSLINKINFO *,MBDEFS_UINT16 *);												


MBDEFS_NONE HandleException1(MODBUSLINKINFO *);
MBDEFS_NONE HandleSuccess(MODBUSLINKINFO *);
MBDEFS_NONE HandleCRCEror(MODBUSLINKINFO *);
MBDEFS_NONE HandleException2(MODBUSLINKINFO *);
MBDEFS_NONE HandleException3(MODBUSLINKINFO *);
MBDEFS_NONE HandleException4(MODBUSLINKINFO *);
MBDEFS_NONE HandleException5(MODBUSLINKINFO *);
MBDEFS_NONE HandleException6(MODBUSLINKINFO *);
MBDEFS_NONE HandleException7(MODBUSLINKINFO *);
MBDEFS_NONE HandleException8(MODBUSLINKINFO *);

#if !MODBUS_TCP
MBDEFS_BOOL MBDriver_InitSerialDriver(	MBDEFS_UINT8, MBDEFS_ULONG,
										MBDEFS_UCHAR, MBDEFS_UCHAR,
										MBDEFS_UCHAR );
#endif

#if MODBUS_TCP
MBDEFS_BOOL MBDriver_InitTCPDriver( MBDEFS_CHAR*, MBDEFS_UINT16 );
#endif

MBDEFS_BOOL MBSendRequest(  MBDEFS_UINT8, MBDEFS_UINT8, MBDEFS_UINT16, MBDEFS_UINT16,
					 MBDEFS_UINT16*, MBDEFS_ULONG, MBDEFS_UINT8 );

MBDEFS_BOOL MBDriver_DeInit();


MBDEFS_BOOL SendAndReceiveMBFrame( MODBUSLINKINFO*, MODBUSLINKINFO*,
								   MBDEFS_ULONG, MBDEFS_UINT8,
								   MBDEFS_UINT16, MBDEFS_UINT16*  );


MBDEFS_BOOL TransmitBytesThroughCommPort( MBDEFS_UINT8*, MBDEFS_UINT16,
										  MBDEFS_UINT8 , MBDEFS_UINT8   );

MBDEFS_BOOL ReceiveCommPortBytes( MBDEFS_UINT16 *, MBDEFS_ULONG, MBDEFS_ULONG , 
								  MBDEFS_UINT8, MBDEFS_UINT8);

MBDEFS_NONE MakeCrc(MBDEFS_UINT8*, MBDEFS_UINT16);
MBDEFS_UINT8 CheckCRC(MBDEFS_UINT8*, MBDEFS_UINT16, MBDEFS_UINT8* );

MBDEFS_UCHAR MBGetLastError();	

#endif /*(_MBMASTER_H)*/
/*** EndHeader */