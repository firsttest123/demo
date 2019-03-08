


void leddrive(unsigned char drive)
{
	LEDEN = 1;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	LCD_DATA = drive;		// LED2.add...
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	LEDEN = 0;	
	LCD_DATA = 0xFF;
	return;
}

void send_data(unsigned char x)
{
	lcdbus = x;	
	LCDRS = 0;
	LCDRW = 0;
	LCDEN = 1;
	_nop_();
	_nop_();
	LCDEN= 0;
	for(j=0;j <= 50;j++);
	return;
}

void writedata(unsigned char  buf1)
{
	LCDRS = 1;
	LCDRW = 0;
	LCDEN = 1;
	lcdbus = buf1;
	LCDEN= 0;
	for(j=0;j<=60;j++);
	return;
}


long int editing_value(long int temp_value,char sign_flag,char decimal_point,char no_digits,char end_loc,unsigned char row_addr)
{
	char minus_flag=0,st_loc;
	unsigned char v,cursor_flag=0,var=0;
	unsigned long int val;
//	if(ch_edit_flag == 0)
	{
		for(i=0;i<16;i++)
			buf[i] = ' ';
	}
	exitflag = 0;
	okflag = 0;
	c = temp_value;
	if((decimal_point == 0) && (sign_flag == 0))
		st_loc = end_loc - no_digits + 1;
	else
		st_loc = end_loc - 5;
	
	t = end_loc;
	while(t >= st_loc)
	{
		buf[t--] = 0x30;
	}
	if(decimal_point != 0)
	{
		buf[end_loc - decimal_point] = '.';		
	}
	t = end_loc;
	while(c != 0)
	{
		compliment_strobe;
		v = (c % 0x0a) + 0x30;
		buf[t] = v;
		c = c/0x0a;
		t = t-1;
		if((decimal_point != 0)&&(t == (end_loc - decimal_point)))
		{
			buf[t] = '.';
			t=t-1;
		}
	}
	if(minus_flag == 1)
		buf[st_loc] = '-';
	//t = end_loc+1;		//old 
	t = end_loc;			
	//lcd_display(buf,0xc0);

		lcd_display(buf,row_addr);

	t = end_loc;			
	var = buf[t] - 0x30;
	cntr = 0;
	if(mode_number != 3 ||first_time_flag == 0)
	{
		cursorblink(row_addr + t);	//
	}
	do
	{
		//poll_data();
		//poll_config_ethernet_alarms();
		if(Menu_timeout_counter<=0)
			 Menu_timeout_flag=1;
		if(Menu_timeout_flag==1)
			return;
		compliment_strobe;
		if(cntr != 0)
		{
			switch(cntr)
			{
				case PRG:prgmode();
				prgmode_flag = 1;
						break; 
				
				case ESC: exitflag = 1;
						break; 

				case DOWN: 
						if(mode_number != 3 ||first_time_flag == 0)
							{
								if((t == st_loc)&&(sign_flag == 1))
								{
									if(++var > 11)
										var = 10;
									if(var == 10)
										lcd_write_data(buf[t] = '-');
									else
										lcd_write_data(buf[t] = 0x30);
								}
								else
								{
									if(var-- <= 0)
										var = 9;
								if((decimal_point != 0) && (t == (end_loc - decimal_point)))
									//lcd_write_instr(0xc0 + t+1);
									lcd_write_data(buf[t+1] = (var + 0x30));
								else
									//lcd_write_instr(0xc0 + t);
									lcd_write_data(buf[t] = (var + 0x30));
								}
								if((decimal_point != 0) && (t == (end_loc - decimal_point)))
									//lcd_write_instr(0xc0 + t+1);
										lcd_write_instr(row_addr + t+1);
								else
									//lcd_write_instr(0xc0 + t);
									lcd_write_instr(row_addr + t);
							//	lcd_write_instr(0xc1 + t);
							}
						
						break;
				case LEFT_SHIFT: 
						if(mode_number != 3 ||first_time_flag == 0)
						{
							if(--t < st_loc)
								t = end_loc;
							if((decimal_point != 0) && (t == (end_loc - decimal_point)))
								t++;
							//cursorblink(0xc0 + t);
							cursorblink(row_addr + t);	
							//lcd_write_instr(0xc0 + t);
								lcd_write_instr(row_addr + t);

							if((decimal_point != 0) && (t == (end_loc - decimal_point+1)))
								t--;
							if((t == st_loc) && (sign_flag == 1))
							{
								if(buf[t] == '.')
									var = 10;
								else
									var = 11;
							}
							else
								var = buf[t] - 0x30;
						}
						break;
					case RIGHT_SHIFT: 
						
						if(mode_number != 3 ||first_time_flag == 0)
						{
							
							if(++t > end_loc)
								t = st_loc;
							if((decimal_point != 0) && (t == (end_loc - decimal_point)))
								t++;
							//cursorblink(0xc0 + t);
							cursorblink(row_addr + t);	
							//lcd_write_instr(0xc0 + t);
								lcd_write_instr(row_addr + t);

							if((decimal_point != 0) && (t == (end_loc - decimal_point+1)))
								t--;
							if((t == st_loc) && (sign_flag == 1))
							{
								if(buf[t] == '.')
									var = 10;
								else
									var = 11;
							}
							else
								var = buf[t] - 0x30;
							//cursor_flag = 1;					
						}
						break;



				case UP:	
						
							if(mode_number != 3 ||first_time_flag == 0)
							{
								if((t == st_loc)&&(sign_flag == 1))
								{
									if(++var > 11)
										var = 10;
									if(var == 10)
										lcd_write_data(buf[t] = '-');
									else
										lcd_write_data(buf[t] = 0x30);
								}
								else
								{
									if(++var > 9)
										var = 0;
								if((decimal_point != 0) && (t == (end_loc - decimal_point)))
									//lcd_write_instr(0xc0 + t+1);
									lcd_write_data(buf[t+1] = (var + 0x30));
								else
									//lcd_write_instr(0xc0 + t);
									lcd_write_data(buf[t] = (var + 0x30));
								}
								if((decimal_point != 0) && (t == (end_loc - decimal_point)))
									//lcd_write_instr(0xc0 + t+1);
										lcd_write_instr(row_addr + t+1);
								else
									//lcd_write_instr(0xc0 + t);
									lcd_write_instr(row_addr + t);
							//	lcd_write_instr(0xc1 + t);
							}
						
						break;
			case OK:		temp_value = 0;
						val = 1;
						okflag = 1;
					if(mode_number != 3 ||first_time_flag == 0)
					{
						for(t = end_loc;t >= st_loc;t--)
						{
							if(buf[t] == '-')
							{
								temp_value = 0 - temp_value;
								break;
							}
							else if(buf[t] != '.')
							{
								temp_value += (buf[t] - 0x30) * val;	
								val = val*10;
							}
						}
					}
						break;
				default: 	
						break;
			}
			cntr=0;
		}		
	//}while((okflag != 1) && (exitflag != 1) && (keytimeout != 1));
		}while((okflag != 1) && (exitflag != 1));
	lcd_write_instr(0x0c);
	return temp_value;
}

