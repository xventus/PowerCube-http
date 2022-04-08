/**
 * @file main.cpp
 * @author Petr Vanek (petr@fotoventus.cz)
 * @brief Energy Dice - A simple indicator of the solar power plant status and free energy. As a server it uses NODE RED
 * @version 0.1
 * @date 2022-04-04
 * 
 * @copyright Copyright (c) 2022 Petr Vanek 
 * 
 */

#include <M5Atom.h>
#include <Arduino.h>

#include "dice.h"
#include "dice_client.h"
#include "ap.h"
#include "file_sys.h"
#include "configuration.h"
#include "cfg_server.h"


/**
 * @brief set of global objects for running
 * 
 */
Dice dice;
DiceClient dc;
Configuration cfg;
ItemFS  ifs;

/**
 * @brief load configuration
 * 
 * @return true - OK
 * @return false - some item is not defined
 */
bool loadConfig() {
  bool rc = true;
  cfg.ssid = ifs.readItem(ItemFS::Data::ssid);
  cfg.pass = ifs.readItem(ItemFS::Data::password);
  cfg.url = ifs.readItem(ItemFS::Data::url);
  if (cfg.ssid.isEmpty() || cfg.pass.isEmpty() || cfg.url.isEmpty()) rc = false;
  return rc;
}

/**
 * @brief Creates an AccessPoint with a web server for configuration. 
 * 
 */
void webConfig() {
    dice.showPower(-1);
    dice.showSucessState();
    AP ap;
    CfgServer ws(&ifs);

    if (ap.init()) {
       dice.showPower(1);
       dice.showSucessState();
       Serial.printf("Configuration IP: %s\n", ap.getIP().toString().c_str());

       ws.init(80);
       ws.serveCfgPage();
       while(true)  { 
         // ws.serveCfgPage();
         dice.showSucessState();
         delay(1000);
       }

    }
    while(true) { dice.showSucessState(); delay(1000); }
}

/**
 * @brief init after reset
 * 
 */
void setup() {
  M5.begin(true, false, true);  
  Serial.printf("startup\n"); 
  dice.init();
  dice.showWaitState();
  

  if (!ifs.init() || !ifs.isRequiredFileExists()) {
     Serial.printf("Invalid file system, please 'Upload Filesystem Image'! \nSTOP\n");
     while(true) {
        // infinite stop
        dice.showPower(-1);
        dice.showErrorState(); 
        delay(1000);
     }
  }
  
  Serial.printf("Filesystem content:\n"); 
  ifs.dumpFiles();

  if (!loadConfig()) {
    Serial.printf("configuration not found, use web config\n"); 
    webConfig();
  } 

  Serial.printf("connecting to AP\n");
  dc.init(cfg.ssid.c_str(), cfg.pass.c_str());
}


/**
 * @brief I'm running out of my mind
 * 
 */
void loop() {

  // if configuration required
  M5.update();
  
  // The user presses a button to configure 
  if (M5.Btn.wasPressed()) {
     Serial.printf("button - user config\n");
     webConfig();
  }

  if (dc.isConnected()) {
    dice.clearState();
    if (dc.getData(cfg.url.c_str())) {
      //ok
      dice.showPower(dc.getPower());
      delay(1000);
    } else {
      // error
      dice.data(false);
      delay(300);
    }
  } else {
    dice.showWaitState();
  }
 
}


