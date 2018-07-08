/*
  Mega2560_smartpanel.ino - ESP-Link, MQTT and smartpanel

  Copyright (C) 2017  Ghee Sung

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * Simple example to demo the esp-link MQTT client using APDS sensor
 * to send the data to MQTT 
 */
#include <Wire.h>
#include <ELClient.h>
#include <ELClientCmd.h>
#include <ELClientMqtt.h>
#include <ArduinoJson.h>
#include <TimeLib.h>

// specific to the tft display
#include <Adafruit_GFX.h>
#include <UTFTGLUE.h>            //we are using UTFT display methods
UTFTGLUE myGLCD(0x9341, A2, A1, A3, A4, A0);
#include <TouchScreen.h>         //Adafruit Library
#define  BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0 
#define WHITE           0xFFFF
#define TEST            0x1BF5
#define JJCOLOR         0x1CB6
#define JJORNG          0xFD03
// MCUFRIEND UNO shield shares pins with the TFT.   Due does NOT work
int YP = A1, YM = 7, XM = A2, XP = 7;  //most comon configuration
TouchScreen myTouch(XP, YP, XM, YM, 300);
TSPoint tp; 

// Declare which fonts we will be using
#if defined(SmallFont)         //.kbv new GLUE defines FreeSmallFont
#undef SmallFont               //.kbv lose it
#define SmallFont NULL         //.kbv use System Font
#else
extern uint8_t SmallFont[];    //.kbv
#endif

uint32_t cx, cy;
uint32_t rx[8], ry[8];
int32_t clx, crx, cty, cby;
float px, py;
int dispx, dispy, text_y_center, swapxy;
uint32_t calx, caly, cals;
char buf[13];

#define BUZZER 48 // buzzer pin


// Initialize a connection to esp-link using the normal hardware serial port both for
// SLIP and for debug messages.
ELClient esp(&Serial3, &Serial);
// Initialize CMD client (for GetTime)
ELClientCmd cmd(&esp);
// Initialize the MQTT client
ELClientMqtt mqtt(&esp);


// all the switch state
boolean doorway = false, livrm = false, balcony = false, livrmfan = false, hallway1 = false, kitchen = false, buzzer = false;

// Current date time
String currentTimeStr = "";

// Callback made from esp-link to notify of wifi status changes
// Here we just print something out for grins
void wifiCb(void* response) {
  ELClientResponse *res = (ELClientResponse*)response;
  if (res->argc() == 1) {
    uint8_t status;
    res->popArg(&status, 1);

    if(status == STATION_GOT_IP) {
      Serial.println("WIFI CONNECTED");
#ifdef DEBUG      
      Serial3.println("WIFI CONNECTED");
#endif      
    } else {
      Serial.print("WIFI NOT READY: ");
      Serial.println(status);
#ifdef DEBUG            
      Serial3.print("WIFI NOT READY: ");
      Serial3.println(status);
#endif
    }
  }
}

bool connected = false; // global variable to track if connected to mqtt

// Callback when MQTT is connected
void mqttConnected(void* response) {
  Serial.println("MQTT connected!");
#ifdef DEBUG  
  Serial3.println("MQTT connected!");
#endif
  connected = true;
  
  // doorbell 
  // first initialise it to arbitary value when startup
  // This is because of MQTT is set to retain mode
  mqtt.publish("bs/rfbridge/cmnd/switch/RfCode","AAAAA",1);

 
  mqtt.subscribe("bs/touchctl/doorbell");
  mqtt.subscribe("bs/+/stat/switch/POWER");
  mqtt.subscribe("bs/+/stat/switch/POWER1");
  mqtt.subscribe("bs/+/stat/switch/POWER2");
  /*mqtt.subscribe("bs/livingrm/stat/switch/POWER2");
  mqtt.subscribe("bs/hallway1/stat/switch/POWER1");
  mqtt.subscribe("bs/hallway1/stat/switch/POWER2");
  mqtt.subscribe("bs/kitchen/stat/switch/POWER");
  mqtt.subscribe("bs/balcony/stat/switch/POWER");
  mqtt.subscribe("bs/livingrm/stat/switch/POWER1");*/
  //mqtt.subscribe("bs/livingrm/stat/switch/RESULT");
  //mqtt.subscribe("bs/hallway1/stat/switch/RESULT");
  //mqtt.subscribe("bs/balcony/stat/switch/RESULT");
  
  // refresh all the button status
  controlPanelQueryStatus();
  
}

