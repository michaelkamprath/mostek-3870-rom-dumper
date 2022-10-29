#ifndef __MK3870__
#define __MK3870__

class MK3870 {
private:

    void setupPins(void);


public:

    MK3870();
    virtual ~MK3870();

    // resets the MK3870 device. Leaves clock in HIGH state.
    void reset(void);

    void tickClockHigh(void);
    void tickClockLow(void);
    
    // ticks external MK3870 clock once. Leaves clock in HIGH state.
    // Two external clock ticks equals one internal clock tick.
    void tickExternalClock(void);

    // ticks internal clock with two external ticks
    void tickInternalClock(void);

    // sets the TEST pin voltage to the indicated value
    void setTestVoltageOff(void);
    void setTestVoltage7(void);
    void setTestVoltage3p5(void);

    // Control the popwer going to the MK3870
    void powerOff(void);
    void powerOn(void);

    // read the byte value currently on MK3870 port 4
    uint8_t readFromPort4(void);

    // write the byte value to MK3870 port 5
    void writeToPort5(uint8_t value);

    // prepares the MK3870 chip for dumping rom
    void prepareForDump(void);

    // writes bytecode then waits indicated internal ticks
    void writeBytecodeAndTick(uint8_t bytecode, uint8_t internal_ticks);

    // dumps MK3870 ROM to memory pointed to by data_ptr. Return true of successful.
    bool dumpROM(uint16_t rom_bytes, uint8_t* data_ptr, int led_pin);

    bool logClockCycles(int num_cycles, int led_pin);
};

#endif // __MK3870__