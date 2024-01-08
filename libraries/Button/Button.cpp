#include "Button.h"

Button::Button(int pin)
{
  pin = pin;
  clickHandler = nullptr;
  longPressHandler = nullptr;
  buttonState = false;
  lastButtonState = false;
  lastDebounceTime = 0;
  clickTime = 0;
  clickFlag = false;
  longPressFlag = false;
}

void Button::init(void (*click)(), void (*longPress)())
{
  clickHandler = click;
  longPressHandler = longPress;

  pinMode(pin, INPUT_PULLUP);
}

void Button::read()
{
  bool reading = digitalRead(pin);

  if (reading != lastButtonState)
  {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > 50)
  {
    if (reading != buttonState)
    {
      buttonState = reading;

      if (buttonState == LOW)
      {
        clickTime = millis();
        clickFlag = true;
      }
      else
      {
        if (clickFlag)
        {
          if ((millis() - clickTime) > 3000)
          {
            longPressHandler();
          }
          else
          {
            clickHandler();
          }

          clickFlag = false;
        }
      }
    }
  }

  lastButtonState = reading;
}
