#include <Adafruit_TinyUSB.h>
#include <Wire.h>
#include <U8g2lib.h>


// Task 2 ii.) Constants — fill in the correct values from the datasheet
//             You can also write the answers to the questions here.
//             1.) 0x58 is the Address for SGP30
//             2.) 0x2003
//             3.) 0x2008
//             4.) A Measurement Response is 6 Bytes long. 
//                 It consists out of (One byte per element): CO2eq MSB Data, CO2eq LSB Data, CRC Checksum for CO2eq, TVOC MSB Data, TVOC LSB Data, CRC Checksum for TVOC
// ------------------------------------------------------------

#define SGP30_ADDR       0x58   // 7-bit I2C address of the SGP30 

// Command codes (2 bytes each, MSB first — see datasheet )
#define CMD_INIT_MSB     0x20   //    first byte
#define CMD_INIT_LSB     0x03   //    second byte
#define CMD_MEAS_MSB     0x20
#define CMD_MEAS_LSB     0x08

// Display: air quality range for mapping CO2 to a percentage, you can change these to test more ranges
#define CO2_MIN          400    // ppm — clean outdoor air
#define CO2_MAX          2000   // ppm — poor indoor air quality

uint16_t processed_co2 = 0;
uint16_t processed_tvoc = 0;

unsigned long last_measurement = 0;
unsigned long current = 0;
unsigned long start_up = 15;    // 15 second warm up for sensor
// ------------------------------------------------------------
//  Display constructor
// ------------------------------------------------------------
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);



//  Task 2 iii.) — Helper functions
  // ------------------------------------------------------------
  //  Raw byte storage for the last SGP30 measurement.
  //
  // We use global variables here to keep the function signatures simple. 
  // The cleaner alternatives (pointers or a result struct) use C concepts not yet introduced. 
  // At this scale globals are fine; in a larger project you would avoid them.
  //
  //  The SGP30 sends each 16-bit value as two separate bytes:
  //    MSB — most significant byte  (upper 8 bits of the value)
  //    LSB — least significant byte (lower 8 bits of the value)
  //  A third byte per value is a CRC checksum — we discard it.
  // ------------------------------------------------------------
uint8_t raw_co2_msb,  raw_co2_lsb;   // assign values with sgp30_read()
uint8_t raw_tvoc_msb, raw_tvoc_lsb;  // assign values with sgp30_read()



void sgp30_cmd(uint8_t msb, uint8_t lsb) {
  // ------------------------------------------------------------
  //  sgp30_cmd : send a 2-byte command to the SGP30
  //
  //  The SGP30 expects all commands as two bytes (MSB first).
  //  Example:  sgp30_cmd(CMD_INIT_MSB, CMD_INIT_LSB);
  // ------------------------------------------------------------
  // TODO: open a transmission to SGP30_ADDR,
  //       write msb, write lsb, close the transmission.
  //       
  Wire.beginTransmission(SGP30_ADDR);
  Wire.write(msb);
  Wire.write(lsb);
  Wire.endTransmission();
}


bool sgp30_read(uint8_t n) {
  // ------------------------------------------------------------
  //  sgp30_read : read one measurement(n bytes) from the SGP30.
  //
  //  Call sgp30_cmd() first, wait max measurement duration, 
  //  then call this function.
  //  The 4 raw bytes are stored in the global variables above.
  //  The two CRC bytes are read from the bus and discarded.
  //
  //  Returns true if all 6 bytes were received, false on error.
  // ------------------------------------------------------------
  // TODO: use Wire.requestFrom(SGP30_ADDR, n) to request n bytes.
  //       If the return value is not n, return false immediately. 
  //       The expected return value is in the Datasheet.
  //       Read the n bytes in order, remember wire.read() can only
  //       read one byte at a time. Look out for CRC bytes,  
  //       we don't need to store those.  
  if(Wire.requestFrom(SGP30_ADDR, n) != n){
    return false;
  }
  raw_co2_msb = Wire.read();
  raw_co2_lsb = Wire.read();
  Wire.read();
  raw_tvoc_msb = Wire.read();
  raw_tvoc_lsb = Wire.read();
  Wire.read();
  return true;
}


