#include "UIDevices.h"

#define WHDEVNAME     0xDC0E
#define WHDEVTYPE     0xDC0F
#define WHIOPIN       0xDC10
#define WHPINMODE     0xDC11
#define WHACTIVE      0xDC12
#define WHADDBTN      0xDC13

#define WHTRIGPIN     0xDC14
#define WHECHOPIN     0xDC15
#define WHACCESS      0xDC1A
#define WHSCALE       0xDC1B
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
 
  if(devType==DT_ONOFF) {NewWFLabel  (row+1,TFT_COL_X(0), TFT_ROW_Y(row), F("On/Off"),WHTITLE);row++;}
  if(devType==DT_DIST)  {NewWFLabel  (row+1,TFT_COL_X(0), TFT_ROW_Y(row), F("Sonar Dist"),WHTITLE);row++;}
  
  NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(row), F("Name:"));row++;
  NewWCharVal (row+1,TFT_COL_X(5), TFT_ROW_Y(row), Dits.DITNameChar, true, WHDEVNAME);

  if(devType==DT_ONOFF) {
    NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(row), F("Access:"));row++;
    NewWEnumVal (row+1,TFT_COL_X(8), TFT_ROW_Y(row), true, WHACCESS);
      AddEnum(WHACCESS,DRO,F("RO"));      // DRO-DRWSS defined in rf_dits.h
      AddEnum(WHACCESS,DRW,F("RW"));
      AddEnum(WHACCESS,DLWRO,F("LWRO"));
      AddEnum(WHACCESS,DRWSS,F("RWSS"));
  }
  if(devType!=DT_ONOFF) {
    NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(row), F("Scale:"));row++;
    NewWEnumVal (row+1,TFT_COL_X(8), TFT_ROW_Y(row), true, WHSCALE);
      AddEnum(WHSCALE,DSCALE1,F("1"));    // DSCALE1-DSCALE1000 defined in rf_dits.h
      AddEnum(WHSCALE,DSCALE10,F("10"));
      AddEnum(WHSCALE,DSCALE100,F("100"));
      AddEnum(WHSCALE,DSCALE1000,F("1K"));
  }
  NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(row), F("Pin:"));row++;
  NewWNumVal  (row+1,TFT_COL_X(4), TFT_ROW_Y(row), TBYTE+TDEC+TSET, WHIOPIN, PINNUMRNG);
  NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(row), F("pinMode:"));row++;
  NewWEnumVal (row+1,TFT_COL_X(8), TFT_ROW_Y(row), true, WHPINMODE);
    AddEnum(WHPINMODE,ACTIVEHION,F("HiON"));     // Tode Pin-Modes defined in LDevices.h
    AddEnum(WHPINMODE,ACTIVELOWON,F("LoON"));
    AddEnum(WHPINMODE,UPACTIVEHION,F("UpHiON"));
    AddEnum(WHPINMODE,UPACTIVELOWON,F("UpLoON"));


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