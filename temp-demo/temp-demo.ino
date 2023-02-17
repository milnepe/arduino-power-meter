/**
    @filename   :   epd2in9-demo.ino
    @brief      :   2.9inch e-paper display demo
    @author     :   Yehui from Waveshare

    Copyright (C) Waveshare     September 9 2017

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documnetation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to  whom the Software is
   furished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#include <SPI.h>
#include "epd2in9.h"
#include "epdpaint.h"
#include "imagedata.h"

#define COLORED 0
#define UNCOLORED 1

/**
    Due to RAM not enough in Arduino UNO, a frame buffer is not allowed.
    In this case, a smaller image buffer is allocated and you have to
    update a partial display several times.
    1 byte = 8 pixels, therefore you have to set 8*N pixels at a time.
*/
unsigned char image[1024];
Paint paint(image, 0, 0);  // width should be the multiple of 8
Epd epd;
unsigned long time_start_ms;
unsigned long temperature_c = 0;

void TempToStr(unsigned long temp_c, char buffer[6]) {
  buffer[0] = temp_c / 1000 % 10 + '0';
  buffer[1] = temp_c / 100 % 10 + '0';
  buffer[3] = temp_c % 100 / 10 + '0';
  buffer[4] = temp_c % 100 % 10 + '0';
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial)
    ;
  if (epd.Init(lut_full_update) != 0) {
    Serial.print("e-Paper init failed");
    return;
  }

  /**
      there are 2 memory areas embedded in the e-paper display
      and once the display is refreshed, the memory area will be auto-toggled,
      i.e. the next action of SetFrameMemory will set the other memory area
      therefore you have to clear the frame memory twice.
  */
  epd.ClearFrameMemory(0xFF);  // bit set = white, bit reset = black
  epd.DisplayFrame();
  epd.ClearFrameMemory(0xFF);  // bit set = white, bit reset = black
  epd.DisplayFrame();

  delay(2000);

  if (epd.Init(lut_partial_update) != 0) {
    Serial.print("e-Paper init failed");
    return;
  }

  epd.SetFrameMemory(epd_bitmap_okdo);
  epd.DisplayFrame();
  epd.SetFrameMemory(epd_bitmap_okdo);
  epd.DisplayFrame();

  time_start_ms = millis();
}

void loop() {
  static char time_string[] = { '0', '0', '.', '0', '0', '\0' };

  if (temperature_c < 10000) {
    temperature_c = (temperature_c + 10);
  } else {
    temperature_c = 0;
  }

  TempToStr(temperature_c, time_string);

  //  paint.SetWidth(32);
  paint.SetWidth(32);
  paint.SetHeight(96);
  paint.SetRotate(ROTATE_90);

  paint.Clear(COLORED);
  paint.DrawStringAt(0, 8, time_string, &Font24, UNCOLORED);
  //  epd.SetFrameMemory(paint.GetImage(), 80, 72, paint.GetWidth(), paint.GetHeight());
  // Bottom left
  epd.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
  // Bottom right (296 - SetHeight(96))
  epd.SetFrameMemory(paint.GetImage(), 0, 200, paint.GetWidth(), paint.GetHeight());

  // Top left (128 - SetWidth(32))
  epd.SetFrameMemory(paint.GetImage(), 96, 0, paint.GetWidth(), paint.GetHeight());

  // Top right (128 - SetWidth(32), 296 - SetHeight(96))
  epd.SetFrameMemory(paint.GetImage(), 96, 200, paint.GetWidth(), paint.GetHeight());

  // Middle ()
  epd.SetFrameMemory(paint.GetImage(), 48, 96, paint.GetWidth(), paint.GetHeight());

  epd.DisplayFrame();

  delay(500);
}
