/*
 *  MIDI_CV_CONV_Test03
 *  
 *  Test01: GATE x 6 ch
 *  Test02: 同期してGATEを出力
 *  Test03: CV x 2ch (dummy)
 *  
 *  2018.12.10
 */

#include <SPI.h>
#include <Wire.h>
#include <I2CLiquidCrystal.h>
#include <MIDI.h>

#define LCD_TRACE   (0)
#define PIN_CHECK   (1)
#define TITLE_STR1  ("MIDI_CV Test 03 ")
#define TITLE_STR2  ("20181210        ")

// Pin assign
const int GateOutPin1 = 2;
const int GateOutPin2 = 3;
const int GateOutPin3 = 4;
const int GateOutPin4 = 5;
const int GateOutPin5 = 6;
const int GateOutPin6 = 7;

const int CheckPin1   = 8;

const int MCP4922Ldac = 9;
const int MCP4922Cs = 10;

// MIDI
MIDI_CREATE_DEFAULT_INSTANCE();

// MCP4922
SPISettings MCP4922_SPISetting(8000000, MSBFIRST, SPI_MODE0);

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

// param
//   channel: 0, 1
//   val: 0 .. 4095

void MCP4922Write(bool channel, uint16_t val)
{
  uint16_t cmd = channel << 15 | 0x3000;
  cmd |= (val & 0x0fff);

  digitalWrite(MCP4922Ldac, HIGH);
  digitalWrite(MCP4922Cs, LOW);
  SPI.transfer(highByte(cmd));
  SPI.transfer(lowByte(cmd));
  digitalWrite(MCP4922Cs, HIGH);
  digitalWrite(MCP4922Ldac, LOW);
}

void cvWrite()
{
  static uint16_t v0 = 0;
  static uint16_t v1 = 0;
  
  MCP4922Write(0, (v0 & 0x0FFF));
  v0++;

  MCP4922Write(1, (v1 & 0x0FFF));
  v1++;
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

  pinMode(MCP4922Cs, OUTPUT);
  digitalWrite(MCP4922Cs, HIGH);  // set CS as inactive
  pinMode(MCP4922Ldac, OUTPUT);
  SPI.begin();
  SPI.beginTransaction(MCP4922_SPISetting);

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
