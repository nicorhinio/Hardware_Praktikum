#include <Adafruit_TinyUSB.h>
#define BLINK_INTERVAL_MS  1000UL  // 1 second on, 1 second off → 1 Hz
//#define NEW_LED_RED 8
//#define NEW_LED_GREEN 9
//#define NEW_LED_BLUE 10

// ------------------------------------------------------------
//  Global variables for Task 1 ii) and beyond
//  Use the smallest type that fits:
//    unsigned long — required for millis() values (32-bit)
//    uint8_t       — sufficient for a cycle/colour counter (8-bit, max 255)
// ------------------------------------------------------------
unsigned long last_blink_ms = 0;  // timestamp of the last LED toggle
bool          led_on        = false;// current LED state

int color = 0; // 0 = red, 1 = green, 2 = blue
int blink_count = 0;
// TODO (Bonus): add a counter for completed blink cycles
//               and a variable to track the current colour

void colorSelector(int c){
  // turn all off
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);

  if (c == 0){
    digitalWrite(LED_RED, LOW);
  }
  if (c == 1) {
    digitalWrite(LED_GREEN, LOW);
  }
  if (c == 2) {
    digitalWrite(LED_BLUE, LOW);
  }

}

void setup() {
  Serial.begin(115200);
  unsigned long start = millis();
  while (!Serial && millis() - start < 3000) {
    // waiting 3 seconds max
  }
  // Set LED pins as outputs.
  // pinMode(pin, OUTPUT) configures a pin for digital output
  // Start with all LEDs off (active-low → HIGH = off)
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_RED, HIGH); // active-low -> LED off
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
}


void loop() {

  // ----------------------------------------------------------
  //  Task 1 i) — Blink using delay()
  //  Comment this section out once you move on to Task 1 ii).
  // ----------------------------------------------------------
  
  //digitalWrite(LED_RED, LOW);
  //led_on = true;
  //delay(1000);
  //digitalWrite(LED_RED, HIGH);
  //led_on = false;
  //delay(1000);

  // ----------------------------------------------------------
  //  Task 1 ii) — Blink using millis()  (comment out i) first)
  //
  // TODO: get the current time with millis().
  // TODO: check whether BLINK_INTERVAL_MS has elapsed since last_blink_ms.
  // TODO: if yes — update last_blink_ms, toggle led_on,
  //               and write the correct HIGH/LOW to LED_RED.

  unsigned long current = millis();

  if ((current - last_blink_ms) >= BLINK_INTERVAL_MS) {
  last_blink_ms = current;

  led_on = !led_on;

  if (led_on) {
    colorSelector(color);
  } else {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);

    blink_count++;

    if (blink_count % 10 == 0) {
      color = (color + 1) % 3;
    }
  }
  }


  // ----------------------------------------------------------
  //  Task 1 iii) — Answer as a comment
  // ----------------------------------------------------------
  // Answer: Der Vorteil von ii) liegt darin, dass das Programm nicht unterbrochen wird. Somit läuft das Programm durchgehen und
  //         es kommt zu keinem blockieren. In dem kleinen Bespiel war das noch kein all zu großes Problem, aber wenn ein Programm mehrere
  //         Aufgaben hat, würde das Delay alle anderen mitblockieren, wohingegen millis() weiter läuft und in jeder schleife prüft, ob es
  //         weiter arbeiten kann:
  // ----------------------------------------------------------
  //  Bonus — Colour cycling after 10 blink cycles
  // ----------------------------------------------------------
  // TODO: count completed blink cycles (one cycle = on + off).
  //       Every 10 cycles, switch to the next colour:
  //         red → green → blue → red → ...
  //       Turn off all LEDs before switching, then turn on only
  //       the new active colour.
}