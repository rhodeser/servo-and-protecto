/***************************************************************************//**
* @file
* Color Tracking Template Code
*
* @version @n 1.0
* @date @n 8/14/2012
*
* @authors @n Kwabena W. Agyeman
* @copyright @n (c) 2012 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Initial Release - 8/14/2012
*******************************************************************************/

#include <CMUcam4.h>
#include <CMUcom4.h>

#define RED_MIN 220
#define RED_MAX 256
#define GREEN_MIN 0
#define GREEN_MAX 50
#define BLUE_MIN 0
#define BLUE_MAX 50
#define LED_BLINK 5 // 5 Hz
#define WAIT_TIME 3000 // 5 seconds
#define PIXELS_THRESHOLD 2 // The percent of tracked pixels needs to be greater than this 0=0% - 255=100%.
#define CONFIDENCE_THRESHOLD 40 // The percent of tracked pixels in the bounding box needs to be greater than this 0=0% - 255=100%.

#define NOISE_FILTER_LEVEL 2 // Filter out runs of tracked pixels smaller than this in length 0 - 255.
CMUcam4 cam(CMUCOM4_SERIAL);

void setup()
{
  cam.begin();
  Serial.begin(115200);
  Serial.print("Starting the camera");
  // Wait for auto gain and auto white balance to run.

  cam.LEDOn(3);                                            
  pinMode(13, OUTPUT); 
  // Turn auto gain and auto white balance off.
  
  cam.autoGainControl(false);
  cam.autoWhiteBalance(false);

  cam.LEDOn(CMUCAM4_LED_ON);
  cam.pollMode(true);
  cam.colorTracking(true);
  cam.setTrackingParameters(RED_MIN, RED_MAX, GREEN_MIN, GREEN_MAX, BLUE_MIN, BLUE_MAX);
}

void loop()
{
  CMUcam4_tracking_data_t data;
  if (cam.trackColor()) Serial.print("trackColor() did not work");
  //cam.trackColor(RED_MIN, RED_MAX, GREEN_MIN, GREEN_MAX, BLUE_MIN, BLUE_MAX);
  else Serial.print("Color was tracked");
  for(int i=0; i<8;i++)
  {
    cam.trackColor();
    Serial.print("tracking again");
    Serial.println();
    cam.getTypeTDataPacket(&data); // Get a tracking packet.
    Serial.print("mx");
    Serial.println();
    Serial.print(data.mx);
    Serial.println();
    Serial.print("my");
    Serial.println();
    Serial.print(data.my);
    Serial.println();
    Serial.print("pixels\n");
    Serial.println();
    //Serial.print(data.my);
    Serial.print(data.pixels);
    Serial.print("confidence");
    Serial.println();    
    Serial.print(data.confidence);
    digitalWrite(13, HIGH);   // set the LED on
    delay(300);              // wait for a second
    digitalWrite(13, LOW);    // set the LED off
    delay(1700); 
    if (data.pixels > PIXELS_THRESHOLD)
    {
        if(data.confidence > CONFIDENCE_THRESHOLD) // The pixels we are tracking are in a dense clump - so maybe they are a single object.
        {
        // Okay, so we now know we are looking at a clump of pixels that are in the color bounds we wanted to track.
/*
        // Linearly makes the LED get brighter as the object moves towards the left.
        analogWrite(LEFT_LED_PIN,  255 - map(CMUCAM4_MIN_NATIVE_COLUMN + data.mx, 0, CMUCAM4_MAX_NATIVE_COLUMN, 0, 255));

        // Linearly makes the LED get brighter as the object moves towards the middle.
        analogWrite(MIDDLE_LED_PIN, 255 - map(abs((CMUCAM4_NATIVE_H_RES/2) - data.mx), 0, (CMUCAM4_NATIVE_H_RES/2), 0, 255));

        // Linearly makes the LED get brighter as the object moves towards the right.
        analogWrite(RIGHT_LED_PIN, 255 - map(CMUCAM4_MAX_NATIVE_COLUMN - data.mx, 0, CMUCAM4_MAX_NATIVE_COLUMN, 0, 255));

        // Might want to make the above functions non-linear as the brightness function of an LED is a non-linear... the effect will look better.
        
*/        cam.LEDOn(5);
          cam.LEDOn(5);
          cam.LEDOn(5);
          Serial.println();
          Serial.print("Got HEEM");
        }
        else
        {
          cam.LEDOn(2);
        //analogWrite(LEFT_LED_PIN, 0);
        //analogWrite(MIDDLE_LED_PIN, 0);
        //analogWrite(RIGHT_LED_PIN, 0);
          Serial.println();
          Serial.print("Still kinda got heem");
          Serial.println();
          digitalWrite(13, HIGH);   // set the LED on
          delay(1000);              // wait for a second
          digitalWrite(13, LOW);    // set the LED off
          delay(1000); 
        } 
    }
    else
    {
      // Turn the LED's off.
          cam.LEDOn(10);
          //digitalWrite(13, HIGH);
      //analogWrite(LEFT_LED_PIN, 0);
      //analogWrite(MIDDLE_LED_PIN, 0);
      //analogWrite(RIGHT_LED_PIN, 0);
    }
  }
  


  // Do something else here.
}

/***************************************************************************//**
* @file
* @par MIT License - TERMS OF USE:
* @n Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* @n
* @n The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* @n
* @n THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*******************************************************************************/
