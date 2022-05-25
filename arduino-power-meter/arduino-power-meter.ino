/*
  Simple analogue power meter to measure current transformer (CT)
  output and calculate mains active power cost per kWHr

  Note: Works best for resistive loads as voltage phase shift
  is not taken into account

  Adapted from code by Open Energy Monitor project
  https://github.com/openenergymonitor/EmonLib

  Author: Peter Milne
  Date: 25 May 2022

  Copyright 2022 Peter Milne
  Released under GNU GENERAL PUBLIC LICENSE
  Version 3, 29 June 2007

 ****************************************************/

#include <SPI.h>
#include "epd2in9.h"
#include "epdpaint.h"
#include "background.h"

/*
  Irms conversion ratio derived from
  CT number of turns / Burden resistor
  Adjust this value to calibrate
*/
const float current_ratio =  2000.0 / 31.35;

// Actual Arduino voltage measured across 5V pin & GND
#define ARDUINO_V 4.66

// Electricity cost per kWHr, not including standing charge
#define COST_PER_KWHR 28.5  // Cost per kWHr (p)

// Mains RMS voltage - can be adjusted
#define MAINS_V_RMS  240

#define NUM_OF_SAMPLES 1000.0
#define ADC_PIN A1

// EPD settings
#define COLORED     0
#define UNCOLORED   1

unsigned char image[1024];
Paint paint(image, 0, 0);    // width should be the multiple of 8
Epd epd;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (epd.Init(lut_full_update) != 0) {
    Serial.print("EPD failed");
    return;
  }
  epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
  epd.DisplayFrame();
  epd.ClearFrameMemory(0xFF);
  epd.DisplayFrame();

  delay(5000);

  if (epd.Init(lut_partial_update) != 0) {
    Serial.print("EPD failed");
    return;
  }

  epd.SetFrameMemory(epd_background);
  epd.DisplayFrame();
  epd.SetFrameMemory(epd_background);
  epd.DisplayFrame();
}

void loop() {
  int adc_raw = 0;
  double adc_bias = 1024 / 2; // halve ADC bits
  double adc_value = 0;
  double adc_sqr_sum = 0;

  for (int n = 0; n < NUM_OF_SAMPLES; n++)
  {
    adc_raw = analogRead(ADC_PIN);

    // Remove ADC bias
    adc_bias = (adc_bias + (adc_raw - adc_bias) / 1024);
    adc_value = adc_raw - adc_bias;

    // Accumulate square of filtered ADC readings
    adc_sqr_sum += adc_value * adc_value;
  }

  // Calculate measured RMS voltage
  float volts_rms = sqrt(adc_sqr_sum / NUM_OF_SAMPLES) * ARDUINO_V / 1024;

  // Scale to mains current RMS (measured Vrms * CT turns / Burden R)
  float Irms = volts_rms * current_ratio;

  // Calculate active power (Irms * Vrms)
  float active_power = Irms * MAINS_V_RMS;

  // Calculate cost per kWHr
  float cost = abs(active_power * COST_PER_KWHR / 1000);

  char power_str[] = {'0', '0', '0', '0', '0', 'W', '\0'};
  val_to_str(active_power, power_str);

  char cost_str[] = {'0', '0', '0', '0', '0', 'p', '\0'};
  val_to_str(cost, cost_str);

  update_display(&paint, &epd, power_str, cost_str);

  Serial.print("Arduino V: ");
  Serial.print(ARDUINO_V);
  Serial.print(" Accumulated ADC: ");
  Serial.print(adc_sqr_sum);
  Serial.print(" Samples: ");
  Serial.print(NUM_OF_SAMPLES);
  Serial.print(" Measured V: ");
  Serial.print(volts_rms, 3);
  Serial.print(" Irms: ");
  Serial.print(Irms);
  Serial.print(" Active Power: ");
  Serial.print(active_power);
  Serial.print(" Cost: ");
  Serial.println(cost);

  delay(1000);
}

void update_display(Paint * paint, Epd * epd, char * power_str, char * cost_str) {
  // Display 128 (height) x 296 (width) pixels - datum is bottom left with leads on left

  // Set size and orientation of numeric display rectangle
  paint->SetWidth(32);
  paint->SetHeight(112);
  paint->SetRotate(ROTATE_90);

  paint->Clear(UNCOLORED);
  // Draw power string in display window
  paint->DrawStringAt(0, 8, power_str, &Font24, COLORED);
  // Set position of bottom left corner of power display rectangle
  // Height then width when display is rotated 90 degrees!
  epd->SetFrameMemory(paint->GetImage(), 48, 166, paint->GetWidth(), paint->GetHeight());

  paint->Clear(UNCOLORED);
  paint->DrawStringAt(0, 8, cost_str, &Font24, COLORED);
  // Set position of cost display
  epd->SetFrameMemory(paint->GetImage(), 16, 166, paint->GetWidth(), paint->GetHeight());

  epd->DisplayFrame();
}

// Convert integer value to display string
void val_to_str(unsigned long v, char * s) {
  if (v > 99999) {
    return;
  }
  s[0] = v / 1000 / 10 + '0';
  s[1] = v / 1000 % 10 + '0';
  s[2] = v / 100 % 10 + '0';
  s[3] = v % 100 / 10 + '0';
  s[4] = v % 100 % 10 + '0';
}
