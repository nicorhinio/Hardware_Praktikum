
// ------------------------------------------------------------
//  Task 6:
//      Use your code from Task 4 and Task 5, and make necessary changes. 
//      Implement the function playMelody() according to the exercise sheet.
//      The array is already filled with the right frequency for each note.
// ------------------------------------------------------------

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

uint16_t durations[10] = {1000, 2000, 1000, 2000, 1000, 2000, 1000, 2000, 1000, 2000};
uint16_t notes[10]     = {262, 294, 330, 349, 392, 440, 494, 523, 587, 659};
volatile uint32_t tCount = 0;
volatile uint8_t melodyIdx = 0;
uint16_t length = 0;
uint32_t freq = 1046;
uint32_t BUZZER_PIN = 29;
bool buzzerState = false;


void setup() {
  NRF_P0->DIRSET = (1UL << BUZZER_PIN);
  Serial.begin(115200);
  Serial.print("Melody is played.");
  playMelody();
}


void loop() {

}


void playMelody() {
  setBuzzerFreq(notes[0]);
  setTimer2(true);
  setTimer1Freq();
  Serial.println("playMelody aufgerufen, Timer gestartet.");
  
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
  NVIC_EnableIRQ(TIMER1_IRQn);
  NRF_TIMER1->TASKS_CLEAR = 1;
  NRF_TIMER1->TASKS_START = 1;
}

extern "C" void TIMER2_IRQHandler() {
  if (NRF_TIMER2->EVENTS_COMPARE[0]){
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;
    tCount++;
    Serial.println(tCount);
    if (tCount >= durations[melodyIdx]){
      tCount = 0;
      melodyIdx++;
      Serial.println(tCount);
      Serial.println(melodyIdx);
      if (melodyIdx >= 10){
        setTimer2(false);
        setBuzzerFreq(0);
      } else {
        setBuzzerFreq(notes[melodyIdx]);
      }
    }
  }
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