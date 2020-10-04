#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#define BTN_IN_1 2
#define BTN_IN_2 3
#define BTN_IN_3 4
#define BTN_IN_4 5

#define CHANNEL_RESET -9
#define CHANNEL_SIZE 4
#define BTN_PIN_ZERO_OFFSET 2
#define INPUT_DELAY_MSEC 500

#define LCD_CLK 9
#define LCD_DIN 10
#define LCD_DC 11
#define LCD_CE 12
#define LCD_RST 13
#define LCD_TEXT_CLEAR_LINE "              "
#define LCD_CHAR_WIDTH 6

#define MUX_OUT_A 6
#define MUX_OUT_B 7
#define MUX_INHIBIT 8

const int buttonPins[CHANNEL_SIZE] = {BTN_IN_1, BTN_IN_2, BTN_IN_3, BTN_IN_4};
const int muxSelector[CHANNEL_SIZE][2] = {{LOW, LOW}, {HIGH, LOW}, {LOW, HIGH}, {HIGH, HIGH}};

int channelStates[CHANNEL_SIZE];
int muxInhibitState = HIGH;
int activeChannel = -1;
unsigned long lastInteractionTime = -1;
char txtChannelStatusBuffer[4];

Adafruit_PCD8544 lcd = Adafruit_PCD8544(LCD_CLK, LCD_DIN, LCD_DC, LCD_CE, LCD_RST);

void updateDisplay()
{
  lcd.fillRect(0, 10, 84, 38, WHITE);

  lcd.setTextSize(1);
  lcd.setCursor(72, 10);
  lcd.setTextColor(BLACK);
  lcd.print(muxInhibitState == HIGH ? "((" : "(X");

  lcd.setTextSize(3);

  for (int c = 0; c < CHANNEL_SIZE; c++)
  {
    if (channelStates[c] == LOW)
    {
      lcd.fillRect((c * 20) + 1, 21, 21, 27, BLACK);
      lcd.setTextColor(WHITE);
    }
    else
    {
      lcd.setTextColor(BLACK);
    }

    snprintf(txtChannelStatusBuffer, 4, "%d", c + 1);
    lcd.setCursor((c * 20) + 4, 24);
    lcd.print(txtChannelStatusBuffer);
  }

  lcd.display();
}

void setActiveChannel(int channel)
{
  if (channel == activeChannel)
  {
    channelStates[channel] = channelStates[channel] == LOW ? HIGH : LOW;
    muxInhibitState = muxInhibitState == LOW ? HIGH : LOW;
  }
  else
  {
    muxInhibitState = HIGH;
    for (int c = 0; c < CHANNEL_SIZE; c++)
    {
      channelStates[c] = HIGH;
    }
  }

  if (channel == CHANNEL_RESET)
  {
    digitalWrite(MUX_INHIBIT, HIGH);
    return;
  }

  activeChannel = channel;
  channelStates[channel] = LOW;

  digitalWrite(MUX_INHIBIT, muxInhibitState);
  digitalWrite(MUX_OUT_A, muxSelector[channel][0]);
  digitalWrite(MUX_OUT_B, muxSelector[channel][1]);
}

void checkButton(int btnPin)
{
  int state = digitalRead(btnPin);
  int channel = 0;

  if (state == LOW)
  {
    lastInteractionTime = millis();
    setActiveChannel(btnPin - BTN_PIN_ZERO_OFFSET);
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
  }

  pinMode(MUX_OUT_A, OUTPUT);
  pinMode(MUX_OUT_B, OUTPUT);
  pinMode(MUX_INHIBIT, OUTPUT);

  setActiveChannel(CHANNEL_RESET);
  digitalWrite(MUX_INHIBIT, LOW);

  lcd.begin();
  lcd.clearDisplay();
  lcd.setContrast(11);

  lcd.fillRect(0, 0, 84, 48, BLACK);
  lcd.display();

  lcd.setTextColor(WHITE);
  lcd.setCursor(0, 10);
  lcd.setTextSize(3);
  lcd.println(" AXi ");
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