int editing_value1(unsigned char *temp_value,char no_digits,char end_loc,unsigned char row_addr){
	char minus_flag=0,st_loc;
	unsigned char v,cursor_flag=0,var=0;
	unsigned int val,dummy_var;
//	if(ch_edit_flag == 0)
	{
		for(i=0;i<16;i++)
			buf[i] = ' ';
	}
	exitflag = prgmode_flag = 0;
	okflag = 0;

	st_loc = end_loc - no_digits + 1;
	t = end_loc;
	while(t >= st_loc)
	{
		buf[t--] = 0x30;
	}
		t = 15;
	c = temp_value[3];
	while(c != 0)
	{
		compliment_strobe;
		v = (c % 0x0a) + 0x30;
		buf[t] = v;
		c = c/0x0a;
		t = t-1;
	}
	t = 11;
	c = temp_value[2];
	while(c != 0)
	{
		compliment_strobe;
		v = (c % 0x0a) + 0x30;
		buf[t] = v;
		c = c/0x0a;
		t = t-1;
		
	}
	t = 7;
	c = temp_value[1];
	while(c != 0)
	{
		compliment_strobe;
		v = (c % 0x0a) + 0x30;
		buf[t] = v;
		c = c/0x0a;
		t = t-1;
		
	}
	
	t = 3;
	c = temp_value[0];
	while(c != 0)
	{
		compliment_strobe;
		v = (c % 0x0a) + 0x30;
		buf[t] = v;
		c = c/0x0a;
		t = t-1;
		
	}
buf[12] = buf[8] = buf[4] = '.';

	
	
	if(minus_flag == 1)
		buf[st_loc] = '-';
	t = end_loc;			

	lcd_display(buf,row_addr);

	t = end_loc;			
	var = buf[t] - 0x30;
	cntr = 0;
	cursorblink(row_addr + t);	//
	do
	{
		//poll_data();
		//poll_config_ethernet_alarms();
		if(Menu_timeout_counter<=0)
		   Menu_timeout_flag=1;
	  if(Menu_timeout_flag==1)
		   return;
		compliment_strobe;
		if(cntr != 0)
		{
			switch(cntr)
			{
				case PRG:prgmode();
							prgmode_flag = 1;
						break; 
				
				case ESC: exitflag = 1;
						break; 

				case DOWN: 
							
									if(var-- <= 0)
										var = 9;
									lcd_write_data(buf[t] = (var + 0x30));
									lcd_write_instr(row_addr + t);
							
						
						break;
				case LEFT_SHIFT: 
						
							--t;
							if(t < st_loc)
							{
									t = end_loc;

							}
							if(t== 12||t == 8|| t == 4)
							{
								--t;
							}
							cursorblink(row_addr + t);	
							lcd_write_instr(row_addr + t);
							var = buf[t] - 0x30;
						
						break;
				case RIGHT_SHIFT: 
						
							++t;
							if(t > end_loc)
							{
									t = st_loc;

							}
							if(t== 12||t == 8|| t == 4)
							{
								++t;
							}
								cursorblink(row_addr + t);	
								lcd_write_instr(row_addr + t);
								var = buf[t] - 0x30;
						
						break;


				case UP:	
						

									if(++var > 9)
										var = 0;
									lcd_write_data(buf[t] = (var + 0x30));
									lcd_write_instr(row_addr + t);
							
						break;
			case OK:	 dummy_var= 0;
						val = 1;
						okflag = 1;
							
						for(t=3;t>0;t--)
						{
							dummy_var += (buf[t] - 0x30) * val;	
								val = val*10;
						}
						if(dummy_var>=255)
						{
							dummy_var = 255;
						}
						temp_value[0] = dummy_var;
						
						
						dummy_var = 0;
						val = 1;
						for(t=7;t>4;t--)
						{
							dummy_var += (buf[t] - 0x30) * val;	
								val = val*10;
						}
						if(dummy_var>=255)
						{
							dummy_var = 255;
						}
						temp_value[1] = dummy_var;
						
						
						dummy_var = 0;
						val = 1;
						for(t=11;t>8;t--)
						{
							dummy_var += (buf[t] - 0x30) * val;	
								val = val*10;
						}	
						if(dummy_var>=255)
						{
							dummy_var = 255;
						}
						temp_value[2] = dummy_var;
						
						
						dummy_var = 0;
						val = 1;
						for(t=15;t>12;t--)
						{
							dummy_var += (buf[t] - 0x30) * val;	
								val = val*10;
						}
						if(dummy_var>=255)
						{
							dummy_var = 255;
						}
						temp_value[3] = dummy_var;
						
						dummy_var = 0;


						break;
				default: 	
						break;
			}
			cntr=0;
		}		
	//}while((okflag != 1) && (exitflag != 1) && (keytimeout != 1));
		}while((okflag != 1) && (exitflag != 1));
	lcd_write_instr(0x0c);
	return temp_value;
}

