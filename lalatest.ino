#include <LiquidCrystal.h>
#include <SPI.h>
#include <Max3421e.h>
#include <usb.h>
 
/* keyboard data taken from configuration descriptor */
LiquidCrystal lcd(37,3,44,39,45,41);
#define KBD_ADDR  1
#define KBD_EP    1
#define KBD_IF    0
#define EP_MAXPKTSIZE  8
#define EP_POLL        0x0a

#define CAPSLOCK    0x39
#define NUMLOCK     0x53
#define SCROLLLOCK  0x47
#define DELETE      0x4C
#define SHIFT       0x0F
#define RETURN      0x2A
#define BANG        0x1E
#define AT          0x1F
#define POUND       0x20
#define DOLLAR      0x21
#define PERCENT     0x22
#define CAP         0x23
#define AND         0x24
#define STAR        0x25
#define OPENBKT     0x26
#define CLOSEBKT    0x27
#define SPACE       0x2C
#define HYPHEN      0x2D
#define EQUAL       0x2E
#define SQBKTOPEN   0x2F
#define SQBKTCLOSE  0x30
#define BACKSLASH   0x31
#define SEMICOLON   0x33
#define INVCOMMA    0x34
#define TILDE       0x35
#define COMMA       0x36
#define PERIOD      0x37
#define FRONTSLASH  0x38

/* Sticky keys output report bitmasks */
#define REP_NUMLOCK       0x01
#define REP_CAPSLOCK      0x02
#define REP_SCROLLLOCK    0x04
/**/
EP_RECORD ep_record[ 2 ];  //endpoint record structure for the keyboard
//
const int stepPin1 = 6; 
const int dirPin1 = 7; 
const int stepPin2 = 4;
const int dirPin2 = 5;
const int Z_stepPin = 12;
const int Z_dirPin = 13;

const int STEPS_PER_REV = 3200;
const float STEPS_PER_RADIAN = STEPS_PER_REV / (2 * M_PI);
const float STEPS_PER_MM = STEPS_PER_REV / 39;

const float ORIGIN_X = 0;
const float ORIGIN_Y = 71.6;
const float initialHeadTo = 117.6;


float currentX = ORIGIN_X;
float currentY = ORIGIN_Y;

float angleError = 0;
float radiusError = 0;

// Sets plotter arm speed for when pen is drawing (down) and in the air (up)
int delayPenDown = 6000; //6000 = normal, 3000 = fast, 1000 = insane
int delayPenUp = 1600; //1600 = normal, 600 = fast
int backforBuff = 0;
int leftrightBuff = 0;

float newx=0;
float newy=0;
float space = 5;
char r ;
//
char buf[8] = {0};
char i;
char inputchar;

/* Sticky key state */
bool numLock = false;
bool capsLock = false;
bool scrollLock = false;
bool line = false;

void setup();
void loop();
 
MAX3421E Max;
USB Usb;
 
