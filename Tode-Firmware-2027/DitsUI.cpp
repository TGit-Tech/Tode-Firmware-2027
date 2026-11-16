#include "DitsUI.h"

char testname[11] = "TestName";

void DitsUI::Begin(DITSEngine& _pDits, Adafruit_GFX& _pGFX, Ebyte& _pEbyte)
{
    pDits = &_pDits;
    pGFX = &_pGFX;
    pEbyte = &_pEbyte;
    tmpDITName = new char[pDits->DITNameChar];
    pinMode(DISPLIGHTPIN, OUTPUT);
    digitalWrite(DISPLIGHTPIN, DISPLIGHTACTIVE);
    
    pGFX->setRotation(2);
    pGFX->setFont(&OneSlot9pt7b);
    
    Serial.print("pDits->DITNameChar=");Serial.println(pDits->DITNameChar);

    DITSEngine::NodeDIT node = pDits->Node(NSTHISNODE);
    if(node.NodeIdx()==NSTHISNODE) { NodeScreen(node,NAVDISPLAY); }
    else { DBUIAERROR((node.NodeIdx()),("DitsUI::Begin ?node.NodeIdx()?!=NSTHISNODE")) }
}

//'''''''''''''''''''''''''''''''''''( UI Navigation )''''''''''''''''''''''''''''''''''''''''''''''''''
uint8_t DitsUI::ScreenNavigate(uint8_t _Button) {
  DBUIAENTER((_Button),("DitsUI::ScreenNavigate(_Button)\n"))
  if(OnScreen==nullptr) { DBUIERROR(("DitsUI::ScreenNavigate OnScreen==nullptr\n")) return _Button; }
  uint8_t NavRtn = OnScreen->OnScreenNavigate(_Button);
  DBUIAINFO((NavRtn,HEX),("DitsUI::ScreenNavigate (NavRtn)\n"))

  // Node Navigation
  if(OnScreen->NScrIdx <= NSNODEMAX) {
    DITSEngine::NodeDIT node = pDits->Node(OnScreen->NScrIdx & NODEIDXMASK);
    if(NavRtn==NAVKEYLEFT) {
      DBUIINFO(("DitsUI::Navigate NavRtn==NAVKEYLEFT Previous Node\n"))
      if (node.Prev()) { NodeScreen(node,NAVDISPLAY); return NAVKEYNONE; }
    }
    if(NavRtn==NAVKEYRIGHT) {
      DBUIINFO(("DitsUI::Navigate NavRtn==NAVKEYRIGHT Next Node\n"))
      if (node.Next()) { NodeScreen(node,NAVDISPLAY);return NAVKEYNONE; }
    }
    if(NavRtn==NAVKEYGET) { NodeScreen(node,NAVGETVALS); return NAVKEYNONE; }
    if(NavRtn==NAVKEYSET) { // SET on This Node Title triggers Setup Menu
      if(node.NodeIdx()==0 && OnScreen->SelectedWHandleUID()==WHTITLE) {
        SetupScreen(NAVDISPLAY);
      }
    }
    return NavRtn;

  // Setup Navigation
  } else if(OnScreen->NScrIdx == NSSETUP) {
    if(NavRtn==NAVKEYGET) { 
      switch(OnScreen->SelectedWHandleUID()) {
        case WHADDRNODE: return NAVKEYNONE;
        case WHDELRNODE: return NAVKEYNONE;
        case WHEDITNODE: NodeEditScreen(NAVDISPLAY); return NAVKEYNONE;
        case WHADDDEV: return NAVKEYNONE;
        case WHDELDEV: return NAVKEYNONE;
        case WHEDITDEV: return NAVKEYNONE;
        case WHSYSTEM: return NAVKEYNONE;
      }
    }
    if(NavRtn==NAVSETVAL) { SetupScreen(NAVSETVAL); return NAVKEYNONE; }
    if(NavRtn==NAVKEYLEFT) { 
      DITSEngine::NodeDIT node = pDits->Node(NSTHISNODE);
      NodeScreen(node, NAVDISPLAY); return NAVKEYNONE; }
  
  // Edit Node Navigation
  } else if (OnScreen->NScrIdx == NSEDITNODE) {
    if(NavRtn==NAVSETVAL)   { NodeEditScreen(NAVSETVAL);  return NAVKEYNONE; }
    if(NavRtn==NAVKEYLEFT)  { SetupScreen(NAVDISPLAY);    return NAVKEYNONE; }
  }

}



