#ifndef _MBLINKUSER_H
#define	_MBLINKUSER_H
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


/***************************************************************************************************
* MBUserPort.h:	
*	This file contains the MACROS calls which have to be filled by the user with his function.
* -----------------------------------
* Modification history
* -----------------------------------
* SlNo,Date,	By,		  Details
* -----------------------------------
* 01,04/04/2001,SRIKANTH, Written
* 02,01/12/2002,RMD,	  Reduced the Number of Files. Only Two C files(MBDriver.c & MBFrames.c)
*						  and Three Header files (MBMaster.h, MBDefs.h & MBlinkUser.h) retained.
* 03,01/01/2003,RMD,	  Added code for MODBUS over TCP/IP
* 04,05/01/2003,RMD,	  Changed the filename from "MBLinkUser.h" to "MBUserPort.h"
* 05,06/01/2003,RMD,	  Macro name changed from 'MBLINKUSER_GETSLAVENO' to 'MBLINKUSER_GETSLAVEADDR'
* 06,07/11/2003,SAN,	  Macros "MAXCOILSREAD, MAXREGSREAD, MAXCOILSWRITE, MAXREGSWRITE, 
*						  MEITYPESPECIFICDATA" moved from MBMaster.h to MB_UserPort.h
* 07,21/11/2003,SAN,	  Macros MAXCOILSREAD, MAXREGSREAD, MAXCOILSWRITE & MAXREGSWRITE shifted from
*						  MBMaster.h to MB_UserPort.h.
* 08,05/01/2003,SAN,	  Removed the macro MBLINKUSER_GETSLAVENO() from stack.
* 09,02/08/2006,GHO,	  Added several macros to support selective inclusion/exclusion of modbus 
*							functions. Not all functions covered yet.
* 10,02/08/2006,GHO,	  Added macros CRC_TABLE_IN_RAM, CRC_TABLE_IN_ROM & CRC_TABLE_CREATE_DYNAMIC
*							to enable users to choose placement of CRC table
* 11,02/08/2006,GHO,	  Added macro "xdata" to support placement of globals in external memory
* 12,11/11/2006,GHO,	  Added all remaining functions under macros to enable inclusion or exclusion of 
*							the functions by setting a value for the macros. All supported MODBUS 
*							functions are now under the control of macros.
***************************************************************************************************/
/********************************************************************************************
 ** MODBUS_TCP
 *
 *  FILENAME	: MB_UserPort.h
 *
 *  PARAMETERS	: None
 *
 *  DESCRIPTION	: Indicates if the MODBUS standard followed is for Serial or TCP/IP.
 *				  The MODBUS/TCP Specifications are different from the Serial standard
 *				  mainly in two ways (1) Serial uses a two byte CRC at the end of the  
 *				  frame where as MODBUS/TCP does not (since error checking and correction
 *				  are handled in other layers of TCP/IP itself) (2) MODBUS/TCP uses an
 *				  additional 6 byte header before the slave address field of a frame
 *				  
 *  RETURNS		: None
 *
 ********************************************************************************************/
 #define MODBUS_TCP		0
	
/********************************************************************************************
 ** MAXCOILSREAD, MAXREGSRMEITYPESPECIFICDATAEAD, MAXCOILSWRITE, MAXREGSWRITE, MEITYPESPECIFICDATA
 *
 *  FILENAME	: MB_UserPort.h
 *
 *  PARAMETERS	: None
 *
 *  DESCRIPTION	: Determines the size of the maximum coils, registers to read and write.
 *					Also determines the size of the MEI Type Specific Data. Size differs 
 *					for TCP/IP and Serial.
 *
 *  RETURNS		: None	 
 *
 *******************************************************************************************/
#if MODBUS_TCP						 /* If it is MODBUS on TCP/IP					*/
	#define MAXCOILSREAD		1976 /* Maximum Coils allowed to read				*/
	#define MAXREGSREAD			 123 /* Maximum Registers allowed to read			*/
	#define MAXCOILSWRITE		1944 /* Maximum Coils allowed to write				*/
	#define MAXREGSWRITE		 121 /* Maximum Registers allowed to write			*/
	#define MEITYPESPECIFICDATA	 252 /* Maximum length of MEI Type Specific Data	*/
