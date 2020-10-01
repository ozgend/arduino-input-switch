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

#define LCD_CLK 12
#define LCD_DIN 11
#define LCD_DC 10
#define LCD_CE 13
#define LCD_RST 1

Nokia_LCD lcd(LCD_CLK, LCD_DIN, LCD_DC, LCD_CE, LCD_RST);

void setup()
{
  Serial.begin(9600);
  Serial.println("AIS start");

  lcd.begin();
  lcd.setContrast(60);
  lcd.clear(true);
  lcd.setInverted(true);

  for (int i = 0; i < CHANNEL_SIZE; i++)
  {
    pinMode(buttonPins[i], INPUT_PULLUP);
    pinMode(relayPins[i], OUTPUT);
  }

  toggleActiveChannel(-9);
  displayPrint(1, " AIS Started");
}

void displayPrint(int row, char text[])
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
  lcd.clear();

  if (activeChannel == channel)
  {
    channelStates[channel] = channelStates[channel] == LOW ? HIGH : LOW;
    displayPrint(4, channelStates[channel] == HIGH ? " " : " mute");
  }
  else
  {
    resetChannelStates();
    channelStates[channel] = LOW;
  }

  setChannelState();

  activeChannel = channel;

  lcd.clear();
  displayPrint(1, "Input: " + activeChannel);

  Serial.print("active channel set to: ");
  Serial.print(activeChannel);
  Serial.print("\n");
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
