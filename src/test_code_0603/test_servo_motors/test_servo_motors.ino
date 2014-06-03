//*********************************************************
// Ultrasonic sensor

#include <NewPing.h>

#define TRIG_PIN  A4  
#define ECHO_PIN  A5  

#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pin and maximum distance.

float dangerThreshold = 10.0; // 25 cm - depends on the floor surface and speed setting
//************************************************************ 
// Servo

#include <Servo.h> 
#define LEFT 30
#define CENTER 90
#define RIGHT 150

Servo ultrasonicServo;  // create servo object to control a servo 
 
int currentPos = 0;    // variable to store the servo position 
//**************************************************************
// Motors

#include <AFMotor.h>

AF_DCMotor motor1(1, MOTOR12_1KHZ);
AF_DCMotor motor2(2, MOTOR12_1KHZ);

#define STOP 0
#define SLOW 150
#define MEDIUM 230
#define MAXSPEED 255

int throttle = 0;
// used to balance power across wheels

//*************************************************************

// setup
void setup() {
    
    // setup motor    
    Serial.begin(9600);

    // setup servo
    ultrasonicServo.attach(9);  // attaches the servo on pin 5 to the servo object
   
    // move servo to center 
    servo_position(CENTER);  
    
    // setup motors
    throttle = MEDIUM;
    setSpeed(throttle);
    //freewheel();
}
//*************************************************************
// Main

#define CLEAR          1
#define EMERGENCY_STOP 2
#define TURN_LEFT      3
#define TURN_RIGHT     4
#define VEER_LEFT      5
#define VEER_RIGHT     6

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
        
        // re-center the ultrasonic
        servo_position(CENTER);
           
        drive_forward();
        //delay(2000);
        //freewheel();
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
