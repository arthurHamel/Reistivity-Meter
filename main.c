//
// Resistivity meter Code -- Testing 
//
#include <pic18f2550.h>
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>



//Define PIN for LCD
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
#define _XTAL_FREQ   20000000UL // This one is just for __delay_ms

#pragma config PLLDIV = 5       // PLL Prescaler Selection bits (Divide by 5 (20 MHz oscillator input))
#pragma config CPUDIV = OSC2_PLL3 // System Clock Postscaler Selection bits ([Primary Oscillator Src: /2][96 MHz PLL Src: /3])
#pragma config USBDIV = 2
#pragma config FOSC = HSPLL_HS  // Oscillator Selection bits (HS oscillator, PLL enabled (HSPLL))

// Everything is Off After this to make sure things do not go wrong

#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = OFF        // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config BORV = 3         // Brown-out Reset Voltage bits (Minimum setting)
#pragma config VREGEN = OFF     // USB Voltage Regulator Enable bit (USB voltage regulator disabled)
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)
#pragma config CCP2MX = OFF     // CCP2 MUX bit (CCP2 input/output is multiplexed with RB3)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = OFF      // MCLR Pin Enable bit (RE3 input pin enabled; MCLR pin disabled)
#pragma config STVREN = OFF     // Stack Full/Underflow Reset Enable bit (Stack full/underflow will not cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)

#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
      // Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)
#pragma config WRTC = OFF       // Configuration Register Write Protection bit
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
      // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)
#pragma config EBTR0 = OFF      // Table Read Protection bit
#pragma config EBTR1 = OFF      // Table Read Protection bit
#pragma config EBTR2 = OFF      // Table Read Protection bit
#pragma config EBTR3 = OFF      // Table Read Protection bit
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit

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
    
    delay_ms(50);

 
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
    while(n--) _delay(5000);
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