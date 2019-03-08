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

/**************************************************************************************************
* MBDriver.c:  This file contains all the stack initialization functions, like initialise networks,
*			   sessions etc. It also contains the stack entry functions and  some global variable 
*			   declaration.
*			    
* -----------------------------------
* Modification history
* -----------------------------------
* SlNo,Date,	By,		  Details
* -----------------------------------
* 01,04/04/2001,SRIKANTH,  Written
* 02,01/12/2002,RMD,  Reduced the Number of Files. Only Two C files(MBDriver.c & MBFrames.c)
*					  and Three Header files (MBMaster.h, MBDefs.h & MBlinkUser.h) retained.
* 03,05/12/2002,RMD,  Added Functions "MBRead" , "MBWrite" & "ProcessMBRequest".
*					  Function "SendAndReceiveMBFrame" that was main user entry point function
*					  changed accordingly.
* 04,01/01/2003,RMD,  Added code for MODBUS over TCP/IP.
* 05,10/01/2003,RMD,  Added Global variable "MBSequenceFlag" for controlling the sequential call of
*					  the entry point functions by the user.
* 06,11/01/2003,RMD,  Function CheckVarAddress() removed. Addresses are directly checked in the
*					  functions MBRead() and MBWrite().
* 07,13/10/2003,SAN,  Modified functions for providing support to function codes 7, 8, 11, 12, 17,
*					  20, 21, 22, 23, 24 & 43.	
* 08,14/10/2003,RMD,  Removed Function definition MBWrite and the function MBRead is renamed as
*					  MBSendRequest. Henceforth there will be a single entry point function for 
*					  making a MODBUS request.			
* 09,14/10/2003,SAN,  Added variable 'g_FrameConstructFunctionArraySIZE' to calculate the size of
*					  'g_FrameConstructFunctionArray'.
* 10,17/10/2003,SAN,  Removed function 'ConstructMBFrameDataStructure'.
* 11,01/07/2006,SGO,  Added a fix related to exception response handling.
* 12,02/08/2006,GHO,  Removed array g_FrameConstructFunctionArray and replaced relevant logic with
*					  switch case as the logic using function pointers was creating compilation
*					  problems on some embedded platforms. Also removed the related variable named
*					  g_FrameConstructFunctionArraySIZE. Removed array g_FrameExceptionFunctionArray
*					  for a similar reason.
* 13,02/08/2006,GHO,  Removed function ProcessMBRequest and moved all its code to MBSendRequest
*					  to save one additional nesting of functions
* 14,02/08/2006,GHO,  Protected several functions and calls in macros to allow selective inclusion
*					  and exclusion of support modbus functions. Not all functions covered yet
* 15,11/11/2006,GHO,  Add all remaining functions under macros to enable inclusion or exclusion of 
*						the functions by setting a value for the macros.
***************************************************************************************************/
#include "MBMaster.h"
#include "MB_UserPort.h"


xdata NETWORK_INFO MB_NetworkInfo;			/* Structure to hold the network details */
xdata MBDEFS_UCHAR MBErrNo = NOERROR;		/* Global variable to hold the error number */   
xdata MBDEFS_UINT8 MBSequenceFlag = 0;		/* Flag for controlling the sequential call of the functions*/   
xdata MBDEFS_UINT8 MBSendBuffer[TX_BUFFER_SIZE]; /*Buffer to hold the sending frame*/
xdata MBDEFS_UINT8 MBReceiveBuffer[RX_BUFFER_SIZE];

/* Supported MEI Types */
xdata MBDEFS_UINT8 MEI_Types_Supported[] ={
										0x0E   /* 1. Read Device Identification */
									};


/***************************************************************************************************
 *  FUNCTION: MBDriver_InitSerialDriver
 *	
 *  FILENAME: MBDriver.c
 *	
 *  PARAMETERS: MBDEFS_UINT8 PortNo: The communication path number over which to communicate.
 *				MBDEFS_ULONG BaudRate: Communication baud rate.
 *				MBDEFS_UCHAR Parity: Parity to be used for communication
 *				MBDEFS_UCHAR DataBits: Number of databits per byte.
 *				MBDEFS_UCHAR StopBits: Number of StopBits per byte.
 *	
 *  DESCRIPTION: Initialises the communication path for serial line.
 *									
 *  RETURNS: MBDEFS_TRUE : If initialisation successful
 *			 MBDEFS_FALSE : If initialisation fails
 *	
 **************************************************************************************************/
#if !MODBUS_TCP
MBDEFS_BOOL MBDriver_InitSerialDriver( MBDEFS_UINT8 PortNo,
									   MBDEFS_ULONG BaudRate,
									   MBDEFS_UCHAR Parity,
									   MBDEFS_UCHAR DataBits,
									   MBDEFS_UCHAR StopBits )
{
	xdata MBDEFS_ULONG NetworkId;

	MBErrNo = NOERROR;
	/* This funtion needs to be called only once at the begining. Once the MODBUS
	is initialized successfully this function can be called only after 
	de-initialization. Under all other conditions if this function is called
	then return FALSE*/
	if(MBSequenceFlag != 0)
	{
		#if DEBUGENABLED
			printf("\nError: The Driver already Initialized\n");
			//fflush(stdout);
		#endif
		MBErrNo = BAD_SEQUENCE;
		return MBDEFS_FALSE;
	}

	NetworkId = MBLINKUSER_INITSERIALCOMMNPATH(PortNo,BaudRate,Parity,DataBits,StopBits);
	if( NetworkId == INITERR )
	{
		#if (DEBUGENABLED)
			printf("\nModbusinit : Init path error\n");
//			fflush(stdout);
		#endif
		return(MBDEFS_FALSE);
	}
	else
	{
		MB_NetworkInfo.NetworkId = NetworkId;
		
	}

	/*After Initialization set the sequence flag to one to allow calling
	of other MODBUS functions*/
	MBSequenceFlag = 1;

	return(MBDEFS_TRUE);
	
}
#endif

