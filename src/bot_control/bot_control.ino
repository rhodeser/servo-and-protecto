#include <NewPing.h>

#define TRIGGER_PIN  8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     9  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define LED          13
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void LED_blink(unsigned int time);

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  pinMode(LED, OUTPUT);
}

void loop() {
  unsigned int distance;
  delay(50);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  Serial.print("Ping: ");
  Serial.print(uS / US_ROUNDTRIP_CM); // Convert ping time to distance in cm and print result (0 = outside set distance range)
  Serial.println("cm");
  distance = uS / US_ROUNDTRIP_CM;
  
  // Change frequency of LED depending on distance
  // For testing purposes only
  if (distance <= 10) halt();
  else if (distance <=35) turnleft();
  else if (distance <=70) turnright();
  else if (distance <=100) forward();
  else digitalWrite(LED, LOW);
}

void turnleft() {
  LED_blink(250);
}

void turnright() {
  LED_blink(500);
}

void forward() {
  LED_blink(1000);
}

void halt() {
  LED_blink(125);
}

void LED_blink(unsigned int time){
  // blink LED and busy wait
  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(time);               // wait for a second
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  delay(time);               // wait for a second  
}
