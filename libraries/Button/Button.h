#ifndef Button_h
#define Button_h

#include <Arduino.h>

class Button
{
protected:
  int pin;
  unsigned long lastDebounceTime;
  unsigned long clickTime;
  bool buttonState;
  bool lastButtonState;
  bool clickFlag;
  bool longPressFlag;

  void (*clickHandler)();
  void (*longPressHandler)();

public:
  Button(int pin);

  void read();
  void init(void (*click)(), void (*longPress)());
};

#endif