uint16_t to_uint16(uint8_t msb, uint8_t lsb) {
  // ------------------------------------------------------------
  //  to_uint16 : combine two bytes into one 16-bit value.
  //
  //  A sensor value like CO2 = 450 ppm cannot fit in a single
  //  byte (max 255). The sensor splits it across two bytes:
  //    MSB holds the upper half: 450 >> 8  = 1   (0x01)
  //    LSB holds the lower half: 450 & 0xFF = 194 (0xC2)
  //
  //  To reconstruct the original value:
  //    shift MSB left by 8 bits  →  0x01 becomes 0x0100 (= 256)
  //    OR with LSB               →  0x0100 | 0xC2 = 0x01C2 (= 450)
  //
  //  The cast to uint16_t before shifting is necessary because
  //  uint8_t would overflow when shifted — always cast first.
  //
  // ------------------------------------------------------------
  return ((uint16_t)msb << 8) | lsb;
}



//  Task 3 — Display helper  (optional, but keeps loop() clean)


void display_values(uint16_t co2, uint16_t tvoc) {
  // ------------------------------------------------------------
  //  display_values : show co2 and tvoc on the OLED
  //
  // ------------------------------------------------------------
  // TODO (Task 3): set cursor, print co2 and tvoc values.
  u8g2.clearBuffer();

  u8g2.setCursor(0, 10);
  u8g2.print("eCO2:  ");
  u8g2.print(co2);
  u8g2.print("ppm");
  u8g2.setCursor(0, 20);   
  u8g2.print("TVOC: ");
  u8g2.print(tvoc);
  u8g2.print("ppb");



  // TODO (Task 4): map co2 to pct (0-100), draw a filled bar with
  //                u8g2.drawBox(x, y, width, height).
  //                Bar width  = map(pct, 0, 100, 0, 128)
  //                Remember: constrain pct to [0, 100] before mapping.
  long mapping = map(co2, 400, 2000, 0, 100);
  mapping = constrain(mapping, 0, 100);
  u8g2.setCursor(0, 30);
  u8g2.print(mapping);
  u8g2.print(" %");
  long bar_mapping = map(mapping, 0, 100, 0, 128);
  u8g2.drawBox(0, 35, bar_mapping, 24);

  u8g2.sendBuffer();
}



void setup() {
  Serial.begin(115200);
  while (!Serial);           // Wait for USB Serial connection
  //     Task 2 i.): I2C scanner
  // TODO: Transmit to each available I2C address, print the adress when receiving an ACK.
  //       You can use decimal adresses when sending but convert them to hex when printing them out.
  //       Use Serial.print(address, HEX) to make it easier.
  u8g2.begin();

  u8g2.setFont(u8g2_font_helvB08_tf);

  u8g2.clearBuffer();

  u8g2.drawStr(0, 20, "Hardware");
  u8g2.drawStr(0, 40, "Praktikum 2026");
  u8g2.sendBuffer();
  delay(1000);

  boot_animation();

  Wire.begin();
  uint8_t addr = 8;
  for(addr; addr < 128; addr++){
    Wire.beginTransmission(addr);
    if(Wire.endTransmission() == 0){
      Serial.print("Found Device: ");
      Serial.println(addr, HEX);
    }
  }
  // Found addresses are: 3C; 51; 58
  
  //     Task 2 iv.): Initialise SGP30 
  // TODO: send the init command, wait for initialization
  //       and print out a message.
  sgp30_cmd(CMD_INIT_MSB, CMD_INIT_LSB);
  for(start_up; start_up > 0; start_up--){
    u8g2.clearBuffer();
    u8g2.setDrawColor(2);
    u8g2.setCursor(0, 128);
    u8g2.print(start_up);
    u8g2.sendBuffer();
    delay(1000);
  }
  Serial.println("SPG30 initialized.");
 

  // --- Task 3 i.): Simple display use ---
  // TODO: initialize display, set a font, display "Hardware Praktikum 2026",
  //       and push it to the screen.
  u8g2.setDrawColor(1);
}

