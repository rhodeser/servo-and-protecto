/* 
 *servo and protecto - ECE 544 Final Project
 *
 * Copyright (c) 2014, Portland State University by Caren Zgheib and Erik Rhodes.  All rights reserved.
 *
 * Author:		Caren Zgheib
 * Collaborator:        Erik Rhodes
 * Date:		June-2014
 * 
 * Description:
 * ------------
 * This program controls the servo and protecto robot car. The bot has one important task "avoid obstacles".
 * The main function "loop" is called after the setup is done. 
 * In that function, there is a counter that will tell the bot when to do its "victory dance". 
 * If it is not the time for dancing, the scanClear() function is called. If the return value is TRUE (i.e. clear), the bot drives forward.
 * Otherwise, it stops then calls "lookLeft()" and "lookRight()" and gets the average distances to its left and right.
 * If the distanceLeft is greater than the distanceRight and greater than the "dangerThreshold" then the bot backs up and rotates left.
 * Else if the distanceRight is greater than the distanceLeft and greater than the "dangerThreshold" then the bot backs up and rotates right.
 * Otherwise, it is blocked on both sides. Therefore, it backs up and makes a u-turn.
 *
 * Note: the "rotate" functions use two wheels one forward and one backward to make a quick move. The "veer" functions only use one wheel to do a slow veer movement.
 *
 * However, if it is the time to dance, then the "coolness()" function is called and the bot performs its victory dance.
 *
 * The scanClear() function calls the lookForward() function and analyzes the data that is returned by it.
 * It goes through all the readings and checks if any of the distances returned is below the COLLISION_DISTANCE. 
 * If it is, then it sets the clear flag to FALSE and continues the analysis.
 * However, if a distance is below the COLLISION_DISTANCE and below the EMERGENCY_DISTANCE then it directly returns FALSE and 
 * exits the function so that the correct reaction takes place.
 * Otherwise, it finishes the analysis and sets the clear flag to TRUE. Then, it checks the average distance on the left and right of the data gathered by looking forward.
 * If the leftDistance is less than the AVOIDANCE_DISTANCE and the rightDistance is greater than the AVOIDANCE_DISTANCE, then the bot veers right.
 * Else if the rightDistance is less than the AVOIDANCE_DISTANCE and the leftDistance is greater than the AVOIDANCE_DISTANCE, then the bot veers left.
 *
 * The look functions set up the angles and call the scan() function.
 * 
 * The scan() function actually performs the scans at each angle by sending a sonar "ping" and waiting for the return signal to measure the distance.
 * Sometime the sonar returns very low values that are not correct which would make the bot stop even if there is no obstacle.
 * To avoid that, in the scan function, if the returned distance is below the EMERGENCY_DISTANCE then the bot sends another ping. If that newly measured distance is still below
 * the EMERGENCY_DISTANCE, then it re-centers the sonar and stops scanning so that the necessary reaction takes place as soon as possible.
 *
 * Acknowledgement: this algorithm is based on the functions and sample sketch provided by oddWires with the purchase of the oddWires Arduino Kit.
 *                  We used the idea behind the code but we repurposed the functions and used our own algorithms which basically made the bot behave 
 *                  better in emergency situations and avoid obstacles in a smoother way. 
 *
 */

//*********************************************************
//* Serial
//*
//* Define the baudrate for the serial output if connected
//*********************************************************
#define BAUDRATE        9600


//*********************************************************
//* Ultrasonic sensor
//* 
//* HC-SR04 Ultrasonic Module					
//*					
//*	Vcc	+5V			
//*	Trig	A4	on the motor shield		
//*	Echo	A5	on the motor shield		
//*	GND
//**********************************************************

#include <NewPing.h>

#define TRIG_PIN  A4  
#define ECHO_PIN  A5  

#define MAX_DISTANCE 200                         // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pin and maximum distance.

