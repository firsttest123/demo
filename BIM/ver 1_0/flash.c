
void remoteprogram(void)
{
	calcramchksum(); // claculate RAM chksum
	if(flashchksum == ramchksum)
		return;
	else
		flashchksum = ramchksum; // load ramchksum into flashchksum
	eraseflash(flash_loc); // erase prg para sector
	eraseflash(flash_loc1); // erase prg para sector
	eraseflash(flash_loc2); // erase prg para sector
	eraseflash(flash_loc3); // erase prg para sector
	flashaddress = flash_loc;

	dataaddress = STARTADDR;
	bufferlength = MAX;
	writeflash(); // write programming parameters into flash memory
	ferrflag = 0; // clear ferrflag
	return;
}


void ramcheck(void)
{ 
	compliment_strobe; 
	ferrflag = 0;	 // clear ferrflag	
	calcramchksum(); // calculate ram checksum
	if(ramchksum != flashchksum) // compare ramchksum with flashchksum if both are not equal
	{
	//	rdpntr = 0xec00;  		// then read from flash
		rdpntr = flash_loc;  		// then read from flash
		if(*rdpntr == 0xaa)    
		{
			//flashaddress = 0xec00; // load flash address
			flashaddress = flash_loc; // load flash address
			dataaddress  = STARTADDR; // load data address
			bufferlength = MAX; // no. of bytes to be read
			readflash();	
			calcramchksum();         // calculate ramchksum
			if(ramchksum != flashchksum)  // compare ramchksum and flashchksum 
			{ 
			 	ferrflag = 1;	// both are not equal then set ferrflag
			}
		}
	   	else
	    {
	     	ferrflag = 1;
		}
    }
 	return;
}
// Function : calculate RAM checksum	
void calcramchksum(void)
{
	compliment_strobe; 
	ramchksum = 0;
	for(datapntr = STARTADDR; datapntr < MAX1; datapntr++)
		ramchksum += *datapntr;
	return;
}		 

// Function: erase flash memory sector

void eraseflash(unsigned short flashaddr)
{
	compliment_strobe; 
	wrpntr = (unsigned char xdata*) flashaddr;
	EA = 0;
	FLSCL |= 0x01;
	PSCTL = 0x03;
	*wrpntr = 0;
	PSCTL = 0;
	FLSCL &= ~0x01;
	EA = 1;
	return;
}


//Function: write to flash memory
//length variable type changed from char to short to accept 1000 op. no.'s
void writeflash(void)
{
	compliment_strobe; 
	EA = 0;
	wrpntr = (unsigned char xdata*) flashaddress;
	FLSCL |= 0x01;
	PSCTL = 0x01;
	*wrpntr = 0xaa;
	PSCTL = 0x00;
	FLSCL &= ~0x01;
	flashaddress += 10;
	wrpntr = (unsigned char xdata*) flashaddress;
	for(datapntr= (unsigned char xdata *)dataaddress; datapntr<(dataaddress+bufferlength); wrpntr++,datapntr++)
	{
		ibyte = *datapntr;
		FLSCL |= 0x01;
		PSCTL = 0x01;
		*wrpntr = ibyte;
		PSCTL = 0x00;
		FLSCL &= ~0x01;
	}
	PSCTL = 0x00;
	FLSCL &= ~0x01;
	EA = 1;
	return;
}


//Function: read data from flash memory
void readflash(void)
{
	compliment_strobe; 
	rdpntr = (unsigned char code*)flashaddress;
	tempbyte = *rdpntr;
	flashaddress += 10;
	rdpntr = (unsigned char code*)flashaddress;
	for(datapntr = (unsigned char xdata*)dataaddress; datapntr<(dataaddress+bufferlength); rdpntr++,datapntr++)
    *datapntr = *rdpntr;
	compliment_strobe; 
	return;
}
