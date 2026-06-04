
// ------------------------------------------------------------
//  Task 4:
//      Make necessary changes to your code from Task 3. Provided tests
//      do not cover all cases.
// ------------------------------------------------------------

#include <Arduino.h>

int BUZZER_PIN = 29;
bool buzzerState = false;
uint32_t freq = 1046;

void setup() {
  NRF_P0->DIRSET = (1UL << BUZZER_PIN); // set GPIO as Output
  setTimer1Freq();
  NRF_TIMER1->TASKS_STOP = 1; // stop timer, so that no sound is played after setup

}


void loop() {
// tests
  setBuzzerFreq(440);  //A4
  delay(300);
  setBuzzerFreq(554);  //C#5
  delay(300);
  setBuzzerFreq(659);  //E5
  delay(300);
  setBuzzerFreq(880);  //A5
  delay(500);
  setBuzzerFreq(5000); //out of range
  delay(3000);
  for (int f = 50; f <= 3000; f += 50) {
    setBuzzerFreq(f);
    delay(20);}
}

void setTimer1Freq() {
  NRF_TIMER1->TASKS_STOP = 1; // stop Timer
  NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer; // select normal mode of timer

  NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_32Bit; // Timer from 0 - 2^32 instead of 0 - 2^16
  NRF_TIMER1->PRESCALER = 4;     // adapt tickrate -> 16 MHz / 2^PRESCALER = 1MHz

  uint32_t compareValue = 1000000UL / (2 * freq); // set Compare Value

  NRF_TIMER1->CC[0] = compareValue; // Compare Value into Compare Register (CC)
  NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk; // Stop Timer and Clear Timer, for compare with index 0 in CC
  
  NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Msk; // activate Interrupt if Compare0 is reached
  NVIC_EnableIRQ(TIMER1_IRQn); // react to interrupt

  NRF_TIMER1->TASKS_CLEAR = 1; // reset Timer -> activate clear -> Timer = 0
  NRF_TIMER1->TASKS_START = 1; // start Timer
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