// The dangerThreshold is used when the bot is trying to decide whether to go left or right
float dangerThreshold = 25.0;                    // 25 cm - depends on the floor surface and speed setting

#define COLLISION_DISTANCE 60                    // in cm - depends on what the user wants and the floor surface and speed setting (keep in mind that the bot does not stop immidiately)
#define AVOIDANCE_DISTANCE 100                   // in cm - depends on what the user wants and the floor surface and speed setting (keep in mind that the bot does not stop immidiately)
#define EMERGENCY_DISTANCE 15                    // in cm - used when the bot is very close to the obstacle and needs to react as soon as it sees it.


//************************************************************ 
//* Servo
//*
//* Servo Connections to motor shield					
//*
//*	Brown	SER1 -			
//*	Red	SER1 +			
//*	Orange	SER1 S	
//* 
//* Servo 1 on the motor shield is Arduino Pin 10
//* Servo 2 on the motor shield is Arduino Pin 9
//************************************************************
#include <Servo.h> 

#define LEFT         180
#define CENTER       100
#define RIGHT        20

#define SERVO_PIN    9

Servo ultrasonicServo;  // create servo object to control the servo 

int currentPos = 0;    // variable to store the servo position

//**************************************************************
//* Motors
//*
//* Throttle is a number between 128 and 255 -
//* less than 128 any the motors may not have enough torque to move
//* motor1 (left) is connected to motor 1 pins M1
//* motor2 (right)is connected to motor 2 pins M2
//*
//* PWM Frequency is set to 1kHz (happens to be the default too)
//**************************************************************

#include <AFMotor.h>

AF_DCMotor motor1(1, MOTOR12_1KHZ);
AF_DCMotor motor2(2, MOTOR12_1KHZ);

#define STOP 0
#define SLOW 190
#define MEDIUM 230
#define MAXSPEED 255

int throttle = 0;          // used to balance power across wheels

//***********************************************
//* Coolness
//*
//* Setup the counter and define the victory time
//***********************************************
#define VICTORY   50
int timeToGetCool = 0;

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


//*************************************************************
//* Setup
//*************************************************************
void setup() {

  // setup motor    
  Serial.begin(BAUDRATE);

  // setup servo
  ultrasonicServo.attach(SERVO_PIN);  // attaches the servo on pin 9

  // move servo to center 
  servo_position(CENTER);  

  // setup motors
  throttle = SLOW;
  setSpeed(throttle);
}


//*************************************************************
//* Main
//*************************************************************

void loop() {

  // Increase the timer every time we are in this loop
  timeToGetCool++;

  // Check if it time to dance
  if (timeToGetCool == VICTORY)
  {
    // Begin coolness
    coolness();
    
    // reset timer
    timeToGetCool = 0;
  }
  else
  {
    if (scanClear())  // If clear to go forward
    {
      drive_forward();
    }
    else              // if path is blocked
    {

      freewheel();    // stop


      // look left
      lookLeft();
      int distanceLeft = getAverageDistance();

      // look right
      lookRight();
      int distanceRight = getAverageDistance();
      
      // re-center the ultrasonic
      servo_position(CENTER);

      // go the least obstructed way
      if (distanceLeft > distanceRight && distanceLeft > dangerThreshold)       //if left is less obstructed 
      {
        // back up
        drive_backward();
        delay(400);

        // rotate left
        rotate_left();
      }
      else if (distanceRight > distanceLeft && distanceRight > dangerThreshold) //if right is less obstructed 
      {
        // back up
        drive_backward();
        delay(400);

        // rotate right
        rotate_right();
      }
      else // equally blocked or less than danger threshold left or right
      {
        // stop
        freewheel();
        delay(20);
        
        // back up
        drive_backward();
        delay(500);
        
        // make a u-turn
        u_turn();
      }   
    } 
  }  

}


//*************************************************************
//* Servo Control
//*************************************************************

