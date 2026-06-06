
// ------------------------------------------------------------
//  Task 7:
//      Use your code from Task 6, adding a parser function to
//      go over any given string and create the array needed to play 
//      it.
//      You are free to use or discard any helper functions and add
//      any helper functions you need for parsing.
// ------------------------------------------------------------

#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <stdlib.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

uint16_t durationsArray[100];
uint16_t notesArray[100];
const char noteNames[] = {'c', 'C', 'd', 'D', 'e', 'f', 'F', 'g', 'G', 'a', 'A', 'b'};
const uint16_t notes[] = {262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494};
char buffer[]="Test:d=4,o=5,b=200:8g,8a,8c6,8a,e6,8p,e6,8p,d6.,8p,8g,8a,8c6,8a,d6,8p,d6,8p,c6,8p,a.,8g,8a,8c6,8a,2c6,d6,b,a,g.,8p,g,2d6,2c6.,p,8g,8a,8c6,8a,e6,8p,e6,8p,d6.,8p,8g,8a,8c6,8a,g6,b,c6,8p,b,8a,p,8g,8a,8c6,8a,2c6,d6,b,a,g,p,g,d6,c6";
volatile uint8_t melodyIdx = 0;
volatile uint32_t tCount = 0;
uint16_t standardDuration = 4;
uint16_t standardOctave = 6;
uint16_t standardBPM = 63;

uint32_t BUZZER_PIN = 29;
bool buzzerState    = false;
uint16_t bufIdx = 0;


String songNotes[100];
uint16_t noteCount = 0;
String songDefaultsStr;
String songNotesStr;

void setup() {
  Serial.begin(115200);
  while(!Serial){
    delay(10);
  }

  u8g2.begin();

  u8g2.setFont(u8g2_font_helvB08_tf);

  u8g2.clearBuffer();

  u8g2.drawStr(0, 20, "Hardware");
  u8g2.drawStr(0, 40, "Praktikum 2026");
  u8g2.sendBuffer();
  delay(3500);

  Serial.println("Starting...");
  NRF_P0->DIRSET = (1UL << BUZZER_PIN);
  String song0 = "";
  String song1 = "Smoke on the Water:d=4,o=4,b=112:c,d#,f.,c,d#,8f#,f,p,c,d#,f.,d#,c";
  String song2 = "GoodSong2:o=5,d=4,b=320,b=320:c,8d,8d,d,2d,c,c,c,c,8d#,8d#,2d#,d,d,d,c,8d,8d,d,2d,c,c,c,c,8d#,8d#,d#,2d#,d,c#,c,c6,1b.,g,f,1g.";
  String song3 = "GoodSong3:o=5,d=8,b=112,b=112:d,d,a,d,e6,d,d6,d,f#,g,c6,f#,g,c6,e,d,d,d,a,d,e6,d,d6,d,f#,g,c6,f#,g,c6,e,d,c,d,a,d,e6,d,d6,d,f#,g,c6,f#,g,c6,e,d,c,d,a,d,e6,d,d6,d,a,d,e6,d,d6";
  String song4 = "Crazy Frog:o=5,d=8,b=125,b=125:16g,16g,a#.,16g,16p,16g,c6,g,f,4g,d6.,16g,16p,16g,d#6,d6,a#,g,d6,g6,16g,16f,16p,16f,d,a#,2g,4p,16f6,d6,c6,a#,4g,a#.,16g,16p,16g,c6,g,f,4g,d6.,16g,16p,16g,d#6,d6,a#,g,d6,g6,16g,16f,16p,16f,d,a#,2g";
  String song5 = "GoodSong5:o=5,d=16,b=100,b=100:g,g,a,a,e,e,8g,g,g,a,a,e,e,8g,g,g,a,a,c6,c6,8b,8b,8a,8g,8f,f,f,g,g,d,d,8f,f,f,g,g,d,d,8f,f,f,g,g,a,b,8c6,8a,8g,8e,4c";
  String song6 = "GoodSong6:o=5,d=8,b=140,b=140:g,e,4p,p,e,f,g,e6,p,e6,p,2c6,g,e,4p,p,e,f,e,g,p,g,p,2f,f,d,4p,p,d,e,f,g,e,4p,p,e,f#,e,d,g,p,e,f#,d,p,a,g.,16f#,g,a,g,f,e,d";
  String song7 = "GoodSong7:o=5,d=8,b=63,b=63:a4,c,e,a,b,e,c,b,c6,e,c,c6,f#,d,a4,f#,e.,16c,a4,4e,c,a4,e,g4,a4,4a4";
  String song8 = "GoodSong8:o=5,d=8,b=200,b=200:g#,4c#,p,4c#6,a#,4g#,4c#,p,4g#,f#,f,f,f#,g#,4c#,4d#,2f,2p,4g#,4c#,p,4c#6,a#,4g#,4c#,p,4g#,f#,f,f,f#,g#,4c#,4d#,2c#";
  String song9 = "GoodSong9:o=4,d=8,b=125,b=125:c6,c6,a#5,c6,p,g5,p,g5,c6,f6,e6,c6,2p,c6,c6,a#5,c6,p,g5,p,g5,c6,f6,e6,c6";
  String song10 = "GoodSong10:o=5,d=8,b=160,b=160:c#6,a#,2p,a#,g#,f#,g#,a#,4c#6,a#,4c#6,d#6,a#,2p,a#,g#,f#,g#,a#,4c#6,a#,4c#6,d#6,b,2p,b,a#,g#,a#,b,4d#6,f#6,4d#6,4f6.,4d#6.,4c#6.,4b.,4a#,4g#";
  String song11 = "GoodSong11:o=5,d=16,b=125,b=125:b,a,4b,4e,4p,8p,c6,b,8c6,8b,4a,4p,8p,c6,b,4c6,4e,4p,8p,a,g,8a,8g,8f#,8a,4g.,f#,g,4a.,g,a,8b,8a,8g,8f#,4e,4c6,2b.,b,c6,b,a,1b";
  parseRTTLSong(song4);
  playMelody();
}


