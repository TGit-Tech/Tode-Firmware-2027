#include "UIDevices.h"

#define WHDEVNAME     0xDC0E
#define WHDEVTYPE     0xDC0F
#define WHIOPIN       0xDC10
#define WHPINMODE     0xDC11
#define WHACTIVE      0xDC12
#define WHADDBTN      0xDC13

//-------------------------------------------------------------------------------------------------
// AddOnOff Device
//-------------------------------------------------------------------------------------------------
void AddOnOff::DisplayScreen() {
  ClearScreen();
  node.NGetName(tmpDITName);
  NewWFLabel  (1,TFT_COL_X(0), TFT_ROW_Y(0), F("Add On/Off"),WHTITLE);
  NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(1), F("Name:"));
  NewWCharVal (2,TFT_COL_X(5), TFT_ROW_Y(1), Dits.DITNameChar, true, WHDEVNAME);
  NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(2), F("Pin:"));
  NewWNumVal  (3,TFT_COL_X(4), TFT_ROW_Y(2), TBYTE+TDEC+TSET, WHIOPIN,3,54);
  NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(3), F("pinMode:"));
  NewWEnumVal (4,TFT_COL_X(8), TFT_ROW_Y(3), true, WHPINMODE);
  NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(4), F("Active:"));
  NewWEnumVal (5,TFT_COL_X(7), TFT_ROW_Y(4), true, WHACTIVE);
  NewWFLabel  (6,TFT_COL_X(5), TFT_ROW_Y(6), F("[ADD]"),WHADDBTN);  

  AddEnum(WHACTIVE,ACTIVEHION,F("HiON"));
  AddEnum(WHACTIVE,ACTIVELOWON,F("LoON"));
  AddEnum(WHPINMODE,INPUT,F("IN"));
  AddEnum(WHPINMODE,OUTPUT,F("OUT"));
  AddEnum(WHPINMODE,INPUT_PULLUP,F("INUP"));
    
  // Use tmpDITName for new name.
  memset(tmpDITName, 0, Dits.DITNameChar);
  gotValue(NSADDONOFF, WHDEVNAME, tmpDITName);
}
uint8_t AddOnOff::Navigate(uint8_t _Button) {
  uint8_t RtnNavKey = OnScreenNavigate(_Button);
  DBUIAENTER((RtnNavKey,HEX),("AddOnOff::Navigate (RtnNavKey)\n"))

  if(RtnNavKey==NAVSETVAL) {
    if(SelectedWHandleUID()==WHIOPIN) {Add_IOPin=SelectedWValue();}
    if(SelectedWHandleUID()==WHPINMODE) {Add_pinMode=SelectedWValue();}
    if(SelectedWHandleUID()==WHACTIVE) {Add_Active=SelectedWValue();}
  }
  if(RtnNavKey==NAVKEYGET) {
    if(SelectedWHandleUID()==WHADDBTN) {
      uint8_t AdddevUID = Dits.AddThisNodeDevice(devType, 0x00, tmpDITName);
      DBUIAAAINFO((devType,HEX),(tmpDITName),(AdddevUID,HEX),("AddOnOff::Navigate [NAVKEYGET][WHADDBTN] Dits.AddThisNodeDevice(devType, Name)=(AdddevUID)\n"))
      if(AdddevUID!=BNONE) {
        if(devType==DT_ONOFF) {
          //NewOnOffDevice(uint8_t _devUID, uint8_t _Pin, uint8_t _pnMode, bool _ActiveLow = false)
          Dits.LDev.NewOnOffDevice(AdddevUID, Add_IOPin, Add_pinMode, Add_Active);
          NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(9), F("...Device Added!"));
          return NAVNEWDEVICE;
        }
      }
    }
  }
  return RtnNavKey;
}
//-------------------------------------------------------------------------------------------------
// DelDev
//-------------------------------------------------------------------------------------------------