void servo_position(int newPos){
  if (newPos > currentPos){
    for(int pos=currentPos; pos < newPos; pos += 1)  // goes from 0 degrees to 180 degrees 
    {                                                // in steps of 1 degree 
      ultrasonicServo.write(pos);                    // tell servo to go to position in variable 'pos' 
      delay(15);                                     // waits 15ms for the servo to reach the position 
    }
    currentPos = newPos;  
  }
  else if (newPos < currentPos){
    for(int pos=currentPos; pos > newPos; pos -= 1)  // goes from 0 degrees to 180 degrees 
    {                                                // in steps of 1 degree 
      ultrasonicServo.write(pos);                    // tell servo to go to position in variable 'pos' 
      delay(15);                                     // waits 15ms for the servo to reach the position 
    }
    currentPos = newPos;  
  }  
}

//*************************************************************
//* Ultrasonic Ping
//*************************************************************

float ping(){
  delay(50);                        // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  unsigned int uS = sonar.ping();   // Send ping, get ping time in microseconds (uS).
  if (uS == 0)                      // out of range (0 = outside set distance range, no ping echo)
    return MAX_DISTANCE;
  else                              // in range
  return uS / US_ROUNDTRIP_CM;      // Convert ping time to distance 
}


//**************************************************************
//* Setup paramteres. For short pile carpet - low battery
//*
//* Vary these params depending on the surface and speed setting
//**************************************************************
#define BRAKE_ACT_TIME   200
#define TURN_ACT_TIME    750
#define ROTATE_ACT_TIME  550
#define UTURN_ACT_TIME   750

// To stop
void freewheel(){
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  delay(BRAKE_ACT_TIME);
}

// To drive forward
void drive_forward(){
  motor1.run(FORWARD);
  motor2.run(FORWARD);
}

// To rotate left. Uses two motors => fast turn
void rotate_left(){
  motor1.run(BACKWARD);
  motor2.run(FORWARD);
  delay(ROTATE_ACT_TIME-180);
  freewheel();
  delay(100);
}

// To rotate right. Uses two motors => fast turn
void rotate_right(){
  motor2.run(BACKWARD);
  motor1.run(FORWARD);
  delay(ROTATE_ACT_TIME-180);
  freewheel();
  delay(100);
}

// To make a u-turn
void u_turn(){
  motor2.run(BACKWARD);
  motor1.run(FORWARD);
  delay(UTURN_ACT_TIME); 
  freewheel();
}

// To drive backward
void drive_backward(){
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
}

// To veer left
void veer_left(){
  motor1.run(RELEASE);
  motor2.run(FORWARD);
  delay(TURN_ACT_TIME/2);
}

// To veer right
void veer_right(){
  motor2.run(RELEASE);
  motor1.run(FORWARD);
  delay(TURN_ACT_TIME/2);
}

// To perform victory dance
void coolness(){
  
  // stop
  freewheel();
  delay(20);
  
  // back up
  drive_backward();
  delay(500);
  
  // right 4
  motor2.run(BACKWARD);
  motor1.run(FORWARD);
  delay(ROTATE_ACT_TIME*4);
  delay(30);
  
  // left 4
  motor1.run(BACKWARD);
  motor2.run(FORWARD);
  delay(ROTATE_ACT_TIME*4);
  delay(30);
  
  // five times
  for (int i=0; i<5 ; i++)
  {
    // right
    motor2.run(BACKWARD);
    motor1.run(FORWARD);
    delay(ROTATE_ACT_TIME/2);
    delay(30);
    
    // left
    motor1.run(BACKWARD);
    motor2.run(FORWARD);
    delay(ROTATE_ACT_TIME/2);
    delay(30);
  }
  
  //stop
  freewheel();
  delay(100);
}

//**************************************************************************************************************
//*
//* Set Speed function
//*
//* You can adjust SPEED_OFFSET to allow for differences between the motor speeds
//* Make it negative if the robot veers to the right and positive if it veers to the left
//*
//**************************************************************************************************************
#define SPEED_OFFSET -10      // this offset is specific to your motor set - adjust till you get a straight path

