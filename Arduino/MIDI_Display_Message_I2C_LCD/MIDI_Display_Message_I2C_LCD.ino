#include <Wire.h>
#include <I2CLiquidCrystal.h>
#include <MIDI.h>

#define TITLE_STR1  ("MIDI Message    ")
#define TITLE_STR2  ("20181204        ")

// MIDI
MIDI_CREATE_DEFAULT_INSTANCE();

// I2C LCD
I2CLiquidCrystal lcd(20, (bool)true);

// MIDI -------------------------------------------------------------------------

void printNoteOnOff(const char* type, byte inChannel, byte inNote, byte inVelocity) 
{
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print(type);
    lcd.print(' ');
    lcd.print(inChannel);
    lcd.print(' ');
    lcd.print(inNote);
    lcd.print(' ');
    lcd.print(inVelocity);  
}

void handleNoteOn(byte inChannel, byte inNote, byte inVelocity)
{
    printNoteOnOff("On", inChannel, inNote, inVelocity);
}

void handleNoteOff(byte inChannel, byte inNote, byte inVelocity)
{
    printNoteOnOff("Off", inChannel, inNote, inVelocity);
}

// -----------------------------------------------------------------------------

void setup()
{
    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);
    MIDI.begin(MIDI_CHANNEL_OMNI);

    lcd.begin(16, 2);
    lcd.print(TITLE_STR1);
    lcd.setCursor(0, 1);
    lcd.print(TITLE_STR2);
}

void loop()
{
    if (MIDI.read()) {

        // Display Raw Midi Message

        byte type    = MIDI.getType();
        byte channel = MIDI.getChannel(); 
        byte data1   = MIDI.getData1();
        byte data2   = MIDI.getData2();

        lcd.setCursor(0, 0);
        lcd.print("                ");
        lcd.setCursor(0, 0);
        lcd.print(type, HEX);
        lcd.print(' ');
        lcd.print(channel, HEX);
        lcd.print(' ');
        lcd.print(data1, HEX);
        lcd.print(' ');
        lcd.print(data2, HEX);
    }
}
