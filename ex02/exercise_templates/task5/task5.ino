
// ------------------------------------------------------------
//  Task 5:
//      Write the body of setTimer2() as specified in the exercise sheet.
//      This should include timer settings.
//      Implement the ISR in TIMER2_IRQHandler().
// ------------------------------------------------------------

#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
volatile uint32_t tCount = 0;


void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Initializing Timer 2 (1ms interval)...");
  setTimer2(true); // enable time with parameter true
}


void loop() {
  static uint32_t lastPrint = 0;  // last time we printed

  if (tCount - lastPrint >= 1000) {  // Compare for print every second
    lastPrint = tCount; // seting last print
    Serial.print("tCount: ");
    Serial.println(tCount);
  }
}


extern "C" void TIMER2_IRQHandler() {
  if (NRF_TIMER2->EVENTS_COMPARE[0]){ // if event was enabled
    NRF_TIMER2->EVENTS_COMPARE[0] = 0; // reset event
    tCount++; // incrementing tCount -> counting the ms
  }
}


void setTimer2(bool enable) {
  NRF_TIMER2->TASKS_STOP = 1;
  if (!enable){ // if enble is false no time will be started
    return;
  }
  NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer; // set correct mode fo rtimer
  NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_32Bit; // set correct bitmode
  NRF_TIMER2->PRESCALER = 4; // set presacaler
  NRF_TIMER2->CC[0] = 1000; // 1MHz / 1000 = 1ms
  NRF_TIMER2->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;// automatix reset for compare
  NRF_TIMER2->INTENSET = TIMER_INTENSET_COMPARE0_Msk; // Interrupt bei Compare0
  NVIC_EnableIRQ(TIMER2_IRQn);
  NRF_TIMER2->TASKS_CLEAR = 1;
  NRF_TIMER2->TASKS_START = 1;
}