#define SPEED_CHANGE_TIME 10  // time in milliseconds to react

void setSpeed(int speed){
  motor1.setSpeed(speed+SPEED_OFFSET);
  motor2.setSpeed(speed);
  delay(SPEED_CHANGE_TIME);
}

//*************************************************************************************************************
//*
//* Scanning Functions
//*
//**************************************************************************************************************

// scanClear()
bool scanClear(){

  bool clear = true;
  bool lowValue = false;

  lookForward();
  for (int i = 0; i < READINGS; i++){
    Serial.print(angleVectors[i].angle);
    Serial.print(" ");  
    Serial.println(angleVectors[i].distance);

    if (angleVectors[i].distance <= COLLISION_DISTANCE)     //if path is not clear
    {
      lowValue = true;

      if (lowValue)
      {
        clear = false;
        if (angleVectors[i].distance <= EMERGENCY_DISTANCE) // if too close abort
        {
          return clear;
        }
      }
    }
  }

  // If it is OK to go forward
  if (clear){                     // not going to collide 

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

 // scan()
void scan(){

  for(int i = 0; i < READINGS; i++){     // loop to sweep the servo (& sensor) 

    angleVectors[i].distance = ping();

    if (angleVectors[i].distance <= 15) // if too close abort
    {
      angleVectors[i].distance = ping();                  // ping again to make sure it is not a noise value
      
      if (angleVectors[i].distance <= EMERGENCY_DISTANCE) // if too close abort
      {
        ultrasonicServo.write(CENTER);                   // set servo to face the starting point
        return;                                          // quit
      }
    }
    
    ultrasonicServo.write(angleVectors[i].angle);        // set servo position
    delay(35);                                           // wait 35 milliseconds for servo to reach position 
  }
  
  ultrasonicServo.write(CENTER);                         // set servo to face the starting point
  delay(35);                                             // wait 35 milliseconds for servo to reach position
}


//*************************************************************************************************************
//*
//* look Functions
//*
//* Note: The angles are chosen relative to the sonar position on top of the servo.
//*       Can be modified depending on the center position of the servo and the center position of the sonar.
//*       For the lookForward() function, 100-140 scans in front of the bot.
//*       For the lookLeft() function 140-180 scans to the left of the bot.
//*       For the lookRight() function 0-40 scans to the right of the bot.
//**************************************************************************************************************

void lookForward(){
  // set up the reading angles for the servo
  angleVectors[0].angle = 100;
  angleVectors[1].angle = 110;
  angleVectors[2].angle = 120;
  angleVectors[3].angle = 130;
  angleVectors[4].angle = 140;
  ultrasonicServo.write(angleVectors[0].angle);           // set servo to face the starting point
  delay(300);                                             // wait 300 milliseconds for servo to reach position
  scan();
} 

void lookLeft(){
  // set up the reading angles for the servo
  angleVectors[0].angle = 140;
  angleVectors[1].angle = 150;
  angleVectors[2].angle = 160;
  angleVectors[3].angle = 170;
  angleVectors[4].angle = 180;
  ultrasonicServo.write(angleVectors[0].angle);           // set servo to face the starting point
  delay(300);                                             // wait 300 milliseconds for servo to reach position
  scan();
}  

void lookRight(){
  // set up the reading angles for the servo
  angleVectors[0].angle = 0;
  angleVectors[1].angle = 10;
  angleVectors[2].angle = 20;
  angleVectors[3].angle = 30;
  angleVectors[4].angle = 40;  
  ultrasonicServo.write(angleVectors[0].angle);           // set servo to face the starting point
  delay(500);                                             // wait 500 milliseconds for servo to reach position
  scan();
}  

//*************************************************************************************************************
//*
//* Averaging Functions
//*
//**************************************************************************************************************

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
    accumulator += angleVectors[i].distance;
  }
  return (accumulator / READINGS);
}


// End of program