long  int editing_value2(unsigned char *temp_value,char no_digits,char end_loc,char end_loc2,unsigned char row_addr){
	char minus_flag=0,st_loc;
	unsigned char buf1[16] ={0};
	unsigned char v,cursor_flag=0,var=0, shift_flag;
	unsigned int val,dummy_var,next_screen_flag =0;
//	if(ch_edit_flag == 0)
	{
		for(i=0;i<16;i++)
			buf[i] = ' ';
		for(i=0;i<16;i++)
			buf1[i] = ' ';
	}
	exitflag = prgmode_flag = 0;
	okflag = 0;

	st_loc = end_loc - no_digits + 1;
	//t = end_loc;
	//while(t >= st_loc)
	//{
		//buf[t--] = 0x30;
	//}
	//t = end_loc2;
	//while(t >= 0)
	//{
		//buf[t--] = 0x30;
	//}
	
	for(t=1;t<=end_loc;t++)
		buf[t] = 0x30;
	for(t=0;t<=end_loc2;t++)
	 buf1[t] = 0x30;
	t =7;
	c = temp_value[5];
	
	while(c != 0)
	{
		compliment_strobe;
		v = (c % 0x0a) + 0x30;
		buf1[t] = v;
		c = c/0x0a;
		t = t-1;
	}
	t =3;
	c = temp_value[4];
	while(c != 0)
	{
		compliment_strobe;
		v = (c % 0x0a) + 0x30;
		buf1[t] = v;
		c = c/0x0a;
		t = t-1;
	}
	
	t = 15;
	c = temp_value[3];
	while(c != 0)
	{
		compliment_strobe;
		v = (c % 0x0a) + 0x30;
		buf[t] = v;
		c = c/0x0a;
		t = t-1;
	}
	t = 11;
	c = temp_value[2];
	while(c != 0)
	{
		compliment_strobe;
		v = (c % 0x0a) + 0x30;
		buf[t] = v;
		c = c/0x0a;
		t = t-1;
		
	}
	t = 7;
	c = temp_value[1];
	while(c != 0)
	{
		compliment_strobe;
		v = (c % 0x0a) + 0x30;
		buf[t] = v;
		c = c/0x0a;
		t = t-1;
		
	}
	
	t = 3;
	c = temp_value[0];
	while(c != 0)
	{
		compliment_strobe;
		v = (c % 0x0a) + 0x30;
		buf[t] = v;
		c = c/0x0a;
		t = t-1;
		
	}


	
	
	if(minus_flag == 1)
		buf[st_loc] = '-';
	
	shift_flag =2;
	next_screen:
	if(next_screen_flag == 0)
	{
		buf[12] = buf[8] = buf[4] = '.';
		cntr = 0;
			if(shift_flag == 2)
				t = st_loc;
			else 
				t = end_loc;
		lcd_display(buf,row_addr);
		//t = st_loc;				
		cursorblink(row_addr + t);	//
		var = buf[t] - 0x30;
	}
	else if(next_screen_flag == 1)
	{
		buf1[0]= buf1[4] = '.';
		cntr = 0;
		if(shift_flag == 2)		
			t = 1;
		else
			t = end_loc2;
		lcd_display(buf1,row_addr);
		//t = 1;				
		cursorblink(row_addr + t);	//
		var = buf1[t] - 0x30;		
	}
	do
	{
		//poll_data();
		//poll_config_ethernet_alarms();
		compliment_strobe;
		if(cntr != 0)
		{
			switch(cntr)
			{
				case PRG:prgmode();
							prgmode_flag = 1;
						break; 
				
				case ESC: exitflag = 1;
						break; 

				case DOWN: 
							
									if(var-- <= 0)
										var = 9;
									if(next_screen_flag == 0)
										lcd_write_data(buf[t] = (var + 0x30));
									else if(next_screen_flag == 1)
										lcd_write_data(buf1[t] = (var + 0x30));
									lcd_write_instr(row_addr + t);
							
						
						break;
				case LEFT_SHIFT: 
						
							--t;
							if(next_screen_flag == 0)
							{
								if(t < st_loc)
								{
									t = end_loc2;
									shift_flag =1;
									//var = buf[t] - 0x30;
									next_screen_flag = 1;
									goto next_screen;
								}
								if(t== 12||t == 8|| t == 4)
								{
									--t;
								}
								cursorblink(row_addr + t);	
								lcd_write_instr(row_addr + t);
								var = buf[t] - 0x30;
							}
							else if(next_screen_flag == 1)
							{
								if(t < 1)
								{
									t = end_loc;
									shift_flag = 1;
									next_screen_flag = 0;
									goto next_screen;
									
								}
								if(t == 0 || t == 4)
								{
									--t;
								}
								cursorblink(row_addr + t);	
								lcd_write_instr(row_addr + t);
								var = buf1[t] - 0x30;
							}
						
						break;
				case RIGHT_SHIFT: 
						
							++t;
							if(next_screen_flag == 0)
							{
								if(t > end_loc)
								{
									//t = st_loc;
									t = 1;
									shift_flag =2;
									next_screen_flag = 1;
									goto next_screen;

								}
								if(t== 12||t == 8|| t == 4)
								{
									++t;
								}
								cursorblink(row_addr + t);	
								lcd_write_instr(row_addr + t);
								var = buf[t] - 0x30;
							}
							else if(next_screen_flag == 1)
							{
								if(t> end_loc2)
								{
									t = st_loc;
									shift_flag = 2;
									next_screen_flag = 0;
									goto next_screen;

								}
								if(t == 0 || t == 4)
									++t;
								cursorblink(row_addr + t);	
								lcd_write_instr(row_addr + t);
								var = buf1[t] - 0x30;
							}
						break;


				case UP:	
						

									if(++var > 9)
										var = 0;
									if(next_screen_flag == 0)
										lcd_write_data(buf[t] = (var + 0x30));
									else if(next_screen_flag == 1)
										lcd_write_data(buf1[t] = (var + 0x30));

									lcd_write_instr(row_addr + t);
							
						break;
			case OK:	 dummy_var= 0;
						val = 1;
						okflag = 1;
							
						for(t=3;t>0;t--)
						{
							dummy_var += (buf[t] - 0x30) * val;	
								val = val*10;
						}
						if(dummy_var>=255)
						{
							dummy_var = 255;
						}
						temp_value[0] = dummy_var;
						
						
						dummy_var = 0;
						val = 1;
						for(t=7;t>4;t--)
						{
							dummy_var += (buf[t] - 0x30) * val;	
								val = val*10;
						}
						if(dummy_var>=255)
						{
							dummy_var = 255;
						}
						temp_value[1] = dummy_var;
						
						
						dummy_var = 0;
						val = 1;
						for(t=11;t>8;t--)
						{
							dummy_var += (buf[t] - 0x30) * val;	
								val = val*10;
						}	
						if(dummy_var>=255)
						{
							dummy_var = 255;
						}
						temp_value[2] = dummy_var;
						
						
						dummy_var = 0;
						val = 1;
						for(t=15;t>12;t--)
						{
							dummy_var += (buf[t] - 0x30) * val;	
								val = val*10;
						}
						if(dummy_var>=255)
						{
							dummy_var = 255;
						}
						temp_value[3] = dummy_var;
						
						dummy_var = 0;

						dummy_var = 0;
						val = 1;
						for(t=3;t>0;t--)
						{
							dummy_var += (buf1[t] - 0x30) * val;	
								val = val*10;
						}
						if(dummy_var>=255)
						{
							dummy_var = 255;
						}
						temp_value[4] = dummy_var;

						dummy_var = 0;
						val = 1;
						for(t=7;t>4;t--)
						{
							dummy_var += (buf1[t] - 0x30) * val;	
								val = val*10;
						}
						if(dummy_var>=255)
						{
							dummy_var = 255;
						}
						temp_value[5] = dummy_var;
						
						dummy_var = 0;

						break;
				default: 	
						break;
			}
			cntr=0;
		}		
	//}while((okflag != 1) && (exitflag != 1) && (keytimeout != 1));
		}while((okflag != 1) && (exitflag != 1));
	lcd_write_instr(0x0c);
	return temp_value;
}


