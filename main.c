#include <avr/io.h>
#include <avr/eeprom.h>
#include<util/delay.h>
#include <avr/sleep.h>
#define data PORTD
#define ctrl PORTC
#define en 1
#define rs 0
#define kprt PORTB
#define kpin PINB
#define kddr DDRB
#define d 200
void read_pw(unsigned int);
unsigned int wp=0;
void read_eep(int);
void change_pw(unsigned char);
void lcd_cmd1();
int keypad();
uint8_t y;
static int ch[10]={'0','1','2','3','4','5','6','7','8','9'};
static uint8_t pww[4];
static int p[4];
int keypad()
{
   uint8_t key[][4]={{1,2,3,0},{4,5,6,0},{7,8,9,0},{'*',0,'#','d'}};
   unsigned char col,row;
   kddr=0xf0;
   kprt=0xff;
   while(1)
   {
      do
      {
	 kprt&=0x0f;
	 col=(kpin&0x0f);
      }while(col!=0x0f);
      do
      {
	 do
	 {
	    _delay_ms(20);
	    col=(kpin&0x0f);
	 }while(col==0x0f);
	 _delay_ms(20);
	 col=(kpin&0x0f);
      }while(col==0x0f);
      while(1)
      {
	 kprt=0xef;
	 col=(kpin&0x0f);
	 if(col!=0x0f)
	 {
	    row=0;
	    break;
	 }
	 kprt=0xdf;
	 col=(kpin&0x0f);
	 if(col!=0x0f)
	 {
	    row=1;
	    break;
	 }
	 kprt=0xbf;
	 col=(kpin&0x0f);
	 if(col!=0x0f)
	 {
	    row=2;
	    break;
	 }
	 kprt=0x7f;
	 col=(kpin&0x0f);
	 row=3;
	 break;
      }
      if(col==0x0e)
      {
	 y=key[row][0];
      return y;
      }
      
      else if(col==0x0d)
      {
	 y= (key[row][1]);
	 return y;
      }
      else if(col==0x0b)
      {
	 y=(key[row][2]);
      return y;
      }
      else if(col==0x07)
      {y=(key[row][3]);
      return y;
      }
   }
}

