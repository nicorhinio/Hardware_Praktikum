// Task 1: Environmental Light Categorization
// Complete the implementation according to the task description.
#include <Arduino.h>
#include <U8g2lib.h>

uint16_t rawValue = 0;
int16_t adc_result;
unsigned long lastMeasurement = 0;
const unsigned long interval = 500;
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
    Serial.println("Starting...");

    // TODO: configure ADC resolution to 12-bit
    NRF_SAADC->ENABLE = SAADC_ENABLE_ENABLE_Enabled;
    NRF_SAADC->RESOLUTION = SAADC_RESOLUTION_VAL_12bit;
    NRF_SAADC->CH[0].PSELP = SAADC_CH_PSELP_PSELP_AnalogInput0;
    NRF_SAADC->CH[0].CONFIG = (SAADC_CH_CONFIG_GAIN_Gain1_6 << SAADC_CH_CONFIG_GAIN_Pos) | (SAADC_CH_CONFIG_REFSEL_Internal << SAADC_CH_CONFIG_REFSEL_Pos) | (SAADC_CH_CONFIG_TACQ_10us << SAADC_CH_CONFIG_TACQ_Pos);
    NRF_SAADC->RESULT.PTR = (uint32_t)&adc_result;
    NRF_SAADC->RESULT.MAXCNT = 1;
    Serial.println("SAADC configurated.");
}

void loop() {
    // TODO: implement non-blocking timing using millis()
    // The system should sample every 500 ms
    if (millis() - lastMeasurement >= interval){
        lastMeasurement = millis();
    // TODO: read raw value from light sensor
    
        rawValue = saadcRawRead();
        Serial.print(rawValue);
        Serial.println("Sample collected.");
    // TODO: clamp the raw value to a calibrated range
        if (rawValue < 50){
            rawValue = 50;
        }else if(rawValue > 3500){
            rawValue = 3500;
        }
    // TODO: normalize the value to a 0–100 scale
        long mapping = map(rawValue, 50, 3500, 0, 100);
        mapping = constrain(mapping, 0, 100);
        Serial.println(mapping);
    // TODO: determine the category using the categorize() function

    // TODO: print raw value, normalized value, and category to Serial
    }
}

uint16_t saadcRawRead(){
    Serial.println("Reading...");
    NRF_SAADC->EVENTS_END = 0;
    NRF_SAADC->TASKS_START = 1;
    while(NRF_SAADC->EVENTS_STARTED);
    NRF_SAADC->EVENTS_STARTED = 0;
    NRF_SAADC->TASKS_SAMPLE = 1;
    while(NRF_SAADC->EVENTS_END);
    NRF_SAADC->TASKS_STOP = 1;
    Serial.println("Reading finished.");
    return adc_result;
}
 