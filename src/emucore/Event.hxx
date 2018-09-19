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
// Copyright (c) 1995-2018 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//============================================================================

#ifndef EVENT_HXX
#define EVENT_HXX

#include <mutex>

#include "bspf.hxx"
#include "StellaKeys.hxx"

/**
  @author  Bradford W. Mott, Stephen Anthony
*/
class Event
{
  public:
    /**
      Enumeration of all possible events in Stella, including both
      console and controller event types as well as events that aren't
      technically part of the emulation core
    */
    enum Type
    {
      NoType,
      ConsoleOn, ConsoleOff, ConsoleColor, ConsoleBlackWhite,
      ConsoleLeftDiffA, ConsoleLeftDiffB,
      ConsoleRightDiffA, ConsoleRightDiffB,
      ConsoleSelect, ConsoleReset,
      ConsoleLeftDiffToggle, ConsoleRightDiffToggle, ConsoleColorToggle, Console7800Pause,

      JoystickZeroUp, JoystickZeroDown, JoystickZeroLeft, JoystickZeroRight,
        JoystickZeroFire, JoystickZeroFire5, JoystickZeroFire9,
      JoystickOneUp, JoystickOneDown, JoystickOneLeft, JoystickOneRight,
        JoystickOneFire, JoystickOneFire5, JoystickOneFire9,

      PaddleZeroDecrease, PaddleZeroIncrease, PaddleZeroAnalog, PaddleZeroFire,
      PaddleOneDecrease, PaddleOneIncrease, PaddleOneAnalog, PaddleOneFire,
      PaddleTwoDecrease, PaddleTwoIncrease, PaddleTwoAnalog, PaddleTwoFire,
      PaddleThreeDecrease, PaddleThreeIncrease, PaddleThreeAnalog, PaddleThreeFire,

      KeyboardZero1, KeyboardZero2, KeyboardZero3,
      KeyboardZero4, KeyboardZero5, KeyboardZero6,
      KeyboardZero7, KeyboardZero8, KeyboardZero9,
      KeyboardZeroStar, KeyboardZero0, KeyboardZeroPound,

      KeyboardOne1, KeyboardOne2, KeyboardOne3,
      KeyboardOne4, KeyboardOne5, KeyboardOne6,
      KeyboardOne7, KeyboardOne8, KeyboardOne9,
      KeyboardOneStar, KeyboardOne0, KeyboardOnePound,

      Combo1, Combo2, Combo3, Combo4, Combo5, Combo6, Combo7, Combo8,
      Combo9, Combo10, Combo11, Combo12, Combo13, Combo14, Combo15, Combo16,

      SALeftAxis0Value, SALeftAxis1Value,
      SARightAxis0Value, SARightAxis1Value,

      MouseAxisXValue, MouseAxisYValue,
      MouseButtonLeftValue, MouseButtonRightValue,

      ChangeState, LoadState, SaveState, TakeSnapshot, Quit,
      PauseMode, OptionsMenuMode, CmdMenuMode, TimeMachineMode, DebuggerMode, LauncherMode,
      Fry, VolumeDecrease, VolumeIncrease,

      UIUp, UIDown, UILeft, UIRight, UIHome, UIEnd, UIPgUp, UIPgDown,
      UISelect, UINavPrev, UINavNext, UIOK, UICancel, UIPrevDir,

      LastType
    };

    class KeyTable {
      public:

        KeyTable(const bool* keyTable, std::mutex& mutex)
          : myKeyTable(keyTable),
            myMutex(mutex),
            myIsEnabled(true)
        {}

        bool operator[](int type) {
          if (!myIsEnabled) return false;

          std::lock_guard<std::mutex> lock(myMutex);

          return myKeyTable[type];
        }

        void enable(bool isEnabled) {
          myIsEnabled = isEnabled;
        }

      private:

        const bool *myKeyTable;
        std::mutex& myMutex;

        bool myIsEnabled;

    };

  public:
    /**
      Create a new event object.
    */
    Event() { clear(); }

  public:
    /**
      Get the value associated with the event of the specified type.
    */
    Int32 get(Type type) {
      std::lock_guard<std::mutex> lock(myMutex);

      return myValues[type];
    }

    /**
      Set the value associated with the event of the specified type.
    */
    void set(Type type, Int32 value) {
      std::lock_guard<std::mutex> lock(myMutex);

      myValues[type] = value;
    }

    /**
      Clears the event array (resets to initial state).
    */
    void clear()
    {
      std::lock_guard<std::mutex> lock(myMutex);

      for(uInt32 i = 0; i < LastType; ++i)
        myValues[i] = Event::NoType;

      for(uInt32 i = 0; i < KBDK_LAST; ++i)
        myKeyTable[i] = false;
    }

    /**
      Get the keytable associated with this event.
    */
    KeyTable getKeys() { return KeyTable(myKeyTable, myMutex); }

    /**
      Set the value associated with the event of the specified type.
    */
    void setKey(StellaKey key, bool state) {
      std::lock_guard<std::mutex> lock(myMutex);

      myKeyTable[key] = state;
    }

    /**
      Tests if a given event represents continuous or analog values.
    */
    static bool isAnalog(Type type)
    {
      switch(type)
      {
        case Event::PaddleZeroAnalog:
        case Event::PaddleOneAnalog:
        case Event::PaddleTwoAnalog:
        case Event::PaddleThreeAnalog:
          return true;
        default:
          return false;
      }
    }

  private:
    // Array of values associated with each event type
    Int32 myValues[LastType];

    // Array of keyboard key states
    bool myKeyTable[KBDK_LAST];

    std::mutex myMutex;

  private:
    // Following constructors and assignment operators not supported
    Event(const Event&) = delete;
    Event(Event&&) = delete;
    Event& operator=(const Event&) = delete;
    Event& operator=(Event&&) = delete;
};

#endif
