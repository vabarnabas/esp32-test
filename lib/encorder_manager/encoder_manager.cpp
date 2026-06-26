#include "encoder_manager.h"
#include <ESP32RotaryEncoder.h>

// A, B, button, VCC(-1 = use real 3.3V pin)
static RotaryEncoder enc(32, 33, 25, -1);

static volatile long accumulated = 0;     // detents turned but not yet consumed
static volatile bool pressedFlag = false;

static void onTurn(long value) {
  static bool inited = false;
  static long last = 0;
  static long remainder = 0;
  if (!inited) { last = value; inited = true; return; }

  remainder += (value - last);
  last = value;

  // emit one menu step per 2 units of raw movement
  while (remainder >= 2)  { accumulated++; remainder -= 2; }
  while (remainder <= -2) { accumulated--; remainder += 2; }
}

static void onPress(unsigned long /*durationMs*/) {
  pressedFlag = true;
}

void encoderBegin() {
  enc.setEncoderType(EncoderType::FLOATING);     // bare encoder -> internal pull-ups
  enc.setBoundaries(-100000, 100000, false);     // wide, no wrap; menu wraps in software
  enc.onTurned(&onTurn);
  enc.onPressed(&onPress);
  enc.begin();
}

void encoderLoop() {
  // interrupt-driven; nothing needed here
}

int encoderTakeDelta() {
  noInterrupts();
  long d = accumulated;
  accumulated = 0;
  interrupts();
  return (int)d;
}

bool encoderPressed() {
  if (pressedFlag) { pressedFlag = false; return true; }  // read-and-clear
  return false;
}