// Low pass filter test

//#define NUM_OF_SAMPLES 10000
#define NUM_OF_SAMPLES 1000

// Integer implementation of low pass filter
// Be carefull with data types!
int16_t intLowPass(int32_t* bias, uint16_t raw) {
  *bias = (*bias + ((raw - *bias) / 1024)); // must be 1024
  return raw - *bias;
}

double doubleLowPass(double* bias, double raw) {
  *bias = (*bias + ((raw - *bias) / 1024.0)); // must be 1024
  return raw - *bias;
}

void setup() {
  Serial.begin(112500);
  while (!Serial);
  Serial.println("Start test...");
  delay(2000);

  const int16_t input[] = {1023, 0};
  int32_t bias = 512; // must be 32-bit int
  int16_t filtered = 0;

  unsigned long start = millis();
  for (unsigned int i = 0; i < NUM_OF_SAMPLES; i++) {
    int16_t raw = input[(i % 2)];

//    Serial.print("raw: ");
//    Serial.print(raw);
//    Serial.print(" bias: ");
//    Serial.print(bias);
//    Serial.print(" bias: ");
//    Serial.print(bias);
    filtered = intLowPass(&bias, raw);
//    Serial.print(" filtered: ");
//    Serial.println(filtered);
  }
  unsigned long finish = millis();
  Serial.println(finish);
  Serial.print("Time expected 41 ms: ");
  Serial.println(finish - start);
  Serial.print("Filtered expected -512: ");
  Serial.println(filtered);

  // Now with double

  double d_bias = 512.0;
  double d_filtered = 0.0;

  start = millis();
  for (unsigned int i = 0; i < NUM_OF_SAMPLES; i++) {
    double raw = input[(i % 2)];

//    Serial.print("raw: ");
//    Serial.print(raw);
//    Serial.print(" bias: ");
//    Serial.print(d_bias);

    d_filtered = doubleLowPass(&d_bias, raw);
//    Serial.print(" filtered: ");
//    Serial.println(d_filtered);    
  }
  finish = millis();
  Serial.println(finish);
  Serial.print("Time expected 41 ms: ");
  Serial.println(finish - start);
  Serial.print("Filtered expected -512: ");
  Serial.println(d_filtered);
}

void loop() {}
