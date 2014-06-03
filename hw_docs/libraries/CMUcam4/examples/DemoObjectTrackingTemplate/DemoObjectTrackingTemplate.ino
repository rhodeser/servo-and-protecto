/***************************************************************************//**
* @file
* Object Tracking Template Code
*
* @version @n 1.0
* @date @n 2/11/2013
*
* @authors @n Kwabena W. Agyeman
* @copyright @n (c) 2013 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Initial Release - 2/11/2013
*******************************************************************************/

#include <CMUcam4.h>
#include <CMUcom4.h>

#include <Servo.h>

#define RED_MIN 230
#define RED_MAX 255
#define GREEN_MIN 230
#define GREEN_MAX 255
#define BLUE_MIN 230
#define BLUE_MAX 255

#define LED_BLINK 5 // 5 Hz
#define WAIT_TIME 5000 // 5 seconds

#define POLL_MODE false // See the command interface manual about poll mode.
#define YUV_MODE false // See the command interface manual about yuv mode.

#define NOISE_FILTER 2

#define PIXELS_THRESHOLD 1
#define CONFIDENCE_THRESHOLD 50

#define SERVO_PIN 9

CMUcam4 cam(CMUCOM4_SERIAL);

Servo servo;

void setup()
{
  cam.begin();

  // Wait for auto gain and auto white balance to run.

  cam.LEDOn(LED_BLINK);
  delay(WAIT_TIME);

  // Turn auto gain and auto white balance off.

  cam.autoGainControl(false);
  cam.autoWhiteBalance(false);

  cam.LEDOn(CMUCAM4_LED_ON);

  cam.pollMode(POLL_MODE);
  cam.colorTracking(YUV_MODE);

  cam.noiseFilter(NOISE_FILTER);

  servo.attach(SERVO_PIN);
}

void loop()
{
  CMUcam4_tracking_data_t data;

  #if !POLL_MODE
  cam.trackColor(RED_MIN, RED_MAX, GREEN_MIN, GREEN_MAX, BLUE_MIN, BLUE_MAX);
  #endif

  for(;;)
  {
    #if POLL_MODE
    cam.trackColor(RED_MIN, RED_MAX, GREEN_MIN, GREEN_MAX, BLUE_MIN, BLUE_MAX);
    #endif

    cam.getTypeTDataPacket(&data); // Get a tracking packet.

    // If poll mode is enabled then you have all the time in the world here...

    // If poll mode is disabled then you have 33.33 ms here...

    // Process the packet data safely here.

    // For example... below is a simple proportional controlled servo driver.

    if((data.pixels >= PIXELS_THRESHOLD) &&
       (data.confidence >= CONFIDENCE_THRESHOLD))
    {
      static int pos = 1500;

      int error = (((CMUCAM4_NATIVE_H_RES / 2) - 1) - data.mx);

      pos = min(max(pos + (error / 2), 1000), 2000);

      servo.writeMicroseconds(pos); // The servo follows the x centroid.
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

