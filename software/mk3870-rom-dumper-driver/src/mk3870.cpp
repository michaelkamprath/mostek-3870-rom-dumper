#include <Arduino.h>
#include "MK3870.h"

//
// Heavily inspired by Sean Riddle's implementation at:
//      http://www.seanriddle.com/3870dumper.pbp
//
#define CLOCK_DELAY_US 2

const uint8_t PORT4_PINS[] =  {PIN_PA0, PIN_PA1, PIN_PA2, PIN_PA3, PIN_PA4, PIN_PA5, PIN_PA6, PIN_PA7};
const uint8_t PORT5_PINS[] =  {PIN_PC0, PIN_PC1, PIN_PC2, PIN_PC3, PIN_PC4, PIN_PC5, PIN_PC6, PIN_PC7};

const uint8_t XTAL2_PIN = PIN_PB1;
const uint8_t RESET_PIN = PIN_PB2;
const uint8_t STROBE_PIN = PIN_PB3;

const uint8_t ACTIVATE_TEST_7V_PIN = PIN_PD6;
const uint8_t ACTIVATE_TEST_3p5V_PIN = PIN_PD7;

MK3870::MK3870()
{
    this->setupPins();
    this->setTestVoltageOff();
    this->writeToPort5(0);
}

MK3870::~MK3870()
{

}

void MK3870::setupPins(void)
{
    // read from mk3870 port 4
    for (uint8_t i = 0; i < 8; i++) {
        pinMode(PORT4_PINS[i], INPUT);
    }
    // write to mk3870 port 5
    for (uint8_t i = 0; i < 8; i++) {
        pinMode(PORT5_PINS[i],OUTPUT);
        digitalWrite(PORT5_PINS[i], HIGH);
    }
    // mk3870 control pins
    pinMode(XTAL2_PIN, OUTPUT);
    digitalWrite(XTAL2_PIN, HIGH);
    pinMode(RESET_PIN, OUTPUT);
    digitalWrite(RESET_PIN, HIGH);
    pinMode(STROBE_PIN, INPUT);
    // test pin voltage control
    pinMode(ACTIVATE_TEST_7V_PIN, OUTPUT);
    pinMode(ACTIVATE_TEST_3p5V_PIN, OUTPUT);
    this->setTestVoltageOff();
}

void MK3870::reset(void)
{
    Serial.println(F("  reseting MK3870"));
    digitalWrite(RESET_PIN, LOW);
    for (int i = 0; i < 255; i++){
        this->tickInternalClock();
    }
    digitalWrite(RESET_PIN, HIGH);
}

void MK3870::tickClockHigh(void) {
    digitalWrite(XTAL2_PIN, HIGH);
    delayMicroseconds(CLOCK_DELAY_US);
}

void MK3870::tickClockLow(void) {
    digitalWrite(XTAL2_PIN, LOW);
    delayMicroseconds(CLOCK_DELAY_US);
}
void MK3870::tickExternalClock(void)
{
    this->tickClockLow();
    this->tickClockHigh();
}

void MK3870::tickInternalClock(void)
{
    this->tickExternalClock();
    this->tickExternalClock();
}

void MK3870::setTestVoltageOff(void)
{
    digitalWrite(ACTIVATE_TEST_7V_PIN, LOW);
    digitalWrite(ACTIVATE_TEST_3p5V_PIN, LOW);
    delayMicroseconds(50);
}

void MK3870::setTestVoltage7(void)
{
    this->setTestVoltageOff();
    digitalWrite(ACTIVATE_TEST_7V_PIN, HIGH);
    delayMicroseconds(50);
}

void MK3870::setTestVoltage3p5(void)
{
    this->setTestVoltageOff();
    digitalWrite(ACTIVATE_TEST_3p5V_PIN, HIGH);
    delayMicroseconds(50);
}

uint8_t MK3870::readFromPort4(void)
{
    uint8_t value = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (digitalRead(PORT4_PINS[i]) == HIGH) {
            value |= (1 << i);
        }
    }
    return value;
}

void MK3870::writeToPort5(uint8_t value)
{
    // invert the value since port is active low
    uint8_t inverted_value = ~value;

    for (uint8_t i = 0; i < 8; i++) {
        uint8_t bit_value = LOW;
        if (inverted_value&(1 << i)) {
            bit_value = HIGH;
        }
        digitalWrite(PORT5_PINS[i],bit_value);
    }
}

void MK3870::prepareForDump(void)
{
    Serial.println(F("  preparing for dump"));

    this->writeToPort5(0);
    this->setTestVoltage3p5();
    this->reset();

    // wait for strobe = LOW after reset
    // need to drive clock "manually" here to detect when strobe
    // resets. When it does reset, ensure the external clock is aligned to
    // internal clock (2 to 1 ratio)
    bool strobe_not_found = true;
    bool half_off = false;
    while (strobe_not_found) {
        this->tickClockLow();
        if (digitalRead(STROBE_PIN) == LOW) {
            strobe_not_found = false;
        }

        this->tickClockHigh();
        if (digitalRead(STROBE_PIN) == LOW) {
            strobe_not_found = false;
        }

        this->tickClockLow();
        if (digitalRead(STROBE_PIN) == LOW) {
            if (strobe_not_found) {
                half_off = true;
            }
            strobe_not_found = false;
        }

        this->tickClockHigh();
        if (digitalRead(STROBE_PIN) == LOW) {
            if (strobe_not_found) {
                half_off = true;
            }
            strobe_not_found = false;
        }
    }
    if (half_off) {
        // if we are here, the strobe is 1/2 a clock off. tick it again
        this->tickExternalClock();
    }
}

