//============================================================================
//
//   SSSS    tt          lll  lll
//  SS  SS   tt           ll   ll
//  SS     tttttt  eeee   ll   ll   aaaa
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2019 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//============================================================================

#include "Event.hxx"
#include "Joystick.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Joystick::Joystick(Jack jack, const Event& event, const System& system)
  : Controller(jack, event, system, Controller::Type::Joystick),
    myControlID(-1)
{
  if(myJack == Jack::Left)
  {
    myUpEvent    = Event::JoystickZeroUp;
    myDownEvent  = Event::JoystickZeroDown;
    myLeftEvent  = Event::JoystickZeroLeft;
    myRightEvent = Event::JoystickZeroRight;
    myFireEvent  = Event::JoystickZeroFire;
    myXAxisValue = Event::SALeftAxis0Value;
    myYAxisValue = Event::SALeftAxis1Value;
  }
  else
  {
    myUpEvent    = Event::JoystickOneUp;
    myDownEvent  = Event::JoystickOneDown;
    myLeftEvent  = Event::JoystickOneLeft;
    myRightEvent = Event::JoystickOneRight;
    myFireEvent  = Event::JoystickOneFire;
    myXAxisValue = Event::SARightAxis0Value;
    myYAxisValue = Event::SARightAxis1Value;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Joystick::update()
{
  // Digital events (from keyboard or joystick hats & buttons)
  setPin(DigitalPin::One, myEvent.get(myUpEvent) == 0);
  setPin(DigitalPin::Two, myEvent.get(myDownEvent) == 0);
  setPin(DigitalPin::Three, myEvent.get(myLeftEvent) == 0);
  setPin(DigitalPin::Four, myEvent.get(myRightEvent) == 0);
  setPin(DigitalPin::Six, myEvent.get(myFireEvent) == 0);

  // Axis events (usually generated by the Stelladaptor)
  int xaxis = myEvent.get(myXAxisValue);
  int yaxis = myEvent.get(myYAxisValue);
  if(xaxis > 16384-4096)
  {
    setPin(DigitalPin::Four, false);
    // Stelladaptor sends "half moved right" for L+R pushed together
    if(xaxis < 16384+4096)
      setPin(DigitalPin::Three, false);
  }
  else if(xaxis < -16384)
    setPin(DigitalPin::Three, false);
  if(yaxis > 16384-4096)
  {
    setPin(DigitalPin::Two, false);
    // Stelladaptor sends "half moved down" for U+D pushed together
    if(yaxis < 16384+4096)
      setPin(DigitalPin::One, false);
  }
  else if(yaxis < -16384)
    setPin(DigitalPin::One, false);

  // Mouse motion and button events
  if(myControlID > -1)
  {
    // The following code was taken from z26
    #define MJ_Threshold 2
    int mousex = myEvent.get(Event::MouseAxisXValue),
        mousey = myEvent.get(Event::MouseAxisYValue);
    if(mousex || mousey)
    {
      if((!(abs(mousey) > abs(mousex) << 1)) && (abs(mousex) >= MJ_Threshold))
      {
        if(mousex < 0)
          setPin(DigitalPin::Three, false);
        else if(mousex > 0)
          setPin(DigitalPin::Four, false);
      }

      if((!(abs(mousex) > abs(mousey) << 1)) && (abs(mousey) >= MJ_Threshold))
      {
        if(mousey < 0)
          setPin(DigitalPin::One, false);
        else if(mousey > 0)
          setPin(DigitalPin::Two, false);
      }
    }
    // Get mouse button state
    if(myEvent.get(Event::MouseButtonLeftValue) ||
       myEvent.get(Event::MouseButtonRightValue))
      setPin(DigitalPin::Six, false);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Joystick::setMouseControl(
    Controller::Type xtype, int xid, Controller::Type ytype, int yid)
{
  // Currently, the joystick takes full control of the mouse, using both
  // axes for its two degrees of movement, and both mouse buttons for the
  // single joystick button
  if(xtype == Controller::Type::Joystick && ytype == Controller::Type::Joystick && xid == yid)
  {
    myControlID = ((myJack == Jack::Left && xid == 0) ||
                   (myJack == Jack::Right && xid == 1)
                  ) ? xid : -1;
  }
  else
    myControlID = -1;

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Joystick::setDeadZone(int deadzone)
{
  deadzone = BSPF::clamp(deadzone, 0, 29);

  _DEAD_ZONE = 3200 + deadzone * 1000;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Joystick::_DEAD_ZONE = 3200;
