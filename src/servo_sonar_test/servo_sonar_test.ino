
//
// Copyright oddWires 2013
// May be freely used by any purchaser of an oddWires Arduino Kit for their sole purpose
//
// L293D Pin Connections					
//					
//	Pin	Function	Connection              Comment
//					
//	1	ENABLE1	        Arduino Pin 3 		Must be on a PWM supported pin (not 9 or 10 because of servo library conflict)
//	2	INPUT1	        Arduino Pin 9		
//	3	OUTPUT1	        Motor 1		
//	4	GND	        Arduino GND		
//	5	GND	        Arduino GND		
//	6	OUTPUT2	        Motor 1		
//	7	INPUT2	        Arduino Pin 8		
//	8	Vs	        Motor drive supply	Do not connect to Arduino +5V
//	9	ENABLE2	        Arduino Pin 11		Must be on a PWM supported pin (not 9 or 10 because of servo library conflict)
//	10	INPUT3	        Arduino Pin 7		
//	11	OUTPUT3	        Motor 2		
//	12	GND	        Arduino GND		
//	13	GND	        Arduino GND		
//	14	OUTPUT4	        Motor 2		
//	15	INPUT4	        Arduino Pin 6		
//	16	Vss	        Arduino +5V							
//					
// Servo Connections					
//	Brown	GND			
//	Red	Motor drive supply (6V battery box in this case - do not use Arduino +5V)			
//	Orange	Arduino Pin 5			
//					
// HC-SR04 Ultrasonic Module					
//					
//	Vcc	Arduino +5V			
//	Trig	Arduino Pin 4			
//	Echo	Arduino Pin 2			
//	GND	Arduino GND			


// Ultrasonic sensor

#include <NewPing.h>

#define TRIG_PIN  A4  
#define ECHO_PIN  A5  

#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pin and maximum distance.

float dangerThreshold = 25.0; // 25 cm - depends on the floor surface and speed setting
 
// Servo

#include <Servo.h> 
//#include <AFmotor.h>
#include <AFMotor.h>
// start copy
AF_DCMotor motor1(1, MOTOR12_1KHZ);
AF_DCMotor motor2(2, MOTOR12_1KHZ);

// end copy

#define LEFT 0
#define CENTER 90
#define RIGHT 180

Servo ultrasonicServo;  // create servo object to control a servo 
 
int currentPos = 0;    // variable to store the servo position 

// Motor

#define ENABLE1 3
#define INPUT1 9
#define INPUT2 8

#define ENABLE2 11
#define INPUT3 7
#define INPUT4 6

// Truth table for motor control
// ENABLE1, ENABLE2, INPUT1, INPUT2, INPUT3, INPUT4

// You may need to reverse the polarity of your motor(s) 
// if you find you get REVERSE instead of FORWARD for either motor

#define LEFT_FORWARD_RIGHT_FORWARD {HIGH, HIGH, LOW, HIGH, HIGH, LOW}
#define LEFT_OFF_RIGHT_FORWARD {LOW, HIGH, LOW, LOW, HIGH, LOW}
#define LEFT_FORWARD_RIGHT_OFF {HIGH, LOW, LOW, HIGH, LOW, LOW}
#define LEFT_REVERSE_RIGHT_FORWARD {HIGH, HIGH, HIGH, LOW, HIGH, LOW}
#define LEFT_FORWARD_RIGHT_REVERSE {HIGH, HIGH, LOW, HIGH, LOW, HIGH}
#define LEFT_REVERSE_RIGHT_REVERSE {HIGH, HIGH, HIGH, LOW, LOW, HIGH}
#define LEFT_OFF_RIGHT_OFF {HIGH, HIGH, LOW, LOW, LOW, LOW}
#define LEFT_FREEWHEEL_RIGHT_FREEWHEEL {LOW, LOW, LOW, LOW, LOW, LOW}

// Driving definitions

//#define FORWARD LEFT_FORWARD_RIGHT_FORWARD
#define REVERSE LEFT_REVERSE_RIGHT_REVERSE
#define LEFT_TURN LEFT_OFF_RIGHT_FORWARD
#define RIGHT_TURN LEFT_FORWARD_RIGHT_OFF
#define ROTATE_LEFT LEFT_REVERSE_RIGHT_FORWARD
#define ROTATE_RIGHT LEFT_FORWARD_RIGHT_REVERSE
#define BRAKE LEFT_OFF_RIGHT_OFF
#define FREEWHEEL LEFT_FREEWHEEL_RIGHT_FREEWHEEL

// PWM motor control settings applied to ENABLE1, ENABLE2

#define SLOW 128;
#define MEDIUM 192;
#define FAST 255;

// run motor at this speed - set to a number between 128 and 255
// less than 128 any the motors may not have enough torque to move
// (of course you could enhance the speed control to accelerate/decelerate when more/less clear space available as indicated by ping)
int throttle = MEDIUM;