void lcd_cmd(unsigned char cmd)
{
   PORTC&=~(1<<rs);
   data=cmd&0xf0;
   ctrl|=(1<<en);
   _delay_us(10);
   ctrl&=~(1<<en);
   _delay_us(100);
   data=cmd<<4;
   ctrl|=(1<<en);
   _delay_us(10);
   ctrl&=~(1<<en);
   _delay_us(100);
}
void lcd_data(uint8_t dta)
{
   ctrl|=(1<<rs);
   data=dta&0xf0;
   ctrl|=(1<<en);
   _delay_us(10);
   ctrl&=~(1<<en);
   _delay_us(100);
   data=dta<<4;
   ctrl|=(1<<en);
   _delay_us(10);
   ctrl&=~(1<<en);
   _delay_us(100);
}
void lcd_init()
{
   DDRD=0xff;
   DDRC=0xff;
   ctrl&=~(1<<en);
   _delay_us(10);
   lcd_cmd(0x33);
   lcd_cmd(0x32);
   lcd_cmd(0x28);
   lcd_cmd(0x0e);
   _delay_us(2000);
   lcd_cmd(0x06);
   _delay_us(100);
}
void lcd_gotoxy(unsigned char x, unsigned char y)
{
   unsigned char add[]={0x80,0xC0};
   lcd_cmd(add[x-1]+y-1);
   _delay_us(100);
}
void lcd_strng( char str[])
{
   unsigned char i=0;
   while(str[i]!=0)
   {
      lcd_data(str[i]);
      i++;
   }
}
void lcd_strng1( char str[])
{
   unsigned char i=0;
   while(str[i]!=0)
   {
      lcd_data(str[i]);
      i++;
      _delay_ms(200);
   }
}
void wrong_pw(unsigned int m)
{
   wp++;
   if(wp!=3)
   {
   lcd_cmd(0x01);
   _delay_ms(250);
   lcd_strng("try again!!");
      _delay_ms(250);
      lcd_cmd(0xc0);
      _delay_ms(250);
   lcd_data(ch[3-wp]);
   _delay_ms(100);
      lcd_strng(" attempt left");
      _delay_ms(1000);
      lcd_cmd1();
      lcd_strng("enter pw again");
      lcd_cmd(0xc0);
   read_pw(m);
   }
   else if(wp==3)
   {
      while(1)
      {
      lcd_cmd1();
      lcd_strng1("max atmpt reached");
       _delay_ms(100);
      lcd_cmd(0xc0);
      lcd_strng1("pls rstrt systm to enter");
      }
   }
      
}
/*void eep_write(unsigned char a,int b)
{
   while(EECR&(1<<EEWE));
      EEAR=b;
   EECR=a;
   EECR|=(1<<EEMWE);
   EECR|=(1<<EEWE);
}
void eep_awrite(unsigned char a[],unsigned char b)
{
   
   int i,j=0;
   for(i=0;i<b+1;b++)
   {
      eep_write(a[i],j);
      j++;
   }
}
unsigned char eep_read(unsigned char a)
{
   while(EECR&(1<<EEWE));
      EEAR=a;
   EECR|=(1<<EERE);
   return EEDR;
}
unsigned char eep_aread(unsigned char a,unsigned char b)
{
   int i,j[a];
   for(i=0;i<b+1;i++)
   {
      j[i]=eep_read(b);
      b++;
   }
   return (j);
}*/
void lcd_cmd1()
{
   _delay_ms(200);
   lcd_cmd(0x01);
   _delay_ms(200);
}
uint8_t pw1[4];
void eep_init(unsigned char x)
{
   read_eep(x);
   if((pw1[0]==1)&&(pw1[1]==1)&&(pw1[2]==1)&&(pw1[3]==1))
   {
      lcd_cmd1();
      lcd_strng("u must change pw");
      _delay_ms(1000);
      change_pw(x);
      lcd_cmd1();
   }
}
void read_eep(int a)
{
   eeprom_read_block((void*)pw1,(const void*)(a*4),4);
}
void write_eep(uint8_t c[],int a)
{
   eeprom_update_block((const void*)c,(void*)(a*4),4);
}
void read_pw(unsigned int c )
{
   unsigned int i;
    read_eep(c);
   for(i=0;i<4;i++)
   {
      p[i]=keypad();
      if(p[i]=='*')
      {
	 
	 lcd_cmd(0x04);
	 lcd_data(' ');
	  lcd_data(' ');
	 lcd_cmd(0x04);
	 i=i-1;;
      }
      else
      lcd_data('*');
      _delay_ms(200);
   }
      for(i=0;i<4;i++)
   {
      if(p[i]!=pw1[i])
      {
	 lcd_cmd(0x01);
	 _delay_ms(250);
	 lcd_strng("pw not matched");
	 wrong_pw(c);
       }
    }
  lcd_cmd1();
  lcd_strng("pw matched");
  wp=0;  
 
 }
 void check_eep()
 {
    unsigned int l,i;
    uint8_t rr[4]={1,1,1,1};
    l=eeprom_read_byte((uint8_t*)1000);
    if(l!=5)
    {
       for(i=0;i<10;i++)
       {
	  write_eep(rr,i);
       }
       eeprom_write_byte((uint8_t*)1000,5);
    }
 }
       
   