/***************************************************************************************************
 *  FUNCTION: MBDriver_InitTCPDriver
 *	
 *  FILENAME: MBDriver.c
 *	
 *  PARAMETERS: MBDEFS_CHAR *SlaveIPAddr : IP address of the MODBUS Slave with which to communicate.
 *				MBDEFS_USHORT IPPortNo : MODBUS Standard defines TCP/IP Port No 502 for its communication.
 *
 *  DESCRIPTION: Initialises the communication path for TCP/IP.
 *									
 *  RETURNS: MBDEFS_TRUE : If initialisation successful
 *			 MBDEFS_FALSE : If initialisation fails
 *	
 **************************************************************************************************/
#if MODBUS_TCP
MBDEFS_BOOL MBDriver_InitTCPDriver(MBDEFS_CHAR *SlaveIPAddr, MBDEFS_USHORT IPPortNo)
{

	xdata MBDEFS_ULONG NetworkId;

	MBErrNo = NOERROR;
	/* This funtion needs to be called only once at the begining. Once the MODBUS
	is initialized successfully this function can be called only after 
	de-initialization. Under all other conditions if this function is called
	then return FALSE*/
	if(MBSequenceFlag != 0)
	{
		#if DEBUGENABLED
			printf("\nError: The Driver already Initialized\n");
			fflush(stdout);
		#endif
		MBErrNo = BAD_SEQUENCE;
		return MBDEFS_FALSE;
	}

	NetworkId = MBLINKUSER_INIT_TCPCOMMNPATH(SlaveIPAddr, IPPortNo);
	
	if( NetworkId == INITERR )
	{
		#if (DEBUGENABLED)
			printf("\nModbusinit : Init path error\n");
			fflush(stdout);
		#endif
		return(MBDEFS_FALSE);
	}
	else
	{
		MB_NetworkInfo.NetworkId = NetworkId;
		
	}

	/*After Initialization set the sequence flag to one to allow calling
	  of other MODBUS functions*/
	MBSequenceFlag = 1;

	return(MBDEFS_TRUE);
	
}
#endif



/********************************************************************************
 *  FUNCTION: MBDriver_DeInit         				     	                    
 *																				
 *  FILENAME: MBDriver.c
 *		
 *  PARAMETERS: None
 *		                                                                
 *  DESCRIPTION:De-initializes the communication path and datastructures.   
 *	
 *  RETURNS: MBDEFS_TRUE : If De-initialisation successful
 *			 MBDEFS_FALSE : If De-initialisation fails
 *	
 ********************************************************************************/
MBDEFS_BOOL MBDriver_DeInit()
{	
	MBErrNo = NOERROR;
	/*If this function is called before initialization of MODBUS then return FALSE*/
	if(MBSequenceFlag == 0)
	{
		#if DEBUGENABLED
			printf("\nError: The Driver not Initialized\n");
//			fflush(stdout);
		#endif
		MBErrNo = BAD_SEQUENCE;
		return MBDEFS_FALSE;
	}
	/*After De-initialization set the sequence flag to zero to allow re-initializing
	  of the MODBUS driver*/
	MBSequenceFlag = 0;

	return(MBLINKUSER_CLOSECOMMNPATH(MB_NetworkInfo.NetworkId));	
}


/************************************************************************************************
 *  FUNCTION: MBSendRequest                                               
 *																				  
 *  FILENAME: MBDriver.c										
 *																				   
 *  PARAMETERS: SlaveNo   	: Session number,  							   	       
 *              FunctionCode: Function code,                 					   
 *              VarAddress  : Starting address of the variables,                   
 *				NItems      : Number of variables to be read or written,           
 *				*Data       : Data buffer,  								       
 *				TimeOut     : TimeOut value for Sending & Receiving Frames         
 *				Retries     : No Of Retries to be applied		 				   
 *																				   	                                                                
 *  DESCRIPTION: Constructs the Modbus Data And Control Function Structure         
 *				 by refering the user given data. Also calls the function          
 *				 to send & receve the Frames									   
 *																				   
 *  RETURNS: MBDEFS_TRUE  (success = no errors occurred during function execution 
 *           MBDEFS_FALSE (failure = if errors occurred during function execution
 *                         
 ************************************************************************************************/	
