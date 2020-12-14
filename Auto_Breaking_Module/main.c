/* 
 * File:   main.c
 * Author: King Kefil Tonouewa
 *
 * Created on March 26, 2020, 6:34 PM
 */

#include <xc.h>
#include <pic18f452.h>
#include <stdint.h>
#include <stdio.h>
#include "config.h"

#define LCD_DATA    LATD //used to display data onto the lcd screen
#define LCD_RS      LATBbits.LB2
#define LCD_RW      LATBbits.LB4
#define LCD_EN      LATBbits.LB3

//***********************************************Global variables*****************************************
static uint8_t pos_track = 0;
static uint8_t line_track = 0;
uint16_t PotVal;//hold result from A/D conversion
uint8_t Car_speed;//hold the car speed(delay)
char Buffer[20];
uint8_t dis;//

//******************************************Function Prototypes********************************************
uint8_t distance(void); //This function is used to find the distance from the obstacle to the ultrasonic sensor
uint8_t Speed_Converter(void);//This function return the the delay amount based on the pot value 
void LCD_clock(void);
void LCD_command(uint8_t);
void LCD_init_8bits(void);
uint8_t LCD_set_pos(uint8_t ,uint8_t);
void LCD_print(uint8_t);
void LCD_print_string( char* );

//***************************************Interrupt Service Routine****************************************
void __interrupt() high_isr (void)
{
    INTCONbits.GIE = 0;//disable further interrupts
     
    //we check if the A/D conversion is finished. 
    if(PIR1bits.ADIF == 1)
    {
        PotVal = ADRES;//PotValue get the the value of the register that contains the conversion
        Car_speed = Speed_Converter();//convert pot value to delay
        PIR1bits.ADIF =0;//re-enable interrupt for future interrupts.
    }
    INTCONbits.GIE = 1;//enable interrupts
}

void Update_Display()
{
    LCD_set_pos(0,0);
    LCD_print_string("Distance:");
    sprintf(Buffer, "%3d",dis );
    LCD_set_pos(9,0);
    LCD_print_string(Buffer);
    LCD_set_pos(13,0);
    LCD_print_string("cm");
}


//*************************************Main code**************************************************
void main(void) {
    WDTCONbits.SWDTEN = 1;
    
    if(!RCONbits.TO)//checking if the watchdog timer have been reset
    TRISBbits.RB1 = 1;  //Set PORT B 1st bit as input (ECHO)
    TRISBbits.RB0 = 0;  //set PORT B 2nd bit as an output (TRANSMITER)
    
    //TIMER 1 SET UPS
    T1CONbits.T1CKPS0 = 0;
    T1CONbits.T1CKPS1 = 0;  //1:1 Prescale value [T1CKPS0=0]
    T1CONbits.TMR1CS  = 0;  //Using Internal Clock (Fosc/4)
    
    //Stepper motor configuration
    TRISC = 0;//for stepper motor
    
    //Analog to digital conversion (Potentiometer)
    TRISAbits.RA0    = 1;   //Set PORTA as input for the potentiometer 
    ADCON1bits.PCFG  = 0x0E;//PORT configuration Control bits. This line set only PORTA 1st bit as an analog pin 
    ADCON1bits.ADFM  = 1;   //Right justified the result obtained
    ADCON1bits.ADCS2 = 1;   
    ADCON0bits.ADCS  = 0;   //Using the internal clock (FOSC/4) [ADCS2 =1]
    ADCON0bits.CHS   = 0;   //Channel 0(AN0)
    ADCON0bits.ADON  = 1;   //power up AD Conversion 
    PIR1bits.ADIF    = 0;   //Conversion in not complete
    PIE1bits.ADIE    = 1;   //Enable the A/D interrupt
    
    //Interrupts configurations
    RCONbits.IPEN   = 1;    //High priority mode
    INTCONbits.GIE  =1;     //Enable interrupts      
    INTCONbits.PEIE =1;     //Enable peripherals interrupts
    
    //Setting the LCD control bits as output
    TRISBbits.RB2 = 0;
    TRISBbits.RB4 = 0; 
    TRISBbits.RB3 = 0; 
    
   
    TRISD = 0x00;//setting portD as an output for the LCD
    
    LCD_RS = 0; //Register select as 0
    LCD_RW = 0; //Read write is 0 because we are only writing for now
    LCD_EN = 0; //Set enable to 0
    
    LATD = 0x00; //put 0 in PORT D (DATA port)
    
    LCD_init_8bits();//Initialize the LCD IN 8 bits mode
    
    __delay_ms(2000);//give time for the LCD to warm up
    
    
    Update_Display();
    while(1)
    {   
        
            //while there is no collision
            while(distance() > 3.5 )
            {
                    ADCON0bits.GO_DONE = 1;//START THE CONVERSION
               
                    LATC = 0b10010000; //first step

                    for(int j = 0; j<=Car_speed; j++)
                    {
                        __delay_ms(1);
                    }

                    LATC = 0b11000000; //second step

                    for(int j = 0; j<=Car_speed; j++)
                    {
                        __delay_ms(1);
                    }

                    LATC = 0b01100000; //third step

                    for(int j = 0; j<=Car_speed; j++)
                    {
                        __delay_ms(1);
                    }

                    LATC = 0b00110000; //fourth step

                    for(int j = 0; j<=Car_speed; j++)
                    {
                        __delay_ms(1);
                    }
                    CLRWDT();//clearing watch dog timer since everything is still working properly
                    Update_Display();             
            }
            
           while(1);//give time to the watch dog timer to stack up
           
             
    };
    
}

