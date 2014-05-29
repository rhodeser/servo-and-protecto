#include <NewPing.h>

#define TRIGGER_PIN  8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     9  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define LED          13
/*
#define MotorS1
#define MotorS2
#define MotorS3
#define MotorS4
*/

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void LED_blink(unsigned int time);

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  pinMode(LED, OUTPUT);
}

void loop() {
  unsigned int distance;
  unsigned int scan_counter = 0;
  unsigned int valid_signal_counter = 0;
  delay(50);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  Serial.print("Ping: ");
  Serial.print(uS / US_ROUNDTRIP_CM); // Convert ping time to distance in cm and print result (0 = outside set distance range)
  Serial.println("cm");
  distance = uS / US_ROUNDTRIP_CM;
  
  // New algorithm
  /*
  forward();
  scan_counter++;
  //check sides
  if (scan_counter > 100)
  {
    //get left, left diagonal, right, right diagonal measurements
    //need to call turn_right(turntime?);
    if (left_distance < side_limit || left_diag_distance < limit)  turn_right();
    if (right_distance < side_limit || right_diag_distance < limit) turn_left();
    scan_counter = 0;
  }
  // check straight ahead
  //getdistance()
  if distance < limit) valid_signal_counter++;
  if distance > limit) valid_signal_counter=0;
  if (valid_signal_counter > 20) // make sure false signals aren't used
  {
   halt(); 
   //decide_direction();
   valid_signal_counter = 0;
   
  }
  */
  // Change frequency of LED depending on distance
  // For testing purposes only
  if (distance <= 10) halt();
  else if (distance <=35) turn_left();
  else if (distance <=70) turn_right();
  else if (distance <=100) forward();
  else digitalWrite(LED, LOW);
}
//Motor control untested, pin assignments needed as well
void turn_left() {
  LED_blink(250);
  /*
  digitalWrite (MotorS1, LOW);
  digitalWrite (MotorA2, HIGH);
  digitalWrite (MotorB1, HIGH);
  digitalWrite (MotorB2, LOW);
  delay(time);
  */
}

void turn_right() {
  LED_blink(500);
    /*
  digitalWrite (MotorS1, HIGH);
  digitalWrite (MotorA2, LOW);
  digitalWrite (MotorB1, LOW);
  digitalWrite (MotorB2, HIGH);
  delay(time);
  */
}

void forward() {
  LED_blink(1000);
      /*
  digitalWrite (MotorS1, HIGH);
  digitalWrite (MotorA2, LOW);
  digitalWrite (MotorB1, HIGH);
  digitalWrite (MotorB2, LOW);
  delay(time);
  */
}

void halt() {
  LED_blink(125);
      /*
  digitalWrite (MotorS1, LOW);
  digitalWrite (MotorA2, LOW);
  digitalWrite (MotorB1, LOW);
  digitalWrite (MotorB2, LOW);
  delay(time);
  */
}

void LED_blink(unsigned int time){
  // blink LED and busy wait
  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(time);               // wait for a second
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  delay(time);               // wait for a second  
}
