#include "UITodeSetup.h"

// Node Edit Widget Handles
#define WHNODENAME      0xDA02    // D for Widget
#define WHSECNET        0xDA03
#define WHRADIOID       0xDA04
#define WHNETID         0xDA05
#define WHRFADDR        0xDA06
#define WHRFFREQ        0xDA07

// Add Device Widget Handlers
#define WHDEVNAME     0xDC0E
#define WHDEVTYPE     0xDC0F
#define WHIOPIN       0xDC10
#define WHPINMODE     0xDC11
#define WHACTIVE      0xDC12
#define WHADDBTN      0xDC13
//-------------------------------------------------------------------------------------------------
void SetupMenu::DisplayScreen() {
    NewWFLabel  (1,TFT_COL_X(0), TFT_ROW_Y(0), F("Setup Menu"),WHTITLE);
    NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(1), F("Add RTode"));
    NewWNumVal  (2,TFT_COL_X(10), TFT_ROW_Y(1), TUINT+THEX+TSETPD, WHADDRNODE);
    NewWFLabel  (3,TFT_COL_X(0), TFT_ROW_Y(2), F("Del RTode"), WHDELRNODE);
    NewWFLabel  (4,TFT_COL_X(0), TFT_ROW_Y(3), F("Tode Setup"), WHTODESETUP);
    NewWFLabel  (5,TFT_COL_X(0), TFT_ROW_Y(4), F("Add Dev"),   WHADDDEV);
    NewWFLabel  (6,TFT_COL_X(0), TFT_ROW_Y(5), F("Del Dev"),   WHDELDEV);
    NewWFLabel  (7,TFT_COL_X(0), TFT_ROW_Y(6), F("Edit Dev"),  WHEDITDEV);
    NewWFLabel  (8,TFT_COL_X(0), TFT_ROW_Y(7), F("System"),    WHSYSTEM);
}
uint8_t SetupMenu::Navigate(uint8_t _Button) {
  uint8_t RtnNavKey = OnScreenNavigate(_Button);
  DBUIAENTER((RtnNavKey,HEX),("SetupMenu::Navigate (RtnNavKey)\n"))

  return RtnNavKey;
}
//-------------------------------------------------------------------------------------------------
void TodeSetup::DisplayScreen() {
  ClearScreen();
  node.NGetName(tmpDITName);
  NewWFLabel  (1,TFT_COL_X(0), TFT_ROW_Y(0), F("Tode Setup"),WHTITLE);
  NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(1), F("Name:"));
  NewWCharVal (2,TFT_COL_X(5), TFT_ROW_Y(1), Dits.DITNameChar, true, WHNODENAME);
  NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(2), F("SecNet:"));
  NewWNumVal  (3,TFT_COL_X(7), TFT_ROW_Y(2), TBYTE+THEX+TSETPD, WHSECNET);
  NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(3), F("Radio:"));
  NewWEnumVal (0,TFT_COL_X(6), TFT_ROW_Y(3), false, WHRADIOID);
  NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(4), F("NetID:"));
  NewWNumVal  (4,TFT_COL_X(6), TFT_ROW_Y(4), TUINT+THEX+TSETPD, WHNETID);
  NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(5), F("RFAddr:"));
  NewWNumVal  (5,TFT_COL_X(7), TFT_ROW_Y(5), TUINT+THEX+TSETPD, WHRFADDR);
  NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(6), F("Freq:"));
  NewWNumVal  (6,TFT_COL_X(7), TFT_ROW_Y(6), TUINT+TDEC+TSETPD, WHRFFREQ);
  
  gotValue(NScrIdx,WHNODENAME,tmpDITName);

  AddEnum(WHRADIOID,0,F("NOTGOT"));
  AddEnum(WHRADIOID,1,F("UNKNOWN"));
  AddEnum(WHRADIOID,2,F("E22400T30D"));
  AddEnum(WHRADIOID,3,F("E22400T33D"));
  AddEnum(WHRADIOID,4,F("E22900T30D"));
  AddEnum(WHRADIOID,5,F("E32433T20D"));
  AddEnum(WHRADIOID,6,F("E32433T30D"));
  AddEnum(WHRADIOID,7,F("E220400T30D"));
}
uint8_t TodeSetup::Navigate(uint8_t _Button) {
  uint8_t RtnNavKey = OnScreenNavigate(_Button);
  DBUIAENTER((RtnNavKey,HEX),("TodeSetup::Navigate (RtnNavKey)\n"))

  if(RtnNavKey==NAVSETVAL) {
    if(SelectedWHandleUID()==WHNODENAME) { Dits.UpdateThisNodeName(SelectedWchValue()); }
    if(SelectedWHandleUID()==WHSECNET)   { Dits.SecNetCode(SelectedWValue()); }
    if(SelectedWHandleUID()==WHNETID)    { Ebyte.NetID(SelectedWValue()); }
    if(SelectedWHandleUID()==WHRFADDR)   { Ebyte.Address(SelectedWValue()); }
    if(SelectedWHandleUID()==WHRFFREQ)   { Ebyte.Frequency(SelectedWValue()); }
    RtnNavKey=NAVGETSETVAL;
  }
  if(RtnNavKey==NAVKEYGET || RtnNavKey==NAVGETSETVAL) {
    bool bAll = (RtnNavKey==NAVKEYGET);
    if(SelectedWHandleUID()==WHNODENAME||bAll) {
      DITSEngine::NodeDIT node = Dits.Node(0); node.NGetName(tmpDITName);
      gotValue(NSTODESETUP, WHNODENAME, tmpDITName);
    }
    if(SelectedWHandleUID()==WHSECNET||bAll)   { gotValue(NSTODESETUP, WHSECNET, Dits.SecNetCode()); }
    if(SelectedWHandleUID()==WHRADIOID||bAll)  { gotValue(NSTODESETUP, WHRADIOID, Ebyte.RadioID()); }
    if(SelectedWHandleUID()==WHNETID||bAll)    { gotValue(NSTODESETUP, WHNETID, Ebyte.NetID()); }
    if(SelectedWHandleUID()==WHRFADDR||bAll)   { gotValue(NSTODESETUP, WHRFADDR, Ebyte.Address()); }
    if(SelectedWHandleUID()==WHRFFREQ||bAll)   { gotValue(NSTODESETUP, WHRFFREQ, Ebyte.Frequency()); }
    return NAVKEYNONE;
  }
  return RtnNavKey;
}
//-------------------------------------------------------------------------------------------------
void AddDevice::DisplayScreen() {
  ClearScreen();
  node.NGetName(tmpDITName);
  NewWFLabel  (1,TFT_COL_X(0), TFT_ROW_Y(0), F("Add Device"),WHTITLE);
  NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(1), F("Name:"));
  NewWCharVal (2,TFT_COL_X(5), TFT_ROW_Y(1), Dits.DITNameChar, true, WHDEVNAME);
  NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(2), F("Type:"));
  NewWEnumVal (3,TFT_COL_X(5), TFT_ROW_Y(2), true, WHDEVTYPE);

  AddEnum(WHDEVTYPE,DT_ONOFF,F("OnOff"));
  AddEnum(WHDEVTYPE,DT_DIST,F("Distance"));
  AddEnum(WHDEVTYPE,DT_STSTP3W,F("StartStop3W"));
  AddEnum(WHDEVTYPE,DT_ANAINPUT,F("AnalogIn"));
  AddEnum(WHDEVTYPE,DT_ANAOUTPUT,F("AnalogOut"));
  AddEnum(WHDEVTYPE,DT_DLSETPOINT,F("DLSetPoint"));
  AddEnum(WHDEVTYPE,DT_DLMATH,F("DLMath"));
    
  // Use tmpDITName for new name.
  memset(tmpDITName, 0, Dits.DITNameChar);
  gotValue(NSADDDEV, WHDEVNAME, tmpDITName);
}
uint8_t AddDevice::Navigate(uint8_t _Button) {
  uint8_t RtnNavKey = OnScreenNavigate(_Button);
  DBUIAENTER((RtnNavKey,HEX),("TodeSetup::Navigate (RtnNavKey)\n"))

  if(RtnNavKey==NAVSETVAL) {
    if(SelectedWHandleUID()==WHDEVTYPE) {
      if(Add_devType!=DT_NOTSET) { /*Clear Bottom part of Screen*/ }
      Add_devType = SelectedWValue();
      if(Add_devType==DT_ONOFF) {
        NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(3), F("Pin:"));
        NewWNumVal  (4,TFT_COL_X(4), TFT_ROW_Y(3), TBYTE+TDEC+TSET, WHIOPIN,3,54);
        NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(4), F("pinMode:"));
        NewWEnumVal (5,TFT_COL_X(8), TFT_ROW_Y(4), true, WHPINMODE);
        NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(5), F("Active:"));
        NewWEnumVal (6,TFT_COL_X(7), TFT_ROW_Y(5), true, WHACTIVE);
        NewWFLabel  (7,TFT_COL_X(5), TFT_ROW_Y(6), F("[ADD]"),WHADDBTN);

        AddEnum(WHACTIVE,ACTIVEHION,F("HiON"));
        AddEnum(WHACTIVE,ACTIVELOWON,F("LoON"));
        AddEnum(WHPINMODE,INPUT,F("IN"));
        AddEnum(WHPINMODE,OUTPUT,F("OUT"));
        AddEnum(WHPINMODE,INPUT_PULLUP,F("INUP"));
        return NAVKEYNONE;
      }
    }
    if(SelectedWHandleUID()==WHIOPIN) {Add_IOPin=SelectedWValue();}
    if(SelectedWHandleUID()==WHPINMODE) {Add_pinMode=SelectedWValue();}
    if(SelectedWHandleUID()==WHACTIVE) {Add_Active=SelectedWValue();}
  }
  if(RtnNavKey==NAVKEYGET) {
    if(SelectedWHandleUID()==WHADDBTN) {
      DBUIAAINFO((Add_devType),(tmpDITName),("DitsUI::AddDevice Dits.AddThisNodeDevice(devType, Name)"))
      uint8_t AdddevUID = Dits.AddThisNodeDevice(Add_devType, 0x00, tmpDITName);
      if(AdddevUID!=BNONE) {
        if(Add_devType==DT_ONOFF) {
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