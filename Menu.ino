
int page = 0;
int sleep = 0;
int pulsev = 0;
int redflag = 0;
int greenflag = 0;
int redled = 12;
int greenled = 13;
int backlight = 10;  //pin-10 uno  pin-35 mega  pin-3 for shield if you have done the MOD   for PWM backlight
unsigned long sleeptime;

int prevpage;
unsigned long currenttime;
unsigned long ssitime;
char voltage[10];
char battpercenttxt [10];


int getBandgap(void) // Returns actual value of Vcc (x 100) 415 = 4.15v
{
        
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
     // For mega boards
     const long InternalReferenceVoltage = 1115L;  // Adjust this value to your boards specific internal BG voltage x1000
        // REFS1 REFS0          --> 0 1, AVcc internal ref. -Selects AVcc reference
        // MUX4 MUX3 MUX2 MUX1 MUX0  --> 11110 1.1V (VBG)         -Selects channel 30, bandgap voltage, to measure
     ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR)| (0<<MUX5) | (1<<MUX4) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
  
#else
     // For 168/328 boards
     const long InternalReferenceVoltage = 1056L;  // Adjust this value to your boards specific internal BG voltage x1000
        // REFS1 REFS0          --> 0 1, AVcc internal ref. -Selects AVcc external reference
        // MUX3 MUX2 MUX1 MUX0  --> 1110 1.1V (VBG)         -Selects channel 14, bandgap voltage, to measure
     ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
       
#endif
     delay(50);  // Let mux settle a little to get a more stable A/D conversion
        // Start a conversion  
     ADCSRA |= _BV( ADSC );
        // Wait for it to complete
     while( ( (ADCSRA & (1<<ADSC)) != 0 ) );
        // Scale the value
     int results = (((InternalReferenceVoltage * 1023L) / ADC) + 5L) / 10L; // calculates for straight line value 
     return results;
}




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
  myGLCD.setCursor(31, 157);
  myGLCD.println("Hallway");
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
  myGLCD.println("Welcome to NGS's family. The Brownstone v0.2 ");
  homescr(); // draw the homescreen
  myGLCD.setColor(WHITE);
  myGLCD.drawRectXY(0, 200, 245, 40); // message box
  refreshDateTime();
}

void refreshDateTime() {
  Serial.print("refreshDateTime  ");
  Serial.println(currentTimeStr);
  myGLCD.fillRect(12, 213, 226, 16, BLACK); // black out the inside of the message box
  myGLCD.setTextColor(WHITE);
  myGLCD.setTextSize(2);
  myGLCD.setCursor(31, 213);
  myGLCD.println(currentTimeStr);

}


