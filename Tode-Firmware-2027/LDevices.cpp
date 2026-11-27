// LDevices.cpp
#include "LDevices.h"

uint16_t LDevices::Value(uint8_t _devUID) {
  for(uint8_t i=0;i<LDidx;i++) {
    if(LDevice[i]==nullptr) continue;
    if(LDevice[i]->DevUID()==_devUID) { 
      uint16_t RtnValue = LDevice[i]->Value();
      //DBHAAINFO((_devUID,HEX),(RtnValue),("DITDevices::Value[GET](DevUID)(RtnValue)\n"))
      return RtnValue;
    }
  }
}
bool LDevices::Value(uint8_t _devUID, uint16_t _Value) {
  for(uint8_t i=0;i<LDidx;i++) {
    if(LDevice[i]==nullptr) continue;
    if(LDevice[i]->DevUID()==_devUID) { 
      bool RtnValue = LDevice[i]->Value(_Value);
      //DBHAAAINFO((_devUID,HEX),(_Value),(RtnValue),("DITDevices::Value(DevUID,[SET]Value)(RtnValue)\n"))
      return RtnValue;
    }
  }
}
bool LDevices::NewOnOffDevice(uint8_t _devUID, uint8_t _Pin, uint8_t _pnMode, bool _ActiveLow) {
  DBHAENTER((_devUID,HEX),("LDevices::NewOnOffDevice(devUID,devType)\n"))
  if (LDidx >= MAXDEVICES) return false;
  OnOff* pDevice = new OnOff(_devUID);
  pDevice->Pin1Mode(_pnMode);
  pDevice->Pin1(_Pin);
  pDevice->Pin2Mode(_ActiveLow);
  for(uint8_t i=0;i<LDidx;i++) { 
    if(LDevice[i]==nullptr) { LDevice[i] = pDevice; return true; }
  }
  LDevice[LDidx] = pDevice;
  LDidx++;
  return true;
}
bool LDevices::LoadDevice(uint8_t _devUID, uint8_t _devType) {
  DBHAAENTER((_devUID,HEX),(_devType,HEX),("LDevices::LoadDevice(devUID,devType)\n"))
  if(LDidx >= MAXDEVICES) return false;
  switch(_devType) {
    case DT_ONOFF:    LDevice[LDidx] = new OnOff(_devUID); break;
    case DT_DIST:     break;
    case DT_STSTP3W:  break;
    case DT_ANAINPUT: break;
    case DT_ANAOUTPUT: break;
    case DT_DLSETPOINT: break;
    case DT_DLMATH:   break;
    default:          return false;
  }
  if(LDevice[LDidx] == nullptr) return false;
  LDidx++;
  return true;
}
bool LDevices::DeleteDevice(uint8_t _devUID) {
  for(uint8_t i=0;i<LDidx;i++) {
    if(LDevice[i]->DevUID()==_devUID) {
      delete LDevice[i]; LDevice[i] = nullptr;
      return true;
    }
  }
  return false;
}
void LDevices::LDevProcLoop() {
  for(uint8_t i=0; i<LDidx; i++) {                         // Run Loop on Devices
    if(LDevice[i]!=nullptr) { LDevice[i]->Loop(); }
  }
}