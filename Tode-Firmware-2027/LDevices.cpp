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
OnOff* LDevices::OnOffDevice(uint8_t _devUID, bool _New, uint8_t _devAttr) {
  DBHAAENTER((_devUID,HEX),(_New),("LDevices::OnOffDevice(devUID,New)\n"))
  // Find existing device
  for(uint8_t i=0; i<LDidx; i++) {
    if(LDevice[i]->DevUID() == _devUID) {
      if(LDevice[i]->devType==DT_ONOFF) return static_cast<OnOff*>(LDevice[i]);
      return nullptr;
    }
  }
  if(!_New || LDidx >= MAXDEVICES) return nullptr;
  LDevice[LDidx] = new OnOff(_devUID);
  LDevice[LDidx]->devAttr = _devAttr;
  if(LDevice[LDidx] == nullptr) return nullptr;
  LDidx++;
  return LDevice[LDidx-1];
}
bool LDevices::LoadDevice(uint8_t _devUID, uint8_t _devType, uint8_t _devAttr) {
  DBHAAAENTER((_devUID,HEX),(_devType,HEX),(_devAttr),("LDevices::LoadDevice(devUID,devType,devAttr)\n"))
  switch(_devType) {
    case DT_ONOFF: return OnOffDevice(_devUID,true,_devAttr)!=nullptr ? true : false;
    case DT_DIST:     break;
    case DT_STSTP3W:  break;
    case DT_ANAINPUT: break;
    case DT_ANAOUTPUT: break;
    case DT_DLSETPOINT: break;
    case DT_DLMATH:   break;
    default:          return false;
  }
  return false;
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