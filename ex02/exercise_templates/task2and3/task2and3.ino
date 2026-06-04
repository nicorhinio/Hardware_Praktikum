
// ------------------------------------------------------------
//  Task 2 and 3:
//      Write the body of setTimer1Freq() as specified in the exercise sheet.
//      This should include timer settings.
//      Implement the ISR in TIMER1_IRQHandler().
//      Make necessary changes for setBuzzerFreq()
// ------------------------------------------------------------

#include <Arduino.h>

int BUZZER_PIN = 29;
bool buzzerState = false;
uint32_t freq = 1046;


void setup() {
  NRF_P0->DIRSET = (1UL << BUZZER_PIN); // set GPIO as Output
  setTimer1Freq();
}


void loop() {

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



void setBuzzerFreq() {

}


extern "C" void TIMER1_IRQHandler() // start function if Copare0 is reached
{
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



