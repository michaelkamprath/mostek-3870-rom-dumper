#include <Arduino.h>
#include "mk3870.h"

#define HEX_CHARS_PER_LINE 16
// local functions
void printAddress(uint16_t addr);
void printByteValue(uint8_t value);

// constants
const uint16_t ROM_BYTES = 2048;
const uint8_t STATUS_LED_PIN = PIN_PB0;

// globals

MK3870 *device = nullptr;
uint8_t *data_ptr = nullptr;

void setup() {
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  Serial.println(F("\n\nMOSTEK 3870 ROM DUMPER\n\n"));

  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  // create device interface
  device = new MK3870();

  // create buffer
  data_ptr = (uint8_t *)malloc(ROM_BYTES);

  if (data_ptr == nullptr) {
    Serial.println(F("ERROR - could not allocate data buffer"));
  }
}

void loop() {
//  digitalWrite(STATUS_LED_PIN, LOW);
  while (Serial.available()) {
    Serial.read();
  }
  Serial.print(F("Press any key to start ROM dump > "));
  while (Serial.available() == 0) {
    ;
  }
  while (Serial.available()) {
    Serial.read();
  }
#if 0
  // Call the code that does only a dump of the MK3870 data bus
  // as it runs through the code on the EEPROM. Useful for 
  // figuring out the instruction timing rewuqirments.
  device->logClockCycles(200,STATUS_LED_PIN);
#else
  Serial.println(F("\nStarting ROM dump ..."));
  if (device->dumpROM(ROM_BYTES, data_ptr, STATUS_LED_PIN)) {
    Serial.println(F("\n\nROM dump"));
    Serial.println(F("================"));
    // print hex data
    int line_char_count = 0;
    for (uint16_t i = 0; i < ROM_BYTES; i++) {
      if (line_char_count == 0) {
        printAddress(i);
        Serial.print(F(":  "));
      }
      else if (line_char_count == 8 ) {
        Serial.print(F(" "));
      }
      printByteValue(data_ptr[i]);
      Serial.print(F(" "));
      line_char_count++;
      if (line_char_count >= HEX_CHARS_PER_LINE) {
        Serial.println(F(""));
        line_char_count = 0;
      }
    }
    Serial.println(F("================\n\n"));
  }
#endif
}

void printByteValue(uint8_t value) {
  if (value <0x0010) {
    Serial.print(F("0"));
  }
  Serial.print(value, HEX);
}

void printAddress(uint16_t addr) {
  if (addr <0x1000) {
    Serial.print(F("0"));
  }
  if (addr <0x0100) {
    Serial.print(F("0"));
  }
  if (addr <0x0010) {
    Serial.print(F("0"));
  }
  Serial.print(addr, HEX);
}