#else
	#define MAXCOILSREAD		2008 /* If it is MODBUS on Serial					*/
	#define MAXREGSREAD			 125
	#define MAXCOILSWRITE		1976
	#define MAXREGSWRITE		 123
	#define MEITYPESPECIFICDATA	 250
#endif		/*(MODBUS_TCP)*/

/******************************************************************************************
 ** MBLINKUSER_INITSERIALCOMMNPATH
 *
 *  FILENAME	: MB_UserPort.h
 *
 *  PARAMETERS	: Unsigned char PortNo
 *				  Unsigned long BaudRate
 *				  Unsigned char Parity
 *				  Unsigned char	DataBits
 *				  Unsigned char	StopBits
 *
 *  DESCRIPTION	: Initialise the communication path for the given serial port or network
 *
 *  RETURNS		: Unsigned long  NetworkId	: Network path ID
 *									   0	: If Initialisation fails
 *
 *******************************************************************************************/
#if !MODBUS_TCP
	#define MBLINKUSER_INITSERIALCOMMNPATH(PortNo, BaudRate, Parity, DataBits, StopBits) Open(PortNo, BaudRate, Parity, DataBits, StopBits)
#endif

/*******************************************************************************************
 ** MBLINKUSER_INIT_TCPCOMMNPATH
 *
 *  FILENAME	: MB_UserPort.h
 *
 *  PARAMETERS	: MBDEFS_CHAR *SlaveIPAddr : IP address of the MODBUS Slave with which to communicate.  
 *
 *  DESCRIPTION	: Initialise the communication path for TCP/IP network
 *
 *  RETURNS		: Unsigned long  NetworkId : Network path ID
 *									   0  :	If Initialisation fails
 *
 *******************************************************************************************/
#if MODBUS_TCP
	#define MBLINKUSER_INIT_TCPCOMMNPATH(SlaveIPAddr, IPPortNo) OpenTCP(SlaveIPAddr, IPPortNo)
#endif

/*******************************************************************************************
 ** MBUSER_CLOSECOMMNPATH
 *
 *  FILENAME	: MB_UserPort.h
 *
 *  PARAMETERS	: unsigned short NetworkId : Network path ID	
 *
 *  DESCRIPTION	: Close the communication path for the given port or network
 *
 *  RETURNS		: Unsigned char - 1 (success = no errors occurred during close of port)
 *               			    - 0 (failure = if errors occurred during close of port)
 *
 *******************************************************************************************/
#if MODBUS_TCP
	#define MBLINKUSER_CLOSECOMMNPATH(NetworkId) CloseTCP(NetworkId)
#else
	#define MBLINKUSER_CLOSECOMMNPATH(NetworkId) Close(NetworkId)
#endif


/*******************************************************************************************
 ** MBLINKUSER_READFROMCOMMNPATH
 *
 *  FILENAME: MB_UserPort.h
 *
 *  PARAMETERS:	  Unsigned short NetworkId
 *				  Unsigned char *ReadBuf
 *				  Unsigned long NoOfBytesToRead	
 *				  Unsigned long	*NoOfBytesRead
 *  DESCRIPTION: Get the data (given no of bytes) from the port 
 *				 into a buffer for the given network
 *
 *  RETURNS:	 Unsigned char - 1  (success = no errors occurred during 
 *                                 reading from port)
 *               			  - 0 (failure = if errors occurred during 
 *                                 reading from port)
 *
 *******************************************************************************************/
#if MODBUS_TCP
	#define MBLINKUSER_READFROMCOMMNPATH(NetworkId, ReadBuf, NoOfBytesToRead, NoOfBytesRead) ReadTCP(NetworkId, ReadBuf, NoOfBytesToRead, NoOfBytesRead )
#else
	#define MBLINKUSER_READFROMCOMMNPATH(NetworkId, ReadBuf, NoOfBytesToRead, NoOfBytesRead) Read(NetworkId, ReadBuf, NoOfBytesToRead, NoOfBytesRead )
#endif

