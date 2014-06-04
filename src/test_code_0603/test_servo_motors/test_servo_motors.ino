//*********************************************************
// Ultrasonic sensor

#include <NewPing.h>

#define TRIG_PIN  A4  
#define ECHO_PIN  A5  

#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pin and maximum distance.

float dangerThreshold = 25.0; // 25 cm - depends on the floor surface and speed setting

#define COLLISION_DISTANCE 25 // in cm - depends on what we want
#define AVOIDANCE_DISTANCE 40
//************************************************************ 
//* Servo
//************************************************************
#include <Servo.h> 
#define LEFT 180
#define CENTER 100
#define RIGHT 20

Servo ultrasonicServo;  // create servo object to control a servo 

int currentPos = 0;    // variable to store the servo position 
//**************************************************************
//* Motors
//**************************************************************'

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


//*************************************************************
//* setup
//*************************************************************
void setup() {

  // setup motor    
  Serial.begin(9600);

  // setup servo
  ultrasonicServo.attach(9);  // attaches the servo on pin 5 to the servo object

  // move servo to center 
  servo_position(CENTER);  

  // setup motors
  throttle = SLOW;
  setSpeed(throttle);
  //freewheel();
}
//*************************************************************
//* Main
//*************************************************************

#define CLEAR          1
#define EMERGENCY_STOP 2
#define TURN_LEFT      3
#define TURN_RIGHT     4
#define VEER_LEFT      5
#define VEER_RIGHT     6

void loop() {

  //float distanceForward = ping();
  //delay(500);
  //if (distanceForward > dangerThreshold) //if path is clear
  if (scanClear())
  {
    drive_forward();
  }
  else // if path is blocked
  {
    //brake();   
    freewheel();

    // look left
    // servo_position(LEFT);  
    //delay(500);
    //float distanceLeft = ping();
    //Serial.print("Left: ");
    //Serial.print(distanceLeft);
    //Serial.println("cm");


    // look right
    //delay(500);
    // servo_position(RIGHT);  
    //float  distanceRight = ping();
    //Serial.print("Right: ");
    //Serial.print(distanceRight);
    //Serial.println("cm");
    //delay(500);

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
      // Check if we are too close to the obstacle
      //if (distanceLeft < 10.0)
      //{
      drive_backward();
      delay(200);
      //}
      // rotate left
      rotate_left();
    }
    else if (distanceRight > distanceLeft && distanceRight > dangerThreshold) //if right is less obstructed 
    {
      // Check if we are too close to the obstacle
      //if (distanceRight < 10.0)
      //{
      drive_backward();
      delay(200);
      //}
      // rotate right
      rotate_right();
    }
    else // equally blocked or less than danger threshold left or right
    {
      freewheel();
      delay(20);
      drive_backward();
      delay(500);
      u_turn();
    }   
  }   

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


// short pile carpet - low battery
#define BRAKE_ACT_TIME   200
#define TURN_ACT_TIME    750
#define ROTATE_ACT_TIME  550
#define UTURN_ACT_TIME   1000

void freewheel(){
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  delay(BRAKE_ACT_TIME);
}

void drive_forward(){
  motor1.run(FORWARD);
  motor2.run(FORWARD);
}

void brake(){ // this will require a modded AFMOTOR
  motor1.run(BRAKE);
  motor2.run(BRAKE);
  delay(BRAKE_ACT_TIME); // braking time
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

void drive_backward(){
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
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

#define SPEED_CHANGE_TIME 10 // time in milliseconds to react

void setSpeed(int speed){
  motor1.setSpeed(speed+SPEED_OFFSET);
  motor2.setSpeed(speed);
  delay(SPEED_CHANGE_TIME);
}

//*************************************************************************************************************
//*
//* Scanning
//*
//**************************************************************************************************************
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
      lowValue = true;

      if (lowValue)
      {
        clear = false;
        //return clear;
      }
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

void lookForward(){
  // set up the reading angles for the servo
  angleVectors[0].angle = 80;
  angleVectors[1].angle = 90;
  angleVectors[2].angle = 100;
  angleVectors[3].angle = 110;
  angleVectors[4].angle = 120;
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


