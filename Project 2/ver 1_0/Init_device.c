//---------------------------------------------------------------
// CYGNAL Integrated Products 
//
// C Code Configuration Tool: F020 INITIALIZATION/CONFIGURATION CODE
//----------------------------------------------------------------
// This file is read only. To insert the code into your  
// application, simply cut and paste or use the "Save As" 
// command in the file menu to save the file in your project 
// directory. 
//----------------------------------------------------------------

//----------------------------------------------------------------
// INCLUDES
//----------------------------------------------------------------

#include <C8051F020.h>	// Register definition file.

//------------------------------------------------------------------------------------
// Global CONSTANTS
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// Function PROTOTYPES
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Config Routine
//------------------------------------------------------------------------------------
void config(void) 
{
	int n;
	WDTCN = 0x07;	// Watchdog Timer Control Register
		//EA = 0;
    WDTCN = 0xDE;   // Disable WDT
    WDTCN = 0xAD;
	//EA = 1;

	XBR0 = 0x06;		
	XBR1 = 0x00;
	XBR2 = 0x44;


	P0MDOUT = 0x75;  // P0.4, P0.2 & P0.0 configured as push-pull output 
  P1MDOUT = 0xFF;
	P2MDOUT = 0x00;
	P3MDOUT = 0x00;
	P74OUT  = 0x33;
	
//	EMI0CN    = 0x02;
//	EMI0TC=0x9a;
	OSCXCN = 0x67;
    for (n=0;n < 255; n++); 
    while ((OSCXCN & 0x80) == 0);	
	OSCICN = 0x88;	

	REF0CN = 0x00;	// Reference Control Register

	/*SPI0CN = 0x02;	// SPI Control Register
	SPI0CFG = 0xc7;	// SPI Configuration Register
	SPI0CKR = 0x07;	// SPI Clock Rate Register,86.4 KHz(11usec)*/
	
	SPI0CN = 0x00;	// SPI Control Register
	SPI0CFG = 0x07;	// SPI Configuration Register
	//SPI0CKR = 0x07;	// SPI Clock Rate Register
	SPI0CKR = 63;	// SPI Clock Rate Register
  SPI0CN |= 0x01;

	
		SCON0 = 0xd0;       // Serial Port Control Register
    SADEN0 = 0x00;      // Serial 0 Slave Address Enable
    SADDR0 = 0x00;      // Serial 0 Slave Address Register

    PCON = 0x00;        // Power Control Register

    SCON1 = 0xd0;       // Serial Port 1 Control Register   
    SADEN1 = 0x00;      // Serial 1 Slave Address Enable
	SADDR1 = 0x00;      // Serial 1 Slave Address Register

	CKCON = 0x00;	// Clock Control Register
	//TH0 = 0xec;		// Timer 0 High Byte
	//TL0 = 0x77;		// Timer 0 Low Byte
	
	TH0 = 0xFF;	// reload the timer0 with 0xf400 for 0.1 msec
	TL0 = 0x84;	//0xF4; 

/* ************************ */

	TH1 = 0xFC;		// Timer 1 High Byte
	TL1 = 0x00;		// Timer 1 Low Byte
	TMOD = 0x21;	// Timer Mode Register
	TCON = 0x50;	// Timer Control Register 

	RCAP2H = 0x00;	// Timer 2 Capture Register High Byte
	RCAP2L = 0x00;	// Timer 2 Capture Register Low Byte	
	TH2 = 0x00;		// Timer 2 High Byte	
	TL2 = 0x00;		// Timer 2 Low Byte	
    T2CON = 0x00;	// Timer 2 Control Register
		
	TMR3RLL = 0x00;	// Timer 3 Reload Register Low Byte
	TMR3RLH = 0x00;	// Timer 3 Reload Register High Byte
	TMR3H = 0x00;	// Timer 3 High Byte
	TMR3L = 0x00;	// Timer 3 Low Byte
    TMR3CN = 0x00;	// Timer 3 Control Register

	RSTSRC = 0x00;	// Reset Source Register

	IE = 0x80;          //Interrupt Enable
	IP = 0x00;          //Interrupt Priority
	EIE1 = 0x01;        //Extended Interrupt Enable 1
	EIE2 = 0x00;        //Extended Interrupt Enable 2
	//EIP1 = 0x01;        //Extended Interrupt Priority 1
	EIP1 = 0x00;        //Extended Interrupt Priority 1
	EIP2 = 0x00;        //Extended Interrupt Priority 2   //End of config

  // IE = 0x8a;          //Interrupt Enable
  //  IP = 0x00;          //Interrupt Priority
  //  EIE1 = 0x01;        //Extended Interrupt Enable 1
  //  EIE2 = 0x00;        //Extended Interrupt Enable 2
  //  EIP1 = 0x01;        //Extended Interrupt Priority 1
   // EIP2 = 0x00;        //Extended Interrupt Priority 2


//ET0=1;
//PT0=1;
ET0=1;
PT0=0;



	RCAP2H = 0x9f;	// 2.5msec Timer 2 Capture Register High Byte-f3f1
	RCAP2L = 0xfe;	// Timer 2 Capture Register Low Byte	
	TH2 = 0x9f;		// Timer 2 High Byte with 2.5msec 	
	TL2 = 0xfe;		// Timer 2 Low Byte	
    T2CON = 0x04;	// Timer 2 Control Register
    ET2 = 1;       // enable Timer2 interrupt  
   // ES0 =1;         // enable uarto serial interrupt
/*	CKCON	 |= 0x40;    // Timer4 uses sys clk as a clk source	
	RCAP4H    = 0xff;    //9600 baud   0xec
    RCAP4L    = 0xd0;    // value loaded for 9600 baud rate	
    TH4	      = 0xff;
    TL4	      = 0xd0;
    T4CON    |= 0x34;    //TR1 = 1; // start timer1
	SCON1	  = 0xd0;    // 9 bit mode,recive enable
 RCAP2H = 0xed;	// 2.5msec Timer 2 Capture Register High Byte-f3f1
	RCAP2L = 0xff;	// Timer 2 Capture Register Low Byte	
	TH2 = 0xed;		// Timer 2 High Byte	
	TL2 = 0xff;		// Timer 2 Low Byte	
    T2CON = 0x04;	// Timer 2 Control Register
    ET2 =1;    */
}


