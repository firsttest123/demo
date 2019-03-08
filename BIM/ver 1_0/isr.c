
// digitdriver contains digit number(0-7)
// dispbuf contains value to be displayed corresponding each digit
// codetable contains 7-segment equivalent hex code decimal digits
// Timer0 is set in 16-bit mode with 2.5ms time delay

/*void timer0(void) interrupt 1 using 3
{
	unsigned char temp_ctrl1=0;
	//TH0 = 0xf7;	
	//TL0 = 0x00; //   reload the timer0 with 0xf700 for 1.875msec
	TH0 = 0xF8;	// reload the timer0 with 0xf400 for 2.5msec
	TL0 = 0xcc;//0xF4; 

	return;
}*/
void timer0(void) interrupt 1 
//void timer0(void) interrupt 1 using 3
{
	unsigned char temp_ctrl1=0;
	//TH0 = 0xf7;	
	//TL0 = 0x00; //   reload the timer0 with 0xf700 for 1.875msec
	TH0 = 0xF8;	// reload the timer0 with 0xf400 for 2.5msec
	TL0 = 0xcc;//0xF4;
	
	if(--digi_counter1<=0)
		digi_counter1=0;
	if(--digi_counter2<=0)
		digi_counter2=0;
	if(--digi_counter3<=0)
		digi_counter3=0;
	if(--digi_counter4<=0)
		digi_counter4=0;
	if(--digi_counter5<=0)
		digi_counter5=0;
	if(--digi_counter6<=0)
		digi_counter6=0;
	if(--digi_counter7<=0)
		digi_counter7=0;
	if(--i2c_wait_cnt<=0)
		i2c_wait_cntr = 0;
}

// 20msec timer
/*void timer2(void) interrupt 5 using 2
{
	TF2 = 0;
	
	return;
}*/


//Serial interrupt is from PC
//After receiving the protocol serialflag is set
// If serialflag is set then the corresponding data is sent to PC
/*void serialint(void) interrupt 4 using 1
{
	DI_buf[0]=1;
	return;
}*/


