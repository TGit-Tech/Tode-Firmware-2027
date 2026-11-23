/**********************************************************************************************//**
 * @file    LDevices.h
 * @brief   Local Device Definitions, Interface and Container.
 *          Implementation is independent of RF-DITS.
 *          However; assigned devUID, from DITS, will be the master matching case.
 *          Where the devices naming is stored.
 * @authors<pre>
 *  TGit-Tech     11/2026   Original
 *  </pre>
 * @todo
 *************************************************************************************************/
#pragma once

#include "Config.h"
#include "SETGS.h"        // EEPROM Local Device Settings via BitFields

#define MAXDEVICES 35

//--------------------------------------------------
// Local Device Interface for all Devices.
class LDeviceI {
public:
  LDeviceI(uint8_t _devUID): devUID(_devUID) {}
  virtual ~LDeviceI() {}
  virtual uint16_t Value() = 0;
  virtual bool Value(uint16_t _Value) = 0;
  virtual void Loop() {}
  virtual uint8_t DevUID() { return devUID; }
protected:
  uint8_t devUID = BNONE;  
};
//---------------------------------------------------------------------------------------
// Local Devices Container
class LDevices{
public:
    uint16_t Value(uint8_t _devUID);
    bool Value(uint8_t _devUID, uint16_t _Value);
    bool NewOnOffDevice(uint8_t _devUID, uint8_t _Pin, uint8_t _pnMode, bool _ActiveLow = false);
    bool LoadDevice(uint8_t _devUID, uint8_t _devType);
    void LDevProcLoop();

private:
    LDeviceI* LDevice[MAXDEVICES] = {nullptr};
    char* tmpname; // General use array for DIT get Names
    uint8_t LDidx = 0;
};
//---------------------------------------------------------------------------------------
// Device Definitions
//---------------------------------------------------------------------------------------
class OnOff : public LDeviceI, public SETGSDigital {
public:
  OnOff(uint8_t _devUID): LDeviceI(_devUID), SETGSDigital(_devUID) {}
  uint16_t Value() override {
    bool ActLo = (Pin2Mode()==ACTIVELOWON);
    bool val = digitalRead(Pin1());
    //DBHAAINFO((Pin1()),(val),("OnOff::Value[GET] digitalRead(Pin)=(val)\n"))
    val = (ActLo) ? !val : val;
    return val;
  }
  bool Value(uint16_t _Value) override {
    bool ActLo = (Pin2Mode()==ACTIVELOWON);
    bool val = (_Value != 0); val = (ActLo) ? !val : val;
    if(Pin1Mode()==INPUT || Pin1Mode()==INPUT_PULLUP) { return false; } else { pinMode(Pin1(),Pin1Mode()); }
    digitalWrite(Pin1(),val); 
    //DBHAAINFO((Pin1()),(val),("OnOff::Value[SET] digitalWrite(Pin,val)\n"))
    return true;
  }
};