uint8_t distance()
{
    
    TMR1 = 0;       //Clear the timer
    
    //Trigger the Ultrasonic sensor
     LATBbits.LATB0 = 1;
     __delay_us(13);
     LATBbits.LATB0 = 0;
     
     while(PORTBbits.RB1 != 1);//wait for echo to go high
     GIE = 0;       //Disable interrupts the reduce the amount of error in the code
     TMR1ON = 1;    //Turn the timer on
     while(PORTBbits.RB1 == 1);//wait for echo to go back low
     TMR1ON = 0;    //Turn the timer off
     
     dis = (TMR1/30.5); 
     GIE = 1;//re-enable the interrupts
     return dis;
}

uint8_t Speed_Converter()
{
    if(PotVal <=100)
    {
        Car_speed = 6;
    }
    else if(PotVal <=205)
    {
        Car_speed = 15;
    }
    else if(PotVal <=310)
    {
        Car_speed = 25;
    }
    else if(PotVal <=410)
    {
        Car_speed = 40;
    }
    else if(PotVal <=510)
    {
        Car_speed = 50;
    }
    else if(PotVal <=610)
    {
        Car_speed = 60;
    }
    else if(PotVal <=700)
    {
        Car_speed = 70;
    }
    else if(PotVal <=800)
    {
        Car_speed = 80;
    }
    else if(PotVal <=900)
    {
        Car_speed = 90;
    }
    else if(PotVal <=1000)
    {
        Car_speed = 100;
    }
    else
    {
        Car_speed =130;
    }
    
    return Car_speed;
}


void LCD_clock(void){
    LCD_EN = 1;
    __delay_ms(1);
    LCD_EN = 0;
    __delay_ms(1);
}

void LCD_command(uint8_t command){
    LCD_RS = 0;
    LCD_RW = 0;
    
    LCD_DATA = command;
    LCD_clock();
    __delay_ms(10); 
}

void LCD_init_8bits(void){
    LCD_RS = 0;
    LCD_RW = 0;
    
    __delay_ms(50);
    //Making the LCD an 8bits interface
    LCD_command(0x30);
    LCD_command(0x30);
    LCD_command(0x30);
    
    //set function
    //8bit mode
    //1 line display
    //font to 5x8
    LCD_command(0x34);
    
    //display off
    //cursor off
    //cursor no blink
    LCD_command(0x08);
    
    //clear the display and set to home
    LCD_command(0x01);
    
    //increment
    //no shift
    LCD_command(0x06);
    
    //display on
    //cursor off
    //cursor not blinking
    LCD_command(0x0C);   
}

uint8_t LCD_set_pos(uint8_t x,uint8_t y){
    
    if(y == 1){     
        x += 0x40;
    }
    
    LCD_command(0x80 | x);
    return x;
}

void LCD_print(uint8_t x){
    
    if(pos_track > 15){
        if(line_track > 0){
            LCD_set_pos(0,0);
            line_track = 0;     
        }else{
            line_track++;
            LCD_set_pos(0,1);
        }
        
        pos_track = 0;
    }
    
    LCD_RS = 1;
    LCD_RW = 0;
    
    LCD_DATA = x;
    
    LCD_clock();
    
    pos_track++;
    
}


void LCD_print_string(char *str){
    uint16_t i = 0;
    
    do{
        LCD_print(str[i]);
        i++;
        
    }while(str[i] != '\0');   
}

