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
#include <rf_dits.h>

constexpr uint8_t DT_NOTSET =       0x7F;
constexpr uint8_t DT_ONOFF =        0x7E;    ///< On/Off Switching Device
constexpr uint8_t DT_DIST =         0x7A;    ///< Distance Sensing Device
constexpr uint8_t DT_STSTP3W =      0x79;    ///< Start Stop 3-Wire
constexpr uint8_t DT_ANAINPUT =     0x78;    ///< Analog Device (Pressure, Temperature)
constexpr uint8_t DT_ANAOUTPUT =    0x77;    ///< Analog Output (PWM control)
constexpr uint8_t DT_DLSETPOINT =   0x76;    ///< Dev Logic Control Boundary
constexpr uint8_t DT_DLMATH =       0x75;    ///< Dev Logic Compare / Math Device

// Tode PinModes
constexpr uint8_t ACTIVEHION =      0;
constexpr uint8_t ACTIVELOWON =     1;
constexpr uint8_t UPACTIVEHION =    2;
constexpr uint8_t UPACTIVELOWON =   3;

#define MAXDEVICES 35     // Maximum Local Device pointers

//--------------------------------------------------
// Local Device Interface for all Devices.
class LDeviceI {
public:
  LDeviceI(uint8_t _devUID, const uint8_t _devType): devUID(_devUID), devType(_devType) {}
  virtual ~LDeviceI() {}
  virtual uint16_t Value() = 0;
  virtual bool Value(uint16_t _Value) = 0;
  virtual void Loop() {}
  virtual uint8_t DevUID() { return devUID; }
  uint8_t devAttr = 0;
  const uint8_t devType;
protected:
  uint8_t devUID = BNONE;
  bool pinModesSet = false;
};
//---------------------------------------------------------------------------------------
// Device Definitions
//---------------------------------------------------------------------------------------
class OnOff : public LDeviceI, public SETGSDigital {
public:
  OnOff(uint8_t _devUID): LDeviceI(_devUID,DT_ONOFF), SETGSDigital(_devUID) {}
  uint16_t Value() override {
    if(!pinModesSet) SetPinModes();
    bool ActLo = (Pin1Mode()==ACTIVELOWON||Pin1Mode()==UPACTIVELOWON);
    bool val = digitalRead(Pin1());
    //DBHAAINFO((Pin1()),(val),("OnOff::Value[GET] digitalRead(Pin)=(val)\n"))
    val = (ActLo) ? !val : val;
    return val;
  }
  bool Value(uint16_t _Value) override {
    if(!pinModesSet) SetPinModes();
    if((devAttr & DACCESSMSK)==DRO) return false;
    bool ActLo = (Pin1Mode()==ACTIVELOWON||Pin1Mode()==UPACTIVELOWON);
    bool val = (_Value != 0); val = (ActLo) ? !val : val;
    digitalWrite(Pin1(),val); 
    //DBHAAINFO((Pin1()),(val),("OnOff::Value[SET] digitalWrite(Pin,val)\n"))
    return true;
  }
  void SetPinModes() {
    if((devAttr & DACCESSMSK)==DRO) {
      if(Pin1Mode()==UPACTIVEHION||Pin1Mode()==UPACTIVELOWON) { pinMode(Pin1(),INPUT_PULLUP); }
      else { pinMode(Pin1(),INPUT); }
    } else { pinMode(Pin1(),OUTPUT); }
    pinModesSet = true;
  }
};
//---------------------------------------------------------------------------------------
// Local Devices Container
class LDevices{
public:
    uint16_t Value(uint8_t _devUID);
    bool Value(uint8_t _devUID, uint16_t _Value);
    OnOff* OnOffDevice(uint8_t _devUID, bool _New = false, uint8_t _devAttr = 0);
    bool LoadDevice(uint8_t _devUID, uint8_t _devType, uint8_t _devAttr);
    bool DeleteDevice(uint8_t _devUID);
    void LDevProcLoop();

private:
    LDeviceI* LDevice[MAXDEVICES] = {nullptr};
    char* tmpname; // General use array for DIT get Names
    uint8_t LDidx = 0;
};