void change_pw(unsigned char ch)
{
   
   int y;
   uint8_t chhh[10]={'0','1','2','3','4','5','6','7','8','9'};
   unsigned char i,chh;
   lcd_cmd(0x01);
   _delay_ms(250);
   lcd_strng("enter old pw");
   _delay_ms(100);
   lcd_cmd(0xc0);
   read_pw(ch);
   lcd_cmd1();
   lcd_strng("enter new pw");
   _delay_ms(200);
   lcd_cmd(0xc0);
   _delay_ms(150);
   for(i=0;i<4;i++)
   {
      y=pww[i]=keypad();
      lcd_data(chhh[y]);
   }
   lcd_cmd1();
   lcd_strng("entered pw is");
   _delay_ms(200);
   lcd_cmd(0xc0);
   _delay_ms(200);
   for(i=0;i<4;i++)
   {
      lcd_data(chhh[pww[i]]);
   }
   _delay_ms(2000);
   lcd_cmd1();
   lcd_strng("chnge pw?1-Y 0-N");
   _delay_ms(200);
   lcd_cmd(0xc0);
   chh=keypad();
   if(chh==1)
   {
      write_eep(pww,(int)ch);
      lcd_cmd1();
      lcd_strng("pw changed");
   }
   else if(chh==0)
   {
      lcd_cmd1();
      lcd_strng("pw not changed");
   }
   else
   {
     lcd_cmd1();
      lcd_strng("invalid command");
   }
}
void motor(unsigned int a)
{
   DDRA=0XFF;
   if(a==1)
   {
      lcd_cmd1();
      PORTA=0B01010101;
      lcd_strng1("door opened!!");
      _delay_ms(1000);
      PORTA=0X00;
   }
   else
   {
      lcd_cmd1();
      PORTA=0B10101010;
      lcd_strng1("DOOR CLOSED!!");
      _delay_ms(1000);
      PORTA=0X00;
      
   }
}
void master_mode()
{
   int i,kk[4]={5,1,8,8},ll[4],jj;
   lcd_cmd1();
   lcd_strng("enter secret code");
   lcd_cmd(0xc0);
   for(jj=0;jj<4;jj++)
   {
   ll[jj]=keypad();
   lcd_data('*');
    _delay_ms(200);
   }
   for(jj=0;jj<4;jj++)
   {
      if(ll[jj]!=kk[jj])
      {
	 
	 while(1)
	 {
	    lcd_cmd1();
	 lcd_strng("wrong pin!!");
	 }
      }
   }
   lcd_cmd1();
   lcd_strng("for pw recvry *");
   lcd_cmd(0xc0);
   lcd_strng("for block unblk #");
   i=keypad();
   if(i=='*')
   {
      lcd_cmd1();
      lcd_strng("ENTR ID NUMBER");
      lcd_cmd(0xc0);
      i=keypad();
      eeprom_read_block((void*)pw1,(const void*)(i*4),4);
      lcd_cmd1();
      lcd_strng("the pw is");
      lcd_cmd(0xc0);
      for(i=0;i<4;i++)
      {
	 lcd_data(ch[pw1[i]]);
	 _delay_ms(200);
      }
      _delay_ms(1000);
   }
   
   else if(i=='#')
   {
      lcd_cmd1();
      lcd_strng("for blking *");
      lcd_cmd(0xc0);
      lcd_strng("for unblking #");
      i=keypad();
      if(i=='*')
      {
	 lcd_cmd1();
	 lcd_strng("entr id to blk");
	 lcd_cmd(0xc0);
	 i=keypad();
	 if(i==0)
	    lcd_strng("invalid id!!");
	 else
	 {
	 eeprom_write_byte((uint8_t*)(1001+i),0);
	 lcd_strng("id blocked!!");
	 _delay_ms(1000);
	 }
      }
      else if(i=='#')
      {
	 lcd_cmd1();
	 lcd_strng("entr id to b unblkd");
	 i=keypad();
	 eeprom_write_byte((uint8_t*)(1001+i),1);
	 lcd_strng("id unblocked");
	 _delay_ms(1000);
	 
      }
   }
}
      
   
   

