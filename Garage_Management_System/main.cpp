#include "mbed.h"

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

}
