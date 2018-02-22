/*
  esplink-mqtt.ino - ESP-Link and MQTT 

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



#include <SPI.h>             // f.k. for Arduino-1.5.2
#define USE_SDFAT
#include <SdFat.h>           // Use the SdFat library
SdFatSoftSpi<12, 11, 13> SD; //Bit-Bang on the Shield pins

// Initialize a connection to esp-link using the normal hardware serial port both for
// SLIP and for debug messages.
ELClient esp(&Serial3, &Serial);
// Initialize CMD client (for GetTime)
ELClientCmd cmd(&esp);
// Initialize the MQTT client
ELClientMqtt mqtt(&esp);


// all the switch state
boolean doorway = false, livrm = false, balcony = false, livrmfan = false, hallway = false, kitchen = false;

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
    } else {
      Serial.print("WIFI NOT READY: ");
      Serial.println(status);
    }
  }
}

bool connected;

// Callback when MQTT is connected
void mqttConnected(void* response) {
  Serial.println("MQTT connected!");
  //mqtt.subscribe("/brownstone/touchctl01/switch/cmnd");
  //mqtt.subscribe("/brownstone/livingrm/switch/stat/POWER");
  mqtt.subscribe("/brownstone/livrmfan/switch/stat/+", 1);
  mqtt.subscribe("/brownstone/doorway/switch/stat/+", 1);
  mqtt.subscribe("/brownstone/livrm/switch/stat/+", 1);
  mqtt.subscribe("/brownstone/balcony/switch/stat/+", 1);
  mqtt.subscribe("/brownstone/hallway/switch/stat/+", 1);
  mqtt.subscribe("/brownstone/kitchen/switch/stat/+", 1);

  Serial.println("Draw startup screen and published to all the queue");
  connected = true;
  drawStartupScreen();
  controlPanelQueryStatus();

  digitalClockDisplay();
}

// Callback when MQTT is disconnected
void mqttDisconnected(void* response) {
  Serial.println("MQTT disconnected");
  connected = false;
  
  //reboot when the MQTT is disconnected
  //delay(5000);
  asm volatile ("  jmp 0");  
}

// Callback when an MQTT message arrives for one of our subscriptions
void mqttData(void* response) {
  ELClientResponse *res = (ELClientResponse *)response;

  Serial.print("Received: topic=");
  String topic = res->popString();
  Serial.println(topic);

  Serial.print("data=");
  String data = res->popString();
  Serial.println(data);

  //boolean doorway = false, livrm = false, balcony = false, livrmfan = false, hallway = false, kitchen = false;
  // living rm light status 
  if (topic == "/brownstone/livrmfan/switch/stat/POWER"){
    
    if (data == "ON"){
      Serial.println("power is on");
      livrmfan = true;
      myGLCD.fillRect(170,80,150,50,GREEN);
    } 
    else {
      Serial.println("power is off");
      livrmfan = false;
      myGLCD.fillRect(170,80,150,50,BLACK);

    }
  }
  if (topic == "/brownstone/doorway/switch/stat/POWER"){
    
    if (data == "ON"){
      Serial.println("power is on");
      doorway = true;
      myGLCD.fillRect(0, 20, 150, 50, GREEN);
    } 
    else {
      Serial.println("power is off");
      doorway = false;
      myGLCD.fillRect(0, 20, 150, 50, BLACK);

    }
  }
  if (topic == "/brownstone/balcony/switch/stat/POWER"){
    
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
  if (topic == "/brownstone/livrm/switch/stat/POWER"){
    
    if (data == "ON"){
      Serial.println("power is on");
      livrm = true;
       myGLCD.fillRect(170, 20, 150, 50, GREEN);
    } 
    else {
      Serial.println("power is off");
      livrm = false;
      
      myGLCD.fillRect(170, 20, 150, 50, BLACK);      

    }
  }
  if (topic == "/brownstone/hallway/switch/stat/POWER"){
    
    if (data == "ON"){
      Serial.println("power is on");
      hallway = true;
       myGLCD.fillRect(0, 140, 150, 50, GREEN);
    } 
    else {
      Serial.println("power is off");
      hallway = false;
      
      myGLCD.fillRect(0, 140, 150, 50, BLACK);      

    }
  }
  if (topic == "/brownstone/kitchen/switch/stat/POWER"){
    
    if (data == "ON"){
      Serial.println("power is on");
      kitchen = true;
       myGLCD.fillRect(170, 140, 150, 50, GREEN);
    } 
    else {
      Serial.println("power is off");
      kitchen = false;
      
      myGLCD.fillRect(170, 140, 150, 50, BLACK);      

    }
  }
 
  homescr();
}

void mqttPublished(void* response) {
  Serial.println("MQTT published");
}


void controlPanelQueryStatus(){
  mqtt.publish("/brownstone/livrmfan/switch/cmnd/Power","8",1);
  mqtt.publish("/brownstone/doorway/switch/cmnd/Power","8",1);
  mqtt.publish("/brownstone/livrm/switch/cmnd/Power","8",1);
  mqtt.publish("/brownstone/balcony/switch/cmnd/Power","8",1);
  mqtt.publish("/brownstone/hallway/switch/cmnd/Power","8",1);
  mqtt.publish("/brownstone/kitchen/switch/cmnd/Power","8",1);
  Serial.println("controlPanelQueryStatus");
}

#define BUZZER 48 
void setup() {

  Serial.begin(115200);
  Serial3.begin(115200);

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
  
  // initialise the ESP-Link and connect to MQTT
  Serial.println("EL-Client starting!");
  esp._debugEn = false;
  // Sync-up with esp-link, this is required at the start of any sketch and initializes the
  // callbacks to the wifi status change callback. The callback gets called with the initial
  // status right after Sync() below completes.
  esp.wifiCb.attach(wifiCb); // wifi status change callback, optional (delete if not desired)
  bool ok;
  do {
    ok = esp.Sync();      // sync up with esp-link, blocks for up to 2 seconds
    if (!ok) Serial.println("EL-Client sync failed!");
  } while(!ok);
  Serial.println("EL-Client synced!");
  
  // Set-up callbacks for events and initialize with esp-link.
  mqtt.connectedCb.attach(mqttConnected);
  mqtt.disconnectedCb.attach(mqttDisconnected);
  mqtt.publishedCb.attach(mqttPublished);
  mqtt.dataCb.attach(mqttData);
  mqtt.setup();

  // get the time from ESP-link NTP time. 
  // the time is already time-zone adjusted.
  uint32_t currtime = cmd.GetTime();
  setTime(currtime);
  setSyncProvider(requestSync);
  digitalClockDisplay();
  refreshDateTime();

  // setup the SDCard reader that come with the touch screen
  setupSDCard();
}


int rebroadcast = 0;
void loop() {
  esp.Process();
  if (connected) {
    controlPanelQueryStatus();
    boolean ispressed;
    // this is the loop when the touch screen is not pressed.
    while ((ispressed =ISPRESSED()) == false){
      esp.Process();
      if (rebroadcast == 10000){
        controlPanelQueryStatus();
        timeStatus();
        //setTime(cmd.GetTime());
        digitalClockDisplay(); 
        refreshDateTime();
        rebroadcast = 0;
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
   
  }
  
}