/*******************************************************************************************
 ** MBLINKUSER_WRITETOCOMMNPATH
 *
 *  FILENAME	: MB_UserPort.h
 *
 *  PARAMETERS	: Unsigned short NetworkId
 *				  Unsigned char *RequestFrameBuf
 *				  Unsigned long NoOfBytesToWrite
 *				  Unsigned long *NoOfBytesWritten
 *
 *  DESCRIPTION	: Write the data (given no of bytes) to the given port 
 *
 *  RETURNS		: Unsigned char - 1 (success = no errors occurred during writing to port).
 *               				- 0 (failure = if errors occurred during writing to port).
 *
 ********************************************************************************************/
#if MODBUS_TCP
	#define MBLINKUSER_WRITETOCOMMNPATH(NetworkId, RequestFrameBuf, NoOfBytesToWrite, NoOfBytesWritten)  WriteTCP(NetworkId, RequestFrameBuf, NoOfBytesToWrite, NoOfBytesWritten)
#else
	#define MBLINKUSER_WRITETOCOMMNPATH(NetworkId, RequestFrameBuf, NoOfBytesToWrite, NoOfBytesWritten)  Write(NetworkId, RequestFrameBuf, NoOfBytesToWrite, NoOfBytesWritten)
#endif


/*******************************************************************************************
 ** MBLINKUSER_FLUSHCOMMNPATH
 *
 *  FILENAME	: MB_UserPort.h
 *
 *  PARAMETERS	: unsigned short NetworkId : Network path ID	
 *
 *  DESCRIPTION	: Clears the Tx and Rx buffers
 *
 *  RETURNS		: None 
 *                   
 *******************************************************************************************/
#define MBLINKUSER_FLUSHCOMMNPATH(NetworkId) Flush(NetworkId)

/********************************************************************************************
 ** MBTARG_GETSYSTIMEMILLISECS
 *
 *  FILENAME	: MB_UserPort.h
 *
 *  PARAMETERS	: None
 *
 *  DESCRIPTION	: Calculates time in milliseconds 
 *				 
 *  RETURNS		: Unsigned long -Returns the system time in milliseconds
 *					(Native function call on the target OS for getting system time 
 *							in milliseconds should be given.
 *
 ********************************************************************************************/
#define MBTARG_GETSYSTIMEMILLISECS() GetTimeInMilliseconds()

/********************************************************************************************
 ** DEBUGENABLED
 *
 *  FILENAME	: MB_UserPort.h
 *
 *  PARAMETERS	: None
 *
 *  DESCRIPTION	: Enables the User debugging printf's on the target system.
 *					To be configured for the target system.
 *				 
 *  RETURNS		: RETURNS: unsigned char - TRUE  (Enable User Debug)                         
 *          			                   FALSE (Disable User Debug)
 *
 ********************************************************************************************/
//#define DEBUGENABLED		 1
#define DEBUGENABLED		 0

/***********************************************************************************************
 ** LITTLE_ENDIAN
 *
 *  PARAMETERS:	 NONE
 *
 *  DESCRIPTION: If the hardware architecture chosen follows the LITTLE_ENDIAN 
 *				 style of storing WORDs, then this symbol must be defined. 
 *				 Depending on the operating system and the hardware platform, 
 *				 there are two ways of storing a WORD variable in memory. 
 *				 In the first case called the BIG ENDIAN style the high byte of 
 *				 the WORD is stored first and then the low byte of the word. 
 *				 This style can generally be found in MOTOROLA based CPU 
 *				 architectures. The other style called the LITTLE ENDIAN, the low 
 *				 byte of the WORD is stored first and then the high byte of the 
 *				 word. This style is more prominent in CPU's following the INTEL 
 *				 architecture. MODBUS follows the BIG ENDIAN style of packing WORDs 
 *				 in its frames. So on platforms supporting LITTLE ENDIAN style the 
 *				 two bytes forming the WORD must be reversed to the data to be 
 *				 copied properly. This is internally done in the stack if the 
 *				 LITTLE_ENDIAN symbol is defined.
 *				 
 *  RETURNS:	 unsigned char - TRUE=1  (LITTLE ENDIAN Type)                         
 *          		   			 FALSE=0 (BIG ENDIAN Type )
 *														                    
 *          				     		   
 ***********************************************************************************************/
