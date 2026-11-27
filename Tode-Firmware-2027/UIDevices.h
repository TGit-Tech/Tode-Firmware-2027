#pragma once

#include <TGTWidgets.h>
#include "DITImplement.h"

#define WHTITLE       0x0A00
#define NAVNEWDEVICE  0xCA
#define NSADDONOFF    0x0A06

class AddOnOff : public TGTScreen {
public:
  AddOnOff(Adafruit_GFX& _GFX, uint16_t _NScrIdx, DITImplement& _Dits, uint8_t _devType)
  : TGTScreen(_GFX,_NScrIdx), Dits(_Dits), node(_Dits.Node(0)), devType(_devType)
  { tmpDITName = new char[Dits.DITNameChar]; }
  ~AddOnOff() { delete[] tmpDITName; }

  const uint8_t devType;
  uint8_t AdddevUID = BNONE;

  void DisplayScreen();
  uint8_t Navigate(uint8_t _Button) override;
  
private:
  DITImplement& Dits;
  DITSEngine::NodeDIT node;
  char* tmpDITName; // General use array for DIT get Names
  byte Add_IOPin = 0;
  byte Add_pinMode = 0;
  byte Add_Active = 0;
};