void MK3870::writeBytecodeAndTick(uint8_t bytecode, uint8_t internal_ticks)
{
    // the byte code should be on the data bus after 1/2 internal clock cycle
    // and then the data bus restored to 0 halfway through the last internal
    // clock cycle.
    //
    // the internal_ticks argument represent the number of internal clock cycles
    // that follows the initial clock cycle that clocked in the byte code onto
    // the data bus.
    this->tickExternalClock();
    this->writeToPort5(bytecode);
    this->tickExternalClock();
    for (uint8_t i = 0; i < internal_ticks-1; i++) {
        this->tickInternalClock();
    }
    this->tickExternalClock();
    this->writeToPort5(0);
    this->tickExternalClock();
}

bool MK3870::dumpROM(uint16_t rom_bytes, uint8_t* data_ptr, int led_pin = -1) {
    if (led_pin >= 0) {
        digitalWrite(led_pin, HIGH);
    }
    this->prepareForDump();
    this->setTestVoltage7();
    this->writeToPort5(0);

    // first, load DCI instructions $2A
    this->tickInternalClock();
    this->writeBytecodeAndTick(0x2A, 3);
    // load MSB of address $0000
    this->writeBytecodeAndTick(0, 9);
    // load LSB of address $0000
    this->writeBytecodeAndTick(0, 9);


    for (uint16_t addr = 0; addr < rom_bytes; addr++) {
        this->writeToPort5(0);
        this->setTestVoltage7();
        // write LM instruction $16
        this->writeBytecodeAndTick(0x16, 3);

        // now read value
        this->setTestVoltage3p5();
        this->tickExternalClock();
        data_ptr[addr] = this->readFromPort4();
        this->tickExternalClock();


        // tick 5 internal clock more times
        this->tickInternalClock();
        this->tickInternalClock();
        this->tickInternalClock();
        this->tickInternalClock();
        this->tickInternalClock();
    }

    // done, reset the MK3870
    this->setTestVoltageOff();
    this->reset();

    if (led_pin >= 0) {
        digitalWrite(led_pin, LOW);
    }

    return true;
}

bool MK3870::logClockCycles(int num_cycles, int led_pin = -1) {
    if (led_pin >= 0) {
        digitalWrite(led_pin, HIGH);
    }

    uint8_t* data = (uint8_t*)malloc(num_cycles*8);

    if (data == nullptr) {
        Serial.println(F("\n\nERROR - could not allocate data buffer"));
        return false;
    }
    this->prepareForDump();
    this->setTestVoltage3p5();

    Serial.print(F("Starting Dump : db = $"));
    Serial.print(this->readFromPort4(),HEX);
    Serial.print(F("\n"));

    // must clock as quickly as possible as the MK3870 gets tripped up with a slow clock.
    // collected data first, then print out later.
    int idx = 0;
    for (int i = 0; i < num_cycles; i++) {
        digitalWrite(XTAL2_PIN, LOW);
        delayMicroseconds(CLOCK_DELAY_US);
        data[idx++] = this->readFromPort4();
        data[idx++] = digitalRead(STROBE_PIN);
        digitalWrite(XTAL2_PIN, HIGH);
        delayMicroseconds(CLOCK_DELAY_US);
        data[idx++] = this->readFromPort4();
        data[idx++] = digitalRead(STROBE_PIN);
        digitalWrite(XTAL2_PIN, LOW);
        delayMicroseconds(CLOCK_DELAY_US);
        data[idx++] = this->readFromPort4();
        data[idx++] = digitalRead(STROBE_PIN);
        digitalWrite(XTAL2_PIN, HIGH);
        delayMicroseconds(CLOCK_DELAY_US);
        data[idx++] = this->readFromPort4();
        data[idx++] = digitalRead(STROBE_PIN);
    }

    idx = 0;
    for (int i = 0; i < num_cycles; i++) {
        Serial.print(F("Clock cycle #"));
        Serial.print(i);
        Serial.print(F("\n"));

        Serial.print(F("  step 0, clock LOW : db = $"));
        Serial.print(data[idx++],HEX);
        Serial.print(F(", strobe = "));
        Serial.print(data[idx++]);
        Serial.print(F("\n"));

        Serial.print(F("  step 1, clock HIGH : db = $"));
        Serial.print(data[idx++],HEX);
        Serial.print(F(", strobe = "));
        Serial.print(data[idx++]);
        Serial.print(F("\n"));


        Serial.print(F("  step 2, clock LOW : db = $"));
        Serial.print(data[idx++],HEX);
        Serial.print(F(", strobe = "));
        Serial.print(data[idx++]);
        Serial.print(F("\n"));

        Serial.print(F("  step 3, clock HIGH : db = $"));
        Serial.print(data[idx++],HEX);
        Serial.print(F(", strobe = "));
        Serial.print(data[idx++]);
        Serial.print(F("\n"));

    }
    this->setTestVoltageOff();
    if (led_pin >= 0) {
        digitalWrite(led_pin, LOW);
    }

    free(data);
    return true;
}