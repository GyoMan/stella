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

#ifndef SWITCHES_HXX
#define SWITCHES_HXX

class Event;
class Properties;
class Settings;

#include "Serializable.hxx"
#include "bspf.hxx"

/**
  This class represents the console switches of the game console.

  @author  Bradford W. Mott
*/
class Switches : public Serializable
{
  /**
    Riot debug class needs special access to the underlying controller state
  */
  friend class RiotDebug;

  public:
    /**
      Create a new set of switches using the specified events and
      properties.

      @param event The event object to use for events
      @param props The ROM properties to use for the currently enabled ROM
      @param settings The settings used by the system
    */
    Switches(Event& event, const Properties& props, const Settings& settings);
    virtual ~Switches() = default;

  public:
    /**
      Get the value of the console switches.

      @return The 8 bits which represent the state of the console switches
    */
    uInt8 read() const { return mySwitches; }

    /**
      Update the switches variable.
    */
    void update();

    /**
      Save the current state of the switches to the given Serializer.

      @param out  The Serializer object to use
      @return  False on any errors, else true
    */
    bool save(Serializer& out) const override;

    /**
      Load the current state of the switches from the given Serializer.

      @param in  The Serializer object to use
      @return  False on any errors, else true
    */
    bool load(Serializer& in) override;

    /**
      Query the 'Console_TelevisionType' switches bit.

      @return  True if 'Color', false if 'BlackWhite'
    */
    bool tvColor() const { return mySwitches & 0x08; }

    /**
      Sets 'Console_TelevisionType' switches bit.
    */
    void setTvColor(bool setColor);

    /**
      Query the 'Console_LeftDifficulty' switches bit.

      @return  True if 'A', false if 'B'
    */
    bool leftDifficultyA() const { return mySwitches & 0x40; }

    /**
      Sets 'Console_LeftDifficulty' switches bit.
    */
    void setLeftDifficultyA(bool setA);

    /**
      Query the 'Console_RightDifficulty' switches bit.

      @return  True if 'A', false if 'B'
    */
    bool rightDifficultyA() const { return mySwitches & 0x80; }

    /**
      Sets 'Console_LeftDifficulty' switches bit.
    */
    void setRightDifficultyA(bool setA);


    /**
      Toggle between 2600 and 7800 mode depending on settings.

      @return  True if 7800 mode enabled, else false
    */
    bool check7800Mode(const Settings& settings);

  private:
    // Reference to the event object to use
    Event& myEvent;

    // State of the console switches
    uInt8 mySwitches;

    // Are we in 7800 or 2600 mode?
    bool myIs7800;

  private:
    // Following constructors and assignment operators not supported
    Switches() = delete;
    Switches(const Switches&) = delete;
    Switches(Switches&&) = delete;
    Switches& operator=(const Switches&) = delete;
    Switches& operator=(Switches&&) = delete;
};

#endif