char st_loc=7,t_t = 11;
unsigned char v,cursor_flag=0,var=0,password_chk_flag1=0;
unsigned int val,tt;

/* Subroutine for writing instruction to  LCD display */
/************************************************************************************
 * Function		: lcd_write_instr
 * Author		: Navya
 * Description	:  i) by sing this function we can write a instruction on lcd
 *				  			 ii) As per timing diagram set the configuration
 ************************************************************************************/

void lcd_write_instr(unsigned char x) //send data
{  static int j;
	
	lcdbus = x;	
	LCDRS = 0;
	LCDRW = 0;
	LCDEN = 1;
	_nop_();
	_nop_();
	LCDEN= 0;
		for(j=0;j <= 50;j++);

			//callserialcomm();
	return;
}   
/*End of subroutine for writing instruction to  LCD */

/************************************************************************************
 * Function		: lcd_write_data
 * Author		: Navya
 * Description	:  i) this subroutine is used to writing data on LCD
 *								 ii) As per timing diagram set the configuration
 ************************************************************************************/

/* Subroutine for writing data to  LCD display */
void lcd_write_data(unsigned char  buf1)
{   
		LCDRS = 1;
	LCDRW = 0;
	LCDEN = 1;
	lcdbus = buf1;
	LCDEN= 0;
		for(j=0;j<=60;j++);
	return;

	
/*	static int j;
	compliment_strobe;
	LCDRS_disable;
	LCDRW_enable;
	LCD_disable;
	LCD_DATA = buf1;
  LCD_enable;
	for(j=0;j <= 60;j++);
		//callserialcomm();
	return;*/
}     
/*end of subroutine*/

