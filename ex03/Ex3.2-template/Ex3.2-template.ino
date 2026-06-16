// Task 2: Robust Temperature and Humidity Monitoring

#include <DHT.h>
#include <math.h>
#include <Arduino.h>
#include <Adafruit_TinyUSB.h>


// --- Configuration ---
// TODO: define sensor pin and type (DHT11)
#define TEMP_PIN 7
#define SENSOR_TYPE DHT11

// --- Objects ---
// TODO: create DHT sensor instance
DHT dht_sensor(TEMP_PIN, SENSOR_TYPE);
// --- Timing ---
unsigned long lastSample = 0;
// TODO: define sampling interval (2 seconds)
unsigned long lastMeasurement = 0;
const unsigned long interval = 2000;
// --- State Variables ---
// TODO: store last valid temperature and humidity
float lastTemp = 0;
float lastHum = 0;
// TODO: maintain a failure counter
int failureCounter = 0;
// --- Computation ---
//float computeDewPoint(float tempC, float relHum) {
    // TODO: implement Magnus formula using natural logarithm
//}

void setup() {
    Serial.begin(115200);
    while (!Serial){
        delay(50);
    }

    // TODO: initialize sensor
    dht_sensor.begin();
}

void loop() {
    unsigned long now = millis();

    // TODO: implement non-blocking sampling (2 s)
    if (now - lastMeasurement >= interval){
        lastMeasurement = now;


    // TODO: read temperature and humidity
        float humidity = dht_sensor.readHumidity();
        float temperatureCelsius = dht_sensor.readTemperature();

    // TODO: handle invalid readings (NaN)
    // - update failure counter (reset on success)
    // - reuse last valid values if needed
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
    }
    Serial.print("T: ");
    Serial.println(temperatureCelsius);
    Serial.print("H: ");
    Serial.println(humidity);
    Serial.println(" ");
    // TODO: compute dew point, print formatted output. If failure count exceeds threshold print a warning. 
    }
}