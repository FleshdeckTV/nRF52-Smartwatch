#include <bluefruit.h>

BLEDfu bledfu;

void setup() {

  Bluefruit.begin();
  Bluefruit.setTxPower(4);    
  bledfu.begin();
  startAdv();
  delay(100);
  enterOTADfu();
}

void startAdv(void){  
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

void loop(){

}