void loop() {

}


void playRTTTL() {

}

void parseRTTLSong(String song){ 
  Serial.println("parseRTTLSong started.");
  size_t firstColon = song.indexOf(':');
  size_t secondColon = song.indexOf(':', firstColon + 1);
  songDefaultsStr = song.substring(firstColon + 1, secondColon); // every helper function is used to fully parse the song
  songNotesStr = song.substring(secondColon + 1);
  String title = song.substring(0, firstColon - 1);
  display_title(title);
  Serial.println(songDefaultsStr);
  Serial.println(songNotesStr);
  Serial.println("parseDefaults starting...");
  parseDefaults(songDefaultsStr);
  Serial.println(standardDuration);
  Serial.println(standardOctave);
  Serial.println(standardBPM);
  Serial.println("parseSongNotes starting...");
  parseSongNotes(songNotesStr);
  for (int j = 0; j < 100; j++){
    Serial.println(songNotes[j]);
  }
  arrayToNote(songNotes, noteCount);
  for (int k = 0; k < 100; k++){
    Serial.println(durationsArray[k]);
    Serial.println(notesArray[k]);
  }
}

void display_title(String title) {
  u8g2.clearBuffer();

  u8g2.setCursor(0, 10);
  u8g2.print("current Song: ");
  u8g2.setCursor(0, 20);
  u8g2.print(title);

  u8g2.sendBuffer();
}

void parseDefaults(String defaults){
  for (int i = 0; i < defaults.length(); i++){
    switch (defaults[i]) {
    case 'd':
      i++;
      i++;
      standardDuration = defaults[i] - '0';
      break;
    case 'o':
      i++;
      i++;
      standardOctave = defaults[i] - '0';
      break;
    case 'b':
      i++;
      i++;
      standardBPM = 0;
      while (i < defaults.length() && isDigit(defaults[i])){
        standardBPM = standardBPM * 10 + (defaults[i] - '0');
        i++;
      }
      i--;
      break;
    default:
      break;
    }
  }
}