// Callback when MQTT is disconnected
void mqttDisconnected(void* response) {
  Serial.println("MQTT disconnected");
  Serial3.println("MQTT disconnected");
  connected = false;
  
  // force a reboot when the MQTT is disconnected
  // this is to reinit everything
  delay(1000);
  asm volatile ("  jmp 0");  
}

// Callback when an MQTT message arrives for one of our subscriptions
void mqttData(void* response) {
  ELClientResponse *res = (ELClientResponse *)response;

  String topic = res->popString();
  Serial.print("Received: topic=");
  Serial.println(topic);
#ifdef DEBUG
  Serial3.print("Received: topic=");
  Serial3.println(topic);
#endif
  String data = res->popString();
  Serial.print("Received: data=");
  Serial.println(data);
#ifdef DEBUG
  Serial3.print("Received: data=");
  Serial3.println(data);
#endif
  // living rm light status 
  if (topic == "bs/livingrm/stat/switch/POWER1"){
    
    if (data == "ON"){
      Serial.println("fan power is on");
      livrmfan = true;
      myGLCD.fillRect(170,80,150,50,GREEN);
    } 
    if (data == "OFF"){
      Serial.println("Fan power is off");
      livrmfan = false;
      myGLCD.fillRect(170,80,150,50,BLACK);

    }
  }
  if (topic == "bs/livingrm/stat/switch/POWER2"){    
    if (data == "ON"){
      Serial.println("living rm light power is on");
      livrm = true;
       myGLCD.fillRect(170, 20, 150, 50, GREEN);
    } 
    if (data == "OFF"){
      Serial.println("living rm light power is off");
      livrm = false;
      
      myGLCD.fillRect(170, 20, 150, 50, BLACK);      

    }

  }
  // hallway
  if (topic == "bs/hallway1/stat/switch/POWER1"){
    
    if (data == "ON"){
      Serial.println("doorway power1 is on");
      doorway = true;
      myGLCD.fillRect(0, 20, 150, 50, GREEN);
    } 
    if (data == "OFF"){
      Serial.println("doorway power1 is off");
      doorway = false;
      myGLCD.fillRect(0, 20, 150, 50, BLACK);

    }
  }
  if (topic == "bs/hallway1/stat/switch/POWER2"){    
    if (data == "ON"){
      Serial.println("hallway power is on");
      hallway1 = true;
       myGLCD.fillRect(0, 140, 150, 50, GREEN);
    } 
    if (data == "OFF"){
      Serial.println("hallway power is off");
      hallway1 = false;
      
      myGLCD.fillRect(0, 140, 150, 50, BLACK);      

    }    
  }
  // balcony
  if (topic == "bs/balcony/stat/switch/POWER"){
    
    if (data == "ON"){
      Serial.println("power is on");
      balcony = true;
      myGLCD.fillRect(0, 80, 150, 50, GREEN);
    } 
    else {
      Serial.println("power is off");
      balcony = false;
      myGLCD.fillRect(0, 80, 150, 50, BLACK);

    }
  }
  // kitchen
  if (topic == "bs/kitchen/stat/switch/POWER"){
    
    if (data == "ON"){
      Serial.println("kitchen light power is on");
      kitchen = true;
       myGLCD.fillRect(170, 140, 150, 50, GREEN);
    } 
    else {
      Serial.println("kitchen light is off");
      kitchen = false;
      
      myGLCD.fillRect(170, 140, 150, 50, BLACK);      

    }
  }
  
  // buzzer
  if (topic == "bs/touchctl/doorbell"){
    if (data == "ON"){
      buzzer = true;

    }
    else
      buzzer = false;
  }
  // redraw the line
  homescr();
  
}

void mqttPublished(void* response) {
  Serial.println("MQTT published");
}

