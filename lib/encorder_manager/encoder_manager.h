#pragma once
#include <Arduino.h>

void encoderBegin();
void encoderLoop();
int  encoderTakeDelta();   // net detents since last call (+CW / -CCW), then clears
bool encoderPressed();     // true once per press, then clears