void setup()
{
    lcd.begin(16,2);
    Serial.begin( 9600 );
    Serial.println("Start");
    lcd.setCursor(0,0);
    lcd.print("Hello!!");
    lcd.setCursor(0,1);
    lcd.print("Start");
    Max.powerOn();
    delay( 200 );
    pinMode(stepPin1,OUTPUT); 
  pinMode(dirPin1,OUTPUT);
  pinMode (stepPin2, OUTPUT);
  pinMode (dirPin2, OUTPUT);
  pinMode (Z_stepPin, OUTPUT);
  pinMode (Z_dirPin,OUTPUT);
  pinMode(8,OUTPUT); //MS for radius motor
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(22, OUTPUT); // MS for angle motor
  pinMode(24, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(28, OUTPUT); // MS for z motor
  pinMode(30, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(34,INPUT);
  pinMode(36,INPUT);
}
////////////////////////////////////////////////////////////////////////////////////
//WRITING FUNCTIONS
void Restore () {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Restoring");
  Serial.println("Restore to the initial position");
  step(stepPin1, -backforBuff);
  step(stepPin2, -leftrightBuff);
  backforBuff = 0;
  leftrightBuff = 0;
  Serial.println ("Finished");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Finished");
}
void Calibrate () {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Calibrate");
  Serial.println ("Calibrating");
  lineTo (0,0);
  Serial.println ("Finished");
}
void drawSpace (float x, float y) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Spacing");
  Serial.println ("Spacing");
  moveTo (x+25.000, y+0);
  newx=currentX-ORIGIN_X;
  newy=currentY-initialHeadTo;
  Serial.println ("Finished");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Finished");
}

void drawR (float x, float y) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Writing R");
      Serial.println ("Writing R");
      lineTo(x+0.000,y+0.000);
      lineTo(x+0.000,y+50.546);
      lineTo(x+16.849,y+50.546);
      lineTo(x+17.776,y+50.508);
      lineTo(x+18.697,y+50.393);
      lineTo(x+19.606,y+50.202);
      lineTo(x+20.496,y+49.937);
      lineTo(x+21.361,y+49.600);
      lineTo(x+22.195,y+49.192);
      lineTo(x+22.992,y+48.717);
      lineTo(x+23.748,y+48.178);
      lineTo(x+24.456,y+47.578);
      lineTo(x+25.113,y+46.921);
      lineTo(x+25.713,y+46.213);
      lineTo(x+26.252,y+45.457);
      lineTo(x+26.727,y+44.660);
      lineTo(x+27.135,y+43.826);
      lineTo(x+27.473,y+42.961);
      lineTo(x+27.737,y+42.071);
      lineTo(x+27.928,y+41.162);
      lineTo(x+28.043,y+40.241);
      lineTo(x+28.081,y+39.314);
      lineTo(x+28.081,y+33.697);
      lineTo(x+28.043,y+32.770);
      lineTo(x+27.928,y+31.849);
      lineTo(x+27.737,y+30.940);
      lineTo(x+27.473,y+30.050);
      lineTo(x+27.135,y+29.185);
      lineTo(x+26.727,y+28.351);
      lineTo(x+26.252,y+27.554);
      lineTo(x+25.713,y+26.798);
      lineTo(x+25.113,y+26.090);
      lineTo(x+24.456,y+25.433);
      lineTo(x+23.748,y+24.833);
      lineTo(x+22.992,y+24.294);
      lineTo(x+22.195,y+23.819);
      lineTo(x+21.361,y+23.411);
      lineTo(x+20.496,y+23.073);
      lineTo(x+19.606,y+22.809);
      lineTo(x+18.697,y+22.618);
      lineTo(x+17.776,y+22.503);
      lineTo(x+16.849,y+22.465);
      lineTo(x+0.000,y+22.465);
      moveTo(x+16.849,y+22.465);
      lineTo(x+16.849,y+22.465);
      lineTo(x+28.081,y+0.000);
      moveTo (x +28.081+ space, y +0);
      newx=currentX-ORIGIN_X;
      newy=currentY-initialHeadTo;
      Serial.println ("Finished");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Finished");
}
void drawM (float x, float y) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Writing M");
      Serial.println ("Writing M");
      lineTo (x + 0,y + 50.546);
      lineTo (x + 16.849,y + 22.465);
      lineTo (x + 33.697,y + 50.546);
      lineTo (x + 33.697,y + 0);
      delay (250);
      moveTo (x + 33.697 + space, y +0);
      delay (250);
      newx=currentX-ORIGIN_X;
      newy=currentY-initialHeadTo;
      Serial.println ("Finished");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Finished");
}
void drawI (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing I");
     Serial.println ("Writing I");
     lineTo (x+0, y+50.546);
     lineTo (x+0, y+0);
     delay (250);
     moveTo (x + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawT (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing T");
     Serial.println ("Writing T");
     moveTo (x+0, y+50.546);
     lineTo (x+0, y+50.546);
     lineTo (x+33.697, y+50.546);
     delay (250);
     moveTo (x+16.849, y+50.546);
     delay (250);
     lineTo (x+16.849, y+ 50.546);
     lineTo (x+16.849, y+ 0 );
     delay (250);
     moveTo (x + 33.697 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawA (float x, float y ) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing A");
     Serial.println ("Writing A");
     lineTo (x+16.849,y+50.546);
     lineTo (x+33.697,y+0);
     delay (250);
     moveTo (x+29.017, y+14.041);
     delay (250);
     lineTo (x+29.017,y+14.041);
     lineTo (x+4.680,y+14.041);
     delay (250);
     moveTo (x + 33.697 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawB (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing B");
     Serial.println ("Writing B");
     lineTo (x+0,y+50.546);
     lineTo (x+14.041,y+50.546);
     lineTo (x+14.993,y+50.506);
     lineTo (x+15.939,y+50.384);
     lineTo (x+16.871,y+50.184);
     lineTo (x+17.783 ,y+49.904);
     lineTo (x+18.667 ,y+49.549);
     lineTo (x+19.519 ,y+49.120);
     lineTo (x+20.330 ,y+48.620);
     lineTo (x+21.097 ,y+48.053);
     lineTo (x+21.813 ,y+47.423);
     lineTo (x+22.472 ,y+46.735);
     lineTo (x+23.071 ,y+45.993);
     lineTo (x+23.605 ,y+45.203);
     lineTo (x+24.070 ,y+44.370);
     lineTo (x+24.463 ,y+43.502);
     lineTo (x+24.781 ,y+42.603);
     lineTo (x+25.021 ,y+41.680);
     lineTo (x+25.182 ,y+40.740);
     lineTo (x+25.263 ,y+39.790);
     lineTo (x+25.263,y+38.837);
     lineTo (x+25.182 ,y+37.887);
     lineTo (x+25.021 ,y+36.947);
     lineTo (x+24.781 ,y+36.024);
     lineTo (x+24.463 ,y+35.125);
     lineTo (x+24.070 ,y+34.257);
     lineTo (x+23.605 ,y+33.424);
     lineTo (x+23.071 ,y+32.634);
     lineTo (x+22.472 ,y+31.893);
     lineTo (x+21.813 ,y+31.204);
     lineTo (x+21.097 ,y+30.574);
     lineTo (x+20.330 ,y+30.007);
     lineTo (x+19.519 ,y+29.507);
     lineTo (x+18.667 ,y+29.078);
     lineTo (x+17.783 ,y+28.723);
     lineTo (x+16.871 ,y+28.444);
     lineTo (x+15.939 ,y+28.243);
     lineTo (x+14.993 ,y+28.122);
     lineTo (x+14.041 ,y+28.081);
     delay (250);
     moveTo (x+14.602 ,y+28.081);
     delay (250);
     lineTo (x+14.602,y+28.081);
     lineTo (x+0.000, y+ 28.081);
     delay (250);
     moveTo (x+14.602 ,y+28.081);
     delay (250);
     lineTo (x+14.602 ,y+28.081);
     lineTo (x+15.715 ,y+28.035);
     lineTo (x+16.821 ,y+27.897);
     lineTo (x+17.911 ,y+27.669);
     lineTo (x+18.979 ,y+27.351);
     lineTo (x+20.016 ,y+26.946);
     lineTo (x+21.017 ,y+26.457);
     lineTo (x+21.974 ,y+25.886);
     lineTo (x+22.881 ,y+25.239);
     lineTo (x+23.731 ,y+24.519);
     lineTo (x+24.519 ,y+23.731);
     lineTo (x+25.239 ,y+22.881);
     lineTo (x+25.886 ,y+21.974);
     lineTo (x+26.456 ,y+21.017);
     lineTo (x+26.946 ,y+20.017);
     lineTo (x+27.351 ,y+18.979);
     lineTo (x+27.669 ,y+17.911);
     lineTo (x+27.897 ,y+16.821);
     lineTo (x+28.035 ,y+15.715);
     lineTo (x+28.081 ,y+14.602);
     lineTo (x+28.081 ,y+13.479);
     lineTo (x+28.035 ,y+12.366);
     lineTo (x+27.897 ,y+11.260);
     lineTo (x+27.669 ,y+10.170);
     lineTo (x+27.351 ,y+9.102);
     lineTo (x+26.946 ,y+8.065);
     lineTo (x+26.456 ,y+7.064);
     lineTo (x+25.886 ,y+6.107);
     lineTo (x+25.239 ,y+5.200);
     lineTo (x+24.519 ,y+4.350);
     lineTo (x+23.731 ,y+3.562);
     lineTo (x+22.881 ,y+2.842);
     lineTo (x+21.974 ,y+2.195);
     lineTo (x+21.017 ,y+1.625);
     lineTo (x+20.016 ,y+1.135);
     lineTo (x+18.979 ,y+0.730);
     lineTo (x+17.911 ,y+0.412);
     lineTo (x+16.821 ,y+0.184);
     lineTo (x+15.715 ,y+0.046);
     lineTo (x+14.602 ,y+0.000);
     lineTo (x+0.000, y+0);
     delay (250);
     moveTo (x + 28.081 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawC (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing C");
     Serial.println ("Writing C");
     moveTo (x+22.465 ,y+50.546);
     delay (250);
     lineTo (x+22.465 ,y+50.546);
     lineTo (x+11.232 ,y+50.546);
     lineTo (x+10.305 ,y+50.508);
     lineTo (x+9.384 ,y+50.393);
     lineTo (x+8.475 ,y+50.202);
     lineTo (x+7.585 ,y+49.937);
     lineTo (x+6.720 ,y+49.600);
     lineTo (x+5.886 ,y+49.192);
     lineTo (x+5.089 ,y+48.717);
     lineTo (x+4.333 ,y+48.178);
     lineTo (x+3.625 ,y+47.578);
     lineTo (x+2.968 ,y+46.921);
     lineTo (x+2.368 ,y+46.213);
     lineTo (x+1.829 ,y+45.457);
     lineTo (x+1.354 ,y+44.660);
     lineTo (x+0.946 ,y+43.826);
     lineTo (x+0.609 ,y+42.961);
     lineTo (x+0.344 ,y+42.071);
     lineTo (x+0.153 ,y+41.162);
     lineTo (x+0.038 ,y+40.241);
     lineTo (x+0.000 ,y+39.314);
     lineTo (x+0 ,y+11.232);
     lineTo (x+0.038 ,y+10.305);
     lineTo (x+0.153 ,y+9.384);
     lineTo (x+0.344 ,y+8.475);
     lineTo (x+0.609 ,y+7.585);
     lineTo (x+0.946 ,y+6.720);
     lineTo (x+1.354 ,y+5.886);
     lineTo (x+1.829 ,y+5.089);
     lineTo (x+2.368 ,y+4.333);
     lineTo (x+2.968 ,y+3.625);
     lineTo (x+3.625 ,y+2.968);
     lineTo (x+4.333 ,y+2.368);
     lineTo (x+5.089 ,y+1.829);
     lineTo (x+5.886 ,y+1.354);
     lineTo (x+6.720 ,y+0.946);
     lineTo (x+7.585 ,y+0.609);
     lineTo (x+8.475 ,y+0.344);
     lineTo (x+9.384 ,y+0.153);
     lineTo (x+10.305 ,y+0.038);
     lineTo (x+11.232 ,y+0.000);
     lineTo (x+22.465 ,y+0);
     delay (250);
     moveTo (x + 22.465 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawD (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing D");
     Serial.println ("Writing D");
     lineTo (x+0.000 ,y+0.000);
     lineTo (x+16.849, y+0);
     lineTo (x+17.776 ,y+0.038);
     lineTo (x+18.697 ,y+0.153);
     lineTo (x+19.606 ,y+0.344);
     lineTo (x+20.496 ,y+0.609);
     lineTo (x+21.361 ,y+0.946);
     lineTo (x+22.195 ,y+1.354);
     lineTo (x+22.992 ,y+1.829);
     lineTo (x+23.748 ,y+2.368);
     lineTo (x+24.456 ,y+2.968);
     lineTo (x+25.113 ,y+3.625);
     lineTo (x+25.713 ,y+4.333);
     lineTo (x+26.252 ,y+5.089);
     lineTo (x+26.727 ,y+5.886);
     lineTo (x+27.135 ,y+6.720);
     lineTo (x+27.473 ,y+7.585);
     lineTo (x+27.737 ,y+8.475);
     lineTo (x+27.928 ,y+9.384);
     lineTo (x+28.043 ,y+10.305);
     lineTo (x+28.081 ,y+11.232);
     lineTo (x+28.081 ,y+39.314);
     lineTo (x+28.043 ,y+40.241);
     lineTo (x+27.928 ,y+41.162);
     lineTo (x+27.737 ,y+42.071);
     lineTo (x+27.473 ,y+42.961);
     lineTo (x+27.135 ,y+43.826);
     lineTo (x+26.727 ,y+44.660);
     lineTo (x+26.252 ,y+45.457);
     lineTo (x+25.713 ,y+46.213);
     lineTo (x+25.113 ,y+46.921);
     lineTo (x+24.456 ,y+47.578);
     lineTo (x+23.748 ,y+48.178);
     lineTo (x+22.992 ,y+48.717);
     lineTo (x+22.195 ,y+49.192);
     lineTo (x+21.361 ,y+49.600);
     lineTo (x+20.496 ,y+49.937);
     lineTo (x+19.606 ,y+50.202);
     lineTo (x+18.697 ,y+50.393);
     lineTo (x+17.776 ,y+50.508);
     lineTo (x+16.849 ,y+50.546);
     lineTo (x+0.000 ,y+50.546);
     lineTo (x+0 ,y+0.000);
     delay (250);
     moveTo (x + 28.081 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawE (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing E");
     Serial.println ("Writing E");
     lineTo (x+22.465,y+50.546);
     lineTo (x+0,y+50.546);
     lineTo (x+0, y+0);
     lineTo (x+22.465, y+0);
     delay (250);
     moveTo (x+16.849,y+28.081);
     delay (250);
     lineTo (x+16.849,y+28.081);
     lineTo (x+0, y+28.081);
     delay (250);
     moveTo (x + 22.465 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawF (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing F");
     Serial.println ("Writing F");
     lineTo (x+22.465,y+50.546);
     lineTo (x+0,y+50.546);
     lineTo (x+0, y+0);
     delay (250);
     moveTo (x+0, y+28.081);
     delay (250);
     lineTo (x+0, y+ 28.081);
     lineTo (x+22.465, y+28.081);
     delay (250);
     moveTo (x + 22.465 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawG (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing G");
     Serial.println ("Writing G");
     moveTo (x+28.081 ,y+50.546);
     delay (250);
     lineTo (x+28.081 ,y+50.546);
     lineTo (x+11.232 ,y+50.546);
     lineTo (x+10.305 ,y+50.508);
     lineTo (x+9.384 ,y+50.393);
     lineTo (x+8.475 ,y+50.202);
     lineTo (x+7.585 ,y+49.937);
     lineTo (x+6.720 ,y+49.600);
     lineTo (x+5.886 ,y+49.192);
     lineTo (x+5.089 ,y+48.717);
     lineTo (x+4.333 ,y+48.178);
     lineTo (x+3.625 ,y+47.578);
     lineTo (x+2.968 ,y+46.921);
     lineTo (x+2.368 ,y+46.213);
     lineTo (x+1.829 ,y+45.457);
     lineTo (x+1.354 ,y+44.660);
     lineTo (x+0.946 ,y+43.826);
     lineTo (x+0.609 ,y+42.961);
     lineTo (x+0.344 ,y+42.071);
     lineTo (x+0.153 ,y+41.162);
     lineTo (x+0.038 ,y+40.241);
     lineTo (x+0.000 ,y+39.314);
     lineTo (x+0 ,y+11.232);
     lineTo (x+0.038 ,y+10.305);
     lineTo (x+0.153 ,y+9.384);
     lineTo (x+0.344 ,y+8.475);
     lineTo (x+0.609 ,y+7.585);
     lineTo (x+0.946 ,y+6.720);
     lineTo (x+1.354 ,y+5.886);
     lineTo (x+1.829 ,y+5.089);
     lineTo (x+2.368 ,y+4.333);
     lineTo (x+2.968 ,y+3.625);
     lineTo (x+3.625 ,y+2.968);
     lineTo (x+4.333 ,y+2.368);
     lineTo (x+5.089 ,y+1.829);
     lineTo (x+5.886 ,y+1.354);
     lineTo (x+6.720 ,y+0.946);
     lineTo (x+7.585 ,y+0.609);
     lineTo (x+8.475 ,y+0.344);
     lineTo (x+9.384 ,y+0.153);
     lineTo (x+10.305 ,y+0.038);
     lineTo (x+11.232 ,y+0.000);
     lineTo (x+28.081 ,y+0);
     lineTo (x+28.081 ,y+28.081);
     lineTo (x+19.657 ,y+28.081);
     delay (250);
     moveTo (x + 28.081 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawH (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing H");
     Serial.println ("Writing H");
     lineTo (x+0, y+50.546);
     lineTo (x+0, y+0);
     delay (250);
     moveTo (x+28.081,y+0);
     delay (250);
     lineTo (x+28.081, y+0);
     lineTo (x+28.081, y+50.546);
     delay (250);
     moveTo (x+28.081,y+28.081);
     delay (250);
     lineTo (x+28.081, y+28.081);
     lineTo (x+0, y+28.081);
     moveTo (x + 28.081 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawJ (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing J");
     Serial.println ("Writing J");
     lineTo (x+16.849, y+50.546);
     lineTo (x+16.849, y+11.232);
     lineTo (x+16.810, y+10.305);
     lineTo (x+16.695, y+9.384);
     lineTo (x+16.505, y+8.475);
     lineTo (x+16.240, y+7.585);
     lineTo (x+15.903, y+6.720);
     lineTo (x+15.495, y+5.886);
     lineTo (x+15.020, y+5.089);
     lineTo (x+14.480, y+4.333);
     lineTo (x+13.880, y+3.625);
     lineTo (x+13.224, y+2.968);
     lineTo (x+12.515, y+2.368);
     lineTo (x+11.760, y+1.829);
     lineTo (x+10.962, y+1.354);
     lineTo (x+10.128, y+0.946);
     lineTo (x+9.263, y+0.609);
     lineTo (x+8.374, y+0.344);
     lineTo (x+7.465, y+0.153);
     lineTo (x+6.544, y+0.038);
     lineTo (x+5.616, y+0);
     lineTo (x+0, y+0);
     delay (250);
     moveTo (x + 16.849 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawK (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing K");
     Serial.println ("Writing K");
     lineTo (x+0, y+50.546);
     lineTo (x+0, y+0);
     delay (250);
     moveTo (x+0, y+19.657);
     delay (250);
     lineTo (x+0, y+19.657);
     lineTo (x+28.081, y+50.546);
     delay (250);
     moveTo (x+9.387, y+29.982);
     delay (250);
     lineTo (x+9.387, y+29.982);
     lineTo (x+28.081, y+0);
     delay (250);
     moveTo (x + 28.081 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawL (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing L");
     Serial.println ("Writing L");
     lineTo (x+0, y+50.546);
     lineTo (x+0, y+0);
     lineTo (x+22.465, y+0);
     delay (250);
     moveTo (x + 22.465 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawN (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing N");
     Serial.println ("Writing N");
     lineTo (x+0, y+0);
     lineTo (x+0, y+50.546);
     lineTo (x+28.081, y+0);
     lineTo (x+28.081, y+50.546);
     delay (250);
     moveTo (x + 28.081 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawO (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing O");
     Serial.println ("Writing O");
     moveTo (x+0.000 ,y+11.232);
     delay (250);
     lineTo (x+0.000 ,y+11.232);
     lineTo (x+0 ,y+39.314);
     lineTo (x+0.038 ,y+40.241);
     lineTo (x+0.153 ,y+41.162);
     lineTo (x+0.344 ,y+42.071);
     lineTo (x+0.609 ,y+42.961);
     lineTo (x+0.946 ,y+43.826);
     lineTo (x+1.354 ,y+44.660);
     lineTo (x+1.829 ,y+45.457);
     lineTo (x+2.368 ,y+46.213);
     lineTo (x+2.968 ,y+46.921);
     lineTo (x+3.625 ,y+47.578);
     lineTo (x+4.333 ,y+48.178);
     lineTo (x+5.089 ,y+48.717);
     lineTo (x+5.886 ,y+49.192);
     lineTo (x+6.720 ,y+49.600);
     lineTo (x+7.585 ,y+49.937);
     lineTo (x+8.475 ,y+50.202);
     lineTo (x+9.384 ,y+50.393);
     lineTo (x+10.305 ,y+50.508);
     lineTo (x+11.232 ,y+50.546);
     lineTo (x+16.849 ,y+50.546);
     lineTo (x+17.776 ,y+50.508);
     lineTo (x+18.697 ,y+50.393);
     lineTo (x+19.606 ,y+50.202);
     lineTo (x+20.496 ,y+49.937);
     lineTo (x+21.361 ,y+49.600);
     lineTo (x+22.195 ,y+49.192);
     lineTo (x+22.992 ,y+48.717);
     lineTo (x+23.748 ,y+48.178);
     lineTo (x+24.456 ,y+47.578);
     lineTo (x+25.113 ,y+46.921);
     lineTo (x+25.713 ,y+46.213);
     lineTo (x+26.252 ,y+45.457);
     lineTo (x+26.727 ,y+44.660);
     lineTo (x+27.135 ,y+43.826);
     lineTo (x+27.473 ,y+42.961);
     lineTo (x+27.737 ,y+42.071);
     lineTo (x+27.928 ,y+41.162);
     lineTo (x+28.043 ,y+40.241);
     lineTo (x+28.081 ,y+39.314);
     lineTo (x+28.081 ,y+11.232);
     lineTo (x+28.043 ,y+10.305);
     lineTo (x+27.928 ,y+9.384);
     lineTo (x+27.737 ,y+8.475);
     lineTo (x+27.473 ,y+7.585);
     lineTo (x+27.135 ,y+6.720);
     lineTo (x+26.727 ,y+5.886);
     lineTo (x+26.252 ,y+5.089);
     lineTo (x+25.713 ,y+4.333);
     lineTo (x+25.113 ,y+3.625);
     lineTo (x+24.456 ,y+2.968);
     lineTo (x+23.748 ,y+2.368);
     lineTo (x+22.992 ,y+1.829);
     lineTo (x+22.195 ,y+1.354);
     lineTo (x+21.361 ,y+0.946);
     lineTo (x+20.496 ,y+0.609);
     lineTo (x+19.606 ,y+0.344);
     lineTo (x+18.697 ,y+0.153);
     lineTo (x+17.776 ,y+0.038);
     lineTo (x+16.849 ,y+0.000);
     lineTo (x+11.232 ,y+0);
     lineTo (x+10.305 ,y+0.038);
     lineTo (x+9.384 ,y+0.153);
     lineTo (x+8.475 ,y+0.344);
     lineTo (x+7.585 ,y+0.609);
     lineTo (x+6.720 ,y+0.946);
     lineTo (x+5.886 ,y+1.354);
     lineTo (x+5.089 ,y+1.829);
     lineTo (x+4.333 ,y+2.368);
     lineTo (x+3.625 ,y+2.968);
     lineTo (x+2.968 ,y+3.625);
     lineTo (x+2.368 ,y+4.333);
     lineTo (x+1.829 ,y+5.089);
     lineTo (x+1.354 ,y+5.886);
     lineTo (x+0.946 ,y+6.720);
     lineTo (x+0.609 ,y+7.585);
     lineTo (x+0.344 ,y+8.475);
     lineTo (x+0.153 ,y+9.384);
     lineTo (x+0.038 ,y+10.305);
     lineTo (x+0.000 ,y+11.232);
     delay (250);
     moveTo (x + 28.081 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawP (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing P");
     Serial.println ("Writing P");
     moveTo (x+0.000 ,y+0.000);
     delay (250);
     lineTo (x+0.000 ,y+0.000);
     lineTo (x+0,y+50.546);
     lineTo (x+16.849 ,y+50.546);
     lineTo (x+17.776 ,y+50.508);
     lineTo (x+18.697 ,y+50.393);
     lineTo (x+19.606 ,y+50.202);
     lineTo (x+20.496 ,y+49.937);
     lineTo (x+21.361 ,y+49.600);
     lineTo (x+22.195 ,y+49.192);
     lineTo (x+22.992 ,y+48.717);
     lineTo (x+23.748 ,y+48.178);
     lineTo (x+24.456 ,y+47.578);
     lineTo (x+25.113 ,y+46.921);
     lineTo (x+25.713 ,y+46.213);
     lineTo (x+26.252 ,y+45.457);
     lineTo (x+26.727 ,y+44.660);
     lineTo (x+27.135 ,y+43.826);
     lineTo (x+27.473 ,y+42.961);
     lineTo (x+27.737 ,y+42.071);
     lineTo (x+27.928 ,y+41.162);
     lineTo (x+28.043 ,y+40.241);
     lineTo (x+28.081 ,y+39.314);
     lineTo (x+28.081 ,y+33.697);
     lineTo (x+28.043 ,y+32.770);
     lineTo (x+27.928 ,y+31.849);
     lineTo (x+27.737 ,y+30.940);
     lineTo (x+27.473 ,y+30.050);
     lineTo (x+27.135 ,y+29.185);
     lineTo (x+26.727 ,y+28.351);
     lineTo (x+26.252 ,y+27.554);
     lineTo (x+25.713 ,y+26.798);
     lineTo (x+25.113 ,y+26.090);
     lineTo (x+24.456 ,y+25.433);
     lineTo (x+23.748 ,y+24.833);
     lineTo (x+22.992 ,y+24.294);
     lineTo (x+22.195 ,y+23.819);
     lineTo (x+21.361 ,y+23.411);
     lineTo (x+20.496 ,y+23.073);
     lineTo (x+19.606 ,y+22.809);
     lineTo (x+18.697 ,y+22.618);
     lineTo (x+17.776 ,y+22.503);
     lineTo (x+16.849 ,y+22.465);
     lineTo (x+0.000 ,y+22.465);
     delay (250);
     moveTo (x + 28.081 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawQ (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing Q");
     Serial.println ("Writing Q");
     moveTo (x+0.000 ,y+11.232);
     delay (250);
     lineTo (x+0.000 ,y+11.232);
     lineTo (x+0 ,y+39.314);
     lineTo (x+0.038 ,y+40.241);
     lineTo (x+0.153 ,y+41.162);
     lineTo (x+0.344 ,y+42.071);
     lineTo (x+0.609 ,y+42.961);
     lineTo (x+0.946 ,y+43.826);
     lineTo (x+1.354 ,y+44.660);
     lineTo (x+1.829 ,y+45.457);
     lineTo (x+2.368 ,y+46.213);
     lineTo (x+2.968 ,y+46.921);
     lineTo (x+3.625 ,y+47.578);
     lineTo (x+4.333 ,y+48.178);
     lineTo (x+5.089 ,y+48.717);
     lineTo (x+5.886 ,y+49.192);
     lineTo (x+6.720 ,y+49.600);
     lineTo (x+7.585 ,y+49.937);
     lineTo (x+8.475 ,y+50.202);
     lineTo (x+9.384 ,y+50.393);
     lineTo (x+10.305 ,y+50.508);
     lineTo (x+11.232 ,y+50.546);
     lineTo (x+16.849 ,y+50.546);
     lineTo (x+17.776 ,y+50.508);
     lineTo (x+18.697 ,y+50.393);
     lineTo (x+19.606 ,y+50.202);
     lineTo (x+20.496 ,y+49.937);
     lineTo (x+21.361 ,y+49.600);
     lineTo (x+22.195 ,y+49.192);
     lineTo (x+22.992 ,y+48.717);
     lineTo (x+23.748 ,y+48.178);
     lineTo (x+24.456 ,y+47.578);
     lineTo (x+25.113 ,y+46.921);
     lineTo (x+25.713 ,y+46.213);
     lineTo (x+26.252 ,y+45.457);
     lineTo (x+26.727 ,y+44.660);
     lineTo (x+27.135 ,y+43.826);
     lineTo (x+27.473 ,y+42.961);
     lineTo (x+27.737 ,y+42.071);
     lineTo (x+27.928 ,y+41.162);
     lineTo (x+28.043 ,y+40.241);
     lineTo (x+28.081 ,y+39.314);
     lineTo (x+28.081 ,y+11.232);
     lineTo (x+28.043 ,y+10.305);
     lineTo (x+27.928 ,y+9.384);
     lineTo (x+27.737 ,y+8.475);
     lineTo (x+27.473 ,y+7.585);
     lineTo (x+27.135 ,y+6.720);
     lineTo (x+26.727 ,y+5.886);
     lineTo (x+26.252 ,y+5.089);
     lineTo (x+25.713 ,y+4.333);
     lineTo (x+25.113 ,y+3.625);
     lineTo (x+24.456 ,y+2.968);
     lineTo (x+23.748 ,y+2.368);
     lineTo (x+22.992 ,y+1.829);
     lineTo (x+22.195 ,y+1.354);
     lineTo (x+21.361 ,y+0.946);
     lineTo (x+20.496 ,y+0.609);
     lineTo (x+19.606 ,y+0.344);
     lineTo (x+18.697 ,y+0.153);
     lineTo (x+17.776 ,y+0.038);
     lineTo (x+16.849 ,y+0.000);
     lineTo (x+11.232 ,y+0);
     lineTo (x+10.305 ,y+0.038);
     lineTo (x+9.384 ,y+0.153);
     lineTo (x+8.475 ,y+0.344);
     lineTo (x+7.585 ,y+0.609);
     lineTo (x+6.720 ,y+0.946);
     lineTo (x+5.886 ,y+1.354);
     lineTo (x+5.089 ,y+1.829);
     lineTo (x+4.333 ,y+2.368);
     lineTo (x+3.625 ,y+2.968);
     lineTo (x+2.968 ,y+3.625);
     lineTo (x+2.368 ,y+4.333);
     lineTo (x+1.829 ,y+5.089);
     lineTo (x+1.354 ,y+5.886);
     lineTo (x+0.946 ,y+6.720);
     lineTo (x+0.609 ,y+7.585);
     lineTo (x+0.344 ,y+8.475);
     lineTo (x+0.153 ,y+9.384);
     lineTo (x+0.038 ,y+10.305);
     lineTo (x+0.000 ,y+11.232);
     delay (250);
     moveTo (x+16.849, y+11.232);
     delay (250);
     lineTo (x+16.849 ,y+11.232);
     lineTo (x+33.697 ,y+0);
     delay (250);
     moveTo (x + 33.697 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawS (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing S");
     Serial.println ("Writing S");
     moveTo (x+28.081 ,y+46.513);
     delay (250);
     lineTo (x+28.081 ,y+46.513);
     lineTo (x+25.434 ,y+47.729);
     lineTo (x+22.699 ,y+48.735);
     lineTo (x+19.895 ,y+49.524);
     lineTo (x+17.037 ,y+50.090);
     lineTo (x+14.144 ,y+50.432);
     lineTo (x+11.232 ,y+50.546);
     lineTo (x+10.266 ,y+50.504);
     lineTo (x+9.307 ,y+50.380);
     lineTo (x+8.362 ,y+50.173);
     lineTo (x+7.439 ,y+49.886);
     lineTo (x+6.543 ,y+49.520);
     lineTo (x+5.683 ,y+49.079);
     lineTo (x+4.863 ,y+48.566);
     lineTo (x+4.091 ,y+47.983);
     lineTo (x+3.372 ,y+47.337);
     lineTo (x+2.711 ,y+46.631);
     lineTo (x+2.113 ,y+45.871);
     lineTo (x+1.582 ,y+45.062);
     lineTo (x+1.124 ,y+44.210);
     lineTo (x+0.740 ,y+43.323);
     lineTo (x+0.434 ,y+42.405);
     lineTo (x+0.208 ,y+41.465);
     lineTo (x+0.064 ,y+40.509);
     lineTo (x+0.002 ,y+39.543);
     lineTo (x+0.024 ,y+38.576);
     lineTo (x+0.129 ,y+37.615);
     lineTo (x+0.316 ,y+36.666);
     lineTo (x+0.585 ,y+35.737);
     lineTo (x+0.932 ,y+34.834);
     lineTo (x+1.355 ,y+33.965);
     lineTo (x+1.852 ,y+33.135);
     lineTo (x+2.418 ,y+32.351);
     lineTo (x+3.050 ,y+31.618);
     lineTo (x+3.742 ,y+30.943);
     lineTo (x+4.490 ,y+30.330);
     lineTo (x+5.288 ,y+29.783);
     lineTo (x+6.130 ,y+29.307);
     lineTo (x+21.951 ,y+21.239);
     lineTo (x+22.793 ,y+20.763);
     lineTo (x+23.591 ,y+20.216);
     lineTo (x+24.339 ,y+19.603);
     lineTo (x+25.031 ,y+18.928);
     lineTo (x+25.663 ,y+18.195);
     lineTo (x+26.229 ,y+17.411);
     lineTo (x+26.726 ,y+16.581);
     lineTo (x+27.149 ,y+15.712);
     lineTo (x+27.496 ,y+14.809);
     lineTo (x+27.765 ,y+13.880);
     lineTo (x+27.952 ,y+12.931);
     lineTo (x+28.057 ,y+11.970);
     lineTo (x+28.079 ,y+11.003);
     lineTo (x+28.017 ,y+10.037);
     lineTo (x+27.873 ,y+9.081);
     lineTo (x+27.647 ,y+8.141);
     lineTo (x+27.341 ,y+7.223);
     lineTo (x+26.957 ,y+6.336);
     lineTo (x+26.499 ,y+5.484);
     lineTo (x+25.968 ,y+4.675);
     lineTo (x+25.371 ,y+3.915);
     lineTo (x+24.710 ,y+3.209);
     lineTo (x+23.990 ,y+2.563);
     lineTo (x+23.218 ,y+1.980);
     lineTo (x+22.398 ,y+1.467);
     lineTo (x+21.538 ,y+1.026);
     lineTo (x+20.642 ,y+0.660);
     lineTo (x+19.719 ,y+0.373);
     lineTo (x+18.774 ,y+0.166);
     lineTo (x+17.815 ,y+0.042);
     lineTo (x+16.849 ,y+0.000);
     lineTo (x+13.937 ,y+0.114);
     lineTo (x+11.044 ,y+0.456);
     lineTo (x+8.186 ,y+1.022);
     lineTo (x+5.382 ,y+1.811);
     lineTo (x+2.647 ,y+2.817);
     lineTo (x+0.000 ,y+4.033);
     delay (250);
     moveTo (x + 28.079 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawU (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing U");
     Serial.println ("Writing U");
     moveTo (x+0.000 ,y+50.559);
     delay (250);
     lineTo (x+0.000 ,y+50.559);
     lineTo (x+0 ,y+14.044);
     lineTo (x+0.051 ,y+12.853);
     lineTo (x+0.202 ,y+11.671);
     lineTo (x+0.453 ,y+10.505);
     lineTo (x+0.802 ,y+9.365);
     lineTo (x+1.247 ,y+8.259);
     lineTo (x+1.783 ,y+7.195);
     lineTo (x+2.408 ,y+6.180);
     lineTo (x+3.117 ,y+5.221);
     lineTo (x+3.905 ,y+4.326);
     lineTo (x+4.766 ,y+3.502);
     lineTo (x+5.693 ,y+2.753);
     lineTo (x+6.681 ,y+2.085);
     lineTo (x+7.721 ,y+1.504);
     lineTo (x+8.808 ,y+1.013);
     lineTo (x+9.932 ,y+0.616);
     lineTo (x+11.085 ,y+0.315);
     lineTo (x+12.260 ,y+0.114);
     lineTo (x+13.448 ,y+0.013);
     lineTo (x+14.640 ,y+0.013);
     lineTo (x+15.828 ,y+0.114);
     lineTo (x+17.003 ,y+0.315);
     lineTo (x+18.156 ,y+0.616);
     lineTo (x+19.280 ,y+1.013);
     lineTo (x+20.367 ,y+1.504);
     lineTo (x+21.407 ,y+2.085);
     lineTo (x+22.395 ,y+2.753);
     lineTo (x+23.323 ,y+3.502);
     lineTo (x+24.183 ,y+4.326);
     lineTo (x+24.971 ,y+5.221);
     lineTo (x+25.680 ,y+6.180);
     lineTo (x+26.305 ,y+7.195);
     lineTo (x+26.841 ,y+8.259);
     lineTo (x+27.286 ,y+9.365);
     lineTo (x+27.635 ,y+10.505);
     lineTo (x+27.886 ,y+11.671);
     lineTo (x+28.038 ,y+12.853);
     lineTo (x+28.088 ,y+14.044);
     lineTo (x+28.088 ,y+50.559);
     delay (250);
     moveTo (x + 28.088 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawV (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing V");
     Serial.println ("Writing V");
     moveTo (x+0, y+50.546);
     delay (250);
     lineTo (x+0, y+50.546);
     lineTo (x+16.849, y+0);
     lineTo (x+33.697, y+50.546);
     delay (250);
     moveTo (x + 33.697 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawW (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing W");
     Serial.println ("Writing W");
     moveTo (x+0, y+50.546);
     delay (250);
     lineTo (x+0, y+50.546);
     lineTo (x+11.232, y+0);
     lineTo (x+22.465, y+33.697);
     lineTo (x+33.697, y+0);
     lineTo (x+44.930, y+50.546);
     delay (250);
     moveTo (x + 33.697 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawX (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing X");
     Serial.println ("Writing X");
     moveTo (x+0, y+50.546);
     delay (250);
     lineTo (x+0, y+50.546);
     lineTo (x+33.697, y+0);
     delay (250);
     moveTo (x+0, y+0);
     delay (250);
     lineTo (x+0, y+0);
     lineTo (x+33.697, y+50.546);
     delay (250);
     moveTo (x + 33.697 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawY (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing Y");
     Serial.println ("Writing Y");
     moveTo (x+0, y+50.546);
     delay (250);
     lineTo (x+0, y+50.546);
     lineTo (x+16.849, y+28.081);
     lineTo (x+16.849, y+0);
     delay (250);
     moveTo (x+16.849, y+28.081);
     delay (250);
     lineTo (x+16.849, y+28.081);
     lineTo (x+33.697, y+50.546);
     delay (250);
     moveTo (x + 33.697 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void drawZ (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing Z");
     Serial.println ("Writing Z");
     moveTo (x+0, y+50.546);
     delay (250);
     lineTo (x+0, y+50.546);
     lineTo (x+28.081, y+50.546);
     lineTo (x+0, y+0);
     lineTo (x+28.081, y+0);
     delay (250);
     moveTo (x + 28.081 + space, y +0);
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void draw1 (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing 1");
     Serial.println ("Writing 1");
     moveTo (x+0, y+ 39.314);
     delay (250);
     lineTo (x+0.000, y+39.314);
     lineTo (x+11.232, y+50.546);
     lineTo (x+11.232, y+0.000);
     moveTo (x + 33.697 + space, y +0); 
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void draw2 (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing 2");
     Serial.println ("Writing 2");
      moveTo (x+22.469,y+0.000);
      delay (250);
      lineTo (x+22.469 ,y+0.000);
      lineTo (x+0.000 ,y+0);
      lineTo (x+21.708 ,y+37.337);
      lineTo (x+22.098 ,y+38.151);
      lineTo (x+22.242 ,y+38.579);
      lineTo (x+22.426 ,y+39.463);
      lineTo (x+22.464 ,y+39.913);
      lineTo (x+22.431 ,y+40.816);
      lineTo (x+22.254 ,y+41.701);
      lineTo (x+21.835 ,y+43.044);
      lineTo (x+21.480 ,y+43.931);
      lineTo (x+21.052 ,y+44.784);
      lineTo (x+20.552 ,y+45.598);
      lineTo (x+19.985 ,y+46.367);
      lineTo (x+19.355 ,y+47.085);
      lineTo (x+18.667 ,y+47.747);
      lineTo (x+17.924 ,y+48.348);
      lineTo (x+17.133 ,y+48.883);
      lineTo (x+16.300 ,y+49.350);
      lineTo (x+15.430 ,y+49.744);
      lineTo (x+14.529 ,y+50.062);
      lineTo (x+13.605 ,y+50.303);
      lineTo (x+12.664 ,y+50.465);
      lineTo (x+11.712 ,y+50.546);
      lineTo (x+10.757 ,y+50.546);
      lineTo (x+9.805 ,y+50.465);
      lineTo (x+8.864 ,y+50.303);
      lineTo (x+7.940 ,y+50.062);
      lineTo (x+7.040 ,y+49.744);
      lineTo (x+6.170 ,y+49.350);
      lineTo (x+5.336 ,y+48.883);
      lineTo (x+4.545 ,y+48.348);
      lineTo (x+3.803 ,y+47.747);
      lineTo (x+3.114 ,y+47.085);
      lineTo (x+2.484 ,y+46.367);
      lineTo (x+1.917 ,y+45.598);
      lineTo (x+1.418 ,y+44.784);
      lineTo (x+0.989 ,y+43.931);
      lineTo (x+0.635 ,y+43.044);
      lineTo (x+0.357 ,y+42.130);
     moveTo (x + 33.697 + space, y +0); 
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void draw3 (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing 3");
     Serial.println ("Writing 3");
      moveTo (x+0.000 ,y+50.546);
      delay (250);
      lineTo (x+0.000 ,y+50.546);
      lineTo (x+11.232, y+50.546);
      lineTo (x+12.185 ,y+50.506);
      lineTo (x+13.131 ,y+50.384);
      lineTo (x+14.063 ,y+50.184);
      lineTo (x+14.974 ,y+49.904);
      lineTo (x+15.859 ,y+49.549);
      lineTo (x+16.710 ,y+49.120);
      lineTo (x+17.522 ,y+48.620);
      lineTo (x+18.289 ,y+48.053);
      lineTo (x+19.005 ,y+47.423);
      lineTo (x+19.664 ,y+46.735);
      lineTo (x+20.263 ,y+45.993);
      lineTo (x+20.797 ,y+45.203);
      lineTo (x+21.262 ,y+44.370);
      lineTo (x+21.655 ,y+43.502);
      lineTo (x+21.973 ,y+42.603);
      lineTo (x+22.213 ,y+41.680);
      lineTo (x+22.374 ,y+40.740);
      lineTo (x+22.455 ,y+39.790);
      lineTo (x+22.455 ,y+38.837);
      lineTo (x+22.374 ,y+37.887);
      lineTo (x+22.213 ,y+36.947);
      lineTo (x+21.973 ,y+36.024);
      lineTo (x+21.655 ,y+35.125);
      lineTo (x+21.262 ,y+34.257);
      lineTo (x+20.797 ,y+33.424);
      lineTo (x+20.263 ,y+32.634);
      lineTo (x+19.664 ,y+31.893);
      lineTo (x+19.005 ,y+31.204);
      lineTo (x+18.289 ,y+30.574);
      lineTo (x+17.522 ,y+30.007);
      lineTo (x+16.710 ,y+29.507);
      lineTo (x+15.859 ,y+29.078);
      lineTo (x+14.974 ,y+28.723);
      lineTo (x+14.063 ,y+28.444);
      lineTo (x+13.131 ,y+28.243);
      lineTo (x+12.185 ,y+28.122);
      lineTo (x+11.232 ,y+28.081);
      lineTo (x+5.616 ,y+28.081);
      delay (250);
      moveTo (x+11.232 ,y+28.081);
      delay (250);
      lineTo (x+11.232, y+28.081);
      lineTo (x+12.160 ,y+28.043);
      lineTo (x+13.081 ,y+27.928);
      lineTo (x+13.990 ,y+27.737);
      lineTo (x+14.880 ,y+27.473);
      lineTo (x+15.744 ,y+27.135);
      lineTo (x+16.578 ,y+26.727);
      lineTo (x+17.376 ,y+26.252);
      lineTo (x+18.132 ,y+25.713);
      lineTo (x+18.840 ,y+25.113);
      lineTo (x+19.496 ,y+24.456);
      lineTo (x+20.096 ,y+23.748);
      lineTo (x+20.636 ,y+22.992);
      lineTo (x+21.111 ,y+22.195);
      lineTo (x+21.519 ,y+21.361);
      lineTo (x+21.856 ,y+20.496);
      lineTo (x+22.121 ,y+19.606);
      lineTo (x+22.312 ,y+18.697);
      lineTo (x+22.427 ,y+17.776);
      lineTo (x+22.465 ,y+16.849);
      lineTo (x+22.465 ,y+11.232);
      lineTo (x+22.427 ,y+10.305);
      lineTo (x+22.312 ,y+9.384);
      lineTo (x+22.121 ,y+8.475);
      lineTo (x+21.856 ,y+7.585);
      lineTo (x+21.519 ,y+6.720);
      lineTo (x+21.111 ,y+5.886);
      lineTo (x+20.636 ,y+5.089);
      lineTo (x+20.096 ,y+4.333);
      lineTo (x+19.496 ,y+3.625);
      lineTo (x+18.840 ,y+2.968);
      lineTo (x+18.132 ,y+2.368);
      lineTo (x+17.376 ,y+1.829);
      lineTo (x+16.578 ,y+1.354);
      lineTo (x+15.744 ,y+0.946);
      lineTo (x+14.880 ,y+0.609);
      lineTo (x+13.990 ,y+0.344);
      lineTo (x+13.081 ,y+0.153);
      lineTo (x+12.160 ,y+0.038);
      lineTo (x+11.232 ,y+0.000);
      lineTo (x+0.000, y+ 0);
     moveTo (x + 33.697 + space, y +0); 
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void draw4 (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing 4");
     Serial.println ("Writing 4");
      moveTo (x+19.657 ,y+0.000);
      delay (250);
      lineTo (x+19.657 ,y+0.000);
      lineTo (x+19.657 ,y+22.465);
      delay (250);
      moveTo (x+28.081 ,y+11.232);
      delay (250);
      lineTo (x+28.081 ,y+11.232);
      lineTo (x+0.000 ,y+11.232);
      lineTo (x+11.232 ,y+50.546);
     moveTo (x + 33.697 + space, y +0); 
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void draw5 (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing 5");
     Serial.println ("Writing 5");
      moveTo (x+22.465 ,y+50.546);
      delay (250);
      lineTo (x+22.465 ,y+50.546);
      lineTo (x+0.000 ,y+50.546);
      lineTo (x+0 ,y+28.081);
      lineTo (x+11.232 ,y+28.081);
      lineTo (x+12.160 ,y+28.043);
      lineTo (x+13.081 ,y+27.928);
      lineTo (x+13.990 ,y+27.737);
      lineTo (x+14.880 ,y+27.473);
      lineTo (x+15.744 ,y+27.135);
      lineTo (x+16.578 ,y+26.727);
      lineTo (x+17.376 ,y+26.252);
      lineTo (x+18.132 ,y+25.713);
      lineTo (x+18.840 ,y+25.113);
      lineTo (x+19.496 ,y+24.456);
      lineTo (x+20.096 ,y+23.748);
      lineTo (x+20.636 ,y+22.992);
      lineTo (x+21.111 ,y+22.195);
      lineTo (x+21.519 ,y+21.361);
      lineTo (x+21.856 ,y+20.496);
      lineTo (x+22.121 ,y+19.606);
      lineTo (x+22.312 ,y+18.697);
      lineTo (x+22.427 ,y+17.776);
      lineTo (x+22.465 ,y+16.849);
      lineTo (x+22.465 ,y+11.232);
      lineTo (x+22.427 ,y+10.305);
      lineTo (x+22.312 ,y+9.384);
      lineTo (x+22.121 ,y+8.475);
      lineTo (x+21.856 ,y+7.585);
      lineTo (x+21.519 ,y+6.720);
      lineTo (x+21.111 ,y+5.886);
      lineTo (x+20.636 ,y+5.089);
      lineTo (x+20.096 ,y+4.333);
      lineTo (x+19.496 ,y+3.625);
      lineTo (x+18.840 ,y+2.968);
      lineTo (x+18.132 ,y+2.368);
      lineTo (x+17.376 ,y+1.829);
      lineTo (x+16.578 ,y+1.354);
      lineTo (x+15.744 ,y+0.946);
      lineTo (x+14.880 ,y+0.609);
      lineTo (x+13.990 ,y+0.344);
      lineTo (x+13.081 ,y+0.153);
      lineTo (x+12.160 ,y+0.038);
      lineTo (x+11.232 ,y+0.000);
      lineTo (x+0.000 ,y+0);
     moveTo (x + 33.697 + space, y +0); 
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void draw6 (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing 6");
     Serial.println ("Writing 6");
      moveTo (x+16.852 ,y+50.556);
      delay  (250);
      lineTo (x+16.852 ,y+50.556);
      lineTo (x+14.558 ,y+49.105);
      lineTo (x+12.388 ,y+47.475);
      lineTo (x+10.356 ,y+45.675);
      lineTo (x+8.476 ,y+43.717);
      lineTo (x+6.760 ,y+41.615);
      lineTo (x+5.218 ,y+39.381);
      lineTo (x+3.861 ,y+37.031);
      lineTo (x+2.697 ,y+34.578);
      lineTo (x+1.735 ,y+32.041);
      lineTo (x+0.979 ,y+29.433);
      lineTo (x+0.436 ,y+26.774);
      lineTo (x+0.109 ,y+24.080);
      lineTo (x+0.000 ,y+21.368);
      lineTo (x+0 ,y+11.235);
      lineTo (x+0.040 ,y+10.282);
      lineTo (x+0.162 ,y+9.336);
      lineTo (x+0.363 ,y+8.404);
      lineTo (x+0.642 ,y+7.492);
      lineTo (x+0.997 ,y+6.607);
      lineTo (x+1.427 ,y+5.756);
      lineTo (x+1.927 ,y+4.944);
      lineTo (x+2.494 ,y+4.177);
      lineTo (x+3.124 ,y+3.461);
      lineTo (x+3.812 ,y+2.801);
      lineTo (x+4.554 ,y+2.202);
      lineTo (x+5.344 ,y+1.668);
      lineTo (x+6.177 ,y+1.203);
      lineTo (x+7.046 ,y+0.810);
      lineTo (x+7.945 ,y+0.492);
      lineTo (x+8.868 ,y+0.252);
      lineTo (x+9.808 ,y+0.091);
      lineTo (x+10.758 ,y+0.010);
      lineTo (x+11.712 ,y+0.010);
      lineTo (x+12.662 ,y+0.091);
      lineTo (x+13.602 ,y+0.252);
      lineTo (x+14.524 ,y+0.492);
      lineTo (x+15.424 ,y+0.810);
      lineTo (x+16.293 ,y+1.203);
      lineTo (x+17.125 ,y+1.668);
      lineTo (x+17.915 ,y+2.202);
      lineTo (x+18.657 ,y+2.801);
      lineTo (x+19.346 ,y+3.461);
      lineTo (x+19.976 ,y+4.177);
      lineTo (x+20.543 ,y+4.944);
      lineTo (x+21.043 ,y+5.756);
      lineTo (x+21.472 ,y+6.607);
      lineTo (x+21.828 ,y+7.492);
      lineTo (x+22.107 ,y+8.404);
      lineTo (x+22.308 ,y+9.336);
      lineTo (x+22.429 ,y+10.282);
      lineTo (x+22.469 ,y+11.235);
      lineTo (x+22.469 ,y+16.852);
      lineTo (x+22.431 ,y+17.780);
      lineTo (x+22.316 ,y+18.701);
      lineTo (x+22.126 ,y+19.610);
      lineTo (x+21.861 ,y+20.500);
      lineTo (x+21.523 ,y+21.365);
      lineTo (x+21.115 ,y+22.199);
      lineTo (x+20.640 ,y+22.997);
      lineTo (x+20.100 ,y+23.753);
      lineTo (x+19.500 ,y+24.461);
      lineTo (x+18.844 ,y+25.118);
      lineTo (x+18.135 ,y+25.718);
      lineTo (x+17.379 ,y+26.257);
      lineTo (x+16.582 ,y+26.733);
      lineTo (x+15.748 ,y+27.140);
      lineTo (x+14.883 ,y+27.478);
      lineTo (x+13.993 ,y+27.743);
      lineTo (x+13.084 ,y+27.934);
      lineTo (x+12.162 ,y+28.048);
      lineTo (x+11.235 ,y+28.087);
      lineTo (x+0.676 ,y+28.087);
     moveTo (x + 33.697 + space, y +0); 
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void draw7 (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing 7");
     Serial.println ("Writing 7");
      moveTo (x+0.000 ,y+50.546);
      delay (250);
      lineTo (x+0.000 ,y+50.546);
      lineTo (x+22.465 ,y+50.546);
      lineTo (x+8.424 ,y+0.000);
      delay (250);
      moveTo (x+11.232 ,y+28.081);
      delay (250);
      lineTo (x+11.232 ,y+28.081);
      lineTo (x+22.465 ,y+28.081);
     moveTo (x + 33.697 + space, y +0); 
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void draw8 (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing 8");
     Serial.println ("Writing 8");
      moveTo (x+0.000 ,y+16.855);
      delay (250);
      lineTo (x+0.000 ,y+16.855);
      lineTo (x+0 ,y+11.237);
      lineTo (x+0.040 ,y+10.284);
      lineTo (x+0.162 ,y+9.338);
      lineTo (x+0.363 ,y+8.405);
      lineTo (x+0.642 ,y+7.493);
      lineTo (x+0.997 ,y+6.608);
      lineTo (x+1.427 ,y+5.757);
      lineTo (x+1.927 ,y+4.944);
      lineTo (x+2.494 ,y+4.178);
      lineTo (x+3.124 ,y+3.462);
      lineTo (x+3.813 ,y+2.802);
      lineTo (x+4.555 ,y+2.202);
      lineTo (x+5.345 ,y+1.668);
      lineTo (x+6.178 ,y+1.203);
      lineTo (x+7.047 ,y+0.810);
      lineTo (x+7.946 ,y+0.493);
      lineTo (x+8.869 ,y+0.252);
      lineTo (x+9.809 ,y+0.091);
      lineTo (x+10.760 ,y+0.010);
      lineTo (x+11.714 ,y+0.010);
      lineTo (x+12.664 ,y+0.091);
      lineTo (x+13.604 ,y+0.252);
      lineTo (x+14.527 ,y+0.493);
      lineTo (x+15.426 ,y+0.810);
      lineTo (x+16.295 ,y+1.203);
      lineTo (x+17.128 ,y+1.668);
      lineTo (x+17.918 ,y+2.202);
      lineTo (x+18.660 ,y+2.802);
      lineTo (x+19.349 ,y+3.462);
      lineTo (x+19.979 ,y+4.178);
      lineTo (x+20.546 ,y+4.944);
      lineTo (x+21.046 ,y+5.757);
      lineTo (x+21.476 ,y+6.608);
      lineTo (x+21.831 ,y+7.493);
      lineTo (x+22.111 ,y+8.405);
      lineTo (x+22.312 ,y+9.338);
      lineTo (x+22.433 ,y+10.284);
      lineTo (x+22.473 ,y+11.237);
      lineTo (x+22.473 ,y+16.855);
      lineTo (x+22.433 ,y+17.808);
      lineTo (x+22.312 ,y+18.754);
      lineTo (x+22.111 ,y+19.686);
      lineTo (x+21.831 ,y+20.598);
      lineTo (x+21.476 ,y+21.483);
      lineTo (x+21.046 ,y+22.335);
      lineTo (x+20.546 ,y+23.147);
      lineTo (x+19.979 ,y+23.914);
      lineTo (x+19.349 ,y+24.630);
      lineTo (x+18.660 ,y+25.290);
      lineTo (x+17.918 ,y+25.889);
      lineTo (x+17.128 ,y+26.423);
      lineTo (x+16.295 ,y+26.889);
      lineTo (x+15.426 ,y+27.281);
      lineTo (x+14.527 ,y+27.599);
      lineTo (x+13.604 ,y+27.839);
      lineTo (x+12.664 ,y+28.001);
      lineTo (x+11.714 ,y+28.082);
      lineTo (x+10.760 ,y+28.082);
      lineTo (x+9.809 ,y+28.001);
      lineTo (x+8.869 ,y+27.839);
      lineTo (x+7.946 ,y+27.599);
      lineTo (x+7.047 ,y+27.281);
      lineTo (x+6.178 ,y+26.889);
      lineTo (x+5.345 ,y+26.423);
      lineTo (x+4.555 ,y+25.889);
      lineTo (x+3.813 ,y+25.290);
      lineTo (x+3.124 ,y+24.630);
      lineTo (x+2.494 ,y+23.914);
      lineTo (x+1.927 ,y+23.147);
      lineTo (x+1.427 ,y+22.335);
      lineTo (x+0.997 ,y+21.483);
      lineTo (x+0.642 ,y+20.598);
      lineTo (x+0.363 ,y+19.686);
      lineTo (x+0.162 ,y+18.754);
      lineTo (x+0.040 ,y+17.808);
      lineTo (x+0.000 ,y+16.855);
      delay (250);
      lineTo (x+1.405 ,y+40.733);
      delay (250);
      lineTo (x+1.405 ,y+40.733);
      lineTo (x+1.440 ,y+41.567);
      lineTo (x+1.546 ,y+42.395);
      lineTo (x+1.722 ,y+43.210);
      lineTo (x+1.966 ,y+44.008);
      lineTo (x+2.277 ,y+44.783);
      lineTo (x+2.653 ,y+45.528);
      lineTo (x+3.091 ,y+46.239);
      lineTo (x+3.587 ,y+46.910);
      lineTo (x+4.138 ,y+47.536);
      lineTo (x+4.741 ,y+48.114);
      lineTo (x+5.390 ,y+48.638);
      lineTo (x+6.082 ,y+49.105);
      lineTo (x+6.810 ,y+49.512);
      lineTo (x+7.571 ,y+49.856);
      lineTo (x+8.358 ,y+50.134);
      lineTo (x+9.165 ,y+50.344);
      lineTo (x+9.988 ,y+50.485);
      lineTo (x+10.819 ,y+50.556);
      lineTo (x+11.654 ,y+50.556);
      lineTo (x+12.486 ,y+50.485);
      lineTo (x+13.308 ,y+50.344);
      lineTo (x+14.116 ,y+50.134);
      lineTo (x+14.903 ,y+49.856);
      lineTo (x+15.663 ,y+49.512);
      lineTo (x+16.392 ,y+49.105);
      lineTo (x+17.083 ,y+48.638);
      lineTo (x+17.732 ,y+48.114);
      lineTo (x+18.335 ,y+47.536);
      lineTo (x+18.886 ,y+46.910);
      lineTo (x+19.383 ,y+46.239);
      lineTo (x+19.820 ,y+45.528);
      lineTo (x+20.196 ,y+44.783);
      lineTo (x+20.507 ,y+44.008);
      lineTo (x+20.751 ,y+43.210);
      lineTo (x+20.927 ,y+42.395);
      lineTo (x+21.033 ,y+41.567);
      lineTo (x+21.069 ,y+40.733);
      lineTo (x+21.069 ,y+37.924);
      lineTo (x+21.033 ,y+37.090);
      lineTo (x+20.927 ,y+36.262);
      lineTo (x+20.751 ,y+35.446);
      lineTo (x+20.507 ,y+34.648);
      lineTo (x+20.196 ,y+33.874);
      lineTo (x+19.820 ,y+33.129);
      lineTo (x+19.383 ,y+32.418);
      lineTo (x+18.886 ,y+31.747);
      lineTo (x+18.335 ,y+31.121);
      lineTo (x+17.732 ,y+30.543);
      lineTo (x+17.083 ,y+30.019);
      lineTo (x+16.392 ,y+29.551);
      lineTo (x+15.663 ,y+29.144);
      lineTo (x+14.903 ,y+28.801);
      lineTo (x+14.116 ,y+28.523);
      lineTo (x+13.308 ,y+28.312);
      lineTo (x+12.486 ,y+28.171);
      lineTo (x+11.654 ,y+28.101);
      lineTo (x+10.819 ,y+28.101);
      lineTo (x+9.988 ,y+28.171);
      lineTo (x+9.165 ,y+28.312);
      lineTo (x+8.358 ,y+28.523);
      lineTo (x+7.571 ,y+28.801);
      lineTo (x+6.810 ,y+29.144);
      lineTo (x+6.082 ,y+29.551);
      lineTo (x+5.390 ,y+30.019);
      lineTo (x+4.741 ,y+30.543);
      lineTo (x+4.138 ,y+31.121);
      lineTo (x+3.587 ,y+31.747);
      lineTo (x+3.091 ,y+32.418);
      lineTo (x+2.653 ,y+33.129);
      lineTo (x+2.277 ,y+33.874);
      lineTo (x+1.966 ,y+34.648);
      lineTo (x+1.722 ,y+35.446);
      lineTo (x+1.546 ,y+36.262);
      lineTo (x+1.440 ,y+37.090);
      lineTo (x+1.405 ,y+37.924);
      lineTo (x+1.405 ,y+40.733);
     moveTo (x + 33.697 + space, y +0); 
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void draw9 (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing 9");
     Serial.println ("Writing 9");
      moveTo (x+5.617 ,y+0.000);
      delay (250);
      lineTo (x+5.617 ,y+0.000);
      lineTo (x+7.911 ,y+1.451);
      lineTo (x+10.081 ,y+3.081);
      lineTo (x+12.113 ,y+4.881);
      lineTo (x+13.993 ,y+6.839);
      lineTo (x+15.710 ,y+8.941);
      lineTo (x+17.252 ,y+11.175);
      lineTo (x+18.609 ,y+13.526);
      lineTo (x+19.772 ,y+15.978);
      lineTo (x+20.735 ,y+18.516);
      lineTo (x+21.490 ,y+21.123);
      lineTo (x+22.033 ,y+23.782);
      lineTo (x+22.360 ,y+26.477);
      lineTo (x+22.469 ,y+29.189);
      lineTo (x+22.469 ,y+39.321);
      lineTo (x+22.429 ,y+40.274);
      lineTo (x+22.308 ,y+41.220);
      lineTo (x+22.107 ,y+42.152);
      lineTo (x+21.828 ,y+43.064);
      lineTo (x+21.472 ,y+43.949);
      lineTo (x+21.043 ,y+44.801);
      lineTo (x+20.543 ,y+45.613);
      lineTo (x+19.976 ,y+46.379);
      lineTo (x+19.346 ,y+47.095);
      lineTo (x+18.657 ,y+47.755);
      lineTo (x+17.915 ,y+48.354);
      lineTo (x+17.125 ,y+48.888);
      lineTo (x+16.293 ,y+49.353);
      lineTo (x+15.424 ,y+49.746);
      lineTo (x+14.524 ,y+50.064);
      lineTo (x+13.602 ,y+50.304);
      lineTo (x+12.662 ,y+50.465);
      lineTo (x+11.712 ,y+50.546);
      lineTo (x+10.758 ,y+50.546);
      lineTo (x+9.808 ,y+50.465);
      lineTo (x+8.868 ,y+50.304);
      lineTo (x+7.945 ,y+50.064);
      lineTo (x+7.046 ,y+49.746);
      lineTo (x+6.177 ,y+49.353);
      lineTo (x+5.344 ,y+48.888);
      lineTo (x+4.554 ,y+48.354);
      lineTo (x+3.812 ,y+47.755);
      lineTo (x+3.124 ,y+47.095);
      lineTo (x+2.494 ,y+46.379);
      lineTo (x+1.927 ,y+45.613);
      lineTo (x+1.427 ,y+44.801);
      lineTo (x+0.997 ,y+43.949);
      lineTo (x+0.642 ,y+43.064);
      lineTo (x+0.363 ,y+42.152);
      lineTo (x+0.162 ,y+41.220);
      lineTo (x+0.040 ,y+40.274);
      lineTo (x+0.000 ,y+39.321);
      lineTo (x+0 ,y+33.704);
      lineTo (x+0.038 ,y+32.776);
      lineTo (x+0.153 ,y+31.855);
      lineTo (x+0.344 ,y+30.946);
      lineTo (x+0.609 ,y+30.056);
      lineTo (x+0.946 ,y+29.191);
      lineTo (x+1.354 ,y+28.357);
      lineTo (x+1.829 ,y+27.559);
      lineTo (x+2.369 ,y+26.804);
      lineTo (x+2.969 ,y+26.095);
      lineTo (x+3.626 ,y+25.438);
      lineTo (x+4.334 ,y+24.838);
      lineTo (x+5.090 ,y+24.299);
      lineTo (x+5.888 ,y+23.823);
      lineTo (x+6.722 ,y+23.416);
      lineTo (x+7.587 ,y+23.078);
      lineTo (x+8.477 ,y+22.813);
      lineTo (x+9.386 ,y+22.623);
      lineTo (x+10.307 ,y+22.508);
      lineTo (x+11.235 ,y+22.469);
      lineTo (x+21.793 ,y+22.469);
     moveTo (x + 33.697 + space, y +0); 
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
void draw0 (float x, float y) {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Writing 0");
     Serial.println ("Writing 0");
      moveTo (x+16.382 ,y+47.749);
      delay (250);
      lineTo (x+16.382 ,y+47.749);
      lineTo (x+16.075 ,y+48.183);
      lineTo (x+15.355 ,y+48.963);
      lineTo (x+14.510 ,y+49.607);
      lineTo (x+13.567 ,y+50.096);
      lineTo (x+13.067 ,y+50.277);
      lineTo (x+12.030 ,y+50.506);
      lineTo (x+10.969 ,y+50.552);
      lineTo (x+9.916 ,y+50.414);
      lineTo (x+8.903 ,y+50.096);
      lineTo (x+8.421 ,y+49.872);
      lineTo (x+7.523 ,y+49.304);
      lineTo (x+7.115 ,y+48.963);
      lineTo (x+6.395 ,y+48.183);
      lineTo (x+4.918 ,y+46.051);
      lineTo (x+3.881 ,y+44.270);
      lineTo (x+2.983 ,y+42.414);
      lineTo (x+2.229 ,y+40.496);
      lineTo (x+1.624 ,y+38.525);
      lineTo (x+1.170 ,y+36.514);
      lineTo (x+0.513 ,y+32.801);
      lineTo (x+0.122 ,y+29.049);
      lineTo (x+-0.000 ,y+25.279);
      lineTo (x+0.122 ,y+21.509);
      lineTo (x+0.513 ,y+17.757);
      lineTo (x+1.171 ,y+14.042);
      lineTo (x+1.624 ,y+12.033);
      lineTo (x+2.229 ,y+10.062);
      lineTo (x+2.983 ,y+8.143);
      lineTo (x+3.881 ,y+6.288);
      lineTo (x+4.918 ,y+4.506);
      lineTo (x+6.088 ,y+2.809);
      lineTo (x+6.738 ,y+1.969);
      lineTo (x+7.115 ,y+1.594);
      lineTo (x+7.960 ,y+0.950);
      lineTo (x+8.903 ,y+0.462);
      lineTo (x+9.916 ,y+0.144);
      lineTo (x+10.969 ,y+0.006);
      lineTo (x+11.501 ,y+0.006);
      lineTo (x+12.554 ,y+0.144);
      lineTo (x+13.067 ,y+0.281);
      lineTo (x+14.049 ,y+0.685);
      lineTo (x+14.947 ,y+1.254);
      lineTo (x+15.355 ,y+1.594);
      lineTo (x+16.075 ,y+2.375);
      lineTo (x+17.552 ,y+4.506);
      lineTo (x+18.589 ,y+6.288);
      lineTo (x+19.487 ,y+8.143);
      lineTo (x+20.241 ,y+10.062);
      lineTo (x+20.846 ,y+12.033);
      lineTo (x+21.300 ,y+14.044);
      lineTo (x+21.957 ,y+17.757);
      lineTo (x+22.348 ,y+21.509);
      lineTo (x+22.470 ,y+25.279);
      lineTo (x+22.348 ,y+29.049);
      lineTo (x+21.957 ,y+32.801);
      lineTo (x+21.299 ,y+36.515);
      lineTo (x+20.846 ,y+38.525);
      lineTo (x+20.241 ,y+40.496);
      lineTo (x+19.487 ,y+42.414);
      lineTo (x+18.589 ,y+44.270);
      lineTo (x+17.552 ,y+46.051);
      lineTo (x+16.382 ,y+47.749);
     moveTo (x + 33.697 + space, y +0); 
     newx=currentX-ORIGIN_X;
     newy=currentY-initialHeadTo;
     Serial.println ("Finished");
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Finished");
}
///////////////////////////////////////////////////////
void step (int stp, int steps) {
  if (steps > 0) {
    digitalWrite (stp+1, LOW);
    }
   else {
    digitalWrite (stp+1, HIGH);
   }
   for (int i =0; i < abs (steps); i++) {
    digitalWrite (stp, LOW);
    delayMicroseconds (1000); //speed control
    digitalWrite (stp, HIGH);
    delayMicroseconds (1000);
   }
  }
 void turnLeft () {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Turn Left");
  digitalWrite (dirPin2, HIGH);
  for (int x = 0 ; x < 400 ; x++) {
  digitalWrite (stepPin2, HIGH);
  delayMicroseconds (400);
  digitalWrite (stepPin2, LOW);
  delayMicroseconds (400);
  }
  leftrightBuff -= 400;
  Serial.println(leftrightBuff);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Finished");
}
void turnRight () {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Turn Right");
  digitalWrite (dirPin2, LOW);
  for (int x = 0 ; x < 400 ; x++) {
  digitalWrite (stepPin2, HIGH);
  delayMicroseconds (400);
  digitalWrite (stepPin2, LOW);
  delayMicroseconds (400);
  }
  leftrightBuff += 400;
  Serial.println(leftrightBuff);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Finished");
}
void penBackw () {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Pen Backward");
  digitalWrite (dirPin1, HIGH);
  for (int x = 0 ; x < 800 ; x++) {
  digitalWrite (stepPin1, HIGH);
  delayMicroseconds (100);
  digitalWrite (stepPin1, LOW);
  delayMicroseconds (100);
  }
  backforBuff -= 800;
  Serial.println(backforBuff);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Finished");
}
void penForw () {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Pen Forward");
  digitalWrite (dirPin1, LOW);
  for (int x = 0 ; x < 800 ; x++) {
  digitalWrite (stepPin1, HIGH);
  delayMicroseconds (100);
  digitalWrite (stepPin1, LOW);
  delayMicroseconds (100);
  }
  backforBuff += 800;
  Serial.println(backforBuff);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Finished");
}
void Z_up () {
  digitalWrite (Z_dirPin, LOW);
  for (int x = 0 ; x < 32000 ; x++) {
  digitalWrite (Z_stepPin, HIGH);
  delayMicroseconds (100);
  digitalWrite (Z_stepPin, LOW);
  delayMicroseconds (100);
  }
}
void Z_down () {
  digitalWrite (Z_dirPin, HIGH);
  for (int x = 0 ; x < 32000 ; x++) {
  digitalWrite (Z_stepPin, HIGH);
  delayMicroseconds (100);
  digitalWrite (Z_stepPin, LOW);
  delayMicroseconds (100);
  }
}
void moveTo (float x, float y) {
  Z_up ();
  headTo (x,y+initialHeadTo);
  Z_down ();
}
 void lineTo (float x, float y) {
  headTo (x,y+initialHeadTo);
 }

 void headTo (float x, float y) {
  float dX = x - currentX;
  float dY = y - currentY;

  int nofSteps =abs (dX);
  if (abs (dY) > nofSteps) {
    nofSteps = abs (dY);
  }
  nofSteps *= 8;

  float stepX = (float) dX / (float) nofSteps;
  float stepY = (float) dY / (float) nofSteps;

  for (int i=0; i <nofSteps; i++) {
    float currentAngle = atan((float) currentX / (float) currentY);
    float currentRadius = sqrt(currentX * currentX + currentY * currentY);
    // Check where we will end up after the step
  float newX = (float) currentX + stepX;
  float newY = (float) currentY + stepY;

 // Check what the new angle and radius will be
  float newAngle = atan(newX / newY);
  float newRadius = sqrt(newX * newX + newY * newY);
  
  // Check the diffs
  float baseAngleDiff = newAngle - currentAngle;
  float radiusDiff = newRadius - currentRadius;

 //angle for the motor to turn
  float angleDiff = baseAngleDiff * 52/10;

 // Check what the diffs meas in actual motor steps
  float angleStepsExact = (float) STEPS_PER_RADIAN * angleDiff;
  float radiusStepsExact = (float) STEPS_PER_MM * radiusDiff;

 // Add the error from last loop run
  angleStepsExact += angleError;
  radiusStepsExact += radiusError;

 // Do the rounding
  int angleSteps = round(angleStepsExact);
  int radiusSteps = round(radiusStepsExact);

 // Move the motors
  step(stepPin1, -radiusSteps);
  step(stepPin2, angleSteps);

 // Delay to control speed
  delayMicroseconds(100);

 // Calculate and keep errors from rounding
  angleError = angleStepsExact - (float) angleSteps;
  radiusError = radiusStepsExact - (float) radiusSteps;

 // Update position
  currentX = newX;
  currentY = newY;
   }
 }
 
void loop()
{
  
   

    digitalWrite (8,HIGH); //MS1,2,3 for radius motor
  digitalWrite (9,HIGH); 
  digitalWrite (10, HIGH); 
  digitalWrite (22, HIGH); //MS1,2,3 for angle motor
  digitalWrite (24, HIGH);
  digitalWrite (26, HIGH);
  digitalWrite (28, HIGH);//MS 1,2,3 for Z motor
  digitalWrite (30, HIGH);
  digitalWrite (32, HIGH);
  digitalWrite (34, HIGH);//2 buttons
  digitalWrite (36, HIGH);

   Max.Task();
    Usb.Task();
    if( Usb.getUsbTaskState() == USB_STATE_CONFIGURING ) {  //wait for addressing state
        kbd_init();
        Usb.setUsbTaskState( USB_STATE_RUNNING );
    }
    if( Usb.getUsbTaskState() == USB_STATE_RUNNING ) {  //poll the keyboard
        kbd_poll();
    }
    
  switch (inputchar) {
    case 'a':
      drawA (newx,newy);
      break;
    case 'b':
      drawB (newx,newy);
      break;
    case 'c':
      drawC (newx,newy);
      break;
    case 'd':
      drawD (newx,newy);
      break;
    case 'e':
      drawE (newx,newy);
      break;
    case 'f':
      drawF (newx,newy);
      break;
    case 'g':
      drawG (newx,newy);
      break;
    case 'h':
      drawH (newx,newy);
      break;
    case 'i':
      drawI (newx,newy);
      break;
    case 'j':
      drawJ (newx,newy);
      break;
    case 'k':
      drawK (newx,newy);
      break;
    case 'l':
      drawL (newx,newy);
      break;
   case 'm':
      drawM (newx,newy) ;
      break;
    case 'n':
      drawN (newx,newy);
      break;
    case 'o':
      drawO (newx,newy);
      break;
    case 'p':
      drawP (newx,newy);
      break;
    case 'q':
      drawQ (newx,newy);
      break;
    case 'r':
      drawR (newx, newy);
      break;
    case 's':
      drawS (newx,newy);
      break;
    case 't':
      drawT (newx,newy);
      break;
    case 'u':
      drawU (newx, newy);
      break;
    case 'v':
      drawV (newx,newy);
      break;
    case 'w':
      drawW (newx,newy);
      break;
    case 'x':
      drawX (newx,newy);
      break;
    case 'y':
      drawY (newx,newy);
      break;
    case 'z':
      drawZ (newx,newy);
      break;    
    case '_':
      drawSpace (newx, newy);
      break;
    case '0':
      draw0 (newx,newy);
      break;
    case '1':
      draw1 (newx,newy);
      break;
    case '2':
      draw2 (newx,newy);
      break;
    case '3':
      draw3 (newx,newy);
      break;
    case '4':
      draw4 (newx,newy);
      break;
    case '5':
      draw5 (newx,newy);
      break;
    case '6':
      draw6 (newx,newy);
      break;
    case '7':
      draw7 (newx,newy);
      break;
    case '8':
      draw8 (newx,newy);
      break;
    case '9':
      draw9 (newx,newy);
      break;
    case ':':
      Calibrate ();
      break;
    case '*':
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Z Up");
      Z_up();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Finished");
      break;
    }
    case '@':
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Z Down");
      Z_down();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Finished");
      break;
    }
    case '$':
    {
      penBackw();
      break;
    }
    case '^':
    {
      penForw();
      break;
    }
    case '&':
    {
      turnLeft();
      break;
    }
    case '(':
    {
      turnRight();
      break;
    }
    case '%':
    {
      Restore();
      break;
    }
    }
    inputchar = ' ';
}
/* Initialize keyboard */
void kbd_init( void )
{
  byte rcode = 0;  //return code
/**/
    /* Initialize data structures */
    ep_record[ 0 ] = *( Usb.getDevTableEntry( 0,0 ));  //copy endpoint 0 parameters
    ep_record[ 1 ].MaxPktSize = EP_MAXPKTSIZE;
    ep_record[ 1 ].Interval  = EP_POLL;
    ep_record[ 1 ].sndToggle = bmSNDTOG0;
    ep_record[ 1 ].rcvToggle = bmRCVTOG0;
    Usb.setDevTableEntry( 1, ep_record );              //plug kbd.endpoint parameters to devtable
    /* Configure device */
    rcode = Usb.setConf( KBD_ADDR, 0, 1 );
    if( rcode ) {
        Serial.print("Error attempting to configure keyboard. Return code :");
        Serial.println( rcode, HEX );
        while(1);  //stop
    }
    /* Set boot protocol */
    rcode = Usb.setProto( KBD_ADDR, 0, 0, 0 );
    if( rcode ) {
        Serial.print("Error attempting to configure boot protocol. Return code :");
        Serial.println( rcode, HEX );
        while( 1 );  //stop
    }
}
/* Poll keyboard and print result */
void kbd_poll( void )
{
 static char old_buf[ 8 ] = { 0 };
 static char leds = 0;
 byte rcode = 0;     //return code
    rcode = Usb.inTransfer( KBD_ADDR, KBD_EP, 8, buf );
    if( rcode != 0 ) {
        return;
    }//if ( rcode..
    for( i = 2; i < 8; i++ ) {
     if( buf[ i ] == 0 ) {  //end of non-empty space
        break;
     }
      if( buf[i] != old_buf[i] ) {   //if new key
        switch( buf[ i ] ) {
          case CAPSLOCK:
            capsLock =! capsLock;
            leds = ( capsLock ) ? leds |= REP_CAPSLOCK : leds &= REP_CAPSLOCK;       // set or clear bit 1 of LED report byte
            break;
          case NUMLOCK:
            numLock =! numLock;
            leds = ( numLock ) ? leds |= REP_NUMLOCK : leds &= REP_NUMLOCK;           // set or clear bit 0 of LED report byte
            break;
          case SCROLLLOCK:
            scrollLock =! scrollLock;
            leds = ( scrollLock ) ? leds |= REP_SCROLLLOCK : leds &= REP_SCROLLLOCK;   // set or clear bit 2 of LED report byte
            break;
          case DELETE:
//            lcd.clear();
//            lcd.home();
            line = false;
            break;
          case RETURN:
            line =! line;
//            lcd.setCursor( 0, line );
            break;
          default:
//            lcd.print("A");                          //output
//            lcd.print( HIDtoA( buf[ i ], buf[ 0 ] ));
            Serial.println(HIDtoA( buf[ i ], buf[ 0 ] ));
            switch(HIDtoA( buf[ i ], buf[ 0 ] )){
              case 53:
              {
                inputchar = '%';
                Serial.println("Restore");
                break;
              }
              case 55:
              {
                inputchar = '&';
                Serial.println("Turn Left");
                break;
              }
              case 57:
              {
                inputchar = '(';
                Serial.println("Turn Right");
                break;
              }
              case 56:
               {
                inputchar = '*';
                Serial.println("Z up");
                break;  
               }
              case 50: 
               {
                inputchar = '@';
                Serial.println("Z down");
                break;
               }
               case 52:
               {
                inputchar = '$';
                Serial.println("Pen Backward");
                break;
               }
               case 54:
               {
                inputchar = '^';
                Serial.println("Pen Forward");
                break;
               }
               case 59:
               {
                inputchar = ':';
                Serial.println(inputchar);
                break;
               }
              case 97:
                {
                  inputchar = 'a';
                  Serial.println(inputchar);
                  break;             
                }
              case 98:
                {
                  inputchar = 'b';
                  Serial.println(inputchar);
                  break;             
                }
              case 99:
                {
                  inputchar = 'c';
                  Serial.println(inputchar);
                  break;             
                }
              case 100:
                {
                  inputchar = 'd';
                  Serial.println(inputchar);
                  break;             
                }
              case 101:
                {
                  inputchar = 'e';
                  Serial.println(inputchar);
                  break;             
                }
              case 102:
                {
                  inputchar = 'f';
                  Serial.println(inputchar);
                  break;             
                }
              case 103:
                {
                  inputchar = 'g';
                  Serial.println(inputchar);
                  break;             
                }
              case 104:
                {
                  inputchar = 'h';
                  Serial.println(inputchar);
                  break;             
                }
              case 105:
               {
                inputchar = 'i';
                Serial.println(inputchar);
                break;
               }
              case 106:
               {
                inputchar = 'j';
                Serial.println(inputchar);
                break;
               }
              case 107:
               {
                inputchar = 'k';
                Serial.println(inputchar);
                break;
               } 
              case 108:
               {
                inputchar = 'l';
                Serial.println(inputchar);
                break;
               } 
              case 109:
               {
                inputchar = 'm';
                Serial.println(inputchar);
                break;
               } 
              case 110:
               {
                inputchar = 'n';
                Serial.println(inputchar);
                break;
               }
              case 111:
               {
                inputchar = 'o';
                Serial.println(inputchar);
                break;
               }
              case 112:
               {
                inputchar = 'p';
                Serial.println(inputchar);
                break;
               } 
              case 113:
               {
                inputchar = 'q';
                Serial.println(inputchar);
                break;
               }
              case 114:
                {
                  inputchar = 'r';
                  Serial.println(inputchar);
                  break;             
                }
               case 115:
               {
                inputchar = 's';
                Serial.println(inputchar);
                break;
               }
               case 116:
               {
                inputchar = 't';
                Serial.println(inputchar);
                break;
               }
               case 117:
               {
                inputchar = 'u';
                Serial.println(inputchar);
                break;
               }
               case 118:
               {
                inputchar = 'v';
                Serial.println(inputchar);
                break;
               }
               case 119:
               {
                inputchar = 'w';
                Serial.println(inputchar);
                break;
               }
               case 120:
               {
                inputchar = 'x';
                Serial.println(inputchar);
                break;
               }
               case 121:
               {
                inputchar = 'y';
                Serial.println(inputchar);
                break;
               }
               case 122:
               {
                inputchar = 'z';
                Serial.println(inputchar);
                break;
               }
               case 95:
               {
                inputchar = '_';
                Serial.println(inputchar);
                break;
               }
               case 64:
               {
                inputchar = '2';
                Serial.println(inputchar);
                break;
               }
            }
            break;
        }
        rcode = Usb.setReport( KBD_ADDR, 0, 1, KBD_IF, 0x02, 0, &leds );
        if( rcode ) {
          Serial.print("Set report error: ");
          Serial.println( rcode, HEX );
        }
     }
    }
    for( i = 2; i < 8; i++ ) {                    //copy new buffer to old
      old_buf[ i ] = buf[ i ];
    }

}
byte HIDtoA( byte HIDbyte, byte mod )
{
  /* upper row of the keyboard, numbers and special symbols */
  if( HIDbyte >= 0x1e && HIDbyte <= 0x27 ) {
    if(( mod & SHIFT ) || numLock ) {    //shift key pressed
      switch( HIDbyte ) {
        case BANG:  return( 0x21 );
        case AT:    return( 0x40 );
        case POUND: return( 0x23 );
        case DOLLAR: return( 0x24 );
        case PERCENT: return( 0x25 );
        case CAP: return( 0x5e );
        case AND: return( 0x26 );
        case STAR: return( 0x2a );
        case OPENBKT: return( 0x28 );
        case CLOSEBKT: return( 0x29 );
      }//switch( HIDbyte...
    }
    else {                  //numbers
      if( HIDbyte == 0x27 ) {  //zero
        return( 0x30 );
      }
      else {
        return( HIDbyte + 0x13 );
      }
    }
  }
  /**/
  /* number pad. Arrows are not supported */
  if(( HIDbyte >= 0x59 && HIDbyte <= 0x61 ) && ( numLock == true )) {  // numbers 1-9
    return( HIDbyte - 0x28 );
  }
  if(( HIDbyte == 0x62 ) && ( numLock == true )) {                      //zero
    return( 0x30 );
  }
  /* Letters a-z */
  if( HIDbyte >= 0x04 && HIDbyte <= 0x1d ) {
    if((( capsLock == true ) && ( mod & SHIFT ) == 0 ) || (( capsLock == false ) && ( mod & SHIFT ))) {  //upper case
      return( HIDbyte + 0x3d );
    }
    else {  //lower case
      return( HIDbyte + 0x5d );
    }
  }//if( HIDbyte >= 0x04 && HIDbyte <= 0x1d...
  /* Other special symbols */
  if( HIDbyte >= 0x2c && HIDbyte <= 0x38 ) {
    switch( HIDbyte ) {
      case SPACE: return( 0x20 );
      case HYPHEN:
        if(( mod & SHIFT ) == false ) {
         return( 0x2d );
        }
        else {
          return( 0x5f );
        }
      case EQUAL:
       if(( mod & SHIFT ) == false ) {
        return( 0x3d );
       }
       else {
        return( 0x2b );
       }
       case SQBKTOPEN:
         if(( mod & SHIFT ) == false ) {
          return( 0x5b );
         }
         else {
          return( 0x7b );
         }
       case SQBKTCLOSE:
         if(( mod & SHIFT ) == false ) {
          return( 0x5d );
         }
         else {
          return( 0x7d );
         }
       case BACKSLASH:
         if(( mod & SHIFT ) == false ) {
           return( 0x5c );
         }
         else {
           return( 0x7c );
         }
       case SEMICOLON:
         if(( mod & SHIFT ) == false ) {
           return( 0x3b );
         }
         else {
           return( 0x3a );
         }
      case INVCOMMA:
        if(( mod & SHIFT ) == false ) {
          return( 0x27 );
        }
        else {
          return( 0x22 );
        }
      case TILDE:
        if(( mod & SHIFT ) == false ) {
          return( 0x60 );
        }
        else {
          return( 0x7e );
        }
      case COMMA:
        if(( mod & SHIFT ) == false ) {
          return( 0x2c );
        }
        else {
          return( 0x3c );
        }
      case PERIOD:
        if(( mod & SHIFT ) == false ) {
          return( 0x2e );
        }
        else {
          return( 0x3e );
        }
      case FRONTSLASH:
        if(( mod & SHIFT ) == false ) {
          return( 0x2f );
        }
        else {
          return( 0x3f );
        }
      default:
        break;
    }
  }
  return( 0 );
}


