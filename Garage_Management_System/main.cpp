#include "mbed.h"
#include "hcsr04.h"
#include "Servo.h"
#include <cstdio>


#define PARKING_SPACE_TOTAL 100

//Light Pin Assignment
#define RED_1_PIN LED1
#define RED_2_PIN D7
#define GREEN_1_PIN LED2
#define GREEN_2_PIN D9

//Piezo Speaker Pin Assignment
#define PIEZO_BUZZER_1_PIN D3
#define PIEZO_BUZZER_2_PIN D11

//Servo Motor Pin Assignment
#define SERVO_MOTOR_1_PIN A4
//#define SERVO_MOTOR_2_PIN D10

//Ultrasonic Sensor Pin Assignment
#define ECHO_1_PIN D0
#define TRIGGER_1_PIN D1
#define ECHO_2_PIN D4
#define TRIGGER_2_PIN D5

//Pressure Sensor Pin Assignement
#define PRESSURE_1_PIN A0
#define PRESSURE_2_PIN A1
#define VCC 5.00
#define R_DIV 9910.0

//LCD Pin Assignment

//Setting up the lights as output
DigitalOut RED_LIGHT_1(RED_1_PIN);
DigitalOut RED_LIGHT_2(RED_2_PIN);
DigitalOut GREEN_LIGHT_1(GREEN_1_PIN);
DigitalOut GREEN_LIGHT_2(GREEN_2_PIN);

//Setting up the Ultrasonic Sensors
DigitalOut ECHO_1(ECHO_1_PIN);
DigitalIn TRIGGER_1(TRIGGER_1_PIN);
DigitalOut ECHO_2(ECHO_2_PIN);
DigitalIn TRIGGER_2(TRIGGER_2_PIN);

//Setting up the Servo Motors

//DigitalOut SERVO_2(SERVO_MOTOR_2_PIN);

//Setting up Piezo Speakers
DigitalOut PIEZO_1(PIEZO_BUZZER_1_PIN);
DigitalOut PIEZO_2(PIEZO_BUZZER_2_PIN);

//Setting up the Pressure Sensors
AnalogIn PRESSURE_1 (PRESSURE_1_PIN);
AnalogIn PRESSURE_2 (PRESSURE_2_PIN);

int main()
{
    
    //Car counting variables
    int Car_In = 0;
    int Car_Out = 0;

    //Setting the red lights to 1
    RED_LIGHT_1 = 0;
    //RED_LIGHT_2 = 1;

    //Setting the green lights to 0
    //GREEN_LIGHT_1 = 0;
    //GREEN_LIGHT_2 = 0;

    //Ultrasonic Sensor Stuff
    //If the value fetched from here is less than a certain value
    //Than we know there is an object close to the gate else we
    //don't change anything
    HCSR04 Ultrasonic_1(TRIGGER_1_PIN,ECHO_1_PIN);
    HCSR04 Ultrasonic_2(TRIGGER_2_PIN,ECHO_2_PIN);
   
    bool isObjectClose_2 = false;
    long Distance_1 = 0;
    long Distance_2 = 0;
    bool isObjectClose_1 = false;
    //Pressure Sensor Stuff
    //If the value fetched from here is greater than a certain value
    //than we know it is a car else we don't change anything
    float Pressure_Sensor_1 = 0;
    float Pressure_Sensor_2 = 0;
    bool isCar_1 = false;
    bool isCar_2 = false;
    //Servo Motor Stuff
    Servo entranceGate(SERVO_MOTOR_1_PIN);
    //Servo exitGate(SERVO_MOTOR_2_PIN);
    int flag = 1;

    while(1) 
    {
        //Calculating the distance from ultrasonic sensor to potential object (Entrance)
        Distance_1 = Ultrasonic_1.distance();
        if(Distance_1 < 8)
        {
                isObjectClose_1 = true;
        }        
        else
        {
                isObjectClose_1 = false;
        }
        

        //Setting the Force sensor stuff (Entrance)
        //isObjectClose_1 = true;
        Pressure_Sensor_1 = PRESSURE_1.read();
        Pressure_Sensor_1 = VCC * Pressure_Sensor_1;
        if(isObjectClose_1)
        {
            //This will need to be calibrated
            if (Pressure_Sensor_1 < 2.5 ) 
            {
                isCar_1 = false;
            }
            else 
            {   
                
                 //open the gate(Entrance) 
                if (flag == 1) 
                {
                    flag = 0;
                    for(float p=0; p<2; p += 0.1) 
                    {
                        entranceGate = p;
                        thread_sleep_for(50);
                    }
                    //isCar_1 = false;
                    RED_LIGHT_1 = 1;
                    GREEN_LIGHT_1 = 0;
                }
                if ( (PRESSURE_1.read()*VCC) < 2.5 ) 
                {
                    thread_sleep_for(10000);
                    for(float p=2; p>0; p -= 0.1) 
                    {
                        entranceGate = p;
                        thread_sleep_for(50);
                    }  
                    GREEN_LIGHT_1 = 1; 
                    RED_LIGHT_1 = 0;
                    flag =1;
                }
                else
                {
                    isCar_1 = true;
                }
            }     
        }
    }
}
