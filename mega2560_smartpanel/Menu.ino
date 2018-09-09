/* 
   The original source code was from JOS TFT but was heavily modified to suit the touch screen panel needs
   
   JOS TFT Touch Beta   version v2.1.0.3 3/07/2013
   Adafruit_TFTLCD.h and Adafruit_TouchScreen.h are from adafruit.com where you can also purchase a really nice 2.8" TFT with touchscreen :)
   Oringinal Sketch written by jeremy saglimbeni: www.thecustomgeek.com 
   Updates for uno/mega nathan sobieck: www.sobisource.com 
       backlight = pin 10 uno / pin 35 mega
       Must use libraries included / which are Adafruit's current Adafruit_GFX, Adafruit_TFTLCD and TouchScreen Libraries
       as of 3/07/13 
     
     *Note if you are using Adafruits TFTLCD Shield. Then you need to uncomment #define USE_ADAFRUIT_SHIELD_PINOUT 
     in Adafruit_TFTLCD > Adafruit_TFTLCD.h  
       However if you are using the shield Backlight PWM will not function unless you make a few simple hardware changes...
     Learn How Here....http://learn.adafruit.com/2-8-tft-touch-shield/controlling-the-backlight
     
*/ 


int page = 0;
unsigned long currenttime;


void drawhomeicon() { // draws a white home icon
  myGLCD.setColor(WHITE);
  myGLCD.drawLine(280, 219, 299, 200);
  myGLCD.drawLine(300, 200, 304, 204);
  myGLCD.drawLine(304, 203, 304, 200);
  myGLCD.drawLine(305, 200, 307, 200);
  myGLCD.drawLine(308, 200, 308, 208);
  myGLCD.drawLine(309, 209, 319, 219);
  myGLCD.drawLine(281, 219, 283, 219);
  myGLCD.drawLine(316, 219, 318, 219);
  myGLCD.drawRectXY(284, 219, 32, 21);
  myGLCD.drawRectXY(295, 225, 10, 15);
}
void drawhomeiconred() { // draws a red home icon
  myGLCD.setColor(RED);
  myGLCD.drawLine(280, 219, 299, 200);
  myGLCD.drawLine(300, 200, 304, 204);
  myGLCD.drawLine(304, 203, 304, 200);
  myGLCD.drawLine(305, 200, 307, 200);
  myGLCD.drawLine(308, 200, 308, 208);
  myGLCD.drawLine(309, 209, 319, 219);
  myGLCD.drawLine(281, 219, 283, 219);
  myGLCD.drawLine(316, 219, 318, 219);
  myGLCD.drawRectXY(284, 219, 32, 21);
  myGLCD.drawRectXY(295, 225, 10, 15);
}

void homescr() {
  boxes();
  myGLCD.setTextColor(WHITE);
  myGLCD.setTextSize(2);
  myGLCD.setCursor(31, 37);
  myGLCD.println("Doorway");
  myGLCD.setCursor(190, 37);
  myGLCD.println("Living Rm");
  myGLCD.setCursor(31, 97);
  myGLCD.println("Balcony");
  myGLCD.setCursor(180, 97);
  myGLCD.println("Liv. Rm Fan");
  myGLCD.setCursor(5, 157);
  myGLCD.println("Out. Kitchen");
  myGLCD.setCursor(200, 157);
  myGLCD.println("Kitchen");
}

void boxes() { // redraw the button outline boxes
  myGLCD.setColor(JJCOLOR);
  myGLCD.drawRectXY(0, 20, 150, 50);
  myGLCD.drawRectXY(170, 20, 150, 50);
  myGLCD.drawRectXY(0, 80, 150, 50);
  myGLCD.drawRectXY(170, 80, 150, 50);
  myGLCD.drawRectXY(0, 140, 150, 50);
  myGLCD.drawRectXY(170, 140, 150, 50);
}



void drawStartupScreen(){
  myGLCD.fillScreen(BLACK);
  myGLCD.setColor(JJCOLOR);
  myGLCD.fillRectXY(0, 0, 320, 10); // status bar
  drawhomeicon(); // draw the home icon
  myGLCD.setCursor(1, 1);
  myGLCD.println("Welcome to The Brownstone v0.5 ");
  homescr(); // draw the homescreen
  myGLCD.setColor(WHITE);
  myGLCD.drawRectXY(0, 200, 245, 40); // message box
  refreshDateTime();
}

void refreshDateTime() {
  Serial.print("Update the display to current time ");
  Serial.println(currentTimeStr);
  Serial3.print("Update the display to current time ");
  Serial3.println(currentTimeStr);

  myGLCD.fillRect(12, 213, 226, 16, BLACK); // black out the inside of the message box
  myGLCD.setTextColor(WHITE);
  myGLCD.setTextSize(2);
  myGLCD.setCursor(31, 213);
  myGLCD.println(currentTimeStr);

}

void clearcenter() { // the reason for so many small "boxes" is that it's faster than filling the whole thing
  myGLCD.setColor(BLACK);
  myGLCD.drawRectXY(0, 20, 150, 50);
  myGLCD.drawRectXY(170, 20, 150, 50);
  myGLCD.drawRectXY(0, 80, 150, 50);
  myGLCD.drawRectXY(170, 80, 150, 50);
  myGLCD.drawRectXY(0, 140, 150, 50);
  myGLCD.drawRectXY(170, 140, 150, 50);
  myGLCD.fillRectXY(22, 37, 106, 16);
  myGLCD.fillRectXY(192, 37, 106, 16);
  myGLCD.fillRectXY(22, 97, 106, 16);
  myGLCD.fillRectXY(192, 97, 106, 16);
  myGLCD.fillRectXY(22, 157, 106, 16);
  myGLCD.fillRectXY(192, 157, 106, 16);
}


