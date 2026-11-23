#include "Config.h"
#include "DITImplement.h"   // Includes rf_dits.h
#include "LDevices.h"
#include "DitsUI.h"
#include <Adafruit_ST7735.h>
#include "DigKeyPadBD241S.h"
#include <TGTEbyte.h>

#define RFSERIAL Serial3      // Tode 'Serial1' JMP-wire (Also Line#92 in DITImplement.h), BD241S 'Serial3' Ebyte
//#define RELAYNODE           // On Compile: Comment out for Handheld.  Leave for Node connected to Relays.

//UI
Adafruit_ST7735 oDSP(/*cs*/6,/*dc*/7,/*rst*/5); //TFT(cs, dc(rs)(cd), rst)
KeyPad* ButtonCapture = nullptr;
DitsUI dUI;
LDevices LDevs;                     // Local Devices Container
DITImplement DITS(RFSERIAL, LDevs);  // RF-DITS Engine Implementation
TGTEbyte oEbyte(RFSERIAL,/*M0*/9,/*M1*/8,/*AUX*/50);
uint32_t msTimer = 0;
int iTestStep = 0;
uint8_t NavKey = 0;
char* tmpname; // General use array for DIT get Names
uint8_t R1devUID = 0xFF, R2devUID = 0xFF, R3devUID = 0xFF, R4devUID = 0xFF;
//-------------------------------------
// Debug Displays to track Test Steps
//-------------------------------------
void DumpDITS() {
  Serial.println(F("\nnvvvvvvvvvvvvvvvvvvvvvvvvvv RADIO1 DITS vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv"));
  // Step 1: Iterate through Nodes
  DITSEngine::NodeDIT node = DITS.Node();
  while (node.Next()) {
    node.NGetName(tmpname);
    Serial.print(F("NODE ["));Serial.print(node.NodeIdx());Serial.print(F("] "));
    Serial.print(tmpname);Serial.print(F(" (RF: 0x"));Serial.print(node.NRFAddr(), HEX);
    Serial.print(F(") (NDITVer: "));Serial.print(node.NDITVer());Serial.println(")");
   // Step 2: Iterate through ALL devices to find children of this node
    DITSEngine::DeviceDIT dev = DITS.Device();
    while (dev.Next(node.NodeIdx())) {    // List Devices matching node.
      dev.DGetName(tmpname);
      Serial.print(F("  |-- DevUID: "));Serial.print(dev.DevUID());
      Serial.print(F(" | Type: "));Serial.print(dev.DevType(), HEX);
      Serial.print(F(" | Attr: 0x"));Serial.print(dev.DevAttr(), HEX);
      Serial.print(F(" | Name: "));Serial.println(tmpname);
    }
  }
  Serial.println(F("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));
}

void setup() {
  RFSERIAL.begin(9600);        // Start Radio Communications for DITS
  Serial.begin(115200);
  Serial.println(F("DitsUI Test"));
  tmpname = new char[DITS.DITNameChar];

  DITS.begin();               // Start RF-DITS protocol
  oEbyte.Begin();
  /*
  Variant	Initialization Flag	Notes
  Black Tab	    INITR_BLACKTAB	  Most common 1.8″ variant
  Green Tab	    INITR_GREENTAB	  Different pixel offsets
  Green Tab 2	  INITR_GREENTAB2	  Alternative green tab version
  Red Tab	      INITR_REDTAB	    RGB color order differences
  Mini 160×80	  INITR_MINI160x80	Smaller form factor display
  144 Green Tab	INITR_144GREENTAB	1.44″ square display (128×128)
  */
  oDSP.initR(INITR_BLACKTAB);   //Initialize ST7735 of the GFX
  
  ButtonCapture = new KeyPad(/*Debounce*/300, /*HoldRepeat*/2000, /*BuzzPin*/0xFF);
  ButtonCapture->StartCapture();

  // Standard RF-DITS protocol settings.
  DITS.RadioPktMaxBytes(58);          // 58 is the default.  This can be deleted.
  DITS.RxExpireMS(500);               // 30s is the default.  We keep small for wired-comm.
  DITS.UpdateThisNodeName("DITS");  // Make sure this node has a readable name.
  DITS.SecNetCode(0);                 // Assign the 'User' network security code.

  #if defined(RELAYNODE)
    DITS.UpdateThisNodeName("Relays");  // Make sure this node has a readable name.
    //DITS.UpdateThisNodeRFAddr(0x0001);  // Make sure DITS has the correct RF Address on record. DON'T USE WITH RADIO
    // Add some local devices to DITS
    R1devUID = DITS.AddThisNodeDevice(DT_ONOFF, DRW, "Relay1");
    DBSAINFO((R1devUID),("DITS.AddThisNodeDevice(DT_ONOFF, DRW, Relay1)=(devUID)\n"))
    R2devUID = DITS.AddThisNodeDevice(DT_ONOFF, DRW, "Relay2");
    DBSAINFO((R2devUID),("DITS.AddThisNodeDevice(DT_ONOFF, DRW, Relay2)=(devUID)\n"))
    R3devUID = DITS.AddThisNodeDevice(DT_ONOFF, DRW, "Relay3");
    DBSAINFO((R3devUID),("DITS.AddThisNodeDevice(DT_ONOFF, DRW, Relay3)=(devUID)\n"))
    R4devUID = DITS.AddThisNodeDevice(DT_ONOFF, DRW, "Relay4");
    DBSAINFO((R4devUID),("DITS.AddThisNodeDevice(DT_ONOFF, DRW, Relay4)=(devUID)\n"))
  
    // Assign local device settings
    //NewOnOffDevice(uint8_t _devUID, uint8_t Pin, uint8_t _pnMode)
    LDevs.NewOnOffDevice(R1devUID, 20, OUTPUT, true);
    LDevs.NewOnOffDevice(R2devUID, 21, OUTPUT, true);
    LDevs.NewOnOffDevice(R3devUID, 22, OUTPUT, true);
    LDevs.NewOnOffDevice(R4devUID, 23, OUTPUT, true);
  #else
    DITS.UpdateThisNodeName("Handheld");
    //DITS.UpdateThisNodeRFAddr(0x0002);  // Make sure DITS has the correct RF Address on record. DON'T USE WITH RADIO
    //DITS.TxAddRemoteNode(0x0001);
  #endif
  // Start DitsUI
  dUI.Begin(DITS, oDSP, oEbyte);
  if(!oEbyte.FixedTransmission()) oEbyte.FixedTransmission(true);
  oEbyte.AirSpeed(2);
  DITS.setRadioChannel(oEbyte.Channel());
  oEbyte.Mode(ERFNORMMODE);
  DITS.UpdateThisNodeRFAddr(oEbyte.Address());
}

void loop() {
  if(DITS.ProcLoop()==RDEVLOADED) { /*Send Remote Values to UI*/ }

  if(dUI.OnScreen!=nullptr) {
    if(dUI.OnScreen->NScrIdx==NSTHISNODE) {
      // Load Local Device Values on Every Loop
      DITSEngine::DeviceDIT dev = DITS.Device();
      while (dev.Next(0)) {
        dUI.gotValue( 0, dev.DevUID(),  LDevs.Value(dev.DevUID()) );
      }
    }
  }

  // Send Button Presses
  uint8_t yButton = ButtonCapture->Button();
  if(yButton!=NAVKEYNONE) { 
    NavKey = dUI.ScreenNavigate(yButton);
  
    if(NavKey==NAVNEWDEVICE) {
      //DITS.AddThisNodeDevice()
      //DITS.AddDevice(dUI.AdddevUID, dUI.Add_devType);
      dUI.AdddevUID = BNONE; dUI.Add_devType = BNONE;
    }
    if(NavKey==NAVSETVAL) {
      uint16_t ScrIdx = dUI.OnScreen->NScrIdx;
      uint16_t WHdevUID = dUI.OnScreen->SelectedWHandleUID();
      uint16_t WValue = dUI.OnScreen->SelectedWValue();
      if(ScrIdx==NSTHISNODE) {
        // A Device was Set on This Node.
        uint16_t WHdevUID = dUI.OnScreen->SelectedWHandleUID();
        LDevs.Value(WHdevUID,dUI.OnScreen->SelectedWValue());
      } else if (ScrIdx<=NSNODEMAX) {
        uint8_t NodeIndex = ScrIdx & 0xFF;
        DITS.TxSetRemoteDevVal(NodeIndex, WHdevUID, WValue);
      } else if (ScrIdx==NSSETUP) {
        if(WHdevUID==WHADDRNODE) { DITS.TxAddRemoteNode(WValue); }
      }
    }
  } // End Button NavKey.
  /*
  // Test.
  #if !defined(RELAYNODE)
    if (millis()-msTimer >= 2000) { 
      msTimer=millis(); iTestStep++;
      if (iTestStep==1) { DumpDITS(); } // Before anything.
      if (iTestStep==2) {DITS.TxAddRemoteNode(0x0001);}
      if (iTestStep==3) { DumpDITS(); } // Before anything.
      // After getting RELAYNODE DIT Records; store its devUID(s) for access
      if (iTestStep==4) {
        DITSEngine::NodeDIT node = DITS.Node();
        while(node.Next()) {
          if(node.NRFAddr()==0x0001) {
            uint8_t devCnt = 0;
            DITSEngine::DeviceDIT dev = DITS.Device();
            while (dev.Next(node.NodeIdx())) {
              switch(devCnt) {
                case 0: R1devUID = dev.DevUID(); break;
                case 1: R2devUID = dev.DevUID(); break;
                case 2: R3devUID = dev.DevUID(); break;
                case 3: R4devUID = dev.DevUID(); break;
              }
              devCnt++;
            }
          }
        }
      }
      if (iTestStep==5) {DITS.TxSetRemoteDevVal(0x0001, R1devUID, 1);}
      if (iTestStep==6) {DITS.TxSetRemoteDevVal(0x0001, R1devUID, 0);}
      if (iTestStep >= 10) iTestStep=0;
    }
  #endif
  */
}
