/***************************************************************************//**
* @file
* Color Tracking Template Code w/ Poll Mode
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

#define RED_MIN 230
#define RED_MAX 255
#define GREEN_MIN 230
#define GREEN_MAX 255
#define BLUE_MIN 230
#define BLUE_MAX 255
#define PIXELS_THRESHOLD 0 // The percent of tracked pixels needs to be greater than this 0=0% - 255=100%.
#define CONFIDENCE_THRESHOLD 50 // The percent of tracked pixels in the bounding box needs to be greater than this 0=0% - 255=100%.

#define LED_BLINK 5 // 5 Hz
#define WAIT_TIME 5000 // 5 seconds

CMUcam4 cam(CMUCOM4_SERIAL3);

void setup()
{
  int a;
  a = cam.begin();
  Serial.begin(19200);
  Serial.print(a);
  Serial.print("Starting the camera\n");
  // Wait for auto gain and auto white balance to run.

  cam.LEDOn(LED_BLINK);
  delay(WAIT_TIME);

  // Turn auto gain and auto white balance off.

  cam.autoGainControl(false);
  cam.autoWhiteBalance(false);
  cam.colorTracking(false);
  cam.LEDOn(CMUCAM4_LED_ON);
}

void loop()
{
  CMUcam4_tracking_data_t data;
  //cam.trackColor(RED_MIN, RED_MAX, GREEN_MIN, GREEN_MAX, BLUE_MIN, BLUE_MAX);
  for(;;)
  {
    cam.trackColor(RED_MIN, RED_MAX, GREEN_MIN, GREEN_MAX, BLUE_MIN, BLUE_MAX);
    cam.getTypeTDataPacket(&data); // Get a tracking packet.
    delay(1000);
    Serial.print("pixels\n");
    Serial.print(data.pixels);
    Serial.println();
    Serial.print("confidence\n");   
    Serial.print(data.confidence);
    Serial.println();
    
    if (data.pixels > PIXELS_THRESHOLD)
    {
        if(data.confidence > CONFIDENCE_THRESHOLD) // The pixels we are tracking are in a dense clump - so maybe they are a single object.
            {
              cam.LEDOn(5);
              cam.LEDOn(5);
              Serial.println();
              Serial.print("Found bright color\n");
            }
        else Serial.print("Within Pixels threshold but not confidence\n");
    }
    else Serial.print("You fucked up\n");
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
