/********************************************************************
Authors:	- Clos, Ana María
            - López Lombardo, Ignacio Nahuel
            - Prior, Xoana Vanesa
            - Salinas, Erika Denise

Date:		19/11/2016
Version:	v1.0

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Chip type               : ATmega48
AVR Core Clock frequency: 8.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 128
*********************************************************************/

#include <mega48.h>
#include <delay.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>


// Declare your global variables here
unsigned char tecla;
bit fl=0;
bit enNombre=1;
unsigned char texto[12];
unsigned char i=0;
// Pin change 0-7 interrupt service routine
interrupt [PC_INT0] void pin_change_isr0(void)
{
// Place your code here  
tecla=PINB;
fl=1;    
delay_ms(15);
}

#define DATA_REGISTER_EMPTY (1<<UDRE0)
#define RX_COMPLETE (1<<RXC0)
#define FRAMING_ERROR (1<<FE0)
#define PARITY_ERROR (1<<UPE0)
#define DATA_OVERRUN (1<<DOR0)

// USART Receiver buffer
#define RX_BUFFER_SIZE0 8
char rx_buffer0[RX_BUFFER_SIZE0];

#if RX_BUFFER_SIZE0 <= 256
unsigned char rx_wr_index0=0,rx_rd_index0=0;
#else
unsigned int rx_wr_index0=0,rx_rd_index0=0;
#endif

#if RX_BUFFER_SIZE0 < 256
unsigned char rx_counter0=0;
#else
unsigned int rx_counter0=0;
#endif

// This flag is set on USART Receiver buffer overflow
bit rx_buffer_overflow0;

// USART Receiver interrupt service routine
interrupt [USART_RXC] void usart_rx_isr(void)
{
char status,data;
status=UCSR0A;
data=UDR0;
if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
   {
   rx_buffer0[rx_wr_index0++]=data;
#if RX_BUFFER_SIZE0 == 256
   // Special case for receiver buffer size=256
   if (++rx_counter0 == 0) rx_buffer_overflow0=1;
#else
   if (rx_wr_index0 == RX_BUFFER_SIZE0) rx_wr_index0=0;
   if (++rx_counter0 == RX_BUFFER_SIZE0)
      {
      rx_counter0=0;
      rx_buffer_overflow0=1;
      }
#endif
   }
}

#ifndef _DEBUG_TERMINAL_IO_
// Get a character from the USART Receiver buffer
#define _ALTERNATE_GETCHAR_
#pragma used+
char getchar(void)
{
char data;
while (rx_counter0==0);
data=rx_buffer0[rx_rd_index0++];
#if RX_BUFFER_SIZE0 != 256
if (rx_rd_index0 == RX_BUFFER_SIZE0) rx_rd_index0=0;
#endif
#asm("cli")
--rx_counter0;
#asm("sei")
return data;
}
#pragma used-
#endif

// USART Transmitter buffer
#define TX_BUFFER_SIZE0 16
char tx_buffer0[TX_BUFFER_SIZE0];

#if TX_BUFFER_SIZE0 <= 256
unsigned char tx_wr_index0=0,tx_rd_index0=0;
#else
unsigned int tx_wr_index0=0,tx_rd_index0=0;
#endif

#if TX_BUFFER_SIZE0 < 256
unsigned char tx_counter0=0;
#else
unsigned int tx_counter0=0;
#endif

// USART Transmitter interrupt service routine
interrupt [USART_TXC] void usart_tx_isr(void)
{
if (tx_counter0)
   {
   --tx_counter0;
   UDR0=tx_buffer0[tx_rd_index0++];
#if TX_BUFFER_SIZE0 != 256
   if (tx_rd_index0 == TX_BUFFER_SIZE0) tx_rd_index0=0;
#endif
   }
}

#ifndef _DEBUG_TERMINAL_IO_
// Write a character to the USART Transmitter buffer
#define _ALTERNATE_PUTCHAR_
#pragma used+
void putchar(char c)
{
while (tx_counter0 == TX_BUFFER_SIZE0);
#asm("cli")
if (tx_counter0 || ((UCSR0A & DATA_REGISTER_EMPTY)==0))
   {
   tx_buffer0[tx_wr_index0++]=c;
#if TX_BUFFER_SIZE0 != 256
   if (tx_wr_index0 == TX_BUFFER_SIZE0) tx_wr_index0=0;
#endif
   ++tx_counter0;
   }
else
   UDR0=c;
#asm("sei")
}
#pragma used-
#endif

