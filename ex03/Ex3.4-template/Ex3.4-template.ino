// Task 4: Smart Plant Monitoring Station

#include <ArduinoBLE.h>
#include <Wire.h>
#include <Adafruit_SGP30.h>
#include <U8g2lib.h>
#include <DHT.h>


// --- Hardware Configuration ---
// TODO: define pins (DHT, light sensor, LED, buzzer)

#define SENSOR_TYPE DHT11

// --- System Constants ---
// TODO: define timing constants for:
// - sensor sampling
// - display refresh (~2 Hz)
// - BLE transmission (1 Hz)
// - warm-up duration (30 s)

uint16_t rawValue = 0;
int16_t adc_result;
unsigned long lightLastMeasurement = 0;
const unsigned long lightInterval = 500;


int TEMP_PIN = 7;
DHT dht_sensor(TEMP_PIN, SENSOR_TYPE);
unsigned long lastSample = 0;
unsigned long dhtLastMeasurement = 0;
const unsigned long dhtInterval = 2000;
float lastTemp = 0;
float lastHum = 0;
int failureCounter = 0;
const float a = 17.62;
const float b = 243.12;
// --- Objects ---
// TODO: initialize display, sensors, BLE service and characteristic

// --- FSM ---
enum SystemState {
    STATE_INIT,
    STATE_HEALTHY,
    STATE_ATTENTION,
    STATE_STRESSED
};

SystemState currentState = STATE_INIT;

// TODO: define timing variables for asynchronous operation

// TODO: define variables for sensor data storage

// TODO: helper function(s), e.g.:
// - state to string conversion
// - value normalization (light)

void setup() {
    Serial.begin(115200);

    while(!Serial){
        delay(10);
    }
    // TODO: initialize hardware (pins, display, sensors)
    Serial.println("Starting...");

    NRF_SAADC->ENABLE = SAADC_ENABLE_ENABLE_Enabled;
    NRF_SAADC->RESOLUTION = SAADC_RESOLUTION_VAL_12bit;
    NRF_SAADC->CH[0].PSELP = SAADC_CH_PSELP_PSELP_AnalogInput0;
    NRF_SAADC->CH[0].CONFIG = (SAADC_CH_CONFIG_GAIN_Gain1_6 << SAADC_CH_CONFIG_GAIN_Pos) | (SAADC_CH_CONFIG_REFSEL_Internal << SAADC_CH_CONFIG_REFSEL_Pos) | (SAADC_CH_CONFIG_TACQ_10us << SAADC_CH_CONFIG_TACQ_Pos);
    NRF_SAADC->RESULT.PTR = (uint32_t)&adc_result;
    NRF_SAADC->RESULT.MAXCNT = 1;
    Serial.println("SAADC configurated.");

    dht_sensor.begin();
    delay(2000);

    // TODO: initialize BLE and start advertising

    // TODO: store system start time (for warm-up)
}

void loop() {
    unsigned long now = millis();

    // TODO: maintain BLE stack (if required)

    // i) TODO: asynchronous sensor acquisition (light, DHT, SGP30)

    //Light Sensor Measurement.
    if (now - lightLastMeasurement >= lightInterval){
        lightLastMeasurement = now;
    
        rawValue = saadcRawRead();
        Serial.print("raw value: ");
        Serial.println(rawValue);

        if (rawValue < 50){
            rawValue = 50;
        }else if(rawValue > 3500){
            rawValue = 3500;
        }
        long mapping = map(rawValue, 50, 3500, 0, 100);
        mapping = constrain(mapping, 0, 100);

    }
    // Temp and Humidity Measurement.
    if (now - dhtLastMeasurement >= dhtInterval){
        dhtLastMeasurement = now;

        float humidity = dht_sensor.readHumidity();
        float temperatureCelsius = dht_sensor.readTemperature();

    if (isnan(temperatureCelsius) || isnan(humidity)){
        failureCounter++;
        if(isnan(temperatureCelsius)){
            temperatureCelsius = lastTemp;
        } 
        if(isnan(humidity)){
            humidity = lastHum;
        }
    } else{
        lastTemp = temperatureCelsius;
        lastHum = humidity;
        failureCounter = 0;
    }

    if (failureCounter >= 5){
        Serial.print("[WARNING] DHT failures: ");
        Serial.println(failureCounter);
    }
    float currentDewPoint = computeDewPoint(temperatureCelsius, humidity);

    // Air Quality Measurement.


    // iii) TODO: warm-up handling (STATE_INIT for 30 s)

    // iii) TODO: compute health score (0–100)

    // iii) TODO: implement state transitions (including critical override)

    // iv) TODO: update OLED display (~2 Hz)

    // v) TODO: implement LED and buzzer behavior (non-blocking)

    // vi) TODO: send BLE telemetry (formatted string, 1 Hz)
}

uint16_t saadcRawRead(){
    NRF_SAADC->EVENTS_END = 0;
    NRF_SAADC->TASKS_START = 1;
    while(!NRF_SAADC->EVENTS_STARTED);
    NRF_SAADC->EVENTS_STARTED = 0;
    NRF_SAADC->TASKS_SAMPLE = 1;
    while(!NRF_SAADC->EVENTS_END);
    NRF_SAADC->TASKS_STOP = 1;
    return adc_result;
}

const char* categorize(int normalized) {
    if (normalized < 30){
        return "LOW";
    }else if(normalized < 70){
        return "MEDIUM";
    }else{
        return "HIGH";
    }
}

float computeDewPoint(float tempC, float relHum) {
    float gamma = ((a * tempC) / (b + tempC)) + log(relHum / 100);
    float dewPoint = (b * gamma) / (a - gamma);
    return dewPoint;
}