void DitsUI::NodeScreen(DITSEngine::NodeDIT& node, uint8_t NavKey) {
  DBUIAENTER((NavKey,HEX),("DitsUI::NodeScreen(NavKey)\n"))

  if(NavKey==NAVDISPLAY) {
    delete OnScreen; OnScreen = nullptr; int row = 0;
    OnScreen = new TGTScreen(*pGFX,node.NodeIdx());
    node.NGetName(tmpDITName);
    OnScreen->NewWChLabel(row+1,TFT_COL_X(0), TFT_ROW_Y(row),pDits->DITNameChar,tmpDITName,WHTITLE);

    DITSEngine::DeviceDIT dev = pDits->Device();
    while (row < TFT_ROWS - 1 && dev.Next(node.NodeIdx())) {
      row++; // Note: WHandleUID is DevUID
      dev.DGetName(tmpDITName);
      OnScreen->NewWChLabel(row+1,TFT_COL_X(0), TFT_ROW_Y(row),pDits->DITNameChar,tmpDITName);
    }
    OnScreen->DisplayScreen();
  }
}
void DitsUI::SetupScreen(uint8_t NavKey) {
  DBUIAENTER((NavKey,HEX),("DitsUI::SetupScreen(NavKey)\n"))
  
  if(NavKey==NAVDISPLAY) {
    delete OnScreen;  OnScreen = nullptr;
    OnScreen = new TGTScreen(*pGFX,NSSETUP);
    OnScreen->NewWFLabel  (1,TFT_COL_X(0), TFT_ROW_Y(0), F("Setup Menu"),WHTITLE);
    OnScreen->NewWFLabel  (2,TFT_COL_X(0), TFT_ROW_Y(1), F("Add RTode"), WHADDRNODE);
    OnScreen->NewWFLabel  (3,TFT_COL_X(0), TFT_ROW_Y(2), F("Del RTode"), WHDELRNODE);
    OnScreen->NewWFLabel  (4,TFT_COL_X(0), TFT_ROW_Y(3), F("Edit Tode"), WHEDITNODE);
    OnScreen->NewWFLabel  (5,TFT_COL_X(0), TFT_ROW_Y(4), F("Add Dev"),   WHADDDEV);
    OnScreen->NewWFLabel  (6,TFT_COL_X(0), TFT_ROW_Y(5), F("Del Dev"),   WHDELDEV);
    OnScreen->NewWFLabel  (7,TFT_COL_X(0), TFT_ROW_Y(6), F("Edit Dev"),  WHEDITDEV);
    OnScreen->NewWFLabel  (8,TFT_COL_X(0), TFT_ROW_Y(7), F("System"),    WHSYSTEM);

    OnScreen->DisplayScreen();
  }
}

void DitsUI::NodeEditScreen(uint8_t NavKey) {
  DBUIAENTER((NavKey,HEX),("DitsUI::NodeEditScreen(NavKey)\n"))
  uint16_t SetWH = OnScreen->SetValueWH;

  if(NavKey==NAVDISPLAY) {
    delete OnScreen;  OnScreen = nullptr;
    OnScreen = new TGTScreen(*pGFX,NSEDITNODE);
    OnScreen->NewWFLabel  (1,TFT_COL_X(0), TFT_ROW_Y(0), F("Edit Tode"));
    OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(1), F("Name:"));
    OnScreen->NewWCharVal (2,TFT_COL_X(5), TFT_ROW_Y(1), 11, true, WHNODENAME);
    OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(2), F("SecNet:"));
    OnScreen->NewWNumVal  (3,TFT_COL_X(7), TFT_ROW_Y(2), VDTBYTE, HEX, true, WHSECNET);
    OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(3), F("Radio:"));
    OnScreen->NewWEnumVal (0,TFT_COL_X(6), TFT_ROW_Y(3), false, WHRADIOID);
    OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(4), F("NetID:"));
    OnScreen->NewWNumVal  (4,TFT_COL_X(6), TFT_ROW_Y(4), VDTUINT, HEX, true, WHNETID);
    OnScreen->NewWFLabel  (0,TFT_COL_X(0), TFT_ROW_Y(5), F("RFAddr:"));
    OnScreen->NewWNumVal  (5,TFT_COL_X(7), TFT_ROW_Y(5), VDTUINT, HEX, true, WHRFADDR);

    OnScreen->AddEnum(WHRADIOID,0,F("NOTGOT"));
    OnScreen->AddEnum(WHRADIOID,1,F("UNKNOWN"));
    OnScreen->AddEnum(WHRADIOID,2,F("E22400T30D"));
    OnScreen->AddEnum(WHRADIOID,3,F("E22400T33D"));
    OnScreen->AddEnum(WHRADIOID,4,F("E22900T30D"));
    OnScreen->AddEnum(WHRADIOID,5,F("E32433T20D"));
    OnScreen->AddEnum(WHRADIOID,6,F("E32433T30D"));
    OnScreen->AddEnum(WHRADIOID,7,F("E220400T30D"));

    OnScreen->DisplayScreen();
    NavKey = NAVGETVALS;
  }
  if(NavKey==NAVSETVAL) {
    if(SetWH==WHNODENAME) { pDits->UpdateThisNodeName(OnScreen->chSetValue); }
    if(SetWH==WHSECNET)   { pDits->SecNetCode(OnScreen->SetValue); }
    if(SetWH==WHNETID)    { pEbyte->NetID(OnScreen->SetValue); }
    if(SetWH==WHRFADDR)   { pEbyte->Address(OnScreen->SetValue); }
    OnScreen->SetValueWH = WHNONE;
    NavKey=NAVGETSETVAL;
  }
  if(NavKey==NAVGETVALS || NavKey==NAVGETSETVAL) {
    bool bAll = (NavKey==NAVGETVALS);
    if(SetWH==WHNODENAME||bAll) {
      DITSEngine::NodeDIT node = pDits->Node(0); node.NGetName(tmpDITName);
      OnScreen->gotValue(NSEDITNODE, WHNODENAME, tmpDITName);
    }
    if(SetWH==WHSECNET||bAll)   { OnScreen->gotValue(NSEDITNODE, WHSECNET, pDits->SecNetCode()); }
    if(SetWH==WHRADIOID||bAll)  { OnScreen->gotValue(NSEDITNODE, WHRADIOID, pEbyte->RadioID()); }
    if(SetWH==WHNETID||bAll)    { OnScreen->gotValue(NSEDITNODE, WHNETID, pEbyte->NetID()); }
    if(SetWH==WHRFADDR||bAll)   { OnScreen->gotValue(NSEDITNODE, WHRFADDR, pEbyte->Address()); }
  }
}