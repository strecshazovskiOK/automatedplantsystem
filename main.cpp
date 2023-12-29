//MASTER
/*
 * automatedplantwateringsystem.cpp
 *
 * Created: 28.12.2023 13:40:28
 * Author : Omercan Kahveci
 */ 
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#define LCD_PortA PORTA
#define LCD_APin DDRA
#define RSPINA PA0
#define ENPINA PA1
#define ADC_PIN 0;

void LCD_ActionA( unsigned char cmnd)
{
	LCD_PortA = (LCD_PortA & 0X0F) | (cmnd & 0XF0);
	LCD_PortA &= ~ (1<<RSPINA);
	LCD_PortA |= (1<<ENPINA);
	_delay_us(1);
	LCD_PortA &= ~ (1<<ENPINA);
	_delay_us(200);
	LCD_PortA = (LCD_PortA & 0X0F) | (cmnd << 4);
	LCD_PortA |= (1<<ENPINA);
	_delay_us(1);
	LCD_PortA &= ~ (1<<ENPINA);
	_delay_ms(2);
}

void LCD_ClearA() //Clear function for LCDA
{
	LCD_ActionA(0X01);
	_delay_ms(5);
	LCD_ActionA(0X80);
}

void LCD_PrintA(char *str)// this function do the print for LCDA
{
	int i;
	for(i=0; str[i]!=0; i++)
	{
		LCD_PortA = (LCD_PortA & 0X0F) | (str[i] & 0XF0);
		LCD_PortA |= (1<<RSPINA);
		LCD_PortA |= (1<<ENPINA);
		_delay_us(1);
		LCD_PortA &= ~(1<<ENPINA);
		_delay_us(200);
		LCD_PortA = (LCD_PortA & 0X0F) | (str[i] << 4);
		LCD_PortA |= (1<<ENPINA);
		_delay_us(1);
		LCD_PortA &= ~ (1<<ENPINA);
		_delay_ms(2);
	}
}
//Write on a specific location LCDA
void LCD_PrintposA(char row, char pos, char *str)
{
	if(row == 0 && pos<16)
		LCD_ActionA((pos & 0X0F) | 0X80);
	else if(row==1 & pos<16)
		LCD_ActionA((pos & 0X0F) | 0XC0);
	LCD_PrintA(str);
}
//INITIALIZE FUNCTION FOR LCDA
void LCD_InitA(void)
{
	LCD_APin = 0XFF;
	_delay_ms(15);
	LCD_ActionA(0X02);
	LCD_ActionA(0X28);
	LCD_ActionA(0X0c);
	LCD_ActionA(0X06);
	LCD_ActionA(0X01);
	_delay_ms(2);
}
void init_ADC(uint8_t ch)
{	
	// SELECTING INPUT CHANNEL WITH GIVEN INFO AS CH( WHICH PORT )
	ADMUX =(ADMUX & 0xF0) | (ch & 0x0F);
	ADMUX |= (1<<REFS0);// setting reference voltage to AVCC
	
	ADCSRA |=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	// enabling ADC and  setting prescaler to 128;
}
uint16_t read_ADC()
{	
	ADCSRA |= (1<<ADSC);//conversion start setted
	
	while(ADCSRA &(1<<ADSC));//wait until conversion complete
	
	return ADC;
}

