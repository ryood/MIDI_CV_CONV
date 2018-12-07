#include <Wire.h>
#include <I2CLiquidCrystal.h>
#include <MIDI.h>

#define LCD_TRACE   (0)
#define PIN_CHECK   (1)
#define TITLE_STR1  ("MIDI to GATE    ")
#define TITLE_STR2  ("20181204        ")

// Pin assign
const int GateOutPin1 = 2;
const int GateOutPin2 = 3;
const int GateOutPin3 = 4;
const int GateOutPin4 = 5;
const int GateOutPin5 = 6;
const int GateOutPin6 = 7;

const int CheckPin1   = 8;

// MIDI
MIDI_CREATE_DEFAULT_INSTANCE();

// I2C LCD
#if (LCD_TRACE)
I2CLiquidCrystal lcd(32, (bool)true);
#endif

// MIDI -------------------------------------------------------------------------

#if (LCD_TRACE)
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
#endif

void handleNoteOn(byte inChannel, byte inNote, byte inVelocity)
{
  digitalWrite(GateOutPin1, HIGH);
#if (LCD_TRACE)
  printNoteOnOff("On", inChannel, inNote, inVelocity);
#endif
}

void handleNoteOff(byte inChannel, byte inNote, byte inVelocity)
{
  digitalWrite(GateOutPin1, LOW);
#if (LCD_TRACE)
  printNoteOnOff("Off", inChannel, inNote, inVelocity);
#endif
}

// -----------------------------------------------------------------------------

void setup()
{
  pinMode(GateOutPin1, OUTPUT);
  pinMode(GateOutPin2, OUTPUT);
  pinMode(GateOutPin3, OUTPUT);
  pinMode(GateOutPin4, OUTPUT);
  pinMode(GateOutPin5, OUTPUT);
  pinMode(GateOutPin6, OUTPUT);
  pinMode(CheckPin1,   OUTPUT);

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.begin(MIDI_CHANNEL_OMNI);

#if (LCD_TRACE)
  lcd.begin(16, 2);
  lcd.print(TITLE_STR1);
  lcd.setCursor(0, 1);
  lcd.print(TITLE_STR2);
  delay(2000);
#endif
}

void loop()
{
#if (PIN_CHECK)
  digitalWrite(CheckPin1, HIGH);
#endif

  if (MIDI.read()) {

    // Display Raw Midi Message

    byte type    = MIDI.getType();
    byte channel = MIDI.getChannel();
    byte data1   = MIDI.getData1();
    byte data2   = MIDI.getData2();

#if (LCD_TRACE)
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
#endif
  }
#if (PIN_CHECK)
  digitalWrite(CheckPin1, LOW);
#endif
}
