// Task 1: Environmental Light Categorization
// Complete the implementation according to the task description.

// TODO: define necessary constants (e.g., calibration range, thresholds, timing)

// TODO: implement a function that categorizes the normalized value
const char* categorize(int normalized) {
    // TODO: return "LOW", "MEDIUM", or "HIGH"
}

void setup() {
    // TODO: initialize Serial communication
    Serial.begin(115200);
    while(!Serial){
        delay(10);
    }
    // TODO: configure ADC resolution to 12-bit
    SAADC_ENABLE = 1;
    SAADC_CH[7].CONFIG = 1;

}

void loop() {
    // TODO: implement non-blocking timing using millis()
    // The system should sample every 500 ms

    // TODO: read raw value from light sensor

    // TODO: clamp the raw value to a calibrated range

    // TODO: normalize the value to a 0–100 scale

    // TODO: determine the category using the categorize() function

    // TODO: print raw value, normalized value, and category to Serial
}