/*subroutine for lcd line display with arguments as the ptr to string and ptr to line address of lcd*/ 
void lcd_display(unsigned char *buf,unsigned char addr) 
{
//compliment_strobe;
	send_data(addr);          //addr corresponds to particular line of lcd

	for(i=0;(buf[i]!='\0')&&(i<16);i++)  {
	writedata(buf[i]);

	}
		
    return;
}    
/*end of subroutine*/

void lcd_init(void) 
{
	compliment_strobe;
/*	LEDEN=0;	// disable LEDS lines
	LCDRS_enable;
	LCDRW_enable;
	LCD_enable;
	
    for(j=0;j<40000;j++);                //poweron delay for 15ms
	lcd_write_instr(0x38);              //function set 0x38
	for(j=0;j<16000;j++);                //delay for 5ms
	lcd_write_instr(0x38);              //function set 0x38
	for(j=0;j<800;j++);                  //delay for 100us
	lcd_write_instr(0x38);              //function set 0x38
	lcd_write_instr(0x38);              //display control for 8-bit interface,2 lines enable
	lcd_write_instr(0x08);              //display off 0x08
	lcd_write_instr(0x01);              //clear display and place cursor at home position 0x01
	for(j=0;j<800;j++);
	lcd_write_instr(0x06);              //entry modeset with cursor move direction as increment 0x06
	for(j=0;j<800;j++);
	lcd_write_instr(0x0c);              //displayon cursoroff and blinkoff 0x0d
	*/
	
	LEDEN=0;	// disable LEDS lines
	
	LCDRS=0;
	LCDRW=0;
	LCDEN=0;
	for(j=0;j<=20000;j++);
		send_data(0x38);
	for(j=0;j<=20000;j++);
		send_data(0x38);
	for(j=0;j<=20000;j++);
		send_data(0x38);
	for(j=0;j<=2000;j++);
		send_data(0x38);
	for(j=0;j<=2000;j++);
		send_data(0x08);
	for(j=0;j<=2000;j++);
		send_data(0x01);
	for(j=0;j<=2000;j++);
		send_data(0x06);
	for(j=0;j<=2000;j++);
		send_data(0x0c);
	for(j=0;j<=2000;j++);

	return;
}       
//---------------------------------------
/*subruotine for cursor blink at a particular location*/
void cursorblink(unsigned char addr)
{
	lcd_write_instr(0x0d);
	lcd_write_instr(addr);
	return;
}
/* Subroutine for writing instruction to  LCD display */
/************************************************************************************
 * Function		: lcdbcd
 * Author		: Navya
 * Description	:  i) by using this function will get a bcd variable 
 ************************************************************************************/
void lcdbcd(char end_loc,char decimal_point)
{
	char v,minus_flag=0;
	for(i=0;i<16;i++)
		buf[i] = ' ';
	
	t=end_loc;
//	while(t >= (7-decimal_point))
	while(t >= (8-decimal_point))
	{
		buf[t--] = 0x30;
	}
	if(decimal_point != 0)
	{
		buf[end_loc - decimal_point] = '.';		
	}
	t=end_loc;
	while(c != 0)
	{
		compliment_strobe;
		v = (c % 0x0a) + 0x30;
		buf[t] = v;
		c = c/0x0a;
		t = t-1;
		if((decimal_point != 0)&&(t == (end_loc - decimal_point)))
		{
			buf[t] = '.';
			t=t-1;
		}
	}
	
	lcd_display(buf,0xc0);

}


//val for polarity editing at 1st location
//dbuf for string reading
//deci_point for decimal point editing
/* Subroutine for writing instruction to  LCD display */
/************************************************************************************
 * Function		: editing_value
 * Author		: Navya
 * Description	:  i) by using this function we can edit the  value and also update display
 
 ************************************************************************************/