//#define LITTLE_ENDIAN 1
#define LITTLE_ENDIAN 0

/*********************************************************************************
 ** xdata
 *
 *  PARAMETERS:	 None
 *
 *  DESCRIPTION: The Source Code Library uses several global and local variables. 
 *				 On some platforms all variables are created on the internal 
 *				 memory of the CPU by default. Since the internal memory of most 
 *				 CPUs is limited, there must be a way of explicitly instructing 
 *				 the compiler to create the variables in "external memory". 
 *				 In the source code library this is achieved by means of the 
 *				 'xdata' directive. Since the keyword to create data in external 
 *				 memory changes from platform to platform, the symbol 'xdata' 
 *				 must be redefined to the keyword supported by the platform 
 *				 being used.
 *				 
 *  RETURNS:	 None                         
 *
 *********************************************************************************/
 #define xdata	

/************************** User Defined Function Prototypes ********************************/

#if !MODBUS_TCP	/* If the network is Serial */
	MBDEFS_ULONG Open(MBDEFS_UINT8, MBDEFS_ULONG, MBDEFS_UINT8, MBDEFS_UINT8, MBDEFS_UINT8);	
	MBDEFS_BOOL	 Read(MBDEFS_ULONG, MBDEFS_UINT8*, MBDEFS_ULONG, MBDEFS_ULONG*);
	MBDEFS_BOOL	 Write(MBDEFS_ULONG, MBDEFS_UINT8*, MBDEFS_ULONG, MBDEFS_ULONG*);
	MBDEFS_BOOL	 Close(MBDEFS_ULONG);
#else	/* If the network is TCP/IP */
	MBDEFS_ULONG OpenTCP(char * IpAddr, unsigned short IPPortNo);
	MBDEFS_BOOL ReadTCP(MBDEFS_ULONG NetworkId, MBDEFS_UINT8 *ReadBuf, MBDEFS_ULONG NoOfBytesToRead, MBDEFS_ULONG *NoOfBytesRead );
	MBDEFS_BOOL WriteTCP(MBDEFS_ULONG NetworkId, MBDEFS_UINT8 *WriteBuf, MBDEFS_ULONG NoOfBytesToWrite, MBDEFS_ULONG *NoOfBytesWritten );
	MBDEFS_BOOL CloseTCP(MBDEFS_ULONG NetworkId);
#endif

	MBDEFS_ULONG GetTimeInMilliseconds();
MBDEFS_NONE	 Flush(MBDEFS_ULONG);
/*******************************************************************************************/


/*********************************************************************************
 ** CRC_TABLE_IN_RAM, CRC_TABLE_IN_ROM, CRC_TABLE_CREATE_DYNAMIC
 *
 *  FILENAME: Modbus_User.h
 *
 *  PARAMETERS:	  None
 *
 *  DESCRIPTION: These macros control where the 512 byte CRC table is created. 
 *					Setting values for these macros is important from the 
 *					perspective of optimisation. If CRC_TABLE_IN_RAM is set to 1,
 *					then 512 bytes of RAM are used up for the CRC table. This results 
 *					in fastest code execution but uses up the most memory. If 
 *					CRC_TABLE_IN_ROM is set to 1 then the CRC table is created as 
 *					constant arrays in ROM/Flash (please check your compiler if
 *					it creates "const" variables in ROM). This uses up more code ROM
 *					and results in lesser speed than the above option. But it saves 
 *					RAM. If CRC_TABLE_CREATE_DYNAMIC is set to '1', then the CRC 
 *					table values are created dynamically every time the CRC is 
 *					generated. This is the most expensive option in terms of speed 
 *					as the CRC table gets dynamically created for every frame but is 
 *					the least expensive in terms of RAM and ROM usage
 *
 *  RETURNS:	 None
 *
 **********************************************************************************/
#define CRC_TABLE_IN_RAM	0
#define CRC_TABLE_IN_ROM	0
#define CRC_TABLE_CREATE_DYNAMIC	1

#if CRC_TABLE_IN_RAM
	#if ( CRC_TABLE_IN_ROM || CRC_TABLE_CREATE_DYNAMIC )
		#error "Choose any one option for CRC table creation"
	#endif
#endif

