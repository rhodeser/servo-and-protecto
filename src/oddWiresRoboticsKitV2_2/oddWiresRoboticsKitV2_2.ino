
//**************************************************************************************************************
//*
//* Copyright oddWires 2013 - may be used under Creative Commons Share and Share Alike License
//*
//**************************************************************************************************************

//**************************************************************************************************************
//*
//* Ultrasonic sensor definitions
//* We are using A4 & A5 on the motor shield for the sensor
//* The sensors are not accurate past 200cm or so
//* The danger threshold needs to be set appropriately for your floor - make it larger on smooth surfaces
//* and vice versa
//*
//* HC-SR04 Ultrasonic Module					
//*					
//*	Vcc	+5V			
//*	Trig	A4			
//*	Echo	A5			
//*	GND	//*
//*					
//**************************************************************************************************************

#include <NewPing.h>

#define TRIG_PIN  A4  
#define ECHO_PIN  A5  

#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). 

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pin and maximum distance.

#define COLLISION_DISTANCE 60 // in cm - depends on the floor surface and speed setting
#define AVOIDANCE_DISTANCE 80 //  
//**************************************************************************************************************
//*
//* Servo definitions 
//*
//* Servo Connections to motor shield					
//*
//*	Brown	SER1 -			
//*	Red	SER1 +			
//*	Orange	SER1 S	
//*
//* Servo 1 on the motor shield is Arduino Pin 10
//* The servo start and stop position may have to be varied from the ones specified here to ensure full
//* movement from 0 to 180		
//*
//**************************************************************************************************************

#include <Servo.h> 
//#include <ServoTimer2.h>

#define LEFT 180
#define CENTER 90
#define RIGHT 0

Servo ultrasonicServo;  // create servo object to control a servo 
 
int currentPos = 0;    // variable to store the servo position 

#define servoPin 10
#define SERVO_MIN_MICROS 650   // initial position (0 degrees) in microseconds
#define SERVO_MAX_MICROS 1075  // full range position (180 degrees) in microseconds 

//**************************************************************************************************************
//*
//* Motor and battery box connections
//*
//* Motor 1
//* LEFT motor (looking from rear to the robot
//*
//* This motor will need the inline connector and some wire extensions to reach the terminals
//*
//* Black wire to left M1 terminal
//* Red wire to right motor terminal
//*
//* Motor 2
//* RIGHT motor (looking from rear to the robot
//*
//* This motor will need the inline connector and some wire extensions to reach the terminals
//*
//* Black wire to RIGHT M1 terminal
//* Red wire to LEFT motor terminal
//*
//* 6V 4 x AA Battery Box 
//*
//* Connect to Blue ternminal marked EXT_PWR
//* Be very careful - do NOT connect the RED wire to GND or you could destroy the motor board
//* +6 V (Red wire)   M+
//* GND (Black wire)  GND
//* Motor definitions 
//*
//* Throttle is a number between 128 and 255 -
//* less than 128 any the motors may not have enough torque to move
//* motor1 is connected to motor 1 pins
//* motor2 is connected to motor 2 pins
//*
//* PWM Frequency is set to 1kHz (happens to be the default too)
//*
//**************************************************************************************************************

#include <AFMotor.h>

AF_DCMotor motor1(1, MOTOR12_1KHZ);
AF_DCMotor motor2(2, MOTOR12_1KHZ);

#define STOP 0
#define SLOW 150
#define MEDIUM 230
#define MAXSPEED 255

int throttle = 0;
// used to balance power across wheels


//**************************************************************************************************************
//*
//* structure to hold ultrasonic readings
//*
//**************************************************************************************************************

struct angleVector{
  int angle;
  int distance;
};

#define READINGS 5
struct angleVector angleVectors[READINGS];

//**************************************************************************************************************
//*
//* Setup
//*
//**************************************************************************************************************

void setup() {
  
    // start the serial interface
    Serial.begin(9600);
    int myspeed = 250;
    // setup servo
    ultrasonicServo.attach(servoPin, SERVO_MIN_MICROS, SERVO_MAX_MICROS);  // attaches the servo on pin 10 to the servo object
    //ultrasonicServo.attach(servoPin);   
    // move servo to center 
    servo_position(CENTER);
  
    // setup motors
    throttle = MEDIUM;
    setSpeed(throttle);
    freewheel();
}

//**************************************************************************************************************
//*
//* Main loop
//*
//* Note that the loop itself does not block (no waiting in the loop)
//*
//* The algorithm used is designed to be simple to understand - you can go on to make this much more sophisticated
//* guiding the steering to veer away before getting into danger for example
//* and accelerating from a stop and decelerating as you get closer to objects
//* and of course you could use more ultrasonic detectors, IR sensors etc. to really fine tune your robot's operation
//* You may need to vary the delays below to give the functions time to complete
//*
//**************************************************************************************************************

#define CLEAR          1
#define EMERGENCY_STOP 2
#define TURN_LEFT      3
#define TURN_RIGHT     4
#define VEER_LEFT      5
#define VEER_RIGHT     6