int main()
{
 unsigned int y,q,w,e,i,r,z,id_sts;
  char lock,nam[][10]={"AAA","   RK","shyam","sonup","sovit","bibu","nisha","geeta","sita","meena","teena"},cmd;
   
  lcd_init();
 // lcd_cmd(0x01);
   lcd_gotoxy(1,1);
   lcd_strng1("...WELCOME TO...") ;
   _delay_ms(1000);
   lcd_gotoxy(2,1);
   _delay_ms(1000);
   lcd_strng1(" ROBOTICS CLUB");
  _delay_ms(1000);
  lcd_cmd1();
  lcd_strng1("PROJECT MADE BY");
  _delay_ms(1000);
  lcd_cmd1();
  lcd_gotoxy(1,3);
  _delay_ms(10);
  lcd_strng1("RAMKRISHNA");
  lcd_gotoxy(2,5);
  lcd_strng1("BEX071");
  
   _delay_ms(1000);
   check_eep();
  while(1)
  {
   do{
   lcd_cmd(0x01);
   _delay_ms(100);
   lcd_strng("enter yor ID NO");
   lcd_gotoxy(2,1);
    y=keypad();
   lcd_data(ch[y]);
      if((y=='*')|(y=='#')){
	 lcd_cmd1();
	 lcd_strng("invalid id!!!");
	 _delay_ms(500);
      }
   }while((y=='*')|(y=='#'));
    _delay_ms(1000);
   lcd_cmd(0x01);
   _delay_ms(100);
   lcd_strng("enter password");
   _delay_ms(100);
   lcd_cmd(0xc0);
   _delay_ms(100);
  
   read_pw(y);
   id_sts=eeprom_read_byte((uint8_t*)(1001+y));
   if(id_sts==0)
   {
      lcd_cmd1();
      lcd_strng("    sorry!!");
      lcd_cmd(0xc0);
      lcd_strng("yor id is blocked");
      _delay_ms(2000);
      lcd_cmd1();
      lcd_strng("ask admin for ");
      lcd_cmd(0xc0);
      lcd_strng("    UNBLOCKING!!");
      _delay_ms(2000);
   }
   else
   {
      
    _delay_ms(1000);
    lcd_cmd1();
    lcd_strng1(".....WELCOME....");
   _delay_ms(100);
   lcd_gotoxy(2,6);
   _delay_ms(100);
   lcd_strng((char*)&(nam[y][10]));
   _delay_ms(2000);
   eep_init(y);
   lcd_cmd(0x01);
   _delay_ms(200);
   do{
      lcd_cmd1();
   lcd_strng("enter * to change");
   _delay_ms(150);
   lcd_cmd(0xc0);
   _delay_ms(150);
   lcd_strng("   password");
   _delay_ms(1500);
   lcd_cmd(0x01);
   _delay_ms(200);
   lcd_strng("enter # to open");
   _delay_ms(150);
   lcd_cmd(0xc0);
   _delay_ms(100);
   lcd_strng("   THE DOOR");
    _delay_ms(2000);
    lcd_cmd(0x01);
    _delay_ms(250);
    lcd_strng("waiting for cmnd");
    _delay_ms(100);
    lcd_cmd(0xc0);
    cmd=keypad();
    lcd_data(cmd);
    if((y==0)&&(cmd==0))
    master_mode();
    if((cmd!='*')&&(cmd!='#'))
    {
       lcd_cmd1();
       lcd_strng("invalid cmnd!!");
       _delay_ms(1000);
    }
 }while((cmd!='*')&&(cmd!='#'));
    if(cmd=='*')
       change_pw(y);
    else if(cmd=='#')
    {
       lcd_cmd1();
       do{
       lcd_strng("enter method for");
	  _delay_ms(500);
       lcd_cmd(0xc0);
       lcd_strng("door locking");
	  _delay_ms(500);
       lcd_cmd1();
       lcd_strng("atmtic lock--*");
	  _delay_ms(500);
       lcd_cmd(0xc0);
       lcd_strng("mannual lock--#");
       lock=keypad();
       }while((lock!='*')&&(lock!='#'));
       if(lock=='*')
       {
	  do
	  {
	  lcd_cmd1();
	  lcd_strng("entr tym to wait");
	     _delay_ms(100);
	  lcd_cmd(0xc0);
	  lcd_strng("for locking");
	     _delay_ms(1500);
	  lcd_cmd1();
	  lcd_strng("in minute");
	  lcd_cmd(0xc0);
	  q=keypad();
	  lcd_data(ch[q]);
	  w=keypad();
	  lcd_data(ch[w]);
	  e=q*10+w;
	     if((q=='*')|(w=='*')|(w=='#')|(q=='#'))
	     {lcd_cmd1();
	      lcd_strng("invalid cmnd");
		_delay_ms(100);
	     }
	
	  }while((q=='*')|(w=='*')|(q=='#')|(w=='#'));
	  motor(1);
	  lcd_cmd1();
	  lcd_strng("door locking in");
	  r=e;
	  for(i=0;i<e;i++)
	  {   
	     lcd_cmd(0xc0);
	     lcd_data(ch[r/10]);
	     lcd_data(ch[r%10]);
	     lcd_strng(" minutes!!");
	     _delay_ms(5900);
	     --r;
	  }
	   motor(0);
       }
       else
       {
	  motor(1);
  	  lcd_cmd1();
	     lcd_strng("press any key");
	     _delay_ms(100);
	    lcd_cmd(0xc0);
	  lcd_strng("for locking!!");
	   
	  i=keypad();
	  motor(0);
       }
    }
    else
    {
       lcd_cmd(0x01);
       _delay_ms(250);
       lcd_strng("invalid cmd!!!");
    }
    lcd_cmd1();
    lcd_strng("press any key for");
    lcd_cmd(0xc0);
    lcd_strng("system restart!!");
    _delay_ms(100);
    z=keypad();
 }}
    return 0;}
    

   
   
   
   
   
 
   
   
   
   
   /*
   lcd_init();
   unsigned char a,b,i;
   lcd_strng("enter sdfsf");
   _delay_ms(1000);
   a=keypad();
   lcd_data(a);
   b=keypad();
   lcd_data(b);
   eep_write(a,0);
   eep_write(b,1);
   _delay_ms(100);
   lcd_strng("writting in eep");
   _delay_ms(100);
   lcd_cmd(0x01);
   _delay_ms(10);
   lcd_data(eep_read(0));
   _delay_ms(100);
   lcd_data(eep_read(1));
   */
