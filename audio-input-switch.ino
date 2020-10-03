#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#define BTN_IN_1 2
#define BTN_IN_2 3
#define BTN_IN_3 4
#define BTN_IN_4 5

#define CH_SWITCH_1 6
#define CH_SWITCH_2 7
#define CH_SWITCH_3 8
#define CH_SWITCH_4 9

#define CHANNEL_SIZE 4
#define BTN_PIN_ZERO_OFFSET 2
#define INPUT_DELAY_MSEC 500

#define LCD_CLK 13
#define LCD_DIN 12
#define LCD_DC 11
#define LCD_CE 10
#define LCD_RST 1
#define LCD_TEXT_CLEAR_LINE "              "
#define LCD_CHAR_WIDTH 6

const int buttonPins[CHANNEL_SIZE] = {BTN_IN_1, BTN_IN_2, BTN_IN_3, BTN_IN_4};
const int relayPins[CHANNEL_SIZE] = {CH_SWITCH_1, CH_SWITCH_2, CH_SWITCH_3, CH_SWITCH_4};

int channelStates[CHANNEL_SIZE];
unsigned long lastInteractionTime = -1;
char txtChannelStatusBuffer[4];

Adafruit_PCD8544 lcd = Adafruit_PCD8544(LCD_CLK, LCD_DIN, LCD_DC, LCD_CE, LCD_RST);

void updateDisplay()
{
  lcd.setTextSize(3);

  for (int c = 0; c < CHANNEL_SIZE; c++)
  {
    if (channelStates[c] == LOW)
    {
      lcd.setTextColor(WHITE, BLACK);
    }
    else
    {
      lcd.setTextColor(BLACK, WHITE);
    }

    // lcd.setCursor(((c * 3) + 1) * LCD_CHAR_WIDTH, 20);
    // snprintf(txtChannelStatusBuffer, 4, " %d ", c + 1)
    lcd.setCursor((c * 20), 20);
    snprintf(txtChannelStatusBuffer, 4, "%d", c + 1);
    lcd.print(txtChannelStatusBuffer);
  }

  lcd.display();
}

void writeChannelStates(bool reset)
{
  for (int c = 0; c < CHANNEL_SIZE; c++)
  {
    if (reset)
    {
      channelStates[c] = HIGH;
    }
    digitalWrite(relayPins[c], channelStates[c]);
  }
}

void toggleChannel(int channel)
{
  channelStates[channel] = channelStates[channel] == LOW ? HIGH : LOW;
  writeChannelStates(false);
}

void checkButton(int btnPin)
{
  int state = digitalRead(btnPin);
  int channel = 0;

  if (state == LOW)
  {
    lastInteractionTime = millis();
    toggleChannel(btnPin - BTN_PIN_ZERO_OFFSET);
  }
}

void checkInputs()
{
  for (int b = 0; b < CHANNEL_SIZE; b++)
  {
    checkButton(buttonPins[b]);
  }
}

void setup()
{
  for (int i = 0; i < CHANNEL_SIZE; i++)
  {
    pinMode(buttonPins[i], INPUT_PULLUP);
    pinMode(relayPins[i], OUTPUT);
  }

  writeChannelStates(true);

  lcd.begin();
  lcd.clearDisplay(); 
  lcd.setContrast(11);

  lcd.setTextColor(BLACK);
  lcd.setCursor(0, 1);
  lcd.setTextSize(2);
  lcd.println("> AXi <");
  lcd.display();

  delay(1000);

  lcd.clearDisplay();
  lcd.setTextSize(1);
  lcd.setCursor(0, 0);
  lcd.setTextColor(WHITE, BLACK);
  lcd.println(">  AXi      >>");
  lcd.setTextColor(BLACK, WHITE);
  lcd.display();
}

void loop()
{
  while (millis() - lastInteractionTime > INPUT_DELAY_MSEC)
  {
    checkInputs();
    updateDisplay();
  }
}