MBDEFS_BOOL MBSendRequest(	MBDEFS_UINT8	SlaveNo,
							   MBDEFS_UINT8		FunctionCode,
							   MBDEFS_UINT16	VarAddress,
							   MBDEFS_UINT16	NItems,
							   MBDEFS_UINT16*	varData,  
							   MBDEFS_ULONG		TimeOut,
							   MBDEFS_UINT8		Retries )
{

	xdata MODBUSLINKINFO SendInfo, RecvInfo;
	xdata MB_USER_PASSED_INFO UserPassedInfo;
	xdata MBDEFS_BOOL RetValBool;

	MBErrNo = NOERROR;
	
	/*If this function is called before initialization of MODBUS then return FALSE*/
	if(MBSequenceFlag == 0)
	{
		#if DEBUGENABLED
			printf("\nError: The Driver not Initialized\n");
//			fflush(stdout);
		#endif
		MBErrNo = BAD_SEQUENCE;
		return MBDEFS_FALSE;
	}
	//test_flag1 =1;
	if(SlaveNo == 0)
	{
		if(!((FunctionCode == 5) || (FunctionCode == 6) || (FunctionCode == 15) || (FunctionCode == 16)))
		{
			MBErrNo = INVALID_SLAVE_NO;
			return(MBDEFS_FALSE);
		}
	}
	else if( !(SlaveNo > 0 && SlaveNo < 248))/*Check the validity of the slave address(1 - 247)*/
	{
		MBErrNo = INVALID_SLAVE_NO;
		return(MBDEFS_FALSE);
	}
	
	if( (FunctionCode<7) || (FunctionCode==15) || (FunctionCode==16) || (FunctionCode==22) || (FunctionCode==23) || (FunctionCode==24) )
	{
		/*check if starting address of registers to be read is Zero*/
		 if( VarAddress == 0)
		 {
	        #if DEBUGENABLED
				printf("\nStarting address could not have zero value");
	        #endif
			MBErrNo = INVALID_ADDRESS; 
			return(MBDEFS_FALSE);
		 }
		 VarAddress--;
	}
	
	UserPassedInfo.VarAddress	= VarAddress;
	UserPassedInfo.varData		= varData;

	SendInfo.UserPassedInfo		= &UserPassedInfo;

	SendInfo.SlaveInfo.SessionNo = SlaveNo;
	SendInfo.FunctionCode		 = FunctionCode;
    SendInfo.SlaveInfo.SlaveNo   = SlaveNo;
	 
	switch (SendInfo.FunctionCode)
	{
		case READCOILSTATUS:
		case READINPUTSTATUS:
		case READHOLDINGREGISTERS:
		case READINPUTREGISTERS:
		case FORCESINGLECOIL:
		case PRESETSINGLEREGISTER:
		{
			SendInfo.FrameLength = 8;
		}
		break;
		#if !MODBUS_TCP
			case DIAGNOSTICS:
			{
				SendInfo.FrameLength = 8;
			}
			break;
			case READEXCEPTIONSTATUS:
			case GETCOMMEVENTCOUNTER:
			case GETCOMMEVENTLOG:
			case REPORTSLAVEID:
			{
				SendInfo.FrameLength = 4;
			}
			break;
		#endif
		case ENCAPSULATEDINTERFACETRANSPORT:
		{
			SendInfo.MEIInfo = MBDEFS_NULL;
			RecvInfo.MEIInfo = MBDEFS_NULL;
			SendInfo.FrameLength = 4;
		}
		break;
		case FORCEMULTIPLECOILS:
		case PRESETMULTIPLEREGISTERS:
		{
			SendInfo.FrameLength = 9;
		}
		break;
		case READFILERECORD:
		case WRITEFILERECORD:
		{
			SendInfo.FrameLength = 5;
		}
		break;
		case MASKWRITEREGISTER:
		{
			SendInfo.FrameLength = 10;
		}
		break;
		case READWRITEMULTIPLEREGISTERS:
		{
			SendInfo.FrameLength = 13;
			
		}
		break;
		case READFIFOQUEUE:
		{
			SendInfo.FrameLength = 6;
		}
		break;
	}

    SendInfo.ResponseFramelength = 2; /*Minimum frame length taken as two - Sl.Add + FC = 2 Bytes */

	SendInfo.Quantity = NItems;

	/* Construct and Send and Receive and Parse the Frame */
	RetValBool = SendAndReceiveMBFrame(&SendInfo,&RecvInfo,TimeOut,Retries,NItems,varData);
//test_flag1 =1;
	if( (SendInfo.FunctionCode == ENCAPSULATEDINTERFACETRANSPORT) &&
		(SendInfo.MEIInfo != MBDEFS_NULL)
	  )
	{
		/* free send info memory */
		free(SendInfo.MEIInfo ->MEIData );		/* free the data buffer			*/
		free(SendInfo.MEIInfo );				/* free the MEIInfo instance	*/
		SendInfo.MEIInfo = MBDEFS_NULL;
	}
	
	
	return(RetValBool);

}


/**********************************************************************************************
 *  FUNCTION: SendAndReceiveMBFrame    	                                    
 *																				
 *  FILENAME: MBDriver.c														
 *																				
 *  PARAMETERS: *SendInfo,*RecvInfo: Pointers to Modbus Data And          	   	
 *									 Control Function Structures                
 *				unsigned long  TimeOut: Time out value							
 *				unsigned char  Retries: No of retries						    
 *				unsigned short NItems : No of items								
 *				unsigned short* varData  : data buffer								
 *																					                                                                
 *  DESCRIPTION:Sends the request and receives back the response                
 *																				
 *  RETURNS: Status : True or false            									
 *																				
 **********************************************************************************************/