int main(void)
 {	 
	 char buffer0[10];  
	 char buffer1[10];  
	 char buffer2[10]; 
	
	//initialize LCDA
	LCD_InitA();
	DDRB=0XFF;
	DDRC =0X01;
	while(1)
	{		
		
		 
	 // Read ADC value
		 LCD_ClearA();
		
		 uint16_t adcValueBIT0;
		 uint16_t adcValueBIT1;
		 uint16_t adcValueBIT2;
		 
		 init_ADC(0);
		 adcValueBIT0=read_ADC();
		 init_ADC(1);
		 adcValueBIT1=read_ADC();
		 init_ADC(2);
		 adcValueBIT2=read_ADC();
		 
		if((adcValueBIT0<531)&&(adcValueBIT1<531)&&(adcValueBIT0<531))//URGENT FULL WATERING to make all livible level
		{
			LCD_PrintposA(0, 0, "URGENT MOD ACTIVATED ");
			LCD_PrintposA(1, 0, "WATERNEEDED ALL!!");
			_delay_ms(100);
			uint16_t FULLLevelCheck=0;
			uint16_t zeroLevelFlag=0;
			uint16_t firstLevelFlag=0;
			uint16_t secondLevelFlag=0;
			
			char FULLlevelBuffer0[10];
			char flevelBuffer0[10];
			char flevelBuffer1[10];
			char flevelBuffer2[10];
			// PORTB=0x01;
			LCD_ClearA();
			LCD_PrintA("MtrStarted-90to90!");
			LCD_ClearA();
			
			PORTB=0b00010000; // start pump
			PORTC=0X01; // START PUMP URGENT MODE
			LCD_ClearA();
			_delay_ms(100);
			LCD_PrintposA(0, 0, "PUMP STARTED!! ");
			LCD_PrintposA(1, 0, "URGENT MODE!");
			LCD_ClearA();
			while(FULLLevelCheck!=1)
			{
				LCD_ClearA();
				if(adcValueBIT0>531&&adcValueBIT0<610)
				{
					LCD_ClearA();
					LCD_PrintposA(0, 0, "GOOD LEVEL L");
					_delay_ms(30);
					LCD_ClearA();
					zeroLevelFlag=1;
				}
				if(adcValueBIT1>531&&adcValueBIT1<610)
				{
					
					 LCD_ClearA();
					 LCD_PrintposA(0, 0, "GOOD LEVEL M");
					 _delay_ms(30);
					 LCD_ClearA();
					 firstLevelFlag=1;
			    }
				if(adcValueBIT2>531&&adcValueBIT2<610)
				{
					
					LCD_ClearA();
					
					LCD_PrintposA(0, 0, "GOOD LEVEL M");
					_delay_ms(30);
					LCD_ClearA();
					secondLevelFlag=1;
				}
				if(zeroLevelFlag==1&&firstLevelFlag==1&&secondLevelFlag==1)
				{	
					PORTB=0b00001000; // stop pump
					PORTC=0x00;
					LCD_ClearA();
					LCD_PrintposA(0, 0, "PUMP STOPPED!! ");
					LCD_PrintposA(1, 0, "GOOD LEVEL ALL");
					_delay_ms(30);
					LCD_ClearA();
					FULLLevelCheck=1;
				}
				
				
				sprintf(flevelBuffer0, "%u", adcValueBIT0+=40);  // Convert integer to string
				LCD_PrintposA(0, 0, "WATERLEVEL L: ");
				LCD_PrintposA(1, 0, flevelBuffer0);
				
				sprintf(flevelBuffer1, "%u", adcValueBIT1+=40);  // Convert integer to string
				LCD_PrintposA(0, 0, "WATERLEVEL M: ");
				LCD_PrintposA(1, 0, flevelBuffer1);
				_delay_ms(30);
				LCD_ClearA();
				
				sprintf(flevelBuffer2, "%u", adcValueBIT2+=40);  // Convert integer to string
				LCD_PrintposA(0, 0, "WATERLEVEL R: ");
				LCD_PrintposA(1, 0, flevelBuffer2);
				_delay_ms(30);
				LCD_ClearA();
			}
		}
		else
		{		
				//ADC0
				 if(adcValueBIT0>531&&adcValueBIT0<610)
				 {	
			 
					 LCD_PrintposA(0, 0, "SOILSENSORLEFT ");
					 LCD_PrintposA(1, 0, "GOOD LEVEL");
					 _delay_ms(100);
			 
					 LCD_ClearA();
				 }
				 else if(adcValueBIT0<531)
				 {
					  LCD_PrintposA(0, 0, "SOILSENSORLEFT ");
					  LCD_PrintposA(1, 0, "WATERNEEDED!!");
					  _delay_ms(100);
					  uint16_t bit0LevelCheck=0;
					  char levelBuffer0[10];
					 // PORTB=0x01;
					  LCD_ClearA();
					  LCD_PrintA("MotorStartedToLeft!");
					  LCD_ClearA();
					  PORTB=0b00010001; // start pump and motor
					  LCD_ClearA();
					  _delay_ms(100);
					  LCD_PrintposA(0, 0, "PUMP STARTED!! ");
					  LCD_PrintposA(1, 0, "Because left need water");
					  LCD_ClearA();
					  while(bit0LevelCheck!=1)
					  {		
						  LCD_ClearA();
						  if(adcValueBIT0>531&&adcValueBIT0<610)
						  {
							  PORTB=0b00001000; // stop pump
							  LCD_ClearA();
							  LCD_PrintposA(0, 0, "PUMP STOPPED!! ");
							  LCD_PrintposA(1, 0, "GOOD LEVEL L");
							  LCD_ClearA();
							  bit0LevelCheck=1;
						  }
						  sprintf(levelBuffer0, "%u", adcValueBIT0+=40);  // Convert integer to string
						  LCD_PrintposA(0, 0, "WATERLEVEL L: ");
						  LCD_PrintposA(1, 0, levelBuffer0);
						  _delay_ms(30);
						  LCD_ClearA();
					  }
			  
			  
					 }
					 else if(adcValueBIT0>610)
					 {
						 LCD_PrintposA(0, 0, "SOILSENSORLEFT ");
						 LCD_PrintposA(1, 0, "GO OVER!!");
						 _delay_ms(100);
			
						 LCD_ClearA();
					 }
					 //ADC1
					 if(adcValueBIT1>531&&adcValueBIT1<610)
					 {
			 
						 LCD_PrintposA(0, 0, "SOILSENSORMID ");
						 LCD_PrintposA(1, 0, "GOOD LEVEL");
						 _delay_ms(100);
			
						 LCD_ClearA();
					 }
					 else if(adcValueBIT1<531)
					 {	
						 uint16_t bit1LevelCheck=0;
						 char levelBuffer1[80];
						 LCD_PrintposA(0, 0, "SOILSENSORMID ");
						 LCD_PrintposA(1, 0, "WATERNEEDED!!");
						 //PORTB=0b00000010;
						 LCD_ClearA();
						 LCD_PrintA("MotorStartedToMID!");
			 
						 LCD_ClearA();
						 PORTB=0b00010010; // start pump and motor
						 _delay_ms(100);
						 LCD_ClearA();
						 LCD_PrintposA(0, 0, "PUMP STARTED!! ");
						 LCD_PrintposA(1, 0, "Because MID need water");
						 LCD_ClearA();
						 while(bit1LevelCheck!=1)
						 {
							 LCD_ClearA();
							 if(adcValueBIT1>531&&adcValueBIT1<610)
							 {
								 PORTB=0b00001000; // stop pump
								 LCD_ClearA();
								 LCD_PrintposA(0, 0, "PUMP STOPPED!! ");
								 LCD_PrintposA(1, 0, "GOOD LEVEL M");
								 LCD_ClearA();
								 bit1LevelCheck=1;
							 }
							 sprintf(levelBuffer1, "%u", adcValueBIT1+=40);  // Convert integer to string
							 LCD_PrintposA(0, 0, "WATERLEVEL M: ");
							 LCD_PrintposA(1, 0, levelBuffer1);
							 _delay_ms(30);
							 LCD_ClearA();
						 }
					 }
					 else if(adcValueBIT1>610)
					 {
						 LCD_PrintposA(0, 0, "SOILSENSORMID ");
						 LCD_PrintposA(1, 0, "GO OVER!!");
						 _delay_ms(100);
			
						 LCD_ClearA();
					 }
		 
					 //ADC2
					 if(adcValueBIT2>531&&adcValueBIT2<610)
					 {
			 
						 LCD_PrintposA(0, 0, "SOILSENSORRIGHT ");
						 LCD_PrintposA(1, 0, "GOOD LEVEL");
						 _delay_ms(100);
			
						 LCD_ClearA();
					 }
					 else if(adcValueBIT2<531)
					 {
						  uint16_t bit2LevelCheck=0;
						 char levelBuffer2[10];
						 LCD_PrintposA(0, 0, "SOILSENSORRIGHT ");
						 LCD_PrintposA(1, 0, "WATERNEEDED!!");
			
			 
						 //PORTB=0b00000100;
						 LCD_ClearA();
						 LCD_PrintA("MotorStartedToRight!");
						 PORTB=0b00010100; // start pump and motor
						 _delay_ms(100);
						 LCD_ClearA();
						 LCD_PrintposA(0, 0, "PUMP STARTED!! ");
						 LCD_PrintposA(1, 0, "Because right need water");
						 LCD_ClearA();
						 while(bit2LevelCheck!=1)
						 {
							 LCD_ClearA();
							 if(adcValueBIT2>531&&adcValueBIT2<610)
							 {
								 PORTB=0b00001000; // stop pump
								 LCD_ClearA();
								 LCD_PrintposA(0, 0, "PUMP STOPPED!! ");
								 LCD_PrintposA(1, 0, "GOOD LEVEL M");
								 LCD_ClearA();
								 bit2LevelCheck=1;
							 }
							 sprintf(levelBuffer2, "%u", adcValueBIT2+=40);  // Convert integer to string
							 LCD_PrintposA(0, 0, "WATERLEVEL R: ");
							 LCD_PrintposA(1, 0, levelBuffer2);
							 _delay_ms(30);
							 LCD_ClearA();
						 }
					 }
					 else if(adcValueBIT2>610)
					 {
						 LCD_PrintposA(0, 0, "SOILSENSORRIGHT ");
						 LCD_PrintposA(1, 0, "GO OVER!!");
						 _delay_ms(100);
			
						 LCD_ClearA();
					 }
		}

		 //PRINTING ADC VALUES
		  sprintf(buffer0, "%u", adcValueBIT0);  // Convert integer to string
		  sprintf(buffer1, "%u", adcValueBIT1);  // Convert integer to string
		  sprintf(buffer2, "%u", adcValueBIT2);  // Convert integer to string

		  // Display ADC value on LCD
		  LCD_PrintposA(0, 0, "L: ");
		  LCD_PrintposA(1, 0, buffer0);
		  _delay_ms(100);
		  LCD_ClearA();
		  LCD_PrintposA(0, 0, "M: ");
		  LCD_PrintposA(1, 0, buffer1);
		  _delay_ms(100);
		  LCD_ClearA();
		  LCD_PrintposA(0, 0, "R: ");
		  LCD_PrintposA(1, 0, buffer2);
		  _delay_ms(100);
		  LCD_ClearA();
		
	}

	
}

