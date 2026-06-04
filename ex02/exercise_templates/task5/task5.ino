
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
  setTimer2(true);
}


void loop() {
  static uint32_t lastPrint = 0;  // speichert letzten Ausgabezeitpunkt

  if (tCount - lastPrint >= 1000) {  // alle 1000ms = 1 Sekunde
    lastPrint = tCount;
    Serial.print("tCount: ");
    Serial.println(tCount);
  }
}


extern "C" void TIMER2_IRQHandler() {
  if (NRF_TIMER2->EVENTS_COMPARE[0]){
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;
    tCount++;
  }
}


void setTimer2(bool enable) {
  NRF_TIMER2->TASKS_STOP = 1;
  if (!enable){
    return;
  }
  NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;
  NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_32Bit;
  NRF_TIMER2->PRESCALER = 4;
  NRF_TIMER2->CC[0] = 1000;
  NRF_TIMER2->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;
  NRF_TIMER2->INTENSET = TIMER_INTENSET_COMPARE0_Msk;
  NVIC_EnableIRQ(TIMER2_IRQn);
  NRF_TIMER2->TASKS_CLEAR = 1;
  NRF_TIMER2->TASKS_START = 1;
}
