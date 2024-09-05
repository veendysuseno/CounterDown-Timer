/*
    Program Timer Penghitung Waktu Mundur
*/
#include <LiquidCrystal.h>  //Pendeklarasian Library
#include <TimeLib.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  // Pemilihan Pin dan pendeklarasian variabel
const int buzzerPin = 10;
const int resetButtonPin = 6;
const int startStopButtonPin = 7;
const int downButtonPin = 8;
const int upButtonPin = 9;

int setupHours = 0;
int setupMinutes = 0;
int setupSeconds = 0;
time_t setupTime = 0;

int currentHours = 0;
int currentMinutes = 0;
int currentSeconds = 0;
time_t currentTime = 0;
time_t startTime = 0;
time_t elapsedTime = 0;

int resetButtonState = LOW;
long resetButtonLongPressCounter = 0;
int startStopButtonState = LOW;
int upButtonState = LOW;
int downButtonState = LOW;
int resetButtonPrevState = LOW;
int startStopButtonPrevState = LOW;
int upButtonPrevState = LOW;
int downButtonPrevState = LOW;
bool resetButtonPressed = false;
bool resetButtonLongPressed = false;
bool startStopButtonPressed = false;
bool upButtonPressed = false;
bool downButtonPressed = false;

const int MODE_IDLE = 0;
const int MODE_SETUP = 1;
const int MODE_RUNNING = 2;
const int MODE_RINGING = 3;

int currentMode = MODE_IDLE;

int dataSelection = 0;

void setup() { //Pengaturan Pin dan Variabel
lcd.begin(16, 2);
pinMode(resetButtonPin, INPUT);
pinMode(startStopButtonPin, INPUT);
pinMode(upButtonPin, INPUT);
pinMode(downButtonPin, INPUT);
pinMode(buzzerPin, OUTPUT);
Serial.begin(9600);
}