//MOTOR SLAVE 
/*
 * ATmega128_Servo_Motor
 *
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
	DDRB |= (1<<PB5);	/* Make OC1A pin as output */ 
	DDRA =0X00;
	DDRC =0xFF;
	
	// bit0 for left
	// bit1 for mid
	// bit2 for right
	TCNT1 = 0;			/* Set timer1 count zero */
	ICR1 =  2000;		/* Set TOP count for timer1 in ICR1 register */

	/* Set Fast PWM, TOP in ICR1, Clear OC1A on compare match, clk/64 */
	TCCR1A = (1<<WGM11)|(1<<COM1A1);
	TCCR1B = (1<<WGM12)|(1<<WGM13)|(1<<CS10)|(1<<CS11);
		/* Set servo shaft at -90° as initial position */
	 OCR1A = 65;
	while(1)
	{	
		 
		if((PINA & (1 << PA0)))
		{	
			PORTC= 0xFF;
			OCR1A = 65;		/* Set servo shaft at -90° position */
			_delay_ms(300);
			OCR1A = 187;
			_delay_ms(300);
			
			
			
		}
		if(PINA&(1<<PA1))
		{	
			PORTC= 0xFF;
			OCR1A = 187;	/* Set servo shaft at 0° position */
			
			
		}
		if (PINA&(1<<PA2))
		{	
			PORTC= 0xFF;
			OCR1A = 300;
			_delay_ms(300);
			OCR1A = 187;
			_delay_ms(300);
			
			
		}
		if (PINA&(1<<PA3))//urgent mod
		{	
			PORTC= 0xFF;
			OCR1A = 300;
			_delay_ms(300);
			OCR1A = 65;
			_delay_ms(300);
			

		}
		/*else
		{
			OCR1A = 300;	//Set servo at +90° position 
			_delay_ms(1500);
			OCR1A = 65;		 ,//Set servo shaft at -90° position 
			_delay_ms(1500);
		}*/
		
	}
}

//UART SLAVE

/*
 * USART.cpp
 *
 * Created: 29.12.2023 15:53:45
 * Author : Omercan kahveci
 */
#define F_CPU 16000000UL
#define BAUD 9600
#define BRC ((F_CPU/16/BAUD)-1)
#include <avr/io.h>
#include <util/delay.h>

void USART_Initialize(void)
{
	UBRR0H = (BRC >> 8);
	UBRR0L = BRC;
	UCSR0B |= (1<<TXEN0) | (1<<UDRIE0);
	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00);
}

void UART_Send_Character(unsigned char c)
{
	while (UCSR0A & (1 << UDRE0) == 0);
	//load data into transmit register
	UDR0 = c;
}

void UART_Send_String(char* s)
{
	while (*s > 0)
	{
		UART_Send_Character(*s++);
		_delay_ms(100);
	}
}


int main(void)
{	
	DDRD=0X00;
	
    /* Replace with your application code */
	USART_Initialize();
    while (1) 
    {
		
		
		if((PIND & (1 << PD0)))
		{
			UART_Send_String("MOTOR WORKING BE CAREFULL!!");
			_delay_ms(1000);
		}
    }
}

