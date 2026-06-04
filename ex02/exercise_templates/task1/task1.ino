
// ------------------------------------------------------------
//  Task 1:
//      Write the body of setP026() as specified in the exercise sheet.
//      To toggle at the right frequency, you can use
//      this function in a loop.
// ------------------------------------------------------------

void setup() {
NRF_P0->DIRSET = (1UL << 26); // set pin 26 as Output
}


void loop() {
  // Set freq to 1 Hz
  setP026(true);
  delay(500);
  setP026(false);
  delay(500);
}


void setP026(boolean high) {
  if (high){
    NRF_P0->OUTSET = (1UL << 26); // turn on
  }
  else{
    NRF_P0->OUTCLR = (1UL << 26); // turn off
  }
}