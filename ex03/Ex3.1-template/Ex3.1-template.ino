// Task 1: Environmental Light Categorization
// Complete the implementation according to the task description.
#include <Arduino.h>
#include <U8g2lib.h>

uint16_t rawValue = 0;

// TODO: define necessary constants (e.g., calibration range, thresholds, timing)

// TODO: implement a function that categorizes the normalized value
// const char* categorize(int normalized) {
    // TODO: return "LOW", "MEDIUM", or "HIGH"
//}

void setup() {
    // TODO: initialize Serial communication
    Serial.begin(115200);
    while(!Serial){
        delay(10);
    }
    ADMUX = 0;
    ADCSRA = 0;

    ADMUX |= (1 << REFS0);
    ADCSRA |= (1 << ADEN);


    // TODO: configure ADC resolution to 12-bit
    NRF_SAADC->ENABLE = SAADC_ENABLE_ENABLE_Enabled
    NRF_SAADC->RESOLUTION = SAADC_RESOLUTION_VAL_12bit;
    NRF_SAADC->CH[0].CONFIG = (SAADC_CH_CONFIG_GAIN_Gain1_6 << SAADC_CH_CONFIG_GAIN_Pos) | (SAADC_CH_CONFIG_REFSEL_Internal << SAADC_CH_CONFIG_REFSEL_Pos);
}

void loop() {
    // TODO: implement non-blocking timing using millis()
    // The system should sample every 500 ms

    // TODO: read raw value from light sensor
    rawValue = Analog0Read();
    // TODO: clamp the raw value to a calibrated range
    if (rawValue < 50){
        rawValue = 50;
    }else if(rawValue > 3500){
        rawValue = 3500;
    }
    // TODO: normalize the value to a 0–100 scale
    long mapping = map(rawValue, 50, 3500, 0, 100);
    mapping = constrain(mapping, 0, 100);
    // TODO: determine the category using the categorize() function

    // TODO: print raw value, normalized value, and category to Serial
}

uint16_t Analog0Read() {

  // start ADC conversion
  ADCSRA |= (1 << ADSC);

  // wait for result
  while((ADCSRA & (1 << ADIF)) == 0) {
  }

  // read the result
  uint16_t res = ADCL;
  res |= (ADCH << 8);

  // remove conversion finished flag
  ADCSRA &= ~(1 << ADIF);

  return res;
}
 