// this is specific to Tasmota firmware.
// by publish a message, we can get the current switch status.
// this is done this way because sometime the mqttData callback is not triggered.
void controlPanelQueryStatus(){
  mqtt.publish("bs/livingrm/cmnd/switch/Power1","8");
  mqtt.publish("bs/livingrm/cmnd/switch/Power2","8");
  mqtt.publish("bs/hallway1/cmnd/switch/Power1","8");
  mqtt.publish("bs/hallway1/cmnd/switch/Power2","8");
  mqtt.publish("bs/balcony/cmnd/switch/Power","8");
  mqtt.publish("bs/kitchen/cmnd/switch/Power","8");
  Serial.println("controlPanelQueryStatus");
}


void setup() {

  // Serial is initialised so that debug message can be shown on the ESP-link web console
  // Serial3 is to display at the Arduino Serial Monitor
  Serial.begin(250000);
  Serial3.begin(250000);

  // setup the buzzer to simulate button click sound
  pinMode(BUZZER, OUTPUT);
  
  // initialise the LCD screen
  bool ret = diagnose_pins();
  digitalWrite(A0, HIGH);
  pinMode(A0, OUTPUT);
  myGLCD.InitLCD( );
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  dispx = myGLCD.getDisplayXSize();
  dispy = myGLCD.getDisplayYSize();
  text_y_center = (dispy / 2) - 6;
  if (ret == false) {
      myGLCD.print("BROKEN TOUCHSCREEN", CENTER, dispy / 2);
      while (1);
  }
  drawStartupScreen();
  
  // get the time from ESP-link NTP time. 
  // the time is already time-zone adjusted.
  uint32_t currtime = cmd.GetTime();
  setTime(currtime);
  setSyncProvider(requestSync);
  digitalClockDisplay();
  refreshDateTime();
  
  // initialise the ESP-Link and connect to MQTT
  Serial.println("EL-Client starting!");
  Serial3.println("EL-Client starting!");
  esp._debugEn = false;
  // Sync-up with esp-link, this is required at the start of any sketch and initializes the
  // callbacks to the wifi status change callback. The callback gets called with the initial
  // status right after Sync() below completes.
  esp.wifiCb.attach(wifiCb); // wifi status change callback, optional (delete if not desired)
  bool ok;
  do {
    ok = esp.Sync();      // sync up with esp-link, blocks for up to 2 seconds
    if (!ok) {
      Serial.println("EL-Client sync failed!");
      Serial3.println("EL-Client sync failed!");
    }
  } while(!ok);
  Serial.println("EL-Client synced!");
  Serial3.println("EL-Client synced!");
  
  // Set-up callbacks for events and initialize with esp-link.
  mqtt.dataCb.attach(mqttData);
  mqtt.connectedCb.attach(mqttConnected);
  mqtt.disconnectedCb.attach(mqttDisconnected);
  mqtt.publishedCb.attach(mqttPublished);

  mqtt.setup();

}

static int rebroadcast = 0; // rebroadcast counter
static uint32_t last;

void loop() {
  esp.Process();
  if (connected) {
    controlPanelQueryStatus();
    boolean ispressed;

    // loop when the touch screen is not pressed.
    while ((ispressed =ISPRESSED()) == false){
      esp.Process();
      //Serial.println((millis()-last));
      // periodically refresh the screen so that the clock is refreshed and
      // the button status is updated. 10000 is arbitrary chosen
      if ((millis()-last) > 20000){
        controlPanelQueryStatus();
        timeStatus();
        digitalClockDisplay(); 
        refreshDateTime();
        rebroadcast = 0;
        last = millis();
      }
      
      //check if buzzer is pressed
      if (buzzer == true){
        Serial.println("Door Bell pressed!");
        Serial3.println("Door Bell pressed!");
        buzzer = false;
        mqtt.publish("bs/touchctl/doorbell","OFF");
        playMusic();
      }
      
      rebroadcast++;

    }
    readCoordinates();
    showpoint();
    
    int x,y;

    //LANDSCAPE CALIBRATION     320 x 240
    x = map(tp.y, 106, 899, 0, 320);
    y = map(tp.x, 167, 880, 0, 240);
  
    navigation();
    last = millis();
  }
  
}


