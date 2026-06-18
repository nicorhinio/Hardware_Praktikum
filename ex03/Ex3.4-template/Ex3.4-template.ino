// Task 4: Smart Plant Monitoring Station

#include <ArduinoBLE.h>
#include <Wire.h>
#include <Adafruit_SGP30.h>
#include <U8g2lib.h>
#include <DHT.h>


// --- Hardware Configuration ---
// TODO: define pins (DHT, light sensor, LED, buzzer)

#define SENSOR_TYPE DHT11
#define TEMP_PIN  D7


// --- System Constants ---
// TODO: define timing constants for:
// - sensor sampling
// - display refresh (~2 Hz)
// - BLE transmission (1 Hz)
// - warm-up duration (30 s)
unsigned long ledLastMeasurement = 0;
bool ledState = false;

uint16_t rawValue = 0;
int16_t adc_result;
unsigned long lightLastMeasurement = 0;
const unsigned long lightInterval = 500;
uint16_t lastMapping = 0;


DHT dht_sensor(TEMP_PIN, SENSOR_TYPE);
unsigned long dhtLastMeasurement = 0;
const unsigned long dhtInterval = 2000;
float lastTemp = 0;
float lastHum = 0;
int failureCounter = 0;

Adafruit_SGP30 sgp;
unsigned long sgpLastMeasurement = 0;
const unsigned long sgpInterval = 1000;
uint16_t last_eCO2 = 0;

const unsigned long warmupInterval = 30000;
unsigned long warmupStartTime = 0;
bool warmupSensorReady = false;

// --- Objects ---
// TODO: initialize display, sensors, BLE service and characteristic

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// only print if nessecary
float shownTemp = -999;
float shownHum = -999;
uint16_t shownLight = 65535;
uint16_t shownCO2 = 65535;

// --- FSM ---
enum SystemState {
    STATE_INIT,
    STATE_HEALTHY,
    STATE_ATTENTION,
    STATE_STRESSED
};

SystemState currentState = STATE_INIT;
SystemState shownState = STATE_INIT;

void healthEvaluation(float temp, float hum, uint16_t light, uint16_t eCO2){
    int points = 0;
    if(temp >= 18 && temp <= 30){
        points += 25;
    }
    if (hum >= 30 && hum <= 75){
        points += 25;
    }
    if (light <= 90 && light >= 25){
        points += 25;
    }
    if (eCO2 < 1200){
        points += 25;
    }
    else if (eCO2 > 2200){
        currentState = STATE_STRESSED;
        setBuzzerFreq(1000);
        return;
    }
    if (points >= 75){
        currentState = STATE_HEALTHY;
    } else if(points >= 50){
        currentState = STATE_ATTENTION;
    } else{
        currentState = STATE_STRESSED;
    }
}


void setBuzzerFreq(uint32_t freq) {
  if ((freq > 3000) || (freq < 100)){
    NRF_TIMER1->TASKS_STOP = 1;
    NRF_P0->OUTCLR = (1UL << BUZZER_PIN);
    return;
  }
  uint32_t compareValue = 1000000UL / (2 * freq);
  NRF_TIMER1->CC[0] = compareValue; // Compare Value into Compare Register (CC)
  NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk; // Stop Timer and Clear Timer, for compare with index 0 in CC
  NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Msk; // activate Interrupt if Compare0 is reached
  NVIC_EnableIRQ(TIMER1_IRQn);
  NRF_TIMER1->TASKS_CLEAR = 1;
  NRF_TIMER1->TASKS_START = 1;
}


extern "C" void TIMER1_IRQHandler() {
  if (NRF_TIMER1->EVENTS_COMPARE[0]) // check if compare0 was activated
    {
        NRF_TIMER1->EVENTS_COMPARE[0] = 0; // reset Event

        buzzerState = !buzzerState; // switch buzzerstate

        if (buzzerState)
            NRF_P0->OUTSET = (1UL << BUZZER_PIN);
        else
            NRF_P0->OUTCLR = (1UL << BUZZER_PIN);
    }
}

// TODO: define timing variables for asynchronous operation

// TODO: define variables for sensor data storage

// TODO: helper function(s), e.g.:
// - state to string conversion
// - value normalization (light)
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

