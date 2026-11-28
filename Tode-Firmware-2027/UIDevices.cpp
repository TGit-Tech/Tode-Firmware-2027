#include "UIDevices.h"

#define WHDEVNAME     0xDC0E
#define WHACCESS      0xDD00
#define WHSCALE       0xDD01

// OnOff
#define WHIOPIN       0xDD02
#define WHPINMODE     0xDD03
// Sonar
#define WHTRIGPIN     0xDD04
#define WHTRIGMODE    0xDD05
#define WHECHOPIN     0xDD06
#define WHECHOMODE    0xDD07
// AnaIn
#define WHANAINPIN    0xDD08
#define WHANAINMODE   0xDD09
//-------------------------------------------------------------------------------------------------
// DeviceEdit
/*
  AddEnum(WHADDDEV,DT_DIST,F("Distance"));
  AddEnum(WHADDDEV,DT_STSTP3W,F("StartStop3W"));
  AddEnum(WHADDDEV,DT_ANAINPUT,F("AnalogIn"));
  AddEnum(WHADDDEV,DT_ANAOUTPUT,F("AnalogOut"));
  AddEnum(WHADDDEV,DT_DLSETPOINT,F("DLSetPoint"));
  AddEnum(WHADDDEV,DT_DLMATH,F("DLMath"));
*/
//-------------------------------------------------------------------------------------------------
void DeviceEdit::DisplayScreen() {
  if(ScreenDisplayed) {return;} else {ScreenDisplayed=true;}
  ClearScreen(); uint8_t row=0;
 
  // Title
  if(devType==DT_ONOFF)     {NewWFLabel  (row+1,TFT_COL_X(0), TFT_ROW_Y(row), F("On/Off"),WHTITLE);row++;}
  if(devType==DT_DIST)      {NewWFLabel  (row+1,TFT_COL_X(0), TFT_ROW_Y(row), F("Sonar Dist"),WHTITLE);row++;}
  if(devType==DT_ANAINPUT)  {NewWFLabel  (row+1,TFT_COL_X(0), TFT_ROW_Y(row), F("AnalogIn"),WHTITLE);row++;}
  
  // Name
  NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(row), F("Name:"));
  NewWCharVal (row+1,TFT_COL_X(5), TFT_ROW_Y(row), Dits.DITNameChar, true, WHDEVNAME);row++;

  // DevAttr (Access & Scale)
  if(devType==DT_ONOFF) {
    NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(row), F("Access:"));
    NewWEnumVal (row+1,TFT_COL_X(7), TFT_ROW_Y(row), true, WHACCESS);row++;
      AddEnum(WHACCESS,DRO,F("RO"));      // DRO-DRWSS defined in rf_dits.h
      AddEnum(WHACCESS,DRW,F("RW"));
      AddEnum(WHACCESS,DLWRO,F("LWRO"));
      AddEnum(WHACCESS,DRWSS,F("RWSS"));
  }
  if(devType!=DT_ONOFF) {
    NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(row), F("Scale:"));
    NewWEnumVal (row+1,TFT_COL_X(8), TFT_ROW_Y(row), true, WHSCALE);row++;
      AddEnum(WHSCALE,DSCALE1,F("1"));    // DSCALE1-DSCALE1000 defined in rf_dits.h
      AddEnum(WHSCALE,DSCALE10,F("10"));
      AddEnum(WHSCALE,DSCALE100,F("100"));
      AddEnum(WHSCALE,DSCALE1000,F("1K"));
  }

  // Pins
  if(devType==DT_ONOFF) {
    NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(row), F("Pin:"));
    NewWNumVal  (row+1,TFT_COL_X(4), TFT_ROW_Y(row), TBYTE+TDEC+TSET, WHIOPIN, PINNUMRNG);row++;
    NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(row), F("pinMode:"));
    NewWEnumVal (row+1,TFT_COL_X(8), TFT_ROW_Y(row), true, WHPINMODE);row++;
      for(uint8_t i=0;i<4;i++) {AddEnum(WHPINMODE,i,EnumPinModes(i));}
  }
  if(devType==DT_DIST) {
    NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(row), F("TrigPin:"));
    NewWNumVal  (row+1,TFT_COL_X(8), TFT_ROW_Y(row), TBYTE+TDEC+TSET, WHTRIGPIN, PINNUMRNG);row++;
    NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(row), F("TrigMode:"));
    NewWEnumVal (row+1,TFT_COL_X(8), TFT_ROW_Y(row), true, WHTRIGMODE);row++;
      for(uint8_t i=0;i<4;i++) {AddEnum(WHTRIGMODE,i,EnumPinModes(i));}
    NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(row), F("EchoPin:"));
    NewWNumVal  (row+1,TFT_COL_X(8), TFT_ROW_Y(row), TBYTE+TDEC+TSET, WHECHOPIN, PINNUMRNG);row++;
    NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(row), F("EchoMode:")); // PINS 20,21 Only Harware Int.
    NewWEnumVal (row+1,TFT_COL_X(8), TFT_ROW_Y(row), true, WHECHOMODE);row++;
      for(uint8_t i=0;i<4;i++) {AddEnum(WHECHOMODE,i,EnumPinModes(i));}    
  }
  if(devType==DT_ANAINPUT) {
    NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(row), F("Pin:"));
    NewWNumVal  (row+1,TFT_COL_X(4), TFT_ROW_Y(row), TBYTE+TDEC+TSET, WHANAINPIN, PINNUMRNG);row++;
    NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(row), F("pinMode:"));
    NewWEnumVal (row+1,TFT_COL_X(8), TFT_ROW_Y(row), true, WHANAINMODE);row++;
      for(uint8_t i=0;i<4;i++) {AddEnum(WHANAINMODE,i,EnumPinModes(i));}
  }

  // Load Dit Records if they exist.   
  DITSEngine::DeviceDIT dev = Dits.Device();
  while(dev.Next(node.NodeIdx())) {
    if(dev.DevUID() == devUID) {
      dev.DGetName(tmpDITName);
      devAttr = dev.DevAttr();
      gotValue(NSDEVICEEDIT, WHDEVNAME, tmpDITName);
      gotValue(NSDEVICEEDIT, WHACCESS, devAttr & DACCESSMSK);
      gotValue(NSDEVICEEDIT, WHSCALE, devAttr & DSCALEMSK);
      break;
    }
  }

  // Get Device Pointers for SETGS changes.
  switch(devType) {
    case DT_ONOFF: 
      devOnOff = Dits.LDev.OnOffDevice(devUID,true); 
      if(devOnOff!=nullptr) {
        devOnOff->devAttr=devAttr;
        gotValue(NSDEVICEEDIT, WHIOPIN, devOnOff->Pin1());
        gotValue(NSDEVICEEDIT, WHPINMODE, devOnOff->Pin1Mode());
      }
      break;
  }
}
uint8_t DeviceEdit::Navigate(uint8_t _Button) {
  uint8_t RtnNavKey = OnScreenNavigate(_Button);
  DBUIAENTER((RtnNavKey,HEX),("DeviceEdit::Navigate (RtnNavKey)\n"))

  if(RtnNavKey==NAVSETVAL) {
    //UpdateThisNodeDevice(byte _devUID, byte _devAttr, const char* name = nullptr)
    if(SelectedWHandleUID()==WHDEVNAME) {Dits.UpdateThisNodeDevice(devUID,devAttr,tmpDITName);}
    if(SelectedWHandleUID()==WHACCESS)  {
      devAttr = (devAttr & ~DACCESSMSK) | SelectedWValue();
      Dits.UpdateThisNodeDevice(devUID,devAttr);
      if(devType==DT_ONOFF && devOnOff!=nullptr) devOnOff->devAttr=devAttr;
    }
    if(SelectedWHandleUID()==WHSCALE)   {
      devAttr = (devAttr & ~DSCALEMSK) | SelectedWValue();
      Dits.UpdateThisNodeDevice(devUID,devAttr);
      if(devType==DT_ONOFF && devOnOff!=nullptr) devOnOff->devAttr=devAttr;
    }
    if(SelectedWHandleUID()==WHIOPIN)   {
      if(devType==DT_ONOFF && devOnOff!=nullptr) devOnOff->Pin1(SelectedWValue());
    }
    if(SelectedWHandleUID()==WHPINMODE) {
      if(devType==DT_ONOFF && devOnOff!=nullptr) devOnOff->Pin1Mode(SelectedWValue());
    }
  }
  return RtnNavKey;
}