// Standard Input/Output functions
#include <stdio.h>
unsigned char armarTexto(unsigned char valor)  
{          
unsigned char j,ok;
unsigned char tope=4;
ok=0;
 if(isdigit(valor))       
    {           
        if(enNombre)
        {
            if(i<tope)
            {           
                texto[i]=valor;  
                texto[i+1]=0;
                i++;   
            }        
        }
// Is in password               
        else  
        {
            for(j=0;j<i && texto[j]!='*';j++) ;
            if(i<j+tope+1)
            {
                texto[i]=valor;  
                texto[i+1]=0;
                i++;             
            }   
        } 
    }   
 else if (valor=='#')     
        {         
            if(enNombre)
            {
                texto[0]=0; 
                i=0;
            }
// Is in password
            else
            {    
                if(texto[i-1]=='*')  // The password was not loaded
                {          
                    enNombre=1;
                    texto[0]=0;                    
                    i=0;
                }
// Password loaded
                else    
                {       
                    for(j=0;j<i;j++)
                        {  
                            if(texto[j]=='*') 
                            {              
                                texto[j+1]=0;
                                i=j+1;
                            }
                                
                        }
                }
            }
            
       }     
else if(valor=='*')
        {

        if(enNombre)
        {   
            if(i)
            {   
                texto[i]='*';
                texto[i+1]=0; 
                i++; 
                enNombre=0;
            }
           
        } 
// Is in password             
        else  
        {                   
            if(texto[i-1]!='*' )
            {                 
                texto[i]=0; 
                enNombre=1;    
                ok=1;             
            }
                     
        } 
                
                
            
        }                
return ok;

}       
void main(void)
{
// Declare your local variables here
unsigned char col, valor,ok,recibido='0',j,k,m;
unsigned char usuario[]="1111*11112222*2222";
unsigned char compara[10];

// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=(1<<CLKPCE);
CLKPR=(0<<CLKPCE) | (0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0);
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Input/Output Ports initialization
// Port B initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (1<<DDB3) | (1<<DDB2) | (1<<DDB1) | (1<<DDB0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=0 Bit2=0 Bit1=0 Bit0=0 
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

// Port C initialization
// Function: Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=Out Bit0=Out 
DDRC=(0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (1<<DDC1) | (1<<DDC0);
// State: Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=0 Bit0=0 
PORTC=(0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

// Port D initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// Interrupt on any change on pins PCINT0-7: On
// Interrupt on any change on pins PCINT8-14: Off
// Interrupt on any change on pins PCINT16-23: Off
EICRA=(0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
EIMSK=(0<<INT1) | (0<<INT0);
PCICR=(0<<PCIE2) | (0<<PCIE1) | (1<<PCIE0);
PCMSK0=(1<<PCINT7) | (1<<PCINT6) | (1<<PCINT5) | (1<<PCINT4) | (0<<PCINT3) | (0<<PCINT2) | (0<<PCINT1) | (0<<PCINT0);
PCIFR=(0<<PCIF2) | (0<<PCIF1) | (1<<PCIF0);

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: On
// USART0 Mode: Asynchronous
// USART Baud Rate: 19200
UCSR0A=(0<<RXC0) | (0<<TXC0) | (0<<UDRE0) | (0<<FE0) | (0<<DOR0) | (0<<UPE0) | (0<<U2X0) | (0<<MPCM0);
UCSR0B=(1<<RXCIE0) | (1<<TXCIE0) | (0<<UDRIE0) | (1<<RXEN0) | (1<<TXEN0) | (0<<UCSZ02) | (0<<RXB80) | (0<<TXB80);
UCSR0C=(0<<UMSEL01) | (0<<UMSEL00) | (0<<UPM01) | (0<<UPM00) | (0<<USBS0) | (1<<UCSZ01) | (1<<UCSZ00) | (0<<UCPOL0);
UBRR0H=0x00;
UBRR0L=0x19;

// Global enable interrupts
#asm("sei")
col=0x01;
i=0;    
while (1)
      {  
      PORTB=col & 0x07;  
      if(fl)
      {
        switch(col & 0x07)
        {       
            case 1:  
                switch(tecla>>4)
                {
                    case 1:  
                        valor='1'; 
                        break;
                    case 2:
                        valor='4';
                        break;
                    case 4:
                        valor='7';
                        break;
                    case 8: 
                        valor='*';
                        break;       
                };
                break;
            case 2:  
                switch(tecla>>4)
                {
                    case 1:  
                        valor='2'; 
                        break;
                    case 2:
                        valor='5';
                        break;
                    case 4:
                        valor='8';
                        break;
                    case 8: 
                        valor='0';
                        break;       
                };
                break;      
            case 4:  
                switch(tecla>>4)
                {
                    case 1:  
                        valor='3'; 
                        break;
                    case 2:
                        valor='6';
                        break;
                    case 4:
                        valor='9';
                        break;
                    case 8: 
                        valor='#';
                        break;       
                };
                break;    
        };  
        tecla=0;
        fl=0;
        delay_ms(60); 
        ok=armarTexto(valor);
        if(ok)
        {           
        recibido='0'; 
        printf("Ingresado: %s\r",texto);     
        for(k=0;k<2;k++)
        {           
            m=0;   
             for(j=9*k;j<(k+1)*9;j++,m++)
            {  
            compara[m]=usuario[j];      

            }
            compara[9]=0;    
            if(!strcmp(texto,compara))   
            {          
            recibido='1';              
            }
              
      }       
        
        texto[0]=0;
        i=0;
        if(recibido=='1') 
        {                
            printf("Acceso concedido\r");
            PORTC.0^=1;                  
            delay_ms(1000);  
            PORTC.0^=1; 
        }
           
        else if(recibido=='0')
        {  
            printf("Acceso denegado\r");
            PORTC.1^=1;
            delay_ms(1000);  
            PORTC.1^=1; 
        }
       
        
        } 
     
        valor=0; 
            
        }
        delay_ms(15);
        col<<=1;
        if(col>8)
            col=0x01;
      }
}