void loop() {
  /*
 int sonar = ping();
 Serial.print(sonar);
 delay(100);
*/
    int route = analyzeRoute();  
    switch(route){
          
      case CLEAR:
        Serial.println("CLEAR");
        drive_forward();
        break;
      
      case EMERGENCY_STOP:
        Serial.println("STOP");
        freewheel();
        delay(20);
        drive_backward();
        delay(100);
        u_turn();
        break;
      
      case TURN_LEFT:
        Serial.println("LEFT");
        rotate_left();
        break;
      
      case TURN_RIGHT:
        Serial.println("RIGHT");
        rotate_right();
        break;
      
      default:
        break;
    }

}


//**************************************************************************************************************
//*
//* Ultrasonic functions
//*
//**************************************************************************************************************

int analyzeRoute(){
    if (scanClear()) //if path is clear
    {
          Serial.println("Clear");
        return CLEAR;
    }
    else // if path is blocked
    {
        Serial.println("Blocked");
        freewheel();  // see if we can implment faster stop
               
        // look left
        lookLeft();
        int distanceLeft = getAverageDistance();
                
        // look right
        lookRight();
        int distanceRight = getAverageDistance();
        
        // re-center the ultrasonic
        servo_position(CENTER);
        
        // go the least obstructed way
               
        if (distanceLeft > distanceRight && distanceLeft > COLLISION_DISTANCE)       //if left is less obstructed 
        {
            return TURN_LEFT;
        }
        else if (distanceRight > distanceLeft && distanceRight > COLLISION_DISTANCE) //if right is less obstructed 
        {
           return TURN_RIGHT;
        }
        else // equally blocked or less than collision distance left or right
        {
           return EMERGENCY_STOP;
        }       
    }
}

bool scanClear(){

  //  Serial.println("in scanClear"); 

  bool clear = true;
  bool lowValue = false;

  lookForward();
  for (int i = 0; i < READINGS; i++){
    Serial.print(angleVectors[i].angle);
    Serial.print(" ");  
    Serial.println(angleVectors[i].distance);
      
      if (angleVectors[i].distance <= COLLISION_DISTANCE) //if path is not clear
      {
            if (lowValue)
                clear = false;
            lowValue = true;
      }
  }
  if (clear){ // not going to collide 
      
      // see if we need to veer 
      int leftDistance = averageLeftDistance();
      int rightDistance = averageRightDistance();
      if (leftDistance < AVOIDANCE_DISTANCE && rightDistance > AVOIDANCE_DISTANCE){
         veer_right(); 
      }
      else if (rightDistance < AVOIDANCE_DISTANCE && leftDistance > AVOIDANCE_DISTANCE){
         veer_left(); 
      }      
  }
  return clear;
}

int averageRightDistance(){
  int accumulator = 0;
  for (int i = 0; i < 2; i++){
      accumulator += angleVectors[i].distance;
  }
  return (accumulator / 2);  
}  

int averageLeftDistance(){
  int accumulator = 0;
  for (int i = 3; i < 5; i++){
      accumulator += angleVectors[i].distance;
  }
  return (accumulator / 2);  
}  

int getAverageDistance(){
  int accumulator = 0;
  for (int i = 0; i < READINGS; i++){
   // Serial.print(angleVectors[i].angle);
   // Serial.print(" ");  
   // Serial.println(angleVectors[i].distance);
      accumulator += angleVectors[i].distance;
  }
  return (accumulator / READINGS);
}

void lookForward(){
  // set up the reading angles for the servo
  angleVectors[0].angle = 70;
  angleVectors[1].angle = 80;
  angleVectors[2].angle = 90;
  angleVectors[3].angle = 100;
  angleVectors[4].angle = 110;
  ultrasonicServo.write(angleVectors[0].angle); // set servo to face the starting point
  delay(300); // wait 300 milliseconds for servo to reach position
  scan();
}  

void lookLeft(){
  // set up the reading angles for the servo
  angleVectors[0].angle = 140;
  angleVectors[1].angle = 150;
  angleVectors[2].angle = 160;
  angleVectors[3].angle = 170;
  angleVectors[4].angle = 180;
  ultrasonicServo.write(angleVectors[0].angle); // set servo to face the starting point
  delay(300); // wait 300 milliseconds for servo to reach position
  scan();
}  

void lookRight(){
  // set up the reading angles for the servo
  angleVectors[0].angle = 0;
  angleVectors[1].angle = 10;
  angleVectors[2].angle = 20;
  angleVectors[3].angle = 30;
  angleVectors[4].angle = 40;
  ultrasonicServo.write(angleVectors[0].angle); // set servo to face the starting point
  delay(500); // wait 500 milliseconds for servo to reach position
  scan();
}  


void scan(){
  
  for(int i = 0; i < READINGS; i++){     // loop to sweep the servo (& sensor)    
     angleVectors[i].distance = ping();
     ultrasonicServo.write(angleVectors[i].angle); // set servo position
     delay(35); // wait 30 milliseconds for servo to reach position
  }
  ultrasonicServo.write(CENTER); // set servo to face the starting point
  delay(35);
}

