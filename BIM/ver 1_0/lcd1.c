/*unsigned char buf[32];



void send_data(unsigned char x)
{
	LCD_DATA = x;	
	LCDRS_enable;
	LCDRW_enable;
	LCD_disable;
	_nop_();
	_nop_();
	LCD_enable;
	for(j=0;j<=2000;j++);
	return;
}

void writedata(void)
{
	LCDRS_disable;
	LCDRW_enable;
	LCD_disable;
	_nop_();
	_nop_();
	LCD_DATA = buf[i];
	LCD_enable;
		for(j=0;j<=2000;j++);
	return;
}

void dismess(void)
{
	char mess[32]={"POLMON DATA LOGGER LOGGER LOGGER"};
	for(i=0;i<=31;i++)
		buf[i]=mess[i];

return;
}	

void display123(void)
{
	LCDRS_enable;
	LCDRW_enable;
	LCD_enable;
	for(j=0;j<=40000;j++);
		send_data(0x38);
	for(j=0;j<=40000;j++);
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
		send_data(0x0d);
	for(j=0;j<=2000;j++);
		send_data(0x80);		// set ddram add to 00H
	for(j=0;j<=2000;j++);
	for(i=0;i<16;i++)
	{
		writedata();
		for(j=0;j<=2000;j++);
	}
	send_data(0xc0);		// set DDRAM ADDress to 40H
	for(j=0;j<=2000;j++);
	for(i=16;i<32;i++)
	{
		writedata();
		for(j=0;j<=2000;j++);
	}
	return;
}
*/

/* Subroutine for writing instruction to  LCD display */
void lcd_write_instr(unsigned char x) 
{  
	compliment_strobe;
    LCD_DATA = x;
    LCDRS_enable;
	LCDRW_enable;
	LCD_disable;
	_nop_();
	_nop_();
	LCD_enable;
		for(j=0;j <= 50;j++);
	return;
}   
/*End of subroutine for writing instruction to  LCD */


/* Subroutine for writing data to  LCD display */
void lcd_write_data(unsigned char  buf1)
{   
	compliment_strobe;
	LCDRS_disable;
	LCDRW_enable;
	LCD_disable;
	LCD_DATA = buf1;
    LCD_enable;
	for(j=0;j <= 60;j++);
	return;
}     
/*end of subroutine*/

/*subroutine for lcd line display with arguments as the ptr to string and ptr to line address of lcd*/ 
void lcd_display(unsigned char *buf,unsigned char addr) 
{
	compliment_strobe;
	lcd_write_instr(addr);          //addr corresponds to particular line of lcd

    for(i=0;(buf[i]!='\0')&&(i<16);i++)       //HAVE TO BE CHANGED TO TILL NULL
    	lcd_write_data(buf[i]);     //passing each character to be displayed
    return;
}    
/*end of subroutine*/

void lcd_init(void) 
{
	compliment_strobe;
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
	return;
}       
//---------------------------------------
/*char editing_selection(unsigned short totalbytes,unsigned short selectiontype,unsigned char line_sel,unsigned char *p1,...)
{
	int temp_selectiontype;
	unsigned char buf[16],temp_disp_flag = 0;
	d_files ap;
	d_open(ap,p1);
	okflag=0;
	exitflag=0;
	i=0;
	temp_selectiontype=selectiontype;
	while(p1 = d_copy(ap,char*));
	lcd_display("1234",0x80);
	do
	{
		compliment_strobe;
		//for(i=0;i<16;i++)
		//	buf[i+16]=p1[i+(selectiontype*16)];
		if(temp_disp_flag == 0)
		{
			temp_disp_flag = 1;
			if(line_sel == 1)
				lcd_display(p1+selectiontype,0x80);
			else
				lcd_display(p1+selectiontype,0xc0);
		}
		if(cntr != 0)
		{
			switch(cntr)
			{
				case 1: exitflag = 1;
						selectiontype = temp_selectiontype;
						break;
				case 2:	break;
				case 3: if(++selectiontype >= totalbytes)
							selectiontype = 0;
						temp_disp_flag = 0;
						break;
				case 4: okflag = 1;
					 	break;
				default: break;
			}	
			cntr=0;
		}	
	}while((okflag != 1)&&(exitflag != 1)&&(keytimeout != 1));
	d_close(ap);
	return selectiontype; 
}*/
	
