// Low pass filter test

#include "lowpass.h"

#define NUM_OF_SAMPLES 1000

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Start test...");
  delay(1000);

  int16_t bias = 512;
  int16_t volatile filtered = 0;

  unsigned long start = millis();
  for (unsigned int i = 0; i < NUM_OF_SAMPLES; i++) {
    int16_t raw = 1023;
    filtered = intLowPass(&bias, raw);

    raw = 0;
    filtered = intLowPass(&bias, raw);
  }
  unsigned long finish = millis();
  Serial.print("Filtered expected -512: ");
  Serial.println(filtered);
  Serial.print("Time: ");
  Serial.print(finish - start);
  Serial.println("ms");

  Serial.println();
  Serial.println("Double version....");

  double d_bias = 512.0;
  double d_filtered = 0.0;

  start = millis();
  for (unsigned int i = 0; i < NUM_OF_SAMPLES; i++) {
    int raw = 1023;
    d_filtered = doubleLowPass(&d_bias, raw);

    raw = 0;
    d_filtered = doubleLowPass(&d_bias, raw);
  }
  finish = millis();
  Serial.print("Filtered expected -512: ");
  Serial.println(d_filtered);
  Serial.print("Time: ");
  Serial.print(finish - start);
  Serial.println("ms");
}

void loop() {}