float ping(){
    delay(50);                      // Wait 50ms between pings (about 20 pings/sec). Removed as there is already a delay for the servo
    unsigned int uS = sonar.ping();   // Send ping, get ping time in microseconds (uS).
    if (uS == 0)                      // out of range (0 = outside set distance range, no ping echo)
        return MAX_DISTANCE;
    else                              // in range
        return uS / US_ROUNDTRIP_CM;  // Convert ping time to distance 
}


//**************************************************************************************************************
//*
//* Drive functions
//* You may need to vary the delays below to give the functions time to complete
//*
//**************************************************************************************************************

//#define BASE_ACT_TIME 100

//#define BRAKE_ACT_TIME   BASE_ACT_TIME
//#define TURN_ACT_TIME    6*BASE_ACT_TIME
//#define ROTATE_ACT_TIME  3.75*BASE_ACT_TIME
//#define UTURN_ACT_TIME   7*BASE_ACT_TIME

// smooth floor
#define BRAKE_ACT_TIME   100
#define TURN_ACT_TIME    600
//#define ROTATE_ACT_TIME  500
#define ROTATE_ACT_TIME  1000
#define UTURN_ACT_TIME   800

// short pile carpet - low battery
//#define BRAKE_ACT_TIME   200
//#define TURN_ACT_TIME    750
//#define ROTATE_ACT_TIME  550
//#define UTURN_ACT_TIME   1000


void freewheel(){
    motor1.run(RELEASE);
    motor2.run(RELEASE);
    delay(BRAKE_ACT_TIME);
}

void brake(){ // this will require a modded AFMOTOR
    motor1.run(BRAKE);
    motor2.run(BRAKE);
    delay(BRAKE_ACT_TIME); // braking time
}

void drive_forward(){
    //setSpeed(myspeed);
    motor1.run(FORWARD);
    motor2.run(FORWARD);
}

void drive_backward(){
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);
}

void turn_left(){
    motor1.run(RELEASE);
    motor2.run(FORWARD);
    delay(TURN_ACT_TIME);
    freewheel();
}

void turn_right(){
    motor2.run(RELEASE);
    motor1.run(FORWARD);
    delay(TURN_ACT_TIME);
    freewheel();
}

void veer_left(){
    motor1.run(RELEASE);
    motor2.run(FORWARD);
    delay(TURN_ACT_TIME/3);
    freewheel();
}

void veer_right(){
    motor2.run(RELEASE);
    motor1.run(FORWARD);
    delay(TURN_ACT_TIME/3);
    freewheel();
}

void rotate_left(){
    motor1.run(BACKWARD);
    motor2.run(FORWARD);
    delay(ROTATE_ACT_TIME);
    freewheel();
}

void rotate_right(){
    //changespeed(MEDIUM);
    motor2.run(BACKWARD);
    motor1.run(FORWARD);
    delay(ROTATE_ACT_TIME);
    freewheel();
}
void u_turn(){
    motor2.run(BACKWARD);
    motor1.run(FORWARD);
    delay(UTURN_ACT_TIME); // twice as long as rotate right to end up 180 degrees around
    freewheel();
}

//**************************************************************************************************************
//*
//* Speed functions
//*
//* You can adjust SPEED_OFFSET to allow for differences between the motor speeds
//* Make it negative if the robot veers to the right and positive if it veers to the left
//*
//*
//**************************************************************************************************************
//Changed Motor speed for right turn, doing right turn only, changed detection collision zone time
#define MIN(a, b) (a < b ? a : b)

#define SPEED_OFFSET -23 // this offset is specific to your motor set - adjust till you get a straight path

void changeSpeed(int speed){
  speed = MIN(speed, MAXSPEED);
  if (speed > throttle){
    for (; throttle < speed; throttle +=1) 
    {
     setSpeed(throttle);
    }
  }
  else{
        for (; throttle > speed; throttle -=1) 
    {
     setSpeed(throttle);
    }
  }
}

#define SPEED_CHANGE_TIME 10 // time in milliseconds to react

void setSpeed(int speed){
    motor1.setSpeed(speed+SPEED_OFFSET);
    motor2.setSpeed(speed);
    delay(SPEED_CHANGE_TIME);
}

//**************************************************************************************************************
//*
//* Servo control
//*
//**************************************************************************************************************


// servo control

//void servo_position(int newPos){
//    if (newPos > currentPos){
//        for(int pos=currentPos; pos < newPos; pos += 1)  // goes from 0 degrees to 180 degrees 
//        {                                                // in steps of 1 degree 
//            ultrasonicServo.write(pos);                  // tell servo to go to position in variable 'pos' 
//            delay(15);                                   // waits 15ms for the servo to reach the position 
//        }
//        currentPos = newPos;  
//    }
//    else if (newPos < currentPos){
//        for(int pos=currentPos; pos > newPos; pos -= 1)  // goes from 0 degrees to 180 degrees 
//        {                                                // in steps of 1 degree 
//            ultrasonicServo.write(pos);                  // tell servo to go to position in variable 'pos' 
//            delay(15);                                   // waits 15ms for the servo to reach the position 
//        }
//        currentPos = newPos;  
//    }  
//}

void servo_position(int newPos){
  ultrasonicServo.write(newPos);
  delay(800);
}









