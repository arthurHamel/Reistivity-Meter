//
// lcd.c - LCD display examplefor dsPIC18F4620
// Written by Ted Burke - Last updated 8-10-2013
//

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>




#define PIN_RS LATBbits.LATB4
#define PIN_E LATBbits.LATB5
 
 
// Function prototypes for transmitting to LCD
void delay_ms(unsigned int n);
void send_nibble(unsigned char nibble);
void send_command_byte(unsigned char byte);
void send_data_byte(unsigned char byte);
void init_LCD();

// Select clock oscillator (default frequency Fosc=1MHz -> Tcy = 4us).
// Disable reset pin, watchdog timer, low voltage programming and
// brown-out reset.
#pragma config FOSC=INTOSC_EC,MCLRE=OFF,WDT=OFF,LVP=OFF


 
// Select which pins the program will use for the LCD screen
// control signals, RS, RW and E.
// NB I had to change these from the names used in my previous
// dsPIC30F4011 example to avoid a clash with the equivalent
// definitions in the XC8 compiler's peripheral library.
 




//Function to Initialise the ADC Module
void ADCInit()
{
   //We use default value for +/- Vref

   //VCFG0=0,VCFG1=0
   //That means +Vref = Vdd (5v) and -Vref=GEN

   //Port Configuration
   //We also use default value here too
   //All ANx channels are Analog

   
     /* ADCON2

      *ADC Result Right Justified.
      *Acquisition Time = 2TAD
      *Conversion Clock = 32 Tosc
  */

   ADCON2=0b10001010;
}

//Function to Read given ADC channel (0-13)
unsigned int ADCRead(unsigned char ch)
{
   if(ch>13) return 0;  //Invalid Channel

   ADCON0=0x00;

   ADCON0=(ch<<2);   //Select ADC Channel

   ADON=1;  //switch on the adc module

   GODONE=1;//Start conversion

   while(GODONE); //wait for the conversion to finish

   ADON=0;  //switch off adc

   return ADRES;
}
void main()
{
   //Let the LCD Module start up
     init_LCD();
     delay_ms(1000);
 // Define two 8 character strings
    char line1[] = "    Voltage     ";
    char line2[] = "   Loading...   ";
      
    // Write the two strings to lines 1 and 2
    int n;
    send_command_byte(0x02); // Go to start of line 1
    for (n=0 ; n<16 ; ++n) send_data_byte(line1[n]);
    send_command_byte(0xC0); // Go to start of line 2
    for (n=0 ; n<16 ; ++n) send_data_byte(line2[n]);
      
    delay_ms(1000);
   //Initialize the ADC Module

   ADCInit();

 

   while(1)
   {
    unsigned int val; //ADC Value

     val=ADCRead(0);//Read Channel 0
     float converted= val*5;
     converted=converted/1024;
     char values[16];
     sprintf(values, "   %.3f Volts      ", converted);
    send_command_byte(0xC0); // Go to start of line 2
    for (n=0 ; n<16 ; ++n) send_data_byte(values[n]);
    

 
   }




}
void init_LCD()
{
     TRISB = 0b00000000; // Set RD0-7 as digital outputs
     
    // Let's just write to the LCD and never read!
    // We'll wait 2ms after every command since we can't
    // check the busy flag.
    PIN_RS = 0;
    PIN_E = 1;
      
    // Some LCD modules require a delay after power-up
    // before you can begin communicating with them, so
    // I'm putting a 1 second delay here just in case.
    delay_ms(1000);
     
    // Initialisation
    delay_ms(16); // must be more than 15ms
    send_nibble(0b0011);
    delay_ms(5); // must be more than 4.1ms
    send_nibble(0b0011);
    delay_ms(1); // must be more than 100us
    send_nibble(0b0011);
    delay_ms(5); // must be more than 4.1ms
    send_nibble(0b0010); // select 4-bit mode
     
    // Display settings
    send_command_byte(0b00101000); // N=0 : 2 lines (half lines!), F=0 : 5x7 font
    send_command_byte(0b00001000); // Display: display off, cursor off, blink off
    send_command_byte(0b00000001); // Clear display
    send_command_byte(0b00000110); // Set entry mode: ID=1, S=0
    send_command_byte(0b00001111); // Display: display on, cursor on, blink on
      
}
  

  
// Delay by specified number of milliseconds
void delay_ms(unsigned int n)
{
    // At Fosc=1Mhz, Tcy is 4us. That's the time
    // taken to perform one machine code instruction.
    // Therefore a delay of 250 x Tcy = 1ms.
    while(n--) _delay(250);
}
  
void send_nibble(unsigned char nibble)
{
    // Set RB0-3 without affecting RD4-7
    LATB = (LATB & 0xF0) + nibble;
    delay_ms(1);
    // Note: data is latched on falling edge of pin E
    PIN_E = 0;
    delay_ms(1);
    PIN_E = 1;
    delay_ms(2); // Enough time even for slowest command
}
  
// Send a command byte (i.e. with pin RS low)
void send_command_byte(unsigned char byte)
{
    PIN_RS = 0;
    send_nibble(byte >> 4);
    send_nibble(byte & 0xF);
}
  
// Send a data byte (i.e. with pin RS high)
void send_data_byte(unsigned char byte)
{
    PIN_RS = 1;
    send_nibble(byte >> 4);
    send_nibble(byte & 0xF);
}