// setup
void setup() {
    
    // setup motor    
    Serial.begin(9600);
    //pinMode(ENABLE1, OUTPUT);
    //pinMode(ENABLE2, OUTPUT);
    //pinMode(INPUT1, OUTPUT);
    //pinMode(INPUT2, OUTPUT);
    //pinMode(INPUT3, OUTPUT);
    //pinMode(INPUT4, OUTPUT);
    
    // setup servo
    ultrasonicServo.attach(9);  // attaches the servo on pin 5 to the servo object
   
    // move servo to center 
    servo_position(CENTER);  
}

// main loop - note that the loop itself does not block (no waiting in the loop)
// functions that need to block (complete before continuing the main loop) need to manage their own delays
//
// the algorithm used is designed to be simple to understand - you can go on to make this much more sophisticated
// guiding the steering to veer away before getting into danger for example
// and accelerating from a stop and decelerating as you get closer to objects
// and of course you could use more ultrasonic detectors, IR sensors etc. to really fine tune your robot's operation

void loop() {
  
        float distanceForward = ping();
        delay(500);
        Serial.print("Center: ");
        Serial.print(distanceForward);
        Serial.println("cm");
        // look left
        servo_position(LEFT);  
        delay(500);
        float distanceLeft = ping();
        Serial.print("Left: ");
        Serial.print(distanceLeft);
        Serial.println("cm");
        
        // look right
        delay(500);
        servo_position(RIGHT);  
        float distanceRight = ping();
        Serial.print("Right: ");
        Serial.print(distanceRight);
        Serial.println("cm");
        delay(500);
        
        
        drive_forward();
        // re-center the ultrasonic
        servo_position(CENTER);
        
           
    }
    

// You may need to vary the delays in the drive functions to enable the function to complete

void freewheel(){
    const int driveControl[] = FREEWHEEL;
    drive(driveControl);
    delay(25);
}

void brake(){
    const int driveControl[] = BRAKE;
    drive(driveControl);
    delay(25); // braking time
}

//void drive_forward(){
//   const int driveControl[] = FORWARD;
//    drive(driveControl);
//}

void drive_forward(){
    motor1.run(FORWARD);
    motor2.run(FORWARD);
}

void drive_backward(){
    const int driveControl[] = REVERSE;
    drive(driveControl);
}

void turn_left(){
    const int driveControl[] = LEFT_TURN;
    drive(driveControl);
    delay(600); // adjust for 90 degrees
}

void turn_right(){
    const int driveControl[] = RIGHT_TURN;
    drive(driveControl);
    delay(600); // adjust for 90 degrees
}

void rotate_left(){
    const int driveControl[] = ROTATE_LEFT;
    drive(driveControl);
    delay(300); // rotate is faster than turn because two motors are driving
}

void rotate_right(){
    const int driveControl[] = ROTATE_RIGHT;
    drive(driveControl);
    delay(300); // rotate is faster than turn because two motors are driving
}

void u_turn(){
    const int driveControl[] = ROTATE_RIGHT;
    drive(driveControl);
    delay(600); // twice as long as rotate right to end up 180 degrees around
}

// motor control via the L293D

void drive(const int settings[6]){
  
    if (settings[0] == HIGH)
        analogWrite(ENABLE1, throttle);
    else
        digitalWrite(ENABLE1, LOW);
    
    if (settings[1] == HIGH)
        analogWrite(ENABLE2, throttle);
    else
        digitalWrite(ENABLE2, LOW);
    
    digitalWrite(INPUT1, settings[2]);
    digitalWrite(INPUT2, settings[3]);
    digitalWrite(INPUT3, settings[4]);
    digitalWrite(INPUT4, settings[5]);
}

// servo control

void servo_position(int newPos){
    if (newPos > currentPos){
        for(int pos=currentPos; pos < newPos; pos += 1)  // goes from 0 degrees to 180 degrees 
        {                                                // in steps of 1 degree 
            ultrasonicServo.write(pos);                  // tell servo to go to position in variable 'pos' 
            delay(15);                                   // waits 15ms for the servo to reach the position 
        }
        currentPos = newPos;  
    }
    else if (newPos < currentPos){
        for(int pos=currentPos; pos > newPos; pos -= 1)  // goes from 0 degrees to 180 degrees 
        {                                                // in steps of 1 degree 
            ultrasonicServo.write(pos);                  // tell servo to go to position in variable 'pos' 
            delay(15);                                   // waits 15ms for the servo to reach the position 
        }
        currentPos = newPos;  
    }  
}

// ultrasonic ping

float ping(){
    delay(50);                        // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
    unsigned int uS = sonar.ping();   // Send ping, get ping time in microseconds (uS).
    if (uS == 0)                      // out of range (0 = outside set distance range, no ping echo)
        return MAX_DISTANCE;
    else                              // in range
        return uS / US_ROUNDTRIP_CM;  // Convert ping time to distance 
}








