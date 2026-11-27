#pragma once

#include <TGTWidgets.h>
#include "DITImplement.h"
#include "LDevices.h"
#include <rf_dits.h>

#define WHTITLE       0x0A00
#define NAVNEWDEVICE  0xCA
#define NSDEVICEEDIT    0x0A06
#define PINNUMRNG     18,69
// D18-D47
// A0-A16 i.e. 54-69
// So Pin Numbers have to exclude 48-53
class DeviceEdit : public TGTScreen {
public:
  DeviceEdit(Adafruit_GFX& _GFX, uint16_t _NScrIdx, DITImplement& _Dits, uint8_t _devUID, uint8_t _devType)
  : TGTScreen(_GFX,_NScrIdx), Dits(_Dits), node(_Dits.Node(0)), devUID(_devUID), devType(_devType)
  { tmpDITName = new char[Dits.DITNameChar]; }
  ~DeviceEdit() { delete[] tmpDITName; }

  void DisplayScreen();
  uint8_t Navigate(uint8_t _Button) override;
  
private:
  DITImplement& Dits;
  DITSEngine::NodeDIT node;
  uint8_t devUID;
  const uint8_t devType;
  uint8_t devAttr = 0;
  char* tmpDITName; // General use array for DIT get Names
  OnOff* devOnOff = nullptr;

};