void loop() {  //Perulangan Program
startStopButtonPressed = false;
upButtonPressed = false;
downButtonPressed = false;
resetButtonPressed = false;
resetButtonLongPressed = false;
resetButtonState = digitalRead(resetButtonPin);
if (resetButtonState != resetButtonPrevState){
    resetButtonPressed = resetButtonState == HIGH;
    resetButtonPrevState = resetButtonState;
}
  else  // apabila dipencet lama
{
    if (resetButtonState == HIGH)
    {
    resetButtonLongPressCounter++;
    if (resetButtonLongPressCounter == 100)
    {
        resetButtonPressed = false;
        resetButtonLongPressed = true;
        resetButtonLongPressCounter = 0;
    }
    }
    else
    {
    resetButtonLongPressCounter = 0;
    resetButtonPressed = false;
    resetButtonLongPressed = false;
    }
}

  //Start dan Stop button
startStopButtonPressed = false;
startStopButtonState = digitalRead(startStopButtonPin);
if (startStopButtonState != startStopButtonPrevState)
{
    startStopButtonPressed = startStopButtonState == HIGH;
    startStopButtonPrevState = startStopButtonState;
}

  //Down button
downButtonPressed = false;
downButtonState = digitalRead(downButtonPin);
if (downButtonState != downButtonPrevState){
    downButtonPressed = downButtonState == HIGH;
    downButtonPrevState = downButtonState;
}

  //Up button
upButtonPressed = false;
upButtonState = digitalRead(upButtonPin);
if (upButtonState != upButtonPrevState){
    upButtonPressed = upButtonState == HIGH;
    upButtonPrevState = upButtonState;
    }

  //Mode management
switch (currentMode)
{
    case MODE_IDLE:
    if (resetButtonPressed)
    {
        Reset();
    }
    if (resetButtonLongPressed)
    {
        currentMode = MODE_SETUP;
    }
    if (startStopButtonPressed)
    {
        currentMode = currentMode == MODE_IDLE ? MODE_RUNNING : MODE_IDLE;
        if (currentMode == MODE_RUNNING)
        {
          // START TIMER!
        startTime = now();
        }
    }
    break;

    case MODE_SETUP:
    if (resetButtonPressed)
    {
        //keluar mode setup
        setupTime = setupSeconds + (60 * setupMinutes) + (3600 * setupHours);
        currentHours = setupHours;
        currentMinutes = setupMinutes;
        currentSeconds = setupSeconds;
        dataSelection = 0;
        currentMode = MODE_IDLE;
    }
    if (startStopButtonPressed)
    {
        // Pilih data berikutnya untuk disesuaikan
        dataSelection++;
        if (dataSelection == 3)
        {
        dataSelection = 0;
        }
    }
    if (downButtonPressed)
    {
        switch (dataSelection)
        {
          case 0: // jam
            setupHours--;
            if (setupHours == -1)
            {
            setupHours = 99;
            }
            break;
          case 1: // menit
            setupMinutes--;
            if (setupMinutes == -1)
            {
            setupMinutes = 59;
            }
            break;
          case 2: // detik
            setupSeconds--;
            if (setupSeconds == -1)
            {
            setupSeconds = 59;
            }
            break;
        }
    }
    if (upButtonPressed)
    {
        switch (dataSelection)
        {
          case 0: // jam
            setupHours++;
            if (setupHours == 100)
            {
            setupHours = 0;
            }
            break;
          case 1: // menit
            setupMinutes++;
            if (setupMinutes == 60)
            {
            setupMinutes = 0;
            }
            break;
          case 2: // detik
            setupSeconds++;
            if (setupSeconds == 60)
            {
            setupSeconds = 0;
            }
            break;
        }
    }
    break;

    case MODE_RUNNING:
    if (startStopButtonPressed)
    {
        currentMode = MODE_IDLE;
    }
    if (resetButtonPressed)
    {
        Reset();
        currentMode = MODE_IDLE;
    }
    break;

    case MODE_RINGING:
    if (resetButtonPressed || startStopButtonPressed || downButtonPressed || upButtonPressed)
    {
        currentMode = MODE_IDLE;
    }
    break;
}

/*
    Time management
  */
switch (currentMode)
{
    case MODE_IDLE:
    case MODE_SETUP:
      // NOP
    break;
    case MODE_RUNNING:
    currentTime = setupTime - (now() - startTime);
    if (currentTime <= 0)
    {
        currentMode = MODE_RINGING;
    }
    break;
    case MODE_RINGING:
    analogWrite(buzzerPin, 20);
    delay(20);
    analogWrite(buzzerPin, 0);
    delay(40);
    break;
}
/*
    LCD management
  */
  //lcd.clear();
lcd.setCursor(0, 0);
switch (currentMode)
{
    case MODE_IDLE:
    lcd.print("Timer ready     ");
    lcd.setCursor(0, 1);
    lcd.print(currentHours);
    lcd.print(" ");
    lcd.print(currentMinutes);
    lcd.print(" ");
    lcd.print(currentSeconds);
    lcd.print("    ");
    break;
    case MODE_SETUP:
    lcd.print("Setup mode: ");
    switch (dataSelection)
    {
        case 0:
        lcd.print("HRS ");
        break;
        case 1:
        lcd.print("MINS");
        break;
        case 2:
        lcd.print("SECS");
        break;
    }
    lcd.setCursor(0, 1);
    lcd.print(setupHours);
    lcd.print(" ");
    lcd.print(setupMinutes);
    lcd.print(" ");
    lcd.print(setupSeconds);
    lcd.print("    ");
    break;
    case MODE_RUNNING:
    lcd.print("Counting down...");
    lcd.setCursor(0, 1);
    if (hour(currentTime) < 10) lcd.print("0");
    lcd.print(hour(currentTime));
    lcd.print(":");
    if (minute(currentTime) < 10) lcd.print("0");
    lcd.print(minute(currentTime));
    lcd.print(":");
    if (second(currentTime) < 10) lcd.print("0");
    lcd.print(second(currentTime));
    break;
    case MODE_RINGING:
    lcd.print("   RING-RING!   ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    break;
}
delay(10);
}

void Reset(){
currentMode = MODE_IDLE;
currentHours = setupHours;
currentMinutes = setupMinutes;
currentSeconds = setupSeconds;
}
