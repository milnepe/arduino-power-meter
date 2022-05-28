/*
  Simple analogue voltmeter
*/

#define NUM_OF_SAMPLES 1000
#define ADC_PIN A1

void setup() {
  Serial.begin(115200);
  while (!Serial);

  for (unsigned int n = 0; n < 100; n++)
  {
    analogRead(ADC_PIN);
  }
}

void loop() {
  double readings = 0;
  for (unsigned int n = 0; n < NUM_OF_SAMPLES; n++)
  {
    int reading = analogRead(ADC_PIN);
    readings += reading;
  }

  Serial.print("Analogue value: ");
  Serial.println(readings / NUM_OF_SAMPLES);

  delay(1000);
}
