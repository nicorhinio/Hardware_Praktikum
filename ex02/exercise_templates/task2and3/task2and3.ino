
// ------------------------------------------------------------
//  Task 2 and 3:
//      Write the body of setTimer1Freq() as specified in the exercise sheet.
//      This should include timer settings.
//      Implement the ISR in TIMER1_IRQHandler().
//      Make necessary changes for setBuzzerFreq()
// ------------------------------------------------------------

#include <Arduino.h>

int BUZZER_PIN = 29; // GPIO P0.29
int BUTTON_PIN = 3; // GPIO P0.03
bool buzzerState = false;
bool buttonState = false;
uint32_t freq = 1046;


void setup() {
  NRF_P0->DIRSET = (1UL << BUZZER_PIN); // set GPIO as Output
  NRF_P0->PIN_CNF[BUTTON_PIN] = (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) | 
  (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) | 
  (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos); // set Button
  setTimer1Freq();
  NRF_TIMER1->TASKS_STOP = 1; // stop timer, so that no sound is played after setup
}


void loop() {
  bool pressed = !(NRF_P0->IN & (1UL << BUTTON_PIN)); // pressed is true if 1 is read from the button
  if(pressed != buttonState){
    buttonState = pressed;
    if (pressed){
      setBuzzerFreq(1046); // when button is pressed sound is played
    }
    else{
      setBuzzerFreq(0); // when button is not pressed, no sound is played
    }
  }
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
  freq = newFreq; // set new Freq
  if (newFreq == 0){ // Case in loop else -> No sound
    NRF_TIMER1->TASKS_STOP = 1;
    NRF_P0->OUTCLR = (1UL << BUZZER_PIN);
    return; // rest of fuction will not be executed
  }
  uint32_t compareValue = 1000000UL / (2 * freq); // set new compare Value
  NRF_TIMER1->TASKS_STOP = 1;
  NRF_TIMER1->CC[0] = compareValue; // push compare VAlue into Register
  NRF_TIMER1->TASKS_CLEAR = 1;
  NRF_TIMER1->TASKS_START = 1;
}


extern "C" void TIMER1_IRQHandler() // start function if Compare0 is reached
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