void displayValues(){
    u8g2.setFont(u8g2_font_helvB08_tf);
    u8g2.clearBuffer();
    u8g2.setCursor(0, 10);
    u8g2.print("Temperature:  ");
    u8g2.print(lastTemp);
    u8g2.print("C");
    u8g2.setCursor(0, 25);   
    u8g2.print("Humidity: ");
    u8g2.print(lastHum);
    u8g2.print("%");
    u8g2.setCursor(0, 40);    
    u8g2.print("Light Level: ");
    u8g2.print(lastMapping);
    u8g2.print("%");
    u8g2.setCursor(0, 55);    
    u8g2.print("eCO2: ");
    u8g2.print(last_eCO2);
    u8g2.print("ppm");

    u8g2.setCursor(75, 55);
    switch (currentState) {
    case (STATE_HEALTHY):{
        u8g2.print("HEALTHY");
        u8g2.setFont(u8g2_font_unifont_t_symbols);
        u8g2.drawUTF8(75, 40, "☺");
        break;

    }
    case (STATE_ATTENTION):{
        u8g2.print("ATTENTION");
        u8g2.setFont(u8g2_font_unifont_t_symbols);
        u8g2.drawUTF8(75, 40, "☹");
        break;
    }
    case (STATE_STRESSED):{
        u8g2.print("STRESSED");
        u8g2.setFont(u8g2_font_unifont_t_symbols);
        u8g2.drawUTF8(75, 40, "☠");
        break;
    }
    }
    u8g2.sendBuffer();
}

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000);

    // TODO: initialize hardware (pins, display, sensors)
    Serial.println("Starting...");

    u8g2.begin();

    u8g2.setFont(u8g2_font_helvB08_tf);

    u8g2.clearBuffer();

    u8g2.drawStr(0, 20, "Hardware");
    u8g2.drawStr(0, 40, "Praktikum 2026");
    u8g2.drawStr(0, 60, "Warming up...");
    u8g2.sendBuffer();

    NRF_SAADC->ENABLE = SAADC_ENABLE_ENABLE_Enabled;
    NRF_SAADC->RESOLUTION = SAADC_RESOLUTION_VAL_12bit;
    NRF_SAADC->CH[0].PSELP = SAADC_CH_PSELP_PSELP_AnalogInput0;
    NRF_SAADC->CH[0].CONFIG = (SAADC_CH_CONFIG_GAIN_Gain1_6 << SAADC_CH_CONFIG_GAIN_Pos) | (SAADC_CH_CONFIG_REFSEL_Internal << SAADC_CH_CONFIG_REFSEL_Pos) | (SAADC_CH_CONFIG_TACQ_10us << SAADC_CH_CONFIG_TACQ_Pos);
    NRF_SAADC->RESULT.PTR = (uint32_t)&adc_result;
    NRF_SAADC->RESULT.MAXCNT = 1;
    Serial.println("SAADC configurated.");

    dht_sensor.begin();

    sgp.begin();
    warmupStartTime = millis();

    // delay(2000); -> warm-up time globally defined, sgp sensor has longest warm-up time, incorrect values are declared as unstable, no need for delay() warm-up.

    // TODO: initialize BLE and start advertising

    // TODO: store system start time (for warm-up)
}

void loop() {
    unsigned long now = millis();

    // TODO: maintain BLE stack (if required)

    // i) TODO: asynchronous sensor acquisition (light, DHT, SGP30)

    //Warm-up: as long as one sensor unstable, every output is considered unstable.
    bool warmingUp = (now - warmupStartTime < warmupInterval);
    if ((now - warmupStartTime) >= warmupInterval && !warmupSensorReady){
        warmupSensorReady = true;
        Serial.println("Warm-Up finished.");
    }



    // Air Quality Measurement.
    if((now - sgpLastMeasurement) >= sgpInterval){
        if (!sgp.IAQmeasure()) {
            Serial.println("SGP30 measurement failed!");
            return;
        }
        sgpLastMeasurement = now;
        last_eCO2 = sgp.eCO2;
        if (!(currentState == STATE_INIT)){
        healthEvaluation(lastTemp, lastHum, lastMapping, last_eCO2);
        }
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
    }

    //Light Sensor Measurement.
    if (now - lightLastMeasurement >= lightInterval){
        //lightLastMeasurement = now;
    
        rawValue = saadcRawRead();

        if (rawValue < 50){
            rawValue = 50;
        }else if(rawValue > 3500){
            rawValue = 3500;
        }
        long mapping = map(rawValue, 50, 3500, 0, 100);
        mapping = constrain(mapping, 0, 100);
        lastMapping = mapping;
    }


    if (now - lightLastMeasurement >= lightInterval){
        lightLastMeasurement = now;
        if (warmingUp){
            currentState = STATE_INIT;
            Serial.println("Warming up...");
        }else{
            bool changed =
            lastTemp != shownTemp ||
            lastHum != shownHum ||
            lastMapping != shownLight ||
            last_eCO2 != shownCO2 ||
            currentState != shownState;

            if (changed) {
                displayValues();

                shownTemp = lastTemp;
                shownHum = lastHum;
                shownLight = lastMapping;
                shownCO2 = last_eCO2;
                shownState = currentState;
            }
        }
    }

    if(currentState == STATE_HEALTHY){
        digitalWrite(LED_GREEN, LOW);
    }
    else if (currentState == STATE_ATTENTION && now - ledLastMeasurement >= 500){
        digitalWrite(LED_YELLOW, ledState ? HIGH : LOW);
        ledState = !ledState;
        ledLastMeasurement = now;
    }
    else if (currentState == STATE_STRESSED && now - ledLastMeasurement >= 250){
        digitalWrite(LED_RED, ledState ? HIGH : LOW);
        ledState = !ledState;
        ledLastMeasurement = now;
    }

    // iii) TODO: warm-up handling (STATE_INIT for 30 s)

    // iii) TODO: compute health score (0–100)

    // iii) TODO: implement state transitions (including critical override)

    // iv) TODO: update OLED display (~2 Hz)
    
    // v) TODO: implement LED and buzzer behavior (non-blocking)

    // vi) TODO: send BLE telemetry (formatted string, 1 Hz)
    
}