#if CRC_TABLE_IN_ROM
	#if ( CRC_TABLE_IN_RAM || CRC_TABLE_CREATE_DYNAMIC )
		#error "Choose any one option for CRC table creation"
	#endif
#endif

#if CRC_TABLE_CREATE_DYNAMIC
	#if ( CRC_TABLE_IN_ROM || CRC_TABLE_IN_RAM )
		#error "Choose any one option for CRC table creation"
	#endif
#endif

#if( (!CRC_TABLE_IN_RAM) && (!CRC_TABLE_IN_ROM) && (!CRC_TABLE_CREATE_DYNAMIC) )
	#error "Atleast one option must be selected for CRC table creation"
#endif


/**********************************************************************************
 ** TX_BUFFER_SIZE, RX_BUFFER_SIZE
 *
 *  FILENAME: MB_UserPort.h
 *
 *  PARAMETERS:	  None
 *
 *  DESCRIPTION: These macros control the size of the transmit and receive buffers
 *				 internally created by the stack. The length of a MODBUS frame is
 *				 limited by the standard to 256 bytes. However some small footprint
 *				 systems may not have sufficient memory to accommodate such large 
 *				 buffers. If this master is required to query a limited number of 
 *				 variables (registers/inputs/coils) then the buffer allocated 
 *				 for holding the transmit and receive frames can be reduced. If the 
 *				 slave responds for any reason with a frame of size larger than 
 *				 RX_BUFFER_SIZE, then the master stack will reject such responses.
 *				 Similarly if a MBSendRequest call is made with nItems set to a 
 *				 value that causes the transmit frame size to exceed TX_BUFFER_SIZE
 *				 then the call returns with an error without doing any transmission.
 *
 *  RETURNS:	 None
 *
 **********************************************************************************/
#define TX_BUFFER_SIZE 256
#define RX_BUFFER_SIZE 256


/*******************************************************************************************************
 *  DESCRIPTION: These following macros control if the corresponding function are supported or not
 ** READ_COILS_SUPPORTED  				- FC 0x01
 ** WRITE_COILS_SUPPORTED				- FC 0x05 and FC 0x0F
 ** DISCRETE_INPUTS_SUPPORTED			- FC 0x02
 ** INPUT_REGISTERS_SUPPORTED			- FC 0x04
 ** READ_HOLDING_REGISTERS_SUPPORTED	- FC 0x03
 ** WRITE_HOLDING_REGISTERS_SUPPORTED	- FC 0x06 and FC 0x10
 ** READ_WRITE_REGISTERS_SUPPORTED		- FC 0x17
 ** MASK_WRITE_REGISTER_SUPPORTED		- FC 0x16
 ** READ_FILE_RECORD_SUPPORTED			- FC 0x14 
 ** WRITE_FILE_RECORD_SUPPORTED			- FC 0x15

	***** Supported in MODBUS Serial Only *****
 ** FETCH_COMM_EVENT_COUNTER_SUPPORTED	- FC 0x0B 
 ** FETCH_COMM_EVENT_LOG_SUPPORTED		- FC 0x0C  
 ** REPORT_SLAVE_ID_SUPPORTED			- FC 0x11 
 ** GET_EXCEPTION_COIL_DATA_SUPPORTED	- FC 0x07
 *
 *  RETURNS:	 unsigned char - TRUE=1 (respective function support enabled)                            
 *          		   			 FALSE=0(respective function support support disabled)
 *
 *  NOTE :  Unless this is enabled the requests from master with this function code will be responded 
 *				as illegal function code.
 *******************************************************************************************************/
#define READ_COILS_SUPPORTED				1
#define WRITE_COILS_SUPPORTED				1
#define DISCRETE_INPUTS_SUPPORTED			1
#define INPUT_REGISTERS_SUPPORTED			1
#define	READ_HOLDING_REGISTERS_SUPPORTED	1
#define	WRITE_HOLDING_REGISTERS_SUPPORTED	1

#define READ_WRITE_REGISTERS_SUPPORTED		1
#define MASK_WRITE_REGISTER_SUPPORTED		1
#define READ_FILE_RECORD_SUPPORTED			1
#define WRITE_FILE_RECORD_SUPPORTED			1

