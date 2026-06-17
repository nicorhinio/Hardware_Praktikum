// Task 3: Air Quality Monitoring with SGP30


#include <Adafruit_SGP30.h>

// --- Objects ---
Adafruit_SGP30 sgp;
unsigned long lastMeasurement = 0;
// --- Timing ---
// TODO: define sampling interval (1 second)
const unsigned long interval = 1000;
// --- Warm-up ---
// TODO: define warm-up duration (15 seconds)
const unsigned long warmupInterval = 15000;
unsigned long warmupStartTime = 0;
// TODO: maintain a warm-up state indicator
bool warmupSensorReady = false;

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000);

    // TODO: initialize the sensor and handle initialization failure
    sgp.begin();
    // TODO: start air quality measurements
    warmupStartTime = millis();
    // TODO: record system start time
}

void loop() {
    unsigned long now = millis();

    if((millis() - lastMeasurement) >= interval){
        if (!sgp.IAQmeasure()) {
            Serial.println("SGP30 measurement failed!");
            return;
        }
        lastMeasurement = millis();
        if (!warmupSensorReady){
            Serial.print("UNSTABLE/WARMUP ");
        }
        Serial.print("eCO2: ");
        Serial.print(sgp.eCO2);
        Serial.println("ppm");
        Serial.print("TVOC: ");
        Serial.print(sgp.TVOC);
        Serial.println("ppb");
        if ((now - warmupStartTime) >= warmupInterval && !warmupSensorReady){
            warmupSensorReady = true;
            Serial.println("Warm-Up finished.");
        }else if (!warmupSensorReady){
            Serial.println("Still in Warm-Up...");
        }
    }

    // TODO: implement periodic sampling at 1 Hz using millis()

    // TODO: perform a measurement and handle failure cases

    // TODO: update warm-up state based on elapsed time

    // TODO: output measurement results
    //       - indicate warm-up vs ready state
    //       - ensure correct formatting for integer values
}
