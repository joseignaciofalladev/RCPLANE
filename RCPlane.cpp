#include <Servo.h>

// ---------- SERVOS ----------
Servo sAil, sEle, sRud, sThr, sCamPan, sCamTilt;

// ---------- INPUT PINS ----------
const byte IN_AIL  = 2;
const byte IN_ELE  = 3;
const byte IN_RUD  = 4;
const byte IN_THR  = 5;
const byte IN_PAN  = 6;
const byte IN_TILT = 7;

// ---------- OUTPUT PINS ----------
const byte OUT_AIL  = 8;
const byte OUT_ELE  = 9;
const byte OUT_RUD  = 10;
const byte OUT_THR  = 11;
const byte OUT_PAN  = 12;
const byte OUT_TILT = 13;

// ---------- SIGNAL LIMITS ----------
const int PWM_MIN = 1000;
const int PWM_MAX = 2000;
const int PWM_NEUTRAL = 1500;
const int PWM_THR_CUT = 1000;

// ---------- FAILSAFE ----------
const unsigned long FAILSAFE_TIME = 500; // ms sin señal
unsigned long lastSignalTime = 0;

void setup() {

  sAil.attach(OUT_AIL);
  sEle.attach(OUT_ELE);
  sRud.attach(OUT_RUD);
  sThr.attach(OUT_THR);
  sCamPan.attach(OUT_PAN);
  sCamTilt.attach(OUT_TILT);

  pinMode(IN_AIL, INPUT);
  pinMode(IN_ELE, INPUT);
  pinMode(IN_RUD, INPUT);
  pinMode(IN_THR, INPUT);
  pinMode(IN_PAN, INPUT);
  pinMode(IN_TILT, INPUT);

  // Arranque seguro
  setFailsafe();
  delay(3000); // tiempo típico de armado del ESC
}

void loop() {

  int ail  = readPWM(IN_AIL);
  int ele  = readPWM(IN_ELE);
  int rud  = readPWM(IN_RUD);
  int thr  = readPWM(IN_THR);
  int pan  = readPWM(IN_PAN);
  int tilt = readPWM(IN_TILT);

  bool signalOK =
    validPWM(ail) &&
    validPWM(ele) &&
    validPWM(rud) &&
    validPWM(thr);

  if (signalOK) {
    lastSignalTime = millis();
    writeControls(ail, ele, rud, thr, pan, tilt);
  }

  // FAILSAFE
  if (millis() - lastSignalTime > FAILSAFE_TIME) {
    setFailsafe();
  }
}

// ---------- FUNCTIONS ----------

int readPWM(byte pin) {
  return pulseIn(pin, HIGH, 25000);
}

bool validPWM(int pwm) {
  return pwm >= PWM_MIN && pwm <= PWM_MAX;
}

void writeControls(int ail, int ele, int rud, int thr, int pan, int tilt) {

  sAil.writeMicroseconds(constrain(ail, PWM_MIN, PWM_MAX));
  sEle.writeMicroseconds(constrain(ele, PWM_MIN, PWM_MAX));
  sRud.writeMicroseconds(constrain(rud, PWM_MIN, PWM_MAX));
  sThr.writeMicroseconds(constrain(thr, PWM_MIN, PWM_MAX));

  // Cámara: si no hay head tracker, se centra
  sCamPan.writeMicroseconds(validPWM(pan) ? pan : PWM_NEUTRAL);
  sCamTilt.writeMicroseconds(validPWM(tilt) ? tilt : PWM_NEUTRAL);
}

void setFailsafe() {

  sThr.writeMicroseconds(PWM_THR_CUT);
  sAil.writeMicroseconds(PWM_NEUTRAL);
  sEle.writeMicroseconds(PWM_NEUTRAL);
  sRud.writeMicroseconds(PWM_NEUTRAL);
  sCamPan.writeMicroseconds(PWM_NEUTRAL);
  sCamTilt.writeMicroseconds(PWM_NEUTRAL);
}