MBDEFS_BOOL SendAndReceiveMBFrame( MODBUSLINKINFO *SendInfo,
								   MODBUSLINKINFO *RecvInfo,
								   MBDEFS_ULONG TimeOut,
								   MBDEFS_UINT8 Retries,
								   MBDEFS_UINT16 NItems,
								   MBDEFS_UINT16 *varData  )
{    
	xdata MBDEFS_BOOL RetValBool = MBDEFS_TRUE;
	xdata MBDEFS_UINT8 Success;
	xdata MBDEFS_UINT8 ParseStatus = 0;
	xdata MBDEFS_ULONG PresentTime,TransmitTime;
	xdata MBDEFS_UINT8 TrialNo = 0;
	xdata MBDEFS_UINT16 ResponseBytes;
	xdata MBDEFS_UINT16 NumOfBytesToTrasmit;	/*For Serial transmission*/

	#if MODBUS_TCP
		xdata MBDEFS_UINT16 TransId,ProtocolId,FrameLength;
	#endif
	#if DEBUGENABLED
		xdata MBDEFS_UINT16 i,j;
	#endif

	switch (SendInfo->FunctionCode)
	{
		#if READ_COILS_SUPPORTED
			case READCOILSTATUS:
				Success = ConstructFC1(SendInfo);
			break;
		#endif
		#if DISCRETE_INPUTS_SUPPORTED
			case READINPUTSTATUS:
				Success = ConstructFC2(SendInfo);
			break;
		#endif
		#if READ_HOLDING_REGISTERS_SUPPORTED
			case READHOLDINGREGISTERS:
				Success = ConstructFC3(SendInfo);
			break;
		#endif
		#if INPUT_REGISTERS_SUPPORTED
			case READINPUTREGISTERS:
				Success = ConstructFC4(SendInfo);
			break;
		#endif
		#if WRITE_COILS_SUPPORTED
			case FORCESINGLECOIL:
				Success = ConstructFC5(SendInfo);
			break;
		#endif
		#if WRITE_HOLDING_REGISTERS_SUPPORTED
			case PRESETSINGLEREGISTER:
				Success = ConstructFC6(SendInfo);
			break;
		#endif
		#if !MODBUS_TCP
			#if GET_EXCEPTION_COIL_DATA_SUPPORTED
				case READEXCEPTIONSTATUS:
					Success = ConstructFC7(SendInfo);
				break;
			#endif
			#if DIAGNOSTICS_SUPPORTED
				case DIAGNOSTICS:
					Success = ConstructFC8(SendInfo);
				break;
			#endif
			#if FETCH_COMM_EVENT_COUNTER_SUPPORTED
				case GETCOMMEVENTCOUNTER:
					Success = ConstructFC11(SendInfo);
				break;
			#endif
			#if FETCH_COMM_EVENT_LOG_SUPPORTED
				case GETCOMMEVENTLOG:
					Success = ConstructFC12(SendInfo);
				break;
			#endif
			#if REPORT_SLAVE_ID_SUPPORTED
				case REPORTSLAVEID:
					Success = ConstructFC17(SendInfo);
				break;
			#endif
		#endif
		
		#if WRITE_COILS_SUPPORTED
			case FORCEMULTIPLECOILS:
				Success = ConstructFC15(SendInfo);
			break;
		#endif
		#if WRITE_HOLDING_REGISTERS_SUPPORTED
			case PRESETMULTIPLEREGISTERS:
				Success = ConstructFC16(SendInfo);
			break;
		#endif
		#if READ_FILE_RECORD_SUPPORTED
			case READFILERECORD:
				Success = ConstructFC20(SendInfo);
			break;
		#endif
		#if WRITE_FILE_RECORD_SUPPORTED
			case WRITEFILERECORD:
				Success = ConstructFC21(SendInfo);
			break;
		#endif
		#if MASK_WRITE_REGISTER_SUPPORTED
			case MASKWRITEREGISTER:
				Success = ConstructFC22(SendInfo);
			break;
		#endif
		#if READ_WRITE_REGISTERS_SUPPORTED
			case READWRITEMULTIPLEREGISTERS:
				Success = ConstructFC23(SendInfo);
			break;
		#endif
		#if READ_FIFO_QUEUE_SUPPORTED
			case READFIFOQUEUE:
				Success = ConstructFC24(SendInfo);
			break;
		#endif
		#if ENCAPSULATED_INTERFACE_TRANSPORT_SUPPORTED
			case ENCAPSULATEDINTERFACETRANSPORT:
				RecvInfo->MEIInfo = MBDEFS_NULL;
				SendInfo->MEIInfo = MBDEFS_NULL;
				Success = ConstructFC43(SendInfo);
			break;
		#endif
		default:
		{
			/* Function Code not supported */
			#if DEBUGENABLED
				printf("\nFunction Code Not Supported");
			#endif
			MBErrNo = UNKNOWN_REQUEST;
			return(MBDEFS_FALSE);
		}
	}
	
	if(Success == MBDEFS_FALSE) 
	{
		/* Frame Construction failed */
		#if DEBUGENABLED
				printf("\nError occurred in constructing the frame");
		#endif
		return(MBDEFS_FALSE);
	}

	/*Calculate the expected number of bytes in the response frame*/
   	ResponseBytes = SendInfo->ResponseFramelength;
	#if MODBUS_TCP					/*For MODBUS over TCP/IP				   */
		ResponseBytes = ResponseBytes + 6;		/*ResponseBytes = MBAP Header */
	#endif
	
	NumOfBytesToTrasmit = SendInfo->FrameLength;/*For Serial transmission*/

	/* Add the TCP/IP Header*/
	#if MODBUS_TCP
		TransId = TRANSACT_ID;
		ProtocolId = PROTOCOL_ID;
		FrameLength = SendInfo->FrameLength - 2;
		MBSendBuffer[0] = ((MBDEFS_UINT8 *)(&TransId))[HIGH];
		MBSendBuffer[1] = ((MBDEFS_UINT8 *)(&TransId))[LOW];
		MBSendBuffer[2] = ((MBDEFS_UINT8 *)(&ProtocolId))[HIGH];
		MBSendBuffer[3] = ((MBDEFS_UINT8 *)(&ProtocolId))[LOW];
		MBSendBuffer[4] = ((MBDEFS_UINT8 *)(&FrameLength))[HIGH];
		MBSendBuffer[5] = ((MBDEFS_UINT8 *)(&FrameLength))[LOW];
		NumOfBytesToTrasmit = FrameLength + 6;
	#endif

	while (Retries >= TrialNo++)
    {
		/*Flush the communication path before transmitting a request*/
		MBLINKUSER_FLUSHCOMMNPATH(MB_NetworkInfo.NetworkId);
		inituart_transmitmode();
		Success = TransmitBytesThroughCommPort(MBSendBuffer, NumOfBytesToTrasmit,
											   Retries, TrialNo);			 
	
   	    TransmitTime = MBTARG_GETSYSTIMEMILLISECS();	/*Get the time of transmit*/

	    if (Success != MBDEFS_TRUE)
        {
			continue;
        }
	    else
        {
			/*If The request was a broadcast message return*/
			if(SendInfo->SlaveInfo.SlaveNo == 0)
			{
				#if DEBUGENABLED
					printf("\nBroadcast Message - Response will not be available\n");
				#endif
				return(MBDEFS_TRUE);
			}

			RetValBool = MBDEFS_TRUE;
			inituart_recvmode();
			do
			{
				Success = ReceiveCommPortBytes(&ResponseBytes,TransmitTime,TimeOut,Retries,TrialNo);
				if( Success == NOBYTES)
			  	{
			  		PresentTime = MBTARG_GETSYSTIMEMILLISECS();
					if( (PresentTime - TransmitTime) >= TimeOut)
					{
						RetValBool = MBDEFS_FALSE;
					}
			  	}
			  	else
				{
					break;
				}
          	}while(RetValBool);

		  	if (RetValBool != MBDEFS_TRUE)	/*if time out is reached*/
          	{
				#if DEBUGENABLED
					printf("\nTime out reached, No bytes received\n");
				#endif
				if (TrialNo	> Retries)
				{
					MBErrNo = TIMEOUT;
					return(MBDEFS_FALSE);
				}
				else
				{
					continue;				
				}
          	}
          	if (Success != MBDEFS_TRUE)
          	{
				continue; 
          	} 
		  
		  	if(MBReceiveBuffer[SLAVEADDRESS] != SendInfo->SlaveInfo.SlaveNo)
			{
				MBErrNo = SLAVEADD_MISMATCH;
				return(MBDEFS_FALSE);
			}
			#if MODBUS_TCP
				((MBDEFS_UINT8*)(&TransId))[HIGH] = MBReceiveBuffer[0];	
				((MBDEFS_UINT8*)(&TransId))[LOW] = MBReceiveBuffer[1];	 	 
				 if(TransId != TRANSACT_ID)
				 {
					MBErrNo = TRANSACT_ID_MISMATCH;
					return(MBDEFS_FALSE);
				 }
			#endif
			
			if(MBReceiveBuffer[FUNCTION] > 0x80)
			{
				RecvInfo->FrameLength = 5;
				#if MODBUS_TCP
					RecvInfo->FrameLength = 9;
				#endif
			}
			else
			{
				RecvInfo->FrameLength = ResponseBytes ;
			}	
			
			#if DEBUGENABLED
				printf("\nReceived frame:");
				printf("\n<-------");
				for(i = 0, j = 0; i < RecvInfo->FrameLength; i++,j++)
				{
					if(j > 14)
					{
						printf("\n\t");
						j = 0;
					}
					printf("%4x",*(MBReceiveBuffer + i));
				}
				printf("\n");
//				fflush(stdout);
			#endif

			RecvInfo->Quantity = SendInfo->Quantity;
			RecvInfo->SlaveInfo.SessionNo = SendInfo->SlaveInfo.SessionNo;

			switch (SendInfo->FunctionCode)
			{
				#if READ_COILS_SUPPORTED
					case READCOILSTATUS:
						ParseStatus=ParseFC1(SendInfo, RecvInfo, varData);
					break;
				#endif
				#if DISCRETE_INPUTS_SUPPORTED
					case READINPUTSTATUS:
						ParseStatus=ParseFC2(SendInfo, RecvInfo, varData);
					break;
				#endif
				#if READ_HOLDING_REGISTERS_SUPPORTED
					case READHOLDINGREGISTERS:
						ParseStatus=ParseFC3(SendInfo, RecvInfo, varData);
					break;
				#endif
				#if INPUT_REGISTERS_SUPPORTED
					case READINPUTREGISTERS:
						ParseStatus=ParseFC4(SendInfo, RecvInfo, varData);
					break;
				#endif
				#if WRITE_COILS_SUPPORTED
					case FORCESINGLECOIL:
						ParseStatus=ParseFC5(SendInfo, RecvInfo, varData);
					break;
				#endif
				#if WRITE_HOLDING_REGISTERS_SUPPORTED
					case PRESETSINGLEREGISTER:
						ParseStatus=ParseFC6(SendInfo, RecvInfo, varData);
					break;
				#endif			
				#if !MODBUS_TCP
					#if GET_EXCEPTION_COIL_DATA_SUPPORTED
						case READEXCEPTIONSTATUS:
							ParseStatus=ParseFC7(SendInfo, RecvInfo, varData);
						break;
					#endif
					#if DIAGNOSTICS_SUPPORTED
						case DIAGNOSTICS:
							ParseStatus=ParseFC8(SendInfo, RecvInfo, varData);
						break;
					#endif 
					#if FETCH_COMM_EVENT_COUNTER_SUPPORTED
						case GETCOMMEVENTCOUNTER:
							ParseStatus=ParseFC11(SendInfo, RecvInfo, varData);
						break;
					#endif
					#if FETCH_COMM_EVENT_LOG_SUPPORTED
						case GETCOMMEVENTLOG:
							ParseStatus=ParseFC12(SendInfo, RecvInfo, varData);
						break;
					#endif
					#if REPORT_SLAVE_ID_SUPPORTED
						case REPORTSLAVEID:
							ParseStatus=ParseFC17(SendInfo, RecvInfo, varData);
						break;
					#endif
				#endif
				#if WRITE_COILS_SUPPORTED
					case FORCEMULTIPLECOILS:
						ParseStatus=ParseFC15(SendInfo, RecvInfo, varData);
					break;
				#endif
				#if WRITE_HOLDING_REGISTERS_SUPPORTED
					case PRESETMULTIPLEREGISTERS:
						ParseStatus=ParseFC16(SendInfo, RecvInfo, varData);
					break;
				#endif
				#if READ_FILE_RECORD_SUPPORTED
					case READFILERECORD:
						ParseStatus=ParseFC20(SendInfo, RecvInfo, varData);
					break;
				#endif
				#if WRITE_FILE_RECORD_SUPPORTED
					case WRITEFILERECORD:
						ParseStatus=ParseFC21(SendInfo, RecvInfo, varData);
					break;
				#endif
				#if MASK_WRITE_REGISTER_SUPPORTED
					case MASKWRITEREGISTER:
						ParseStatus=ParseFC22(SendInfo, RecvInfo, varData);
					break;
				#endif
				#if READ_WRITE_REGISTERS_SUPPORTED
					case READWRITEMULTIPLEREGISTERS:
						ParseStatus=ParseFC23(SendInfo, RecvInfo, varData);
					break;
				#endif
				#if READ_FIFO_QUEUE_SUPPORTED
					case READFIFOQUEUE:
						ParseStatus=ParseFC24(SendInfo, RecvInfo, varData);
					break;
				#endif
				#if ENCAPSULATED_INTERFACE_TRANSPORT_SUPPORTED
					case ENCAPSULATEDINTERFACETRANSPORT:
						ParseStatus=ParseFC43(SendInfo, RecvInfo, varData);
					break;
				#endif
			}

			#if DEBUGENABLED
				/*printf("\nParse status: %u ",ParseStatus);*/
			#endif

			switch (ParseStatus)
			{
				case 0:
					HandleSuccess(SendInfo);
				break;
				case 1:
					HandleException1(SendInfo);
				break;
				case 2:
					HandleException2(SendInfo);
				break;
				case 3:
					HandleException3(SendInfo);
				break;
				case 4:
					HandleException4(SendInfo);
				break;
				case 5:
					HandleException5(SendInfo);
				break;
				case 6:
					HandleException6(SendInfo);
				break;
				case 7:
					HandleException7(SendInfo);
				break;
				case 8:
					HandleException8(SendInfo);
				break;
				case 9:
					HandleCRCEror(SendInfo);
				break;
			}					
			
			break;
		}
    }/*End of while loop*/		
				
	if(ParseStatus > 0 && ParseStatus <= CRCERROR)
	{
		if(ParseStatus == CRCERROR)
		{
			MBErrNo = CHECKSUM_ERROR;
		}
		else
		{
			MBErrNo = ParseStatus;
		}
		return(MBDEFS_FALSE);
	}
	
	if( (SendInfo->FunctionCode == ENCAPSULATEDINTERFACETRANSPORT) &&
		(RecvInfo->MEIInfo != MBDEFS_NULL)
	  )
	{
		if(RecvInfo->MEIInfo ->MEIData != MBDEFS_NULL)
		{
			free(RecvInfo->MEIInfo ->MEIData );		/* free the data buffer			*/
			RecvInfo->MEIInfo ->MEIData = MBDEFS_NULL;
		}
		free(RecvInfo->MEIInfo );				/* free the MEIInfo instance	*/
		RecvInfo->MEIInfo = MBDEFS_NULL;
	}
	return(Success);
}