/*subruotine for cursor blink at a particular location*/
void cursorblink(unsigned char addr)
{
	lcd_write_instr(0x0d);
	lcd_write_instr(addr);
	return;
}

void lcdbcd(char end_loc,char decimal_point)
{
	char v,minus_flag=0;
	for(i=0;i<16;i++)
		buf[i] = ' ';
	if(c < 0)
	{
		minus_flag = 1;
		c = 0 - c;
	}
	t=end_loc;
	while(t >= (7-decimal_point))
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
	if(minus_flag == 1)
		buf[end_loc - 5] = '-';
	lcd_display(buf,0xc0);
}
//val for polarity editing at 1st location
//dbuf for string reading
//deci_point for decimal point editing
int editing_value(int temp_value,char sign_flag,char decimal_point,char no_digits,char end_loc)
{
	char minus_flag=0,st_loc;
	unsigned char v,cursor_flag=0,var=0;
	unsigned int val;
	for(i=0;i<16;i++)
		buf[i] = ' ';
	exitflag = 0;
	okflag = 0;
	c = temp_value;
	if((decimal_point == 0) && (sign_flag == 0))
		st_loc = end_loc - no_digits + 1;
	else if(decimal_point == 0)
		st_loc = end_loc - 4;
	else if(decimal_point == 1)//modified by roberts
	{
		if(set_point_edit_flag==1)
			st_loc=end_loc-no_digits;
		else
			st_loc = end_loc - 5;
	}
	else
		st_loc = end_loc - 5;
	if(c < 0)
	{
		minus_flag = 1;
		c = 0 - c;
	}

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
	t = end_loc;			//modified by roberts
	lcd_display(buf,0xc0);

	if(active_top_up_flag==1)
	{
		lcd_display("sec",0xca);
	}
	t = end_loc;			//modified by roberts

	cursorblink(0xc0 + t);	//modified by roberts
	do
	{
		compliment_strobe;
		if(cntr != 0)
		{
			switch(cntr)
			{
				case 1: exitflag = 1;
						break; 

				case 2: 
					//	if(no_edit_flag == 0)
						{
							if(--t < st_loc)
								t = end_loc;
							if((decimal_point != 0) && (t == (end_loc - decimal_point)))
								t++;
							cursorblink(0xc0 + t);
							lcd_write_instr(0xc0 + t);
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
							//cursor_flag = 1;					//modified by roberts
						}
						break;

				case 3:	//if(no_edit_flag == 0)
						{
							//if(cursor_flag == 1)				//modified by roberts
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
									lcd_write_instr(0xc0 + t+1);
								else
									lcd_write_instr(0xc0 + t);
							//	lcd_write_instr(0xc1 + t);
							}
						}
						break;
			case 4:		temp_value = 0;
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
	}while((okflag != 1) && (exitflag != 1) && (keytimeout != 1));
	lcd_write_instr(0x0c);
	return temp_value;
}


char st_loc=7,t_t = 11;
unsigned char v,cursor_flag=0,var=0,password_chk_flag1=0;
unsigned int val,tt;



void lcd_rtc_edit(char td_flag)
{
	unsigned char buf[16],cursor_flag=0;
	for(i=0;i<16;i++)
		buf[i] = ' ';
	for(i=4;i<12;i++)
		buf[i] = '0';
	readrtc();
	exitflag = 0;
	okflag = 0;
	if(td_flag == 0)
	{
		lcd_display("      TIME      ",0x80);
		buf[6] =buf[9] =':';
		buf[11]=(tseconds%10)+0x30;
		buf[10]=(tseconds/10)+0x30;
		buf[8]=(tminutes%10)+0x30;
		buf[7]=(tminutes/10)+0x30;
		buf[5]=(thours%10)+0x30;
		buf[4]=(thours/10)+0x30;
	}
	else
	{
		lcd_display("      DATE      ",0x80);
		buf[6] =buf[9] ='/';
		buf[11]=(tyear%10)+0x30;
		buf[10]=(tyear/10)+0x30;
		buf[8]=(tmonth%10)+0x30;
		buf[7]=(tmonth/10)+0x30;
		buf[5]=(tdate%10)+0x30;
		buf[4]=(tdate/10)+0x30;
	}
	lcd_display(buf,0xc0);
	t = 12;
	do
	{
		compliment_strobe;
		if(cntr != 0)
		{
			switch(cntr)
			{
				case 1: exitflag = 1;
						break; 

				case 2: if(--t < 4)
							t = 11;
						if((t == 9)||(t == 6))
							t = t-1;
						cursorblink(0xc0 + t);
						lcd_write_instr(0xc0 + t);
						var = buf[t] - 0x30;
						cursor_flag = 1;
						break;

				case 3:	if(cursor_flag == 1)
						{
							if(++var > 9)
								var = 0;
							lcd_write_data(buf[t] = (var + 0x30));
							lcd_write_instr(0xc0 + t);
						//	lcd_write_instr(0xc1 + t);
						}
						break;
				case 4:	if(td_flag == 1)
						{
							tyear = ((buf[10] - 0x30) * 10)+(buf[11] - 0x30);
							tmonth = ((buf[7] - 0x30) * 10)+(buf[8] - 0x30);
							tdate = ((buf[4] - 0x30) * 10)+(buf[5] - 0x30);
							if(tdate > 31)
								tdate = 1;
							if(tmonth > 12)
								tmonth = 1;
							if((tmonth == 2)&&((tyear%4) == 0))
							{
								if(tdate > 29)
									tdate = 1;
							}
							else if(tmonth == 2)
							{
								if(tdate > 28)
									tdate = 1;
							}	
						   	else if((tmonth == 4)||(tmonth == 11)||(tmonth == 9)||(tmonth == 6))
							{
								if(tdate > 30)
									tdate = 1;
							}
							else if((tmonth == 1)||(tmonth == 3)||(tmonth == 5)||(tmonth == 7)||(tmonth == 12)||(tmonth == 8)||(tmonth == 10))
							{
								if(tdate > 31)
									tdate = 1;
							}
						}
						else
						{
							tseconds = ((buf[10] - 0x30) * 10)+(buf[11] - 0x30);
							tminutes = ((buf[7] - 0x30) * 10)+(buf[8] - 0x30);
							thours = ((buf[4] - 0x30) * 10)+(buf[5] - 0x30);
							if(tseconds >= 60)
								tseconds = 0;
							if(tminutes >= 60)
								tminutes = 0;
							if(thours >= 60)
								thours = 0;
						}
						okflag = 1;
						rtcwrite();
						break;
				default: 	
						break;
			}
			cntr=0;
		}		
	}while((okflag != 1) && (exitflag != 1) && (keytimeout != 1));
	lcd_write_instr(0x0c);
}

char LCD_editing_selection(char sel_flag,char totlabytes,unsigned char *str1/*,unsigned char *str2,unsigned char *str3,unsigned char *str4*/)
{
	char mode=sel_flag;
	exitflag = 0;
	okflag = 0;
	cntr  =INC;
	--sel_flag;
	do
	{
		compliment_strobe;
		if(cntr != 0)
		{
			switch(cntr)
			{
				case PRG: 	exitflag = 1;
							sel_flag=mode;
							break;
				case INC:	if(++sel_flag >= totlabytes)
							{
								if(prg_sel_flag==1)
								{
									if(supervisor_flag==1)
									{
										sel_flag=2;
									}
									else
									{
										sel_flag=0;
									}
								}
								else
								{
									sel_flag = 0;
								}
							}
							 lcd_display(str1+(sel_flag*16),0xc0);
							 //if(prg_sel_flag == 1)
							 //{
							 	//lcd_write_instr(0x8c);
								//lcd_write_data(sel_flag+1+0x30);
							//}
							/*if(sel_flag == 0)
								lcd_display(str2,0xc0);
							else if(sel_flag == 1)
								lcd_display(str3,0xc0);
							else if(sel_flag == 2)
								lcd_display(str4,0xc0);
							else;*/
							break;
			
				case OK:  okflag = 1;
							break;
				default: 
							break;
			}	
			cntr=0;
		}	
	}while((okflag != 1)&&(exitflag != 1)&&(keytimeout != 1));
	return sel_flag;
}


