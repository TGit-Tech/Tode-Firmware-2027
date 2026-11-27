#include "UITodeSetup.h"

// Widget Handled within Screen so keep WH defines local/hidden.
// Node Edit Widget Handles
#define WHNODENAME      0xDA02    // D for Widget
#define WHSECNET        0xDA03
#define WHRADIOID       0xDA04
#define WHNETID         0xDA05
#define WHRFADDR        0xDA06
#define WHRFFREQ        0xDA07

// AddDevice Widget Handlers WH


// SetupMenu, Most do Screen Change except Add Remote Node WH
#define WHADDRNODE    0xDB07
//-------------------------------------------------------------------------------------------------
// SetupMenu
//-------------------------------------------------------------------------------------------------
void SetupMenu::DisplayScreen() {
  ClearScreen();
  NewWFLabel  (1,TFT_COL_X(0),  TFT_ROW_Y(0), F("Setup Menu"),    WHTITLE);
  NewWFLabel  (0,TFT_COL_X(0),  TFT_ROW_Y(1), F("AddRT"));
  NewWNumVal  (2,TFT_COL_X(6),  TFT_ROW_Y(1), TUINT+THEX+TSETPD,  WHADDRNODE);
  NewWFLabel  (0,TFT_COL_X(0),  TFT_ROW_Y(2), F("DelRT"));
  NewWEnumVal (3,TFT_COL_X(6),  TFT_ROW_Y(2), true,               WHDELRNODE);
  NewWFLabel  (0,TFT_COL_X(0),  TFT_ROW_Y(3), F("AddDev"));
  NewWEnumVal (4,TFT_COL_X(7),  TFT_ROW_Y(3), true,               WHADDDEV);
  NewWFLabel  (0,TFT_COL_X(0),  TFT_ROW_Y(4), F("DelDev"));
  NewWEnumVal (5,TFT_COL_X(7),  TFT_ROW_Y(4), true,               WHDELDEV);
  NewWFLabel  (0,TFT_COL_X(0),  TFT_ROW_Y(5), F("EdtDev"));
  NewWEnumVal (6,TFT_COL_X(7),  TFT_ROW_Y(5), true,               WHEDITDEV);
  NewWFLabel  (7,TFT_COL_X(0),  TFT_ROW_Y(6), F("Tode Setup"),    WHTODESETUP);
  NewWFLabel  (8,TFT_COL_X(0),  TFT_ROW_Y(7), F("System"),        WHSYSTEM);
  
  AddEnum(WHADDDEV,DT_ONOFF,F("OnOff"));
  AddEnum(WHADDDEV,DT_DIST,F("Distance"));
  AddEnum(WHADDDEV,DT_STSTP3W,F("StartStop3W"));
  AddEnum(WHADDDEV,DT_ANAINPUT,F("AnalogIn"));
  AddEnum(WHADDDEV,DT_ANAOUTPUT,F("AnalogOut"));
  AddEnum(WHADDDEV,DT_DLSETPOINT,F("DLSetPoint"));
  AddEnum(WHADDDEV,DT_DLMATH,F("DLMath"));

  DITSEngine::NodeDIT node = Dits.Node();
  while(node.Next()) {
    if(node.NodeIdx()==0) continue;
    node.NGetName(tmpDITName);
    AddEnum(WHDELRNODE, node.NodeIdx(),tmpDITName);
    DBUIAAINFO((node.NodeIdx()),(tmpDITName),("SetupMenu::DisplayScreen AddEnum(WHDELRNODE, (NodeIdx),(tmpDITName))"))
  }

  DITSEngine::DeviceDIT dev = Dits.Device();
  while (dev.Next(NSTHISNODE)) {
    dev.DGetName(tmpDITName);
    AddEnum(WHDELDEV,dev.DevUID(),tmpDITName);
    AddEnum(WHEDITDEV,dev.DevUID(),tmpDITName);
  }
}
uint8_t SetupMenu::Navigate(uint8_t _Button) {
  uint8_t RtnNavKey = OnScreenNavigate(_Button);
  DBUIAAENTER((_Button,HEX),(RtnNavKey,HEX),("SetupMenu::Navigate(_Button)[RtnNavKey]\n"))

  if(RtnNavKey==NAVSETVAL) {

    // Add/Edit Device goes to Add/Edit Device Screen i.e. Handled by DitsUI ScreenNavigate.
    if(SelectedWHandleUID()==WHADDRNODE) { 
      Dits.TxAddRemoteNode(SelectedWValue());
      DBUIAINFO((SelectedWValue(),HEX),("SetupMenu::Navigate [Dits.TxAddRemoteNode(RFAddr)]\n"))
      return NAVKEYNONE;
    }
    if(SelectedWHandleUID()==WHDELRNODE) {
      Dits.DelRemoteNode(SelectedWValue());
      DelEnums(WHDELRNODE);
      DITSEngine::NodeDIT node = Dits.Node();  // Update Delete Node Enum List
      while(node.Next()) {
        if(node.NodeIdx()==0) continue;
        node.NGetName(tmpDITName);
        AddEnum(WHDELRNODE, node.NodeIdx(),tmpDITName);
      }
      return NAVKEYNONE;
    }
    if(SelectedWHandleUID()==WHDELDEV) {
      Dits.DelThisNodeDevice(SelectedWValue());
      Dits.LDev.DeleteDevice(SelectedWValue());
      DelEnums(WHDELDEV);
      DelEnums(WHEDITDEV);
      DITSEngine::DeviceDIT dev = Dits.Device();
      while (dev.Next(NSTHISNODE)) {          // Update Delete Device Enum List
        dev.DGetName(tmpDITName);
        AddEnum(WHDELDEV,dev.DevUID(),tmpDITName);
        AddEnum(WHEDITDEV,dev.DevUID(),tmpDITName);
      }
    }
    return RtnNavKey;
  }
}
//-------------------------------------------------------------------------------------------------
// TodeSetup
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

  AddEnum(WHRADIOID,0,F("NOTGOT"));
  AddEnum(WHRADIOID,1,F("UNKNOWN"));
  AddEnum(WHRADIOID,2,F("E22400T30D"));
  AddEnum(WHRADIOID,3,F("E22400T33D"));
  AddEnum(WHRADIOID,4,F("E22900T30D"));
  AddEnum(WHRADIOID,5,F("E32433T20D"));
  AddEnum(WHRADIOID,6,F("E32433T30D"));
  AddEnum(WHRADIOID,7,F("E220400T30D"));

  getValues();
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
    getValues(true);
    return NAVKEYNONE;
  }
  return RtnNavKey;
}
void TodeSetup::getValues(bool SelOnly) {
  if(SelectedWHandleUID()==WHNODENAME||!SelOnly) {
    DITSEngine::NodeDIT node = Dits.Node(0); node.NGetName(tmpDITName);
    gotValue(NSTODESETUP, WHNODENAME, tmpDITName);
  }
  if(SelectedWHandleUID()==WHSECNET ||!SelOnly) { gotValue(NSTODESETUP, WHSECNET,  Dits.SecNetCode()); }
  if(SelectedWHandleUID()==WHRADIOID||!SelOnly) { gotValue(NSTODESETUP, WHRADIOID, Ebyte.RadioID()); }
  if(SelectedWHandleUID()==WHNETID  ||!SelOnly) { gotValue(NSTODESETUP, WHNETID,   Ebyte.NetID()); }
  if(SelectedWHandleUID()==WHRFADDR ||!SelOnly) { gotValue(NSTODESETUP, WHRFADDR,  Ebyte.Address()); }
  if(SelectedWHandleUID()==WHRFFREQ ||!SelOnly) { gotValue(NSTODESETUP, WHRFFREQ,  Ebyte.Frequency()); }
}