/****************************************************************************************
 ** TransmitBytesThroughCommPort
 *	
 *  FILENAME:MBDriver.c
 *
 *  PARAMETERS:	 unsigned char *RequestFrameBuf
 *				 unsigned short NoOfBytesToWrite
 *				 unsigned char Retries
 *				 unsigned char TrialNo
 *
 *  DESCRIPTION:  Function to write the given buffer to the given port
 *
 *  RETURNS:  MBDEFS_BOOL - MBDEFS_TRUE  (success = no errors occurred during writing)
 *               		   - MBDEFS_FALSE (failure = if errors occurred during writing)
 *																						
 ****************************************************************************************/
MBDEFS_BOOL TransmitBytesThroughCommPort(MBDEFS_UINT8 *RequestFrameBuf,
                                         MBDEFS_UINT16 NoOfBytesToWrite,
										 MBDEFS_UINT8 Retries,
										 MBDEFS_UINT8 TrialNo )
{
	xdata MBDEFS_BOOL RetValBool;
	xdata MBDEFS_ULONG BytesWrote = 0, Ctr = 0;

	#if (DEBUGENABLED)
		xdata MBDEFS_UINT16 testind, i;
		printf("\nRequest frame transmitting:");
		printf("\n------->");
		for(testind = 0, i = 0; testind<NoOfBytesToWrite; testind++, i++)
		{
			if(i > 14)
			{
				printf("\n\t");
				i = 0;
			}
			printf("%4x", RequestFrameBuf[testind]);
		}
		printf("\n");
//		fflush(stdout);
	#endif

	do
	{
		NoOfBytesToWrite -= (MBDEFS_USHORT)BytesWrote;
//		test_flag5 = NoOfBytesToWrite;
		/* Transmit the frame to the given port using a user interface function */
	    RetValBool = MBLINKUSER_WRITETOCOMMNPATH(MB_NetworkInfo.NetworkId, &RequestFrameBuf[Ctr], NoOfBytesToWrite, &BytesWrote);
	    if( RetValBool != MBDEFS_TRUE )
		{
			#if DEBUGENABLED
				printf("\nWrite to Comm path error");
//				fflush(stdout);
			#endif
			if(TrialNo > Retries)
			{
				MBErrNo = NETWORK_WRITE_ERR;				
			}
			return(RetValBool);
		}
		Ctr += BytesWrote;
		//test_flag4 = Ctr;
	}while( BytesWrote < NoOfBytesToWrite);

	return(MBDEFS_TRUE);
}

 /*********************************************************************************
 *  ReceiveCommPortBytes
 *
 *  FILENAME: MBDriver.c
 *
 *  PARAMETERS:	 Unsigned short *ResponseBytes
 *				 Unsigned long  TransmitTime
 *				 Unsigned long  TimeOut
 *				 unsigned char Retries
 *				 unsigned char TrialNo				 
 *
 *  DESCRIPTION:  Function to read from the network 
 *
 *  RETURNS:  MBDEFS_BOOL - MBDEFS_TRUE  (success = no errors occurred)
 *               		   - MBDEFS_FALSE (failure = if errors occurred)
 *
 ***********************************************************************************/