/*int editing_value(int temp_value,char sign_flag,char decimal_point,char no_digits,char end_loc,unsigned char row_addr)
{
	char minus_flag=0,st_loc;
	unsigned char v,cursor_flag=0,var=0;
	unsigned int val;
//	if(ch_edit_flag == 0)
	{
		for(i=0;i<16;i++)
			buf[i] = ' ';
	}
	exitflag = 0;
	okflag = 0;
	c = temp_value;
	if((decimal_point == 0) && (sign_flag == 0))
		st_loc = end_loc - no_digits + 1;
	else
		st_loc = end_loc - 5;
	
	t = end_loc;
	while(t >= st_loc)
	{
		buf[t--] = 0x30;
	}
	if(decimal_point != 0)
	{
		buf[end_loc - decimal_point] = '.';		
	}
	t = end_loc;
	while(c != 0)
	{
		compliment_strobe;
		v = (c % 0x0a) + 0x30;
		buf[t] = v;
		c = c/0x0a;
		t = t-1;
		if((decimal_point != 0)&&(t == (end_loc - decimal_point)))
		{
			buf[t] = '.';
			t=t-1;
		}
	}
	if(minus_flag == 1)
		buf[st_loc] = '-';
	//t = end_loc+1;		//old 
	t = end_loc;			
	//lcd_display(buf,0xc0);

		lcd_display(buf,row_addr);

	
	t = end_loc;			

	//cursorblink(0xc0 + t);	
	cursorblink(row_addr + t);	//
	do
	{
		compliment_strobe;
		if(cntr != 0)
		{
			switch(cntr)
			{
				case PRG: exitflag = 1;
						break; 

				case LEFT_SHIFT: 
						if(no_edit_flag == 0)
						{
							if(--t < st_loc)
								t = end_loc;
							if((decimal_point != 0) && (t == (end_loc - decimal_point)))
								t++;
							//cursorblink(0xc0 + t);
							cursorblink(row_addr + t);	
							//lcd_write_instr(0xc0 + t);
								lcd_write_instr(row_addr + t);

							if((decimal_point != 0) && (t == (end_loc - decimal_point+1)))
								t--;
							if((t == st_loc) && (sign_flag == 1))
							{
								if(buf[t] == '.')
									var = 10;
								else
									var = 11;
							}
							else
								var = buf[t] - 0x30;
							//cursor_flag = 1;				
						}
						break;

				case UP:	
						
							//if(cursor_flag == 1)				
							{
								if((t == st_loc)&&(sign_flag == 1))
								{
									if(++var > 11)
										var = 10;
									if(var == 10)
										lcd_write_data(buf[t] = '-');
									else
										lcd_write_data(buf[t] = 0x30);
								}
								else
								{
									if(++var > 9)
										var = 0;
								if((decimal_point != 0) && (t == (end_loc - decimal_point)))
									//lcd_write_instr(0xc0 + t+1);
									lcd_write_data(buf[t+1] = (var + 0x30));
								else
									//lcd_write_instr(0xc0 + t);
									lcd_write_data(buf[t] = (var + 0x30));
								}
								if((decimal_point != 0) && (t == (end_loc - decimal_point)))
									//lcd_write_instr(0xc0 + t+1);
										lcd_write_instr(row_addr + t+1);
								else
									//lcd_write_instr(0xc0 + t);
									lcd_write_instr(row_addr + t);
							//	lcd_write_instr(0xc1 + t);
							}
						
						break;
			case OK:		temp_value = 0;
						val = 1;
						okflag = 1;
						for(t = end_loc;t >= st_loc;t--)
						{
							if(buf[t] == '-')
							{
								temp_value = 0 - temp_value;
								break;
							}
							else if(buf[t] != '.')
							{
								temp_value += (buf[t] - 0x30) * val;	
								val = val*10;
							}
						}
						break;
				default: 	
						break;
			}
			cntr=0;
		}		
	//}while((okflag != 1) && (exitflag != 1) && (keytimeout != 1));
		}while((okflag != 1) && (exitflag != 1));
	lcd_write_instr(0x0c);
	return temp_value;
}



char st_loc=7,t_t = 11;
unsigned char v,cursor_flag=0,var=0,password_chk_flag1=0;
unsigned int val,tt;*/



//val for polarity editing at 1st location
//dbuf for string reading
//deci_point for decimal point editing

char LCD_editing_selection( char sel_flag,char totlabytes,unsigned char row_addr,unsigned char *str1/*,unsigned char *str2,unsigned char *str3,unsigned char *str4*/)
{
	char mode=sel_flag;
	exitflag = prgmode_flag = 0;
	okflag = 0;
	cntr  =UP;
	--sel_flag;
	do
	{		
    if(Menu_timeout_counter<=0)
			 Menu_timeout_flag=1;
		if(Menu_timeout_flag==1)
			return;
		//poll_data();
		//poll_config_ethernet_alarms();
		compliment_strobe;
		if(cntr != 0)
		{
			switch(cntr)
			{
				case ESC:	 	exitflag = 1;
										sel_flag=mode;
						break; 
				
				case PRG:prgmode();
							prgmode_flag = 1;
						break; 
				

				case UP:	if(++sel_flag >= totlabytes)
								sel_flag = 0;

							/*	if(first_time_flag == 1)	
								{
									if(sel_flag==3)
										sel_flag = 4;
								}*/
	
								lcd_display(str1+(sel_flag*16),row_addr);
							 if(prg_sel_flag == 1)
							 {
							 	lcd_write_instr(0x8c);
								lcd_write_data(sel_flag+1+0x30);
							}
							
								
							break;
					case DOWN:	
						
					
				//	if(++sel_flag >= totlabytes)
						//		sel_flag = 0;

									sel_flag--;
					if(sel_flag<0)
						sel_flag = totlabytes-1;

							/*	if(first_time_flag == 1)	
								{
									if(sel_flag==3)//if it is serial num screen
										sel_flag = 2;
								}*/


								lcd_display(str1+(sel_flag*16),row_addr);
							 if(prg_sel_flag == 1)
							 {
							 	lcd_write_instr(0x8c);
								lcd_write_data(sel_flag+1+0x30);
							}
							
								
							break;

				case OK:  okflag = 1;
							break;
				default: 
							break;
			}	
			
			cntr=0;

		}	
			mode_number = sel_flag;

	//}while((okflag != 1)&&(exitflag != 1)&&(keytimeout != 1));
		}while((okflag != 1)&&(exitflag != 1));
	return sel_flag;
}
void clear_display(void)
{
		lcd_display("                ",0x80);
		lcd_display("                ",0xc0);
		/*lcd_display("                ",0x94);
		lcd_display("                ",0xD4);*/
}
/* Subroutine for writing instruction to  LCD display */
/************************************************************************************
 * Function		: lcd_write_instr
 * Author		: Navya
 * Description	:  i) by sing this function we can write a instruction on lcd
 *				  			 ii) As per timing diagram set the configuration
 ************************************************************************************/