void redraw() { // redraw the page
  if ((prevpage != 6) || (page !=7)) {
    clearcenter();
  }
  if (page == 0) {
    homescr();
  }
}
void yled(int xled) { // "flashes" the "yellow" LED
  /*for(i = xled ; i >= 0; i-=1) { 
    digitalWrite(greenled, LOW);
    digitalWrite(redled, HIGH);
    delay(1);
    digitalWrite(greenled, HIGH);
    digitalWrite(redled, LOW);
    delay(1);    
  }
  digitalWrite(greenled, LOW);
  if (greenflag == 1) {
    digitalWrite(redled, LOW);
    digitalWrite(greenled, HIGH);
  }
  if (redflag == 1) {
    digitalWrite(greenled, LOW);
    digitalWrite(redled, HIGH);
  }*/
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


void decimate(char test[],int dec) {  // creates decimal function  decimate(string containing integer, number of decimal places)

 int i=0;  
 int length=strlen(test);
 char msg[10]="";

 strcpy(msg,test);

 if (length <= dec) {
   for(i=dec;i>(dec-length);i--)  msg[i] = msg[i-(dec-length+1)];
   for(i=0;i<(dec+1-length);i++)  msg[i]='0';
   length = strlen(msg);
 }
 for (i=length;i>(length-dec);i--)  msg[i]=msg[i-1];
 msg[length-dec]='.';

 strcpy(test,msg);
} 

void navigation(){
  int y = map(tp.y, 106, 899, 0, 320);
  int x = map(tp.x, 167, 880, 0, 240);
   tp.x = y;
   tp.y = x;
  Serial.print("\r\ntp.x="); Serial.print(tp.x);
  Serial.print(" tp.y="); Serial.println(tp.y);   
  Serial.print("Page ");Serial.println(page);
  digitalWrite(BUZZER, HIGH);
  

   // area 1
    if (tp.y > 20 && tp.y < 70 && tp.x > 0 && tp.x < 150) { // if this area is pressed
      Serial.println("Doorway Clicked ");
      if (doorway == true){
        buzzoff();
        mqtt.publish("/brownstone/doorway/switch/cmnd/Power","OFF",1);
        myGLCD.fillRect(0, 20, 150, 50,BLACK);
        doorway = false;
      }
      else{
        buzzon();
        mqtt.publish("/brownstone/doorway/switch/cmnd/Power","ON",1);
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
      //mqtt.publish("/brownstone/livrm/switch/cmnd/Power","");
      if (livrm == true){
        buzzoff();
        mqtt.publish("/brownstone/livrm/switch/cmnd/Power","OFF",1);
        myGLCD.fillRect(170, 20, 150, 50, BLACK);
        livrm = false;
      }
      else{
        buzzon();
        mqtt.publish("/brownstone/livrm/switch/cmnd/Power","ON",1);
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
      //mqtt.publish("/brownstone/balcony/switch/cmnd/Power","");
      if (balcony == true){
        buzzoff();
        mqtt.publish("/brownstone/balcony/switch/cmnd/Power","OFF",1);
        myGLCD.fillRect(0, 80, 150, 50,BLACK);
        balcony = false;
      }
      else{
        buzzon();
        mqtt.publish("/brownstone/balcony/switch/cmnd/Power","ON",1);
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
      //mqtt.publish("/brownstone/livrmfan/switch/cmnd/Power","");
      if (livrmfan == true){
        buzzoff();
        mqtt.publish("/brownstone/livrmfan/switch/cmnd/Power","OFF",1);
        Serial.println("MQTT OFF");
        myGLCD.fillRect(170,80,150,50,BLACK);
        livrmfan = false;
      }
      else{
        buzzon();
        mqtt.publish("/brownstone/livrmfan/switch/cmnd/Power","ON",1);
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
      Serial.println("Hallway Clicked ");
      if (hallway == true){
        buzzoff();
        mqtt.publish("/brownstone/hallway/switch/cmnd/Power","OFF",1);
        Serial.println("MQTT OFF");
        myGLCD.fillRect(0,140,150,50,BLACK);
        hallway = false;
      }
      else{
        buzzon();
        mqtt.publish("/brownstone/hallway/switch/cmnd/Power","ON",1);
        Serial.println("MQTT ON");
        myGLCD.fillRect(0,140,150,50,GREEN);
        hallway = true;
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
        mqtt.publish("/brownstone/kitchen/switch/cmnd/Power","OFF",1);
        Serial.println("MQTT OFF");
        myGLCD.fillRect(170,140,150,50,BLACK);
        kitchen = false;
      }
      else{
        buzzon();
        //livrmfan = true;
        mqtt.publish("/brownstone/kitchen/switch/cmnd/Power","ON",1);
        Serial.println("MQTT ON");
        myGLCD.fillRect(170,140,150,50,GREEN);
        kitchen = true;
      }
      page = 0;
      homescr();
      for (int i = 0; i<20000;i++);
    }
    // home
    if (tp.y > 200 && tp.y < 240 && tp.x > 280 && tp.x < 320) { // if the home icon is pressed
      Serial.println("home");
      //digitalWrite(BUZZER, HIGH);
      playMusic();
      if (page == 0) { // if you are already on the home page
        drawhomeiconred(); // draw the home icon red
        delay(250); // wait a bit
        drawhomeicon(); // draw the home icon back to white
        return; // if you were on the home page, stop.
      }
      else { // if you are not on the settings, home, or keyboard page
        page = prevpage; // a value to keep track of what WAS on the screen to redraw/erase only what needs to be
        page = 0; // make the current page home
        redraw(); // redraw the page
      }
    }
    // message area
    if (tp.y > 1 && tp.y < 240 && tp.x > 1 && tp.x < 44) {
      refreshDateTime(); // erase the message
    }


    digitalWrite(BUZZER, LOW);
}


