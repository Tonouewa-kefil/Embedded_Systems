#include "mbed.h"
#include "HCSR04.h"
#include "Servo.h"

#define PARKING_SPACE_TOTAL 100

//Light Pin Assignment
#define RED_1_PIN D6
#define RED_2_PIN D7
#define GREEN_1_PIN D8
#define GREEN_2_PIN D9

//Piezo Speaker Pin Assignment
#define PIEZO_BUZZER_1_PIN D3
#define PIEZO_BUZZER_2_PIN D11

//Servo Motor Pin Assignment
#define SERVO_MOTOR_1_PIN D2
#define SERVO_MOTOR_2_PIN D10

//Ultrasonic Sensor Pin Assignment
#define ECHO_1_PIN D0
#define TRIGGER_1_PIN D1
#define ECHO_2_PIN D4
#define TRIGGER_2_PIN D5

//Pressure Sensor Pin Assignement
#define PRESSURE_1_PIN A0
#define PRESSURE_2_PIN A1
#define VCC 5.00
#define R_DIV 3230.0

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
DigitalOut SERVO_1(SERVO_MOTOR_1_PIN);
DigitalOut SERVO_2(SERVO_MOTOR_2_PIN);

//Setting up Piezo Speakers
DigitalOut PIEZO_1(PIEZO_BUZZER_1_PIN);
DigitalOut PIEZO_2(PIEZO_BUZZER_2_PIN);

//Setting up the Pressure Sensors
AnalogIn PRESSURE_1 (PRESSURE_1_PIN);
AnalogIn PRESSURE_2 (PRESSURE_2_PIN);

int main()
{
    //Car counting variables
    int8_t Car_In = 0; 
    int8_t Car_Out = 0;

    //Setting the red lights to 1 
    RED_LIGHT_1 = 1;
    RED_LIGHT_2 = 1;
    
    //Setting the green lights to 0
    GREEN_LIGHT_1 = 0;
    GREEN_LIGHT_2 = 0;
    
    //Ultrasonic Sensor Stuff
        //If the value fetched from here is less than a certain value
        //Than we know there is an object close to the gate else we 
        //don't change anything
    HCSR04 Ultrasonic_1(TRIGGER_1_PIN,ECHO_1_PIN);
    HCSR04 Ultrasonic_2(TRIGGER_2_PIN,ECHO_2_PIN);
    bool isObjectClose_1 = false;
    bool isObjectClose_2 = false;
    uint8_t Distance_1 = 0;
    uint8_t Distance_2 = 0;
    
    //Pressure Sensor Stuff
        //If the value fetched from here is greater than a certain value 
        //than we know it is a car else we don't change anything
    int Pressure_Sensor_1 = 0;
    float fsrR_1;
    float fsrV_1;
    float fsrG_1;
    float force_1;
    int Pressure_Sensor_2 = 0;
    float fsrR_2;
    float fsrV_2;
    float fsrG_2;
    float force_2;
    bool isCar_1 = false; 
    bool isCar_2 = false;
    
    //Servo Motor Stuff
    Servo entranceGate(SERVO_MOTOR_1_PIN);
    Servo exitGate(SERVO_MOTOR_2_PIN);
    
    
    
    while(1)
    {
        //Calculating the distance from ultrasonic sensor to potential object (Entrance)
        Distance_1 = Ultrasonic_1.distance(1);
        if(Distance_1 < 3)
        {
                isObjectClose_1 = true;
        }
        else
        {
                isObjectClose_1 = false;
        }
        
        //Reading the value of the pressure sensor located at the entrance if there is an object close to the gate
        if(isObjectClose_1)
        {
             Pressure_Sensor_1 = PRESSURE_1.read();
            //If the value from the pressure sensor is greater than a certain value then it is a car
            if (Pressure_Sensor_1 != 0)
            {
                 // Use ADC reading to calculate voltage:
                float fsrV_1 = Pressure_Sensor_1 * VCC / 1023.0;
                
                // Use voltage and static resistor value to 
                // calculate FSR resistance:
                float fsrR_1 = R_DIV * (VCC / fsrV_1 - 1.0);
                // Guesstimate force based on slopes in figure 3 of
                // FSR datasheet:
                float fsrG_1 = 1.0 / fsrR_1; // Calculate conductance
                // Break parabolic curve down into two linear slopes:
                if (fsrR_1 <= 600) 
                  force_1 = (fsrG_1 - 0.00075) / 0.00000032639;
                else
                  force_1 =  fsrG_1 / 0.000000642857;  
                
                if(force_1 > 0)
                {
                    isCar_1 = true;    
                }          
                else
                {
                    isCar_1 = false;
                }
            }
        }
        
        //If a car is detected then open the gate and give the driver a green light.
        if(isCar_1)
        {
            //open the gate(Entrance)   
            for(float p=0; p<1.0; p += 0.1) 
            {
                entranceGate = p;
                wait(0.2);
            }
            GREEN_LIGHT_1 = 1;
            delay
            /////////////////////////////////////////////////////something need to happen between this steps
            //close the gate(Entrance)   
            for(float p=1; p>0.1; p -= 0.1) 
            {
                entranceGate = p;
                wait(0.2);
            }
            RED_LIGHT_1 = 1;
        
        
        }
        
        //Calculating the distance from ultrasonic sensor to potential object (Exit)
        Distance_2 = Ultrasonic_2.distance(1);
        if(Distance_2 < 3)
        {
                isObjectClose_2 = true;
        }
        else
        {
                isObjectClose_2 = false;
        }
        
        //Reading the value of the pressure sensor located at the exit if there is an object close to the gate
        if(isObjectClose_2)
        {
            Pressure_Sensor_2 = PRESSURE_2.read();
            //If the value from the pressure sensor is greater than a certain value then it is a car
            if (Pressure_Sensor_2 != 0)
            {
                 // Use ADC reading to calculate voltage:
                float fsrV_2 = Pressure_Sensor_2 * VCC / 1023.0;
                
                // Use voltage and static resistor value to 
                // calculate FSR resistance:
                float fsrR_2 = R_DIV * (VCC / fsrV_2 - 1.0);
                // Guesstimate force based on slopes in figure 3 of
                // FSR datasheet:
                float fsrG_2 = 1.0 / fsrR_2; // Calculate conductance
                // Break parabolic curve down into two linear slopes:
                if (fsrR_2 <= 600) 
                  force_2 = (fsrG_2 - 0.00075) / 0.00000032639;
                else
                  force_2 =  fsrG_2 / 0.000000642857;  
                
                if(force_2 > 0)
                {
                    isCar_2 = true;    
                }          
                else
                {
                    isCar_2 = false;
                }

        }
        
        //If a car is detected then open the gate and give the driver a green light.
        if(isCar_2)
        {
            //open the gate(Exit)   
            for(float p=0; p<1.0; p += 0.1) 
            {
                exitGate = p;
                wait(0.2);
            }
            GREEN_LIGHT_1 = 1;
            
            //Close the gate (Exit)
            for(float p=1; p>0.1; p -= 0.1) 
            {
                exitGate = p;
                wait(0.2);
            }
            RED_LIGHT_1 = 1;
        }
    }
}
}