/*void lcd_write_instr(unsigned char x) 
{  static int j;
	compliment_strobe;
    LCD_DATA = x;
    LCDRS_enable;
	LCDRW_enable;
	LCD_disable;
	_nop_();
	_nop_();
	LCD_enable;
		for(j=0;j <= 50;j++);
			//callserialcomm();
	return;
}  */ 
/*End of subroutine for writing instruction to  LCD */

/************************************************************************************
 * Function		: lcd_write_data
 * Author		: Navya
 * Description	:  i) this subroutine is used to writing data on LCD
 *								 ii) As per timing diagram set the configuration
 ************************************************************************************/

/* Subroutine for writing data to  LCD display */
/*void lcd_write_data(unsigned char  buf1)
{   
	static int j;
	compliment_strobe;
	LCDRS_disable;
	LCDRW_enable;
	LCD_disable;
	LCD_DATA = buf1;
  LCD_enable;
	for(j=0;j <= 60;j++);
		//callserialcomm();
	return;
}     */
/*end of subroutine*/

/*subroutine for lcd line display with arguments as the ptr to string and ptr to line address of lcd*/ 
/*void lcd_display(unsigned char *buf,unsigned char addr) 
{
//compliment_strobe;
	send_data(addr);          //addr corresponds to particular line of lcd

	for(i=0;(buf[i]!='\0')&&(i<16);i++)  {
	writedata(buf[i]);

	}
		
    return;
}  */  
/*end of subroutine*/

/*void lcd_init(void) 
{
	compliment_strobe;

	LEDEN=0;	// disable LEDS lines
	
	LCDRS=0;
	LCDRW=0;
	LCDEN=0;
	for(j=0;j<=20000;j++);
		send_data(0x38);
	for(j=0;j<=20000;j++);
		send_data(0x38);
	for(j=0;j<=20000;j++);
		send_data(0x38);
	for(j=0;j<=2000;j++);
		send_data(0x38);
	for(j=0;j<=2000;j++);
		send_data(0x08);
	for(j=0;j<=2000;j++);
		send_data(0x01);
	for(j=0;j<=2000;j++);
		send_data(0x06);
	for(j=0;j<=2000;j++);
		send_data(0x0c);
	for(j=0;j<=2000;j++);

	return;
}      */ 
//---------------------------------------
/*subruotine for cursor blink at a particular location*/
/*void cursorblink(unsigned char addr)
{
	lcd_write_instr(0x0d);
	lcd_write_instr(addr);
	return;
}*/
/* Subroutine for writing instruction to  LCD display */
/************************************************************************************
 * Function		: lcdbcd
 * Author		: Navya
 * Description	:  i) by using this function will get a bcd variable 
 ************************************************************************************/
/*void lcdbcd(char end_loc,char decimal_point)
{
	char v,minus_flag=0;
	for(i=0;i<16;i++)
		buf[i] = ' ';
	
	t=end_loc;
//	while(t >= (7-decimal_point))
	while(t >= (8-decimal_point))
	{
		buf[t--] = 0x30;
	}
	if(decimal_point != 0)
	{
		buf[end_loc - decimal_point] = '.';		
	}
	t=end_loc;
	while(c != 0)
	{
		compliment_strobe;
		v = (c % 0x0a) + 0x30;
		buf[t] = v;
		c = c/0x0a;
		t = t-1;
		if((decimal_point != 0)&&(t == (end_loc - decimal_point)))
		{
			buf[t] = '.';
			t=t-1;
		}
	}
	
	lcd_display(buf,0xc0);

}*/


//val for polarity editing at 1st location
//dbuf for string reading
//deci_point for decimal point editing
/* Subroutine for writing instruction to  LCD display */
/************************************************************************************
 * Function		: editing_value
 * Author		: Navya
 * Description	:  i) by using this function we can edit the  value and also update display
 
 ************************************************************************************/
