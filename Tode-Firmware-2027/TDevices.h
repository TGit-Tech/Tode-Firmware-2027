#pragma once

class TDeviceI {
public:
  TDeviceI(uint8_t _devUID) : devUID(_devUID) {

  }
  virtual uint16_t Value() = 0;
  virtual bool Value(uint16_t _Value) = 0;
  virtual void Loop() {}
  virtual uint8_t DevUID() { return devUID; }
protected:
  uint8_t devUID = BNONE;
};
//---------------------------------------------------------------------------------------
class OnOff : public TDeviceI {
public:
  OnOff(uint8_t _devUID): TDeviceI(_devUID), devsetgs(_devUID) {}
  uint16_t Value() override {
    pinMode(devsetgs.Pin1(), devsetgs.Pin1Mode());
    bool val = digitalRead(devsetgs.Pin1());
    return (devsetgs.Pin2Mode()==ACTIVELOWON) ? !val : val;
  }
  bool Value(uint16_t _Value) override {
    if(devsetgs.Pin1Mode()==INPUT) return false;
    if (devsetgs.Pin2Mode()==ACTIVELOWON) { if(_Value==0) { _Value=1;} else { _Value=0; } }
    digitalWrite(devsetgs.Pin1(),_Value); return true;
  }
protected:
  SETGSDigital devsetgs;
};
//---------------------------------------------------------------------------------------
class TDevices {
public:
  TDevices() {}
  Loop() {
    for(uint8_t i=0; i<DevIdx; i++) {
      if(Device[i]!=nullptr) {
        Device[i]->Loop();
      }
    }
  }
  bool AddDevice(uint8_t _devUID, uint8_t _devType) { 
    DBHAAINFO((_devUID,HEX),(_devType,HEX),("TDevices::AddDevice(devUID,devType)\n"))
    if(DevIdx>=10) return false;
    switch(_devType) {
      case DT_NOTSET: break;
      case DT_ONOFF: Device[DevIdx] = new OnOff(_devUID); break;
      case DT_DIST: break;
      case DT_STSTP3W: break;
      case DT_ANAINPUT: break;
      case DT_ANAOUTPUT: break;
      case DT_DLSETPOINT: break;
      case DT_DLMATH: break;
      if(Device[DevIdx]==nullptr) return false;
      DevIdx++; return true;
    }
  }
  bool DeviceSet(byte _DevUID, int _Value) {
    for(uint8_t i=0; i<DevIdx; i++) {
      if(Device[i]!=nullptr) { if(Device[i]->DevUID()==_DevUID) Device[i]->Value(_Value); }
    }
  }
  uint16_t DeviceGet(byte _DevUID) {
    for(uint8_t i=0; i<DevIdx; i++) {
      if(Device[i]!=nullptr) { if(Device[i]->DevUID()==_DevUID) return Device[i]->Value(); }
    }
    return INONE;    
  }

protected:
  TDeviceI* Device[10] = {nullptr};
private:
  uint8_t DevIdx = 0;
};