MBDEFS_BOOL ReceiveCommPortBytes(  MBDEFS_UINT16 *ResponseBytes,
								   MBDEFS_ULONG TransmitTime ,
								   MBDEFS_ULONG TimeOut,
								   MBDEFS_UINT8 Retries,
								   MBDEFS_UINT8 TrialNo )
{
	xdata MBDEFS_BOOL RetValBool = 0;
	xdata MBDEFS_UINT16 TotalBytesToRead =0, InitialBytesToRead = 0;
	xdata MBDEFS_ULONG PresentTime, BytesRead = 0, Ctr = 0;
	xdata MBDEFS_ULONG NetworkId;
	static xdata MBDEFS_UINT8 instance = 0;
	
   	/**************************** Read from port starts ***************************/

	NetworkId = MB_NetworkInfo.NetworkId;
	
	//test_flag2	 = *ResponseBytes;
	/*First the bytes till Function Code Field are read. Then depending on  
	the normal function Code or Exception function code, the remaining bytes 
	to be read are decided*/
	InitialBytesToRead = 2;
	#if MODBUS_TCP
		InitialBytesToRead += 6;/*For MODBUS over TCP 6 bytes of MBAP header should also be read*/
	#endif
	TotalBytesToRead = InitialBytesToRead;
	
	/*Allocate memory for storing the received response bytes -- Max Frame Length is 256 */

	/* Read up to Function code */
	RetValBool = MBLINKUSER_READFROMCOMMNPATH(NetworkId, MBReceiveBuffer, TotalBytesToRead, &BytesRead);
	//test_flag3 = 1;
	if(RetValBool == MBDEFS_TRUE)
	{
		if( BytesRead == 0 )
		{
			return(NOBYTES);		/* Return still no bytes are available */
		}
		Ctr = BytesRead;
		while(BytesRead < TotalBytesToRead)
		{
			//test_flag3 = 2;
			PresentTime = MBTARG_GETSYSTIMEMILLISECS();
			if( (PresentTime - TransmitTime) > TimeOut)
			{
				MBErrNo = TIMEOUT;
				return(MBDEFS_FALSE);
			}
			TotalBytesToRead -= (MBDEFS_USHORT)BytesRead;
			RetValBool = MBLINKUSER_READFROMCOMMNPATH(NetworkId, &MBReceiveBuffer[Ctr], TotalBytesToRead, &BytesRead);
			if(RetValBool == MBDEFS_FALSE)
			{
				if(TrialNo > Retries)
				{
					MBErrNo = NETWORK_READ_ERR;														
				}
				return(MBDEFS_FALSE);	
			}
			Ctr += BytesRead;	
		}
	}
	else
	{
		if(TrialNo > Retries)
		{
			MBErrNo = NETWORK_READ_ERR;		
		}
		return(MBDEFS_FALSE);
	}

	/*Once the Response Header is read, decide the number of remaining response bytes to be read*/
	if(MBReceiveBuffer[FUNCTION] > 0x80)
	{
		switch (MBReceiveBuffer[FUNCTION])
		{
		case 0xAB:  /* Encapsulated Interface Transport */
			{
				TotalBytesToRead = 2;
				break;
			}
		default:
			{
				TotalBytesToRead = 1;
				break;
			}
		}			
	}
	else
	{
		#if MODBUS_TCP  
			TotalBytesToRead = *ResponseBytes - 8 ;
		#else
			TotalBytesToRead = *ResponseBytes - 2 ;
		#endif
	}
	
	Ctr = InitialBytesToRead;
	BytesRead = 0;
	instance = 0; /* Before entering the loop assign zero */
	do
	{
		//test_flag2	 = 1;
		//test_flag3 = 3;
		TotalBytesToRead -= (MBDEFS_USHORT)BytesRead;
		//test_flag4 = TotalBytesToRead;
		//test_flag5 = BytesRead;
		RetValBool = MBLINKUSER_READFROMCOMMNPATH(NetworkId, &MBReceiveBuffer[Ctr], TotalBytesToRead , &BytesRead);
		if( RetValBool == MBDEFS_FALSE )
		{
			#if DEBUGENABLED
				printf("\nRead from path error");
//				fflush(stdout);
			#endif
			if(TrialNo > Retries)
			{
				MBErrNo = NETWORK_READ_ERR;
			}
			return(MBDEFS_FALSE);	
		}
		PresentTime = MBTARG_GETSYSTIMEMILLISECS();
		if( (PresentTime - TransmitTime) > TimeOut)
		{
			MBErrNo = TIMEOUT;
			return(MBDEFS_FALSE);
		}

		Ctr += BytesRead;

		/* For Encapsulated Interface Transport */
		switch (MBReceiveBuffer[FUNCTION])
		{
			case ENCAPSULATEDINTERFACETRANSPORT:
			{
				switch (MBReceiveBuffer[MEITYPE])
				{
					case READDEVICEIDENTIFICATION:  /* Read Device Identification */
					{
						switch (instance)
						{
							case 0:
							{
								if( (Ctr-1) == MEITYPE)
								{
									TotalBytesToRead = 5; 
									BytesRead = 0;
									/* Read Dev Id Code + Conformity Level + More Follows + 
									Next Object Id + Number of Objects */
									*ResponseBytes += 5;
									instance ++; /* instance =1 */
								}
								break;
							}
							case 1: case 3: case 5:
							{
								if( (Ctr == *ResponseBytes) && (MBReceiveBuffer[NUMBEROFOBJECTS] > 0) && (instance <= MBReceiveBuffer[NUMBEROFOBJECTS] * 2) )
								{
									TotalBytesToRead = 2; /* Object Id + Object Length */
									BytesRead = 0;
									*ResponseBytes += 2;
									instance ++; /* instance =2,4,6 */
								}
								break;
							}
							case 2: case 4: case 6:
							{
								if( Ctr == *ResponseBytes )
								{
									TotalBytesToRead = MBReceiveBuffer[*ResponseBytes -1]; /* Object Val */
									BytesRead = 0;
									*ResponseBytes += MBReceiveBuffer[*ResponseBytes -1];
									instance ++; /* instance =3,5,7 */
								}
								break;
							}
							default:
								break;
						}
						break;
					}
					default:
					{
						break;
					}
				}
				break;
			}
			
		#if !MODBUS_TCP
			case REPORTSLAVEID:
			{
			    if((Ctr == *ResponseBytes) && (instance == 0))
				{
					TotalBytesToRead = MBReceiveBuffer[BYTECOUNTRESPONSE] - 2; 
					BytesRead = 0;
					*ResponseBytes += MBReceiveBuffer[BYTECOUNTRESPONSE] - 2;
					instance++;
				}
				break;
			}
					
			case GETCOMMEVENTLOG:
		#endif
			case READFILERECORD:
			case WRITEFILERECORD:
			{
			    if((Ctr == *ResponseBytes) && (instance == 0))
				{
					TotalBytesToRead = MBReceiveBuffer[BYTECOUNTRESPONSE]; 
					BytesRead = 0;
					*ResponseBytes += MBReceiveBuffer[BYTECOUNTRESPONSE];
					instance++;
				}
				break;
			}

			
			case READFIFOQUEUE:
			{
			    if((Ctr == *ResponseBytes) && (instance == 0))
				{
					*((MBDEFS_UINT8 *)&TotalBytesToRead + HIGH) = MBReceiveBuffer[BYTECOUNTRESPONSE]; 
					*((MBDEFS_UINT8 *)&TotalBytesToRead + LOW) = MBReceiveBuffer[BYTECOUNTRESPONSE + 1]; 
					BytesRead = 0;
					*ResponseBytes += TotalBytesToRead;
					instance++;
				}
				break;
			}

			default:
			{
				break;
			}
		}
	}while( BytesRead < TotalBytesToRead);


#if !MODBUS_TCP
	/* Now read the CRC Bytes */
	TotalBytesToRead = 2;
	*ResponseBytes += 2;
	BytesRead = 0;
	do
	{
		TotalBytesToRead -= (MBDEFS_USHORT)BytesRead;
		RetValBool = MBLINKUSER_READFROMCOMMNPATH(NetworkId, &MBReceiveBuffer[Ctr], TotalBytesToRead , &BytesRead);
		if( RetValBool == MBDEFS_FALSE )
		{
			#if DEBUGENABLED
				printf("\nRead from path error");
//				fflush(stdout);
			#endif
			if(TrialNo > Retries)
			{
				MBErrNo = NETWORK_READ_ERR;
			
			}
			return(MBDEFS_FALSE);	
		}
		PresentTime = MBTARG_GETSYSTIMEMILLISECS();
		if( (PresentTime - TransmitTime) > TimeOut)
		{
			MBErrNo = TIMEOUT;
			return(MBDEFS_FALSE);
		}

		Ctr += BytesRead;

		
	}while( BytesRead < TotalBytesToRead);
#endif

	/**************************** Read from port ends *************************/

	return(MBDEFS_TRUE);

} /*end of ReceiveCommPortBytes function*/


/***************************************************************************************
 *  MBGetLastError 
 *
 *  FILENAME	: MBDriver.c
 *
 *  PARAMETERS	: None
 *				 
 *  DESCRIPTION	: Returns the most recent error value.
 * 
 *  RETURNS		: unsigned char MBErrNo   
 *
 ****************************************************************************************/
MBDEFS_UCHAR MBGetLastError()
{
	return(MBErrNo);
}


