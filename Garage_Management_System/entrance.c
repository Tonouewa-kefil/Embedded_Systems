#include "mbed.h"
#include "hcsr04.h"
#include "Servo.h"
#include "TextLCD.h"
#include <cstdio>


#define PARKING_SPACE_TOTAL 8

//Light Pin Assignment
#define RED_1_PIN D3
#define GREEN_1_PIN D5


//Servo Motor Pin Assignment
#define SERVO_MOTOR_1_PIN A5

//Ultrasonic Sensor Pin Assignment
#define ECHO_1_PIN D4
#define TRIGGER_1_PIN D2


//Pressure Sensor Pin Assignement
#define PRESSURE_1_PIN A0
#define VCC 5.00


//LCD Pin Assignment

//Setting up the lights as output
DigitalOut RED_LIGHT_1(RED_1_PIN);
DigitalOut GREEN_LIGHT_1(GREEN_1_PIN);

//Setting up the Ultrasonic Sensors
DigitalOut ECHO_1(ECHO_1_PIN);
DigitalIn TRIGGER_1(TRIGGER_1_PIN);

//Setting up the Pressure Sensors
AnalogIn PRESSURE_1 (PRESSURE_1_PIN);

TextLCD lcd(PTC2, PTC3, PTC4, PTD0, PTD2,PTD3, TextLCD::LCD16x2); // rs, e, d4-d7
Serial entrance(PTC17,PTC16,115200);

int  Space_available = PARKING_SPACE_TOTAL;

int main()
{
    lcd.cls();
    wait(0.02);
    lcd.printf("AVAILABLE SPACE");
    lcd.locate(7,1);
    lcd.printf("%d",Space_available);
    //Car counting variables
    int Car_In = 1;

    //Setting the red lights to 1
    RED_LIGHT_1 = 1;

    //Ultrasonic Sensor Stuff
    //If the value fetched from here is less than a certain value
    //Than we know there is an object close to the gate else we
    //don't change anything
    HCSR04 Ultrasonic_1(TRIGGER_1_PIN,ECHO_1_PIN);
    long Distance_1 = 0;
    bool isObjectClose_1 = false;
    
    //Pressure Sensor Stuff
    //If the value fetched from here is greater than a certain value
    //than we know it is a car else we don't change anything
    float Pressure_Sensor_1 = 0;
    bool isCar_1 = false;

    //Servo Motor Stuff
    Servo entranceGate(SERVO_MOTOR_1_PIN);

    int flag = 1;
    while(1) 
    {
        
        //Calculating the distance from ultrasonic sensor to potential object (Entrance)
        if(flag == 1)
        {
            Distance_1 = Ultrasonic_1.distance();
            Pressure_Sensor_1 = PRESSURE_1.read();
            Pressure_Sensor_1 = VCC * Pressure_Sensor_1;
            if(Distance_1 < 8 && Pressure_Sensor_1 > 2.5 )
            {
                //isObjectClose_1 = true;
                
                //This will need to be calibrated
                    //open the gate(Entrance) 
                    if (flag == 1) 
                    {
                        flag = 0;
                        for(float p=0; p<2; p += 0.1) 
                        {
                            entranceGate = p;
                            thread_sleep_for(50);
                        }
                        //is Car_1 = false;
                        RED_LIGHT_1 = 0;
                        GREEN_LIGHT_1 = 1;
                    }
                
            }
        }
        else        
        {
        //Setting the Force sensor stuff (Entrance)
        //isObjectClose_1 = true;
            if((PRESSURE_1.read()*VCC) < 2.5 ) 
                {
                    thread_sleep_for(10000);
                    for(float p=2; p>0; p -= 0.1) 
                    {
                        entranceGate = p;
                        thread_sleep_for(50);  
                    }  
                    GREEN_LIGHT_1 = 0; 
                    RED_LIGHT_1 = 1;
                    flag =1;
                    flag =1;
                    Space_available = Space_available - 1;
                }
                if(entrance.readable()) 
                {
                    int val = entrance.getc();
                    Space_available = Space_available + val;
                }
                    lcd.locate(7,1);
                    lcd.printf("%d",Space_available);
            }     
        }
    }