// This is the main routine to monitor which buttons on the touch screen was clicked.
void navigation(){
  //int y = map(tp.y, 106, 899, 0, 320);
  //int x = map(tp.x, 167, 880, 0, 240);
  tp.x = map(tp.y, 106, 899, 0, 320);
  tp.y = map(tp.x, 167, 880, 0, 240);
  Serial.print("\r\ntp.x="); Serial.print(tp.x);
  Serial.print(" tp.y="); Serial.println(tp.y);   
  Serial.print("Page ");Serial.println(page);
  digitalWrite(BUZZER, HIGH);
  

   // area 1
    if (tp.y > 20 && tp.y < 70 && tp.x > 0 && tp.x < 150) { // if this area is pressed
      Serial.println("Doorway Clicked ");
      if (doorway == true){
        buzzoff();
        mqtt.publish("bs/hallway1/cmnd/switch/Power1","OFF",1);
        myGLCD.fillRect(0, 20, 150, 50,BLACK);
        doorway = false;
      }
      else{
        buzzon();
        mqtt.publish("bs/hallway1/cmnd/switch/Power1","ON",1);
        myGLCD.fillRect(0, 20, 150, 50,GREEN);
        doorway = true;
      }
      page = 0;
      homescr();
      for (int i = 0; i<20000;i++);
    }
    // area 2
    if (tp.y > 20 && tp.y < 70 && tp.x > 170 && tp.x < 320) {
      Serial.println("Living Rm Clicked ");
      if (livrm == true){
        buzzoff();
        mqtt.publish("bs/livingrm/cmnd/switch/Power2","OFF",1);
        myGLCD.fillRect(170, 20, 150, 50, BLACK);
        livrm = false;
      }
      else{
        buzzon();
        mqtt.publish("bs/livingrm/cmnd/switch/Power2","ON",1);
        myGLCD.fillRect(170, 20, 150, 50, GREEN);
        livrm = true;
      }
      page = 0;
      homescr();
      for (int i = 0; i<20000;i++);
    }
    
    // area 3
    if (tp.y > 80 && tp.y < 130 && tp.x > 0 && tp.x < 150) {
      Serial.println("Balcony Clicked ");
      if (balcony == true){
        buzzoff();
        mqtt.publish("bs/balcony/cmnd/switch/Power","OFF",1);
        myGLCD.fillRect(0, 80, 150, 50,BLACK);
        balcony = false;
      }
      else{
        buzzon();
        mqtt.publish("bs/balcony/cmnd/switch/Power","ON",1);
        myGLCD.fillRect(0, 80, 150, 50,GREEN);
        balcony = true;
      }
      page = 0;
      homescr();
      for (int i = 0; i<20000;i++);
    }
    // area 4
    if (tp.y > 80 && tp.y < 130 && tp.x > 170 && tp.x < 320) {
      Serial.println("Liv Rm Fan Clikced ");
      if (livrmfan == true){
        buzzoff();
        mqtt.publish("bs/livingrm/cmnd/switch/Power1","OFF",1);
        Serial.println("MQTT OFF");
        myGLCD.fillRect(170,80,150,50,BLACK);
        livrmfan = false;
      }
      else{
        buzzon();
        mqtt.publish("bs/livingrm/cmnd/switch/Power1","ON",1);
        Serial.println("MQTT ON");
        myGLCD.fillRect(170,80,150,50,GREEN);
        livrmfan = true;
      }
      page = 0;
      homescr();
      for (int i = 0; i<20000;i++);
    }
    // area 5
    if (tp.y > 140 && tp.y < 190 && tp.x > 0 && tp.x < 150) {
      Serial.println("hallway1 Clicked ");
      if (hallway1 == true){
        buzzoff();
        mqtt.publish("bs/hallway1/cmnd/switch/Power2","OFF",1);
        Serial.println("MQTT OFF");
        myGLCD.fillRect(0,140,150,50,BLACK);
        hallway1 = false;
      }
      else{
        buzzon();
        mqtt.publish("bs/hallway1/cmnd/switch/Power2","ON",1);
        Serial.println("MQTT ON");
        myGLCD.fillRect(0,140,150,50,GREEN);
        hallway1 = true;
      }
      page = 0;
      homescr();
      for (int i = 0; i<20000;i++);
    }
    // area 6
    if (tp.y > 140 && tp.y < 190 && tp.x > 170 && tp.x < 320) {
      Serial.println("kitchen Clicked ");
      if (kitchen == true){
        buzzoff();
        mqtt.publish("bs/kitchen/cmnd/switch/Power","OFF",1);
        Serial.println("MQTT OFF");
        myGLCD.fillRect(170,140,150,50,BLACK);
        kitchen = false;
      }
      else{
        buzzon();
        //livrmfan = true;
        mqtt.publish("bs/kitchen/cmnd/switch/Power","ON",1);
        Serial.println("MQTT ON");
        myGLCD.fillRect(170,140,150,50,GREEN);
        kitchen = true;
      }
      page = 0;
      homescr();
      for (int i = 0; i<20000;i++);
    }
    // home. This is the reset button for the Mega2560
    if (tp.y > 200 && tp.y < 240 && tp.x > 280 && tp.x < 320) { // if the home icon is pressed
      Serial.println("home button pressed! resetting the Mega2560!");
      Serial3.println("home button pressed! resetting the Mega2560!");
      buzzreboot();
      asm volatile ("  jmp 0"); 
    }
    // message area
    if (tp.y > 1 && tp.y < 240 && tp.x > 1 && tp.x < 44) {
      refreshDateTime(); //refresh the datetime when this is pressed.
    }
    digitalWrite(BUZZER, LOW);
}


