/*
 *  MIDI_CV_CONV_Test02
 *  
 *  Test01: GATE x 6 ch
 *  Test02: 同期してGATEを出力
 *  
 *  2018.12.1
 */
 
#include <Wire.h>
#include <I2CLiquidCrystal.h>
#include <MIDI.h>

#define LCD_TRACE   (0)
#define PIN_CHECK   (1)
#define TITLE_STR1  ("MIDI_CV Test 02 ")
#define TITLE_STR2  ("20181210        ")

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
I2CLiquidCrystal lcd(63, (bool)true);
#endif

volatile uint8_t cv[2];
volatile uint8_t gateBits;

// MIDI Input -------------------------------------------------------------------------

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
#if (LCD_TRACE)
  printNoteOnOff("On", inChannel, inNote, inVelocity);
#endif

  // cv
  if (inChannel < 2) {
    cv[inChannel - 1] = inNote;
  }

  // gate
  if (inChannel <= 6) {
    bitSet(gateBits, (inChannel + 1));
  }
}

void handleNoteOff(byte inChannel, byte inNote, byte inVelocity)
{
#if (LCD_TRACE)
  printNoteOnOff("Off", inChannel, inNote, inVelocity);
#endif

  // gate
  if (inChannel <= 6) {
    bitClear(gateBits, (inChannel + 1));
  }
}

// CV/GATE Output --------------------------------------------------------------

void cvWrite()
{
}

void gateWrite()
{
  PORTD = gateBits;
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
    byte type    = MIDI.getType();
    byte channel = MIDI.getChannel();
    byte data1   = MIDI.getData1();
    byte data2   = MIDI.getData2();

#if (LCD_TRACE)
    // Display Raw Midi Message
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

  gateWrite();
  cvWrite();

#if (PIN_CHECK)
  digitalWrite(CheckPin1, LOW);
#endif
}
