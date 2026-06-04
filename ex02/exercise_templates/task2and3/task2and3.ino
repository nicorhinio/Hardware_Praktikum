
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
  NRF_P0->DIRSET = (1UL << BUZZER_PIN);
  setTimer1Freq();
}


void loop() {

}



void setTimer1Freq() {
  NRF_TIMER1->TASKS_STOP = 1;

    NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;

    NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_32Bit;

    NRF_TIMER1->PRESCALER = 4;     // 1 MHz Timer

    uint32_t compareValue = 1000000UL / (2 * freq);

    NRF_TIMER1->CC[0] = compareValue;

    NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;

    NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Msk;

    NVIC_EnableIRQ(TIMER1_IRQn);

    NRF_TIMER1->TASKS_CLEAR = 1;
    NRF_TIMER1->TASKS_START = 1;

}



void setBuzzerFreq() {

}


extern "C" void TIMER1_IRQHandler() {
  if (NRF_TIMER1->EVENTS_COMPARE[0])
    {
        NRF_TIMER1->EVENTS_COMPARE[0] = 0;

        buzzerState = !buzzerState;

        if (buzzerState)
            NRF_P0->OUTSET = (1UL << BUZZER_PIN);
        else
            NRF_P0->OUTCLR = (1UL << BUZZER_PIN);
    }
}