void boot_animation() {

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  // --------------------------------------------------
  // 1) Loading Bar Animation
  // --------------------------------------------------

  for (int w = 0; w <= 108; w += 6) {

    u8g2.clearBuffer();

    u8g2.drawFrame(8, 26, 112, 12);

    u8g2.drawBox(10, 28, w, 8);

    u8g2.setCursor(22, 18);
    u8g2.print("Booting...");

    u8g2.sendBuffer();

    delay(35);
  }

  // --------------------------------------------------
  // 2) Smooth title slide-in
  // --------------------------------------------------

  for (int x = 128; x > 8; x -= 6) {

    u8g2.clearBuffer();

    u8g2.setFont(u8g2_font_ncenB08_tr);

    u8g2.drawStr(x, 26, "Hardware");

    u8g2.drawStr(x, 48, "Praktikum");

    u8g2.sendBuffer();

    delay(25);
  }

  // --------------------------------------------------
  // 3) Fade-like blink effect
  // --------------------------------------------------

  for (int i = 0; i < 2; i++) {

    u8g2.clearBuffer();
    u8g2.sendBuffer();
    delay(60);

    u8g2.clearBuffer();

    u8g2.drawStr(8, 26, "Hardware");
    u8g2.drawStr(8, 48, "Praktikum");

    u8g2.sendBuffer();

    delay(60);
  }

  // --------------------------------------------------
  // 4) Final screen
  // --------------------------------------------------

  u8g2.clearBuffer();

  // Outer frame
  u8g2.drawRFrame(0, 0, 128, 64, 4);

  // Title
  u8g2.setFont(u8g2_font_ncenB08_tr);

  u8g2.drawStr(10, 20, "Hardware");

  u8g2.drawStr(10, 40, "Praktikum");

  // Year
  u8g2.setFont(u8g2_font_logisoso16_tr);

  u8g2.drawStr(70, 58, "2026");

  // Decorative line
  u8g2.drawLine(8, 46, 118, 46);

  // Credits
  u8g2.setFont(u8g2_font_5x8_tr);

  u8g2.drawStr(8, 58, "Daniel Zein");
    
  u8g2.drawStr(8, 8, "Nico");


  u8g2.sendBuffer();

  delay(5000);
}

void loop() {
  // --- Task 2 iv.): Send measure command and read response ---
  // TODO: call sgp30_cmd() with the measure command bytes.
  if (millis() - last_measurement >= 50){ //originally value was 1000, changed to 50ms for smoothing
  last_measurement = millis();
  sgp30_cmd(CMD_MEAS_MSB, CMD_MEAS_LSB);
  current = millis();
  while (millis() < current + 12){
    // timing measurement 12 ms
  }
  // TODO: call sgp30_read().
  //       If it returns false, print an error message and return early.
  if (sgp30_read(3) == false){
    Serial.println("Error 404.");
    return;
  }

  // TODO: Reconstruct 16-bit values from the raw bytes ----
  processed_co2 = to_uint16(raw_co2_msb, raw_co2_lsb);
  processed_tvoc = to_uint16(raw_tvoc_msb, raw_tvoc_lsb);

  // TODO: print co2 and tvoc with appropriate labels and units.
  Serial.print("eCO2: ");
  Serial.print(processed_co2);
  Serial.print("ppm  ");
  Serial.print("TVOC: ");
  Serial.print(processed_tvoc);
  Serial.println("ppb");

  // --- Task 3 ii.): Print the sgp30 values on the display
  //                  in addition to the Serial monitor 
  display_values(processed_co2, processed_tvoc);
  // --- Task 4: Map CO2 to a percentage ---
  // TODO: use map() to scale co2 from raw values to 0-100%.
  //       Then use constrain() to make sure the percetange 
  //       doesnt go outside 0-100.
  }
}