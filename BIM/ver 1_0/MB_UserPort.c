#include <stdio.h>
#include "MBMaster.h"

#if !MODBUS_TCP
/* Function to initialize/open a network/port */
MBDEFS_ULONG Open(MBDEFS_UINT8 PortNo, MBDEFS_ULONG BaudRate, 
				 MBDEFS_UINT8 Parity, MBDEFS_UINT8 DataBits, MBDEFS_UINT8 StopBits)
{

		MBDEFS_ULONG NetworkId = 0;
	
		NetworkId = 1;
	 inituart_transmitmode();
	 setbaudrate();
	//transmitchar(0xff);
	
		/* TODO: Write Code to open the Serial port */

		return(NetworkId);
}
#endif

#if !MODBUS_TCP
/* Function to read bytes from the network/port */
MBDEFS_BOOL Read(MBDEFS_ULONG NetworkId, MBDEFS_UINT8 *ReadBuf, 
				 MBDEFS_ULONG NoOfBytesToRead, 
				 MBDEFS_ULONG *NoOfBytesRead )
{
	
	/* TODO: Write Code to read data bytes from Serial port */
	
	//do
	//{
		test_flag2 = 1;
		recvcounter = 27;  //100 msec  //4;	
		while(test_flag2)					//	 = 1;
		{
			if(serialflag1 == 1)
			{
				test_flag2 = 0;
				serialflag1 = 0;
				*ReadBuf = rbyte ;
				//*ReadBuf++;
				//*NoOfBytesRead = *NoOfBytesRead + 1;
				*NoOfBytesRead =  1;
			}
			else if(recvcounter <= 0)
			{
				test_flag2 = 0;
				return(MBDEFS_FALSE);
			}
		}
	//}
	//while(*NoOfBytesRead < NoOfBytesToRead);
    
    

	return(MBDEFS_TRUE);
}
#endif

#if !MODBUS_TCP
/* Function to write bytes to the network/port */
MBDEFS_BOOL Write(MBDEFS_ULONG NetworkId, MBDEFS_UINT8 *WriteBuf, 
				  MBDEFS_ULONG NoOfBytesToWrite, 
				  MBDEFS_ULONG *NoOfBytesWritten )
{
	/* TODO: Write Code to write data bytes to the Serial port */
	
	//test_flag3 = *WriteBuf;
	transmitchar((unsigned char) *WriteBuf);	
//	 func_test3((int)*WriteBuf);
//	if(first_time_flag == 1)
//	{
//		if(NoOfBytesToWrite == 8)
//		{
//			test_flag1++;
//		}
//		if(test_flag1 == 2)
//		{
//			*NoOfBytesWritten =  1;
//			first_time_flag =0;
//		}
//		else
//			*NoOfBytesWritten =  0;
//	}
//	else
//	{
		*NoOfBytesWritten =  1;
//	}
	return(MBDEFS_TRUE);
}
#endif

#if !MODBUS_TCP
/* Function to close a network/port */
MBDEFS_BOOL Close(MBDEFS_ULONG NetworkId)
{
	/* TODO: Write Code to close the Serial port */

	return(MBDEFS_TRUE);
}
#endif

/* Function to  flush a communication path */
MBDEFS_NONE Flush(MBDEFS_ULONG NetworkId)
{
	
	/* TODO: Write Code to flush the port/nework before sending a MODBUS request */
	
}

#if MODBUS_TCP
/* Function to initialize/open a TCP/IP port  */
unsigned long OpenTCP(char * IpAddr, unsigned short IPPortNo)
{
	/* TODO: Write Code to Open the TCP/IP port */

	return(MBDEFS_TRUE);
}
#endif

#if MODBUS_TCP
/* Function to read bytes from the network/port */
MBDEFS_BOOL ReadTCP(MBDEFS_ULONG NetworkId, MBDEFS_UINT8 *ReadBuf, 
					MBDEFS_ULONG NoOfBytesToRead, 
					MBDEFS_ULONG *NoOfBytesRead )
{
	/* TODO: Write Code to read data bytes from TCP/IP port */

	return(MBDEFS_TRUE);
}
#endif

#if MODBUS_TCP
/* Function to write bytes to the network/port */
MBDEFS_BOOL WriteTCP(MBDEFS_ULONG NetworkId, MBDEFS_UINT8 *WriteBuf, 
					 MBDEFS_ULONG NoOfBytesToWrite, 
					 MBDEFS_ULONG *NoOfBytesWritten )
{
	/* TODO: Write Code to write data bytes to TCP/IP port */

	return(MBDEFS_TRUE);
}
#endif

#if MODBUS_TCP
/* Function to close a network/port */
MBDEFS_BOOL CloseTCP(MBDEFS_ULONG NetworkId)
{
	/* TODO: Write Code to close the TCP/IP port */

	return(MBDEFS_TRUE);
}
#endif

/* Function to get system time in milliseconds */
MBDEFS_ULONG GetTimeInMilliseconds()
{
	MBDEFS_ULONG mbtime = 0;

	/* TODO: Write Code to get the system time */

	return (mbtime);
}