#include <Nokia_LCD.h>

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

const int defaultChannelStates[CHANNEL_SIZE] = {HIGH, HIGH, HIGH, HIGH};

int buttonPins[CHANNEL_SIZE] = {BTN_IN_1, BTN_IN_2, BTN_IN_3, BTN_IN_4};
int relayPins[CHANNEL_SIZE] = {CH_SWITCH_1, CH_SWITCH_2, CH_SWITCH_3, CH_SWITCH_4};
int channelStates[CHANNEL_SIZE];

int activeChannel = -1;
unsigned long lastInteractionTime = -1;

#define LCD_CLK 13
#define LCD_DIN 12
#define LCD_DC 11
#define LCD_CE 10
#define LCD_RST 1

Nokia_LCD lcd(LCD_CLK, LCD_DIN, LCD_DC, LCD_CE, LCD_RST);

void setup()
{
  lcd.begin();
  lcd.setInverted(false);
  lcd.setContrast(11);
  lcd.clear(false);
  displayPrint(5, " AIS init");

  for (int i = 0; i < CHANNEL_SIZE; i++)
  {
    pinMode(buttonPins[i], INPUT_PULLUP);
    pinMode(relayPins[i], OUTPUT);
  }

  toggleActiveChannel(-9);
  displayPrint(0, "AIS");
  displayPrint(5, "no input set");
}

void displayPrint(int row, const char *text)
{
  lcd.setCursor(0, row);
  lcd.print(text);
}

void resetChannelStates()
{
  memcpy(channelStates, defaultChannelStates, sizeof(channelStates));
}

void setChannelState()
{
  for (int c = 0; c < CHANNEL_SIZE; c++)
  {
    digitalWrite(relayPins[c], channelStates[c]);
  }
}

void toggleActiveChannel(int channel)
{
  displayPrint(5, "             ");
  displayPrint(4, "    +    ");

  if (activeChannel == channel)
  {
    channelStates[channel] = channelStates[channel] == LOW ? HIGH : LOW;
    displayPrint(4, channelStates[channel] == LOW ? "    +    " : "< mute >");
  }
  else
  {
    resetChannelStates();
    channelStates[channel] = LOW;
  }

  setChannelState();

  activeChannel = channel;

  displayPrint(3, getChannelAlias(channel));
}

char *getChannelAlias(int channel)
{
  char *channelAlias = " X ";
  switch (channel)
  {
  case 0:
    channelAlias = "Input [A]";
    break;
  case 1:
    channelAlias = "Input [B]";
    break;
  case 2:
    channelAlias = "Input [C]";
    break;
  case 3:
    channelAlias = "Input [D]";
    break;
  case 4:
    channelAlias = "Input [BT]";
    break;
  default:
    channelAlias = "channel x";
  }
  return channelAlias;
}

void checkButton(int btnPin)
{
  int state = digitalRead(btnPin);
  int channel = 0;

  if (state == LOW)
  {
    lastInteractionTime = millis();
    toggleActiveChannel(btnPin - BTN_PIN_ZERO_OFFSET);
  }
}

void checkButtons()
{
  for (int b = 0; b < CHANNEL_SIZE; b++)
  {
    checkButton(buttonPins[b]);
  }
}

void processEvent()
{
  while (millis() - lastInteractionTime > INPUT_DELAY_MSEC)
  {
    checkButtons();
  }
}

void loop()
{
  processEvent();
}