#if !MODBUS_TCP		/* supported in MODBUS Serial only */
	#define FETCH_COMM_EVENT_COUNTER_SUPPORTED	1
	#define FETCH_COMM_EVENT_LOG_SUPPORTED		1
	#define REPORT_SLAVE_ID_SUPPORTED			1
	#define GET_EXCEPTION_COIL_DATA_SUPPORTED	1
#endif

/*********************************************************************************
 ** DIAGNOSTICS_SUPPORTED  
 *
 *  PARAMETERS:	 None
 *
 *  DESCRIPTION: Indicates whether the modbus master supports diagnostic functions
 *				 
 *  RETURNS:	 unsigned char - TRUE=1 (Diagnostics responses enabled)                            
 *          		   			 FALSE=0(Diagnostics responses disabled)
 *
 *  NOTE :  Unless this is enabled the requests from master seeking diagnostic
 *			features of the slave will be responded as illegal function code.
 *********************************************************************************/
#define DIAGNOSTICS_SUPPORTED	 1

/*******************************************************************************************************
 ** ENCAPSULATED_INTERFACE_TRANSPORT_SUPPORTED  
 *
 *  PARAMETERS:	 None
 *
 *  DESCRIPTION: The MODBUS Encapsulated Interface (MEI)Transport is a mechanism for tunneling
 *					service requests and method invocations,  as well as their returns, inside
 *					MODBUS PDUs. This macro indicates whether MEI Transport mechanism is supported 
 *					by the slave or not.
 *				 
 *  RETURNS:	 unsigned char - TRUE=1 (Encapsulated Interface Transport Support enabled)                            
 *          		   			 FALSE=0(Encapsulated Interface Transport Support disabled)
 *
 *  NOTE :  Unless this is enabled the requests from master with this function code will be responded 
 *				as illegal function code.
 *******************************************************************************************************/
#define ENCAPSULATED_INTERFACE_TRANSPORT_SUPPORTED		1

#if ENCAPSULATED_INTERFACE_TRANSPORT_SUPPORTED
 /***************************************************************************************
 ** USER_DEFINED_MEI_TYPES_SUPPORTED
 *
 *  PARAMETERS:	 None
 *
 *  DESCRIPTION: Indicates whether the modbus slave supports User Defined MEI Types
 *				 
 *  RETURNS:	 unsigned char - TRUE=1 (User Defined MEI Type responses enabled).                            
 *          		   			 FALSE=0(User Defined MEI Type responses disabled).
 *
 *  NOTE :  Includes the User defined MEI Types in Modbus Encapsulated Interface 
 *			Transport (FC = 0x2B). ( Refer function EncapsulatedInterfaceTransport()
 *			and macros USER_DEFINED_SUBFCX, ENCAPSULATED_INTERFACE_TRANSPORT_SUPPORTED ).
 *			Unless this is enabled the requests from master with this function code 
 *			will be responded as illegal function code.
 ***************************************************************************************/
#define USER_DEFINED_MEI_TYPES_SUPPORTED				0
#endif /* ENCAPSULATED_INTERFACE_TRANSPORT_SUPPORTED */

/***************************************************************************************
 ** READ_FIFO_QUEUE_SUPPORTED
 *
 *  PARAMETERS:	 None
 *
 *  DESCRIPTION: Indicates whether the MODBUS slave supports Read FIFO QUEUE FC =24 (0x18).
 *				 
 *  RETURNS:	 unsigned char - TRUE=1 (Read FIFO QUEUE enabled).                            
 *          		   			 FALSE=0(Read FIFO QUEUE disabled).
 *
 *  NOTE :  The function code 24 (0x18) allows to read the contents of a First-In-First-Out
 *			(FIFO) queue of register in a remote device. The function returns a count of 
 *			the registers in the queue, followed by the queued data. Up to 32 registers can  
 *			be read: the count, plus up to 31 queued data registers. The queue count register 
 *			is returned first, followed by the queued data registers.The function reads the 
 *			queue contents, but does not clear them.
 ***************************************************************************************/
#define READ_FIFO_QUEUE_SUPPORTED	1

#endif /*(_MBLINKUSER_H)*/
/*** EndHeader */
