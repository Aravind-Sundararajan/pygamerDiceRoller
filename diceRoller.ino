#include "audio.h"
#include <Adafruit_Arcada.h>
#include <Adafruit_SPIFlash.h>


Adafruit_Arcada arcada;
extern Adafruit_SPIFlash Arcada_QSPI_Flash;

int nSides = 6, nDice = 1, modifier = 0, lastTotal = 0, thisRoll = 0, rollTotal = 0, last10Rolls[10] = {0,0,0,0,0,0,0,0,0,0};
String lastRoll = "";
// Check the timer callback, this function is called every millisecond!
//volatile uint16_t milliseconds = 0;
//void timercallback() {
//  analogWrite(13, milliseconds);  // pulse the LED
//  if (milliseconds == 0) {
//    milliseconds = 255;
//  } else {
//    milliseconds--;
//  }
//}

void setup() {
  arcada.arcadaBegin();
  arcada.displayBegin();
  for (int i=0; i<25; i++) {
    arcada.setBacklight(i);
    delay(1);
  }
  arcada.accel.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!
  arcada.accel.setClick(2, 10);
  arcada.display->fillScreen(ARCADA_BLACK);
  //arcada.timerCallback(1000, timercallback);
}

void loop() {
  bool playsound = false;
  delay(100);  // add some delay so our screen doesnt flicker
  uint8_t pressed_buttons = arcada.readButtons();  
  uint8_t click = arcada.accel.getClick();
  arcada.display->setCursor(0, 0);
  arcada.display->setTextWrap(true);
  arcada.display->fillScreen(ARCADA_BLACK);
  float vbat = arcada.readBatterySensor();
  arcada.display->setTextColor(ARCADA_GREEN);
  arcada.display->print("Batt: "); arcada.display->print(vbat); arcada.display->println("V");
  arcada.display->setTextColor(ARCADA_WHITE);
  arcada.display->println("# of dice(whiteBtns):"+String(nDice)); 
  arcada.display->println("# of sides(blackBtns):" + String(nSides));
  arcada.display->println("modifier(joyX):" + String(modifier));
  arcada.display->println("Shake=Roll! Last 10 Rolls:");
  for (int n = 0; n < 10; n++) {
    if (last10Rolls[n]!=0) {
      arcada.display->setTextColor(ARCADA_BLUE);
      arcada.display->print(String(last10Rolls[n]) + " ");
      arcada.display->setTextColor(ARCADA_WHITE);
    } else {arcada.display->print("N/A ");}
  }
  arcada.display->println();
  arcada.display->setTextColor(ARCADA_RED);
  arcada.display->println("this Roll: ");
  arcada.display->setTextColor(ARCADA_WHITE);
  arcada.display->println(rollTotal);
  arcada.display->println(lastRoll);
  
  if (pressed_buttons & ARCADA_BUTTONMASK_SELECT) {nSides-= 1;}
  if (pressed_buttons & ARCADA_BUTTONMASK_START) {nSides+= 1;}
  if (pressed_buttons & ARCADA_BUTTONMASK_A) {nDice+=1;}
  if (pressed_buttons & ARCADA_BUTTONMASK_B) {nDice-=1;}
  int joy = arcada.readJoystickX();
  if (abs(joy)>50){
	modifier+=sign(joy);
  }
  if (click & 0x10) {
    arcada.enableSpeaker(true);
    playTune(audio, sizeof(audio));
    arcada.enableSpeaker(false);
    lastRoll = "";
    rollTotal = 0;
    for (int i = 0; i < nDice; i++) {
      thisRoll = random(1,nSides);
      lastRoll =lastRoll + "(1d" + String(nSides) + ")" + String(thisRoll) + "+"; 
      rollTotal +=thisRoll;
    }
  rollTotal += modifier;
  lastRoll= lastRoll + String(modifier);
  for (int z = 0; z < 9; z++) {
	last10Rolls[z] = last10Rolls[z+1];
  }
	last10Rolls[9] = rollTotal;
  }
}

int sign(int x){
return (x>0) - (x<0);
}

void playTune(const uint8_t *audio, uint32_t audio_length) {
  uint32_t t;
  uint32_t prior, usec = 1000000L / SAMPLE_RATE;
  analogWriteResolution(8);
  for (uint32_t i=0; i<audio_length; i++) {
    while((t = micros()) - prior < usec);
    analogWrite(A0, (uint16_t)audio[i] / 8);
    analogWrite(A1, (uint16_t)audio[i] / 8);
    prior = t;
  }
}
/*****************************************************************/