/*int editing_value(int temp_value,char sign_flag,char decimal_point,char no_digits,char end_loc,unsigned char row_addr)
{
	char minus_flag=0,st_loc;
	unsigned char v,cursor_flag=0,var=0;
	unsigned int val;
//	if(ch_edit_flag == 0)
	{
		for(i=0;i<16;i++)
			buf[i] = ' ';
	}
	exitflag = 0;
	okflag = 0;
	c = temp_value;
	if((decimal_point == 0) && (sign_flag == 0))
		st_loc = end_loc - no_digits + 1;
	else
		st_loc = end_loc - 5;
	
	t = end_loc;
	while(t >= st_loc)
	{
		buf[t--] = 0x30;
	}
	if(decimal_point != 0)
	{
		buf[end_loc - decimal_point] = '.';		
	}
	t = end_loc;
	while(c != 0)
	{
		compliment_strobe;
		v = (c % 0x0a) + 0x30;
		buf[t] = v;
		c = c/0x0a;
		t = t-1;
		if((decimal_point != 0)&&(t == (end_loc - decimal_point)))
		{
			buf[t] = '.';
			t=t-1;
		}
	}
	if(minus_flag == 1)
		buf[st_loc] = '-';
	//t = end_loc+1;		//old 
	t = end_loc;		
	//lcd_display(buf,0xc0);

		lcd_display(buf,row_addr);

	
	t = end_loc;			

	//cursorblink(0xc0 + t);
	cursorblink(row_addr + t);	//
	do
	{
		compliment_strobe;
		if(cntr != 0)
		{
			switch(cntr)
			{
				case PRG: exitflag = 1;
						break; 

				case LEFT_SHIFT: 
						if(no_edit_flag == 0)
						{
							if(--t < st_loc)
								t = end_loc;
							if((decimal_point != 0) && (t == (end_loc - decimal_point)))
								t++;
							//cursorblink(0xc0 + t);
							cursorblink(row_addr + t);	
							//lcd_write_instr(0xc0 + t);
								lcd_write_instr(row_addr + t);

							if((decimal_point != 0) && (t == (end_loc - decimal_point+1)))
								t--;
							if((t == st_loc) && (sign_flag == 1))
							{
								if(buf[t] == '.')
									var = 10;
								else
									var = 11;
							}
							else
								var = buf[t] - 0x30;
							//cursor_flag = 1;				
						}
						break;

				case UP:	
						
							//if(cursor_flag == 1)				
							{
								if((t == st_loc)&&(sign_flag == 1))
								{
									if(++var > 11)
										var = 10;
									if(var == 10)
										lcd_write_data(buf[t] = '-');
									else
										lcd_write_data(buf[t] = 0x30);
								}
								else
								{
									if(++var > 9)
										var = 0;
								if((decimal_point != 0) && (t == (end_loc - decimal_point)))
									//lcd_write_instr(0xc0 + t+1);
									lcd_write_data(buf[t+1] = (var + 0x30));
								else
									//lcd_write_instr(0xc0 + t);
									lcd_write_data(buf[t] = (var + 0x30));
								}
								if((decimal_point != 0) && (t == (end_loc - decimal_point)))
									//lcd_write_instr(0xc0 + t+1);
										lcd_write_instr(row_addr + t+1);
								else
									//lcd_write_instr(0xc0 + t);
									lcd_write_instr(row_addr + t);
							//	lcd_write_instr(0xc1 + t);
							}
						
						break;
			case OK:		temp_value = 0;
						val = 1;
						okflag = 1;
						for(t = end_loc;t >= st_loc;t--)
						{
							if(buf[t] == '-')
							{
								temp_value = 0 - temp_value;
								break;
							}
							else if(buf[t] != '.')
							{
								temp_value += (buf[t] - 0x30) * val;	
								val = val*10;
							}
						}
						break;
				default: 	
						break;
			}
			cntr=0;
		}		
	//}while((okflag != 1) && (exitflag != 1) && (keytimeout != 1));
		}while((okflag != 1) && (exitflag != 1));
	lcd_write_instr(0x0c);
	return temp_value;
}



char st_loc=7,t_t = 11;
unsigned char v,cursor_flag=0,var=0,password_chk_flag1=0;
unsigned int val,tt;*/



//val for polarity editing at 1st location
//dbuf for string reading
//deci_point for decimal point editing

/*char LCD_editing_selection(char sel_flag,char totlabytes,unsigned char row_addr,unsigned char *str1 char *str2,unsigned char *str3,unsigned char *str4)
{
	char mode=sel_flag;
	exitflag = 0;
	okflag = 0;
	cntr  =UP;
	--sel_flag;
	do
	{
		compliment_strobe;
		if(cntr != 0)
		{
			switch(cntr)
			{
				case ESC:	 	exitflag = 1;
										sel_flag=mode;
				case PRG: 	exitflag = 1;
							sel_flag=mode;
							break;
				case UP:	if(++sel_flag >= totlabytes)
								sel_flag = 0;

							
								
								 //lcd_display(str1+(sel_flag*20),0xc0);
								lcd_display(str1+(sel_flag*16),row_addr);
							 if(prg_sel_flag == 1)
							 {
							 	lcd_write_instr(0x8c);
								lcd_write_data(sel_flag+1+0x30);
							}
							
							
								
							break;
			
				case OK:  okflag = 1;
							break;
				default: 
							break;
			}	
			
			cntr=0;
		}	
		
	//}while((okflag != 1)&&(exitflag != 1)&&(keytimeout != 1));
		}while((okflag != 1)&&(exitflag != 1));
	
	return sel_flag;
}*/
/*void clear_display(void)
{
		lcd_display("                ",0x80);
		lcd_display("                ",0xc0);
}*/


