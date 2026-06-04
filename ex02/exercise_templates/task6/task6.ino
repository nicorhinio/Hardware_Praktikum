
// ------------------------------------------------------------
//  Task 6:
//      Use your code from Task 4 and Task 5, and make necessary changes. 
//      Implement the function playMelody() according to the exercise sheet.
//      The array is already filled with the right frequency for each note.
// ------------------------------------------------------------

#include <Arduino.h>
uint16_t durations[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t notes[10]     = {262, 294, 330, 349, 392, 440, 494, 523, 587, 659};
volatile uint32_t tCount = 0;
volatile uint8_t melodyIdx = 0;


void setup() {
  setTimer2(true);
  playMelody();
}


void loop() {

}


void playMelody() {

}

void setBuzzerFreq(uint32_t newFreq) {
  freq = newFreq;
  if ((newFreq > 3000) || (newFreq < 100)){
    NRF_TIMER1->TASKS_STOP = 1;
    NRF_P0->OUTCLR = (1UL << BUZZER_PIN);
    return;
  }
  uint32_t compareValue = 1000000UL / (2 * freq);
  NRF_TIMER1->TASKS_STOP = 1;
  NRF_TIMER1->CC[0] = compareValue;
  NRF_TIMER1->TASKS_CLEAR = 1;
  NRF_TIMER1->TASKS_START = 1;
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