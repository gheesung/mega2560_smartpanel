// acknowledgement https://github.com/prenticedavid/MCUFRIEND_kbv
// all the touch screen related code

#define TOUCH_ORIENTATION  LANDSCAPE
//#define TOUCH_ORIENTATION  PORTRAIT

// Variables used in the loop
static int count;
static int detected = 0;


void readResistiveTouch(void)
{
    tp = myTouch.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
}

bool ISPRESSED(void)
{
    //Serial.println("ISPRESSED");
    // .kbv this was too sensitive !!
    // now touch has to be stable for 50ms
    int count = 0;
    bool state, oldstate;
    while (count < 10) {
        readResistiveTouch();
        state = tp.z > 20 && tp.z < 1000;
        if (state == oldstate) count++;
        else  count = 0;
        oldstate = state;

    }
    return oldstate;
}

void showpoint(void)
{
    Serial.print("\r\nx="); Serial.print(tp.x);
    Serial.print(" y="); Serial.print(tp.y);
    Serial.print(" z="); Serial.print(tp.z);
}


void showpins(int A, int D, int value, const char *msg)
{
    char buf[40];
    sprintf(buf, "%s (A%d, D%d) = %d", msg, A - A0, D, value);
    Serial.println(buf);
}

boolean diagnose_pins()
{
    int i, j, value, Apins[2], Dpins[2], Values[2], found = 0;
    Serial.println("Making all control and bus pins INPUT_PULLUP");
    Serial.println("Typical 30k Analog pullup with corresponding pin");
    Serial.println("would read low when digital is written LOW");
    Serial.println("e.g. reads ~25 for 300R X direction");
    Serial.println("e.g. reads ~30 for 500R Y direction");
    Serial.println("");
    for (i = A0; i < A5; i++) pinMode(i, INPUT_PULLUP);
    for (i = 2; i < 10; i++) pinMode(i, INPUT_PULLUP);
    for (i = A0; i < A4; i++) {
        for (j = 5; j < 10; j++) {
            pinMode(j, OUTPUT);
            digitalWrite(j, LOW);
            value = analogRead(i);   // ignore first reading
            value = analogRead(i);
            if (value < 100) {
                showpins(i, j, value, "Testing :");
                if (found < 2) {
                    Apins[found] = i;
                    Dpins[found] = j;
                    Values[found] = value;
                    found++;
                }
            }
            pinMode(j, INPUT_PULLUP);
        }
    }
    if (found == 2) {
        Serial.println("Diagnosing as:-");
        int idx = Values[0] < Values[1];
        for (i = 0; i < 2; i++) {
            showpins(Apins[i], Dpins[i], Values[i],
                     (Values[i] < Values[!i]) ? "XM,XP: " : "YP,YM: ");
        }
        XM = Apins[!idx]; XP = Dpins[!idx]; YP = Apins[idx]; YM = Dpins[idx];
        myTouch = TouchScreen(XP, YP, XM, YM, 300);
        return true;
    }
    Serial.println("BROKEN TOUCHSCREEN");
    return false;
}

void readCoordinates()
{
    int iter = 100; // higher value to increase sensitivity
    int cnt = 0;
    uint32_t tx = 0;
    uint32_t ty = 0;
    boolean OK = false;

    while (OK == false)
    {
        cnt = 0;
        iter = 100;  // higher value to increase sensitivity
        do
        {
            readResistiveTouch();
            // showpoint(tp);
            if (tp.z > 20 && tp.z < 1000)
            {
                tx += tp.x;
                ty += tp.y;
                cnt++;
            }
        } while (cnt < iter );
        if (cnt >= iter)
        {
            OK = true;
            Serial.println("Read Confirmed");

        }
        else
        {
            tx = 0;
            ty = 0;
            cnt = 0;
        }
    }

    cx = tx / iter;
    cy = ty / iter;
}


void toHex(uint32_t num)
{
    buf[0] = '0';
    buf[1] = 'x';
    buf[10] = 'U';
    buf[11] = 'L';
    buf[12] = 0;
    for (int zz = 9; zz > 1; zz--)
    {
        if ((num & 0xF) > 9)
            buf[zz] = (num & 0xF) + 55;
        else
            buf[zz] = (num & 0xF) + 48;
        num = num >> 4;
    }
}

void showNumI(char *msg, uint32_t val, int x, int y)
{
    myGLCD.print(msg, x, y);
    myGLCD.printNumI(val, x + 50, y);
}