void parseSongNotes(String notesStr){
  for (int i = 0; i <= notesStr.length(); i++){
    switch (notesStr[i]) {
    case ',':
      i++;
      break;
    default:
      break;
    }
    int noteStart = i;
    while(notesStr[i] != ',' && i < notesStr.length()){
      i++;
    };
    songNotes[noteCount] = notesStr.substring(noteStart, i);
    noteCount++;
  }
}

void arrayToNote(String array[100], uint16_t count){
  for(int a = 0; a < noteCount; a++){
    String note = array[a];
    uint16_t b = 0;
    uint16_t duration = standardDuration;
    // check for duration in note
    if (b < note.length() && isDigit(note[b])) {
      duration = 0;
      while (b < note.length() && isDigit(note[b])) {
        duration = duration * 10 + (note[b] - '0');
        b++;
      }
    }
    char noteName = note[b]; // note
    b++;
    // check for #
    bool sharp = false;
    if (b < note.length() && note[b] == '#') {
      sharp = true;
      b++;
    }
    // chck for octave
    uint8_t octave = standardOctave;
    if (b < note.length() && isDigit(note[b])) {
      octave = 0;
      while (b < note.length() && isDigit(note[b])) {
        octave = octave * 10 + (note[b] - '0');
        b++;
      }
    }
    
    bool dotted = false;
    if (b < note.length() && note[b] == '.') {
      dotted = true;
      b++;
    }
    uint32_t quarterMs  = 60000UL / standardBPM;
    uint32_t durationMs = (quarterMs * 4) / duration;
    if (dotted) durationMs = durationMs + durationMs / 2;

    durationsArray[a] = (uint16_t)durationMs;

    if (noteName == 'p') {
      notesArray[a] = 0;
    } else {
      notesArray[a] = freqFromNote(noteName, sharp, octave);
    }          
  }
}

uint16_t freqFromNote(char note, bool sharp, uint8_t octave) {
  uint16_t baseFreq = 0; // Basefrequency
  // note c with sharp is true -> C
  // e# -> b
  switch (note){
    case 'c':
      baseFreq = sharp ? 277 : 262;
      break;
    case 'd':
      baseFreq = sharp ? 311 : 294;
      break;
    case 'e':
      baseFreq = 330; 
      break;
    case 'f':
      baseFreq = sharp ? 370 : 349; 
      break;
    case 'g':
      baseFreq = sharp ? 415 : 392; 
      break;
    case 'a':
      baseFreq = sharp ? 466 : 440; 
      break;
    case 'b':
      baseFreq = 494;
      break;
    default:
      return 0;
    }
    
    // adjusting octave if unequal to standard octave
    if (octave > 5){
      baseFreq = baseFreq << (octave - 5);
    }
    else if (octave < 5){
      baseFreq = baseFreq >> (5 - octave);
    }

    return baseFreq;
}


bool isDigit(char c) { 
  return c >= '0' && c <= '9';
}


void setBuzzerFreq(uint32_t freq) {
  if ((freq > 3000) || (freq < 100)){
    NRF_TIMER1->TASKS_STOP = 1;
    NRF_P0->OUTCLR = (1UL << BUZZER_PIN);
    return;
  }
  uint32_t compareValue = 1000000UL / (2 * freq);
  NRF_TIMER1->CC[0] = compareValue; // Compare Value into Compare Register (CC)
  NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk; // Stop Timer and Clear Timer, for compare with index 0 in CC
  NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Msk; // activate Interrupt if Compare0 is reached
  NVIC_EnableIRQ(TIMER1_IRQn);
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

void playMelody() {
  setBuzzerFreq(notesArray[0]);
  setTimer2(true);
  Serial.println("playMelody aufgerufen, Timer gestartet.");
  
}

extern "C" void TIMER2_IRQHandler() {
  if (NRF_TIMER2->EVENTS_COMPARE[0]){
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;
    tCount++;
    Serial.println(tCount);
    if (tCount >= durationsArray[melodyIdx]){
      tCount = 0;
      melodyIdx++;
      Serial.println(tCount);
      Serial.println(melodyIdx);
      if (melodyIdx >= noteCount){
        setTimer2(false);
        setBuzzerFreq(0);
      } else {
        setBuzzerFreq(notesArray[melodyIdx]);
      }
    }
  }
}
