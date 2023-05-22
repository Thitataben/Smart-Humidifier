#include <DHT.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

/// Define Pins ///
/*
 * define DHTPIN 3
 * define DHTTYPE DHT11
 * define POWER_WATERPIN 40
 * define SIGNAL_WATERPIN A15
 * define BUZZERPIN 2
 * define RELAYPIN 6
 * define LEDPIN 10-12 (GREEN 10, YELLOW 11, RED 12)
 * define KEYPADPIN 30-37
 * define 4DIGITPIN 43-48,50-53,A10,A11
 */

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht (3, DHT11);

byte colPins[4] = {33,32,31,30};
byte rowPins[4] = {34,35,36,37};

char keys[4][4] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);

/// Set Sensor Value ///
float humidity = 0;
float temperature = 0;
int waterlevel = 0;
float newwaterlevel = 0;

int kcount = 0; // Counter show put colour
int dcount = 0; // Counter show end colour
char oldkey = 'E';

String numInput = "";
char numEnd = '#'; 
int numcheck = 0; // Stop loop input number time

/*
int digit1 = 48; //PWM Display pin 1
int digit2 = 51; //PWM Display pin 2
int digit3 = 52; //PWM Display pin 6
int digit4 = 47; //PWM Display pin 8
int segA = A10; //Display pin 14
int segB = 53; //Display pin 16
int segC = 45; //Display pin 13
int segD = 43; //Display pin 3
int segE = A11; //Display pin 5
int segF = 50; //Display pin 11
int segG = 46; //Display pin 15
*/

int start_num ;  // Number to countdown from input time + 11 start:30 put:41
unsigned long time;

int digitcount = 0; // Set digitcount for start countdown time
int workcount = 0; // Set workcount for correct start countdown time

/*
int re6 = 0;
int reA2 = 0;
*/

void setup()
{
  // Arduino Boot-up //
  Serial.begin(9600);
  Serial.println("Starting...");

  // Temp & Humidity Sensor Boot //
  dht.begin();
  pinMode(3,OUTPUT);

  // Relay Boot //
  pinMode(6,OUTPUT);

  // LCD Boot //
  lcd.init();
  lcd.backlight();

  // Water Sensor Set-up //
  pinMode(40,OUTPUT);
  digitalWrite(40,LOW);

  // Buzzer(Sound) Set-up //
  pinMode(2,OUTPUT);

  // LED Set-up //
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(12,OUTPUT);
  digitalWrite(10,LOW);
  digitalWrite(11,LOW);
  digitalWrite(12,LOW);

  /*
  /// 4-digit 7-segmen Set-up
  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  pinMode(segE, OUTPUT);
  pinMode(segF, OUTPUT);
  pinMode(segG, OUTPUT);

  pinMode(digit1, OUTPUT);
  pinMode(digit2, OUTPUT);
  pinMode(digit3, OUTPUT);
  pinMode(digit4, OUTPUT);

  //start_num = numInput.toInt();
  */

  // Welcome Text //
  lcd.setCursor(0,0);
  lcd.print("Humidifier");
  lcd.setCursor(0,1);
  lcd.print("Start-up");

  delay(2000);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Please");
  lcd.setCursor(0,1);
  lcd.print("Select Mode:");
  delay(2000);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("A-Humid  B-Temp");
  lcd.setCursor(0,1);
  lcd.print("C-Water  D-Color");
}

void loop()
{
  // Keypad Input //
  char key = keypad.getKey();
  if (key != NO_KEY) {
    Serial.println(key);
  }

  // Test Key * - Numpad Input //
  if (key == '*') {
    oldkey = '*';
    Serial.println("Setting Time Mode");
    Serial.println(start_num);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Time Mode");
    lcd.setCursor(0,1);
    lcd.print("Set Time(s): ");
    char a;
    while (1)
    {
      a = keypad.getKey();
      if(a == '#')
       {
        digitcount += 1;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Time Mode");
        lcd.setCursor(0,1);
        lcd.print("Counting down");
        break;
      }
      if(a){
        Serial.print(a);
        numInput += a;
      }
    }
    a = ' ';
    Serial.println(numInput);
    start_num = numInput.toInt();
    Serial.println(start_num);
  }

  if (key == '#') {
    oldkey = '#';
  }

  // Key A - Relative Humidity Display //
  if (key == 'A' && oldkey != 'D') {
    oldkey = 'E';
    Serial.print("%RH: ");
    Serial.print(humidity);
    Serial.println(" %");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("%RH Mode:");
    lcd.setCursor(0,1);
    lcd.print("%RH: ");
    lcd.setCursor(5,1);
    lcd.print(humidity);
    lcd.setCursor(11,1);
    lcd.print("%");
  }

  // Key B - Temperature Display //
  if (key == 'B') {
    oldkey = 'E';
    Serial.print("T: ");
    Serial.print(temperature);
    Serial.println(" *C");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Temperature Mode");
    lcd.setCursor(0,1);
    lcd.print("T: ");
    lcd.setCursor(3,1);
    lcd.print(temperature);
    lcd.setCursor(9,1);
    lcd.print("*C");
  }

  // Key C - Remaining Water Level Display //
  if (key == 'C') {
    oldkey = 'E';
    Serial.print("Water level value: ");
    Serial.println(waterlevel); 
    Serial.println(newwaterlevel);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Water Level Mode");
    lcd.setCursor(0,1);
    lcd.print("%WL: ");
    lcd.setCursor(5,1);
    lcd.print(newwaterlevel);
    lcd.setCursor(11,1);
    lcd.print("%");
    delay(3000);
    if (newwaterlevel < 30) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Water Level Mode");
      lcd.setCursor(0,1);
      lcd.print("Fill More Water");
    }
  }

  // Key D - RGB LEDs Display //
  if (key == 'D') {
    dcount += 1;
    oldkey = 'D';
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Colour Mode");
    lcd.setCursor(0,1);
    lcd.print("Set Colour:");
    if (kcount == 0) {
      Serial.print("Set colour: ");
      kcount += 1;
    }
  }
  if (oldkey == 'D' && key == '1','2','3','4') {
    if (key == '1' && kcount == 1) {
      Serial.print("Colour: ");
      Serial.println("Green");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Colour: Green");
      digitalWrite(10,HIGH);
      digitalWrite(11,LOW);
      digitalWrite(12,LOW);
    }
    if (key == '2' && kcount == 1) {
      Serial.print("Colour: ");
      Serial.println("Yellow");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Colour: Yellow");
      digitalWrite(10,LOW);
      digitalWrite(11,HIGH);
      digitalWrite(12,LOW);
    }
    if (key == '3'&& kcount == 1) {
      Serial.print("Colour: ");
      Serial.println("Red");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Colour: Red");
      digitalWrite(10,LOW);
      digitalWrite(11,LOW);
      digitalWrite(12,HIGH);
    }
    if (key == '4' && kcount == 1) {
      Serial.print("Colour: ");
      Serial.println("Don't Have Colour");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Colour:");
      lcd.setCursor(0,1);
      lcd.print("Don't Have");
      digitalWrite(10,LOW);
      digitalWrite(11,LOW);
      digitalWrite(12,LOW);
    }
  }
  if (dcount == 2 && key == 'D') {
      oldkey = 'E'; //// set any letter that isnt D
      Serial.println("End Colour");
      kcount = 0;
      dcount = 0;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("End Colour Mode");
  }

  /// Humidity & Temperature Sensor ///
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  /*
  re6 = digitalRead(6);
  reA2 = analogRead(A2);
  */

  if (humidity < 80) {
    //Serial.println(re6);
    digitalWrite(6,HIGH);
    workcount += 1;
    //Serial.print("reA2");
    //Serial.println(reA2);
    //delay(500);
  }
  else {
    //Serial.println(re6);
    digitalWrite(6,LOW);
    workcount = 0;
  }

  /// Water Sensor ///
  digitalWrite(40,HIGH);
  delay(100);
  waterlevel = analogRead(A15);
  newwaterlevel = map(waterlevel, 0, 1023, 0, 100);
  digitalWrite(40,LOW);

  /// Buzzer ///
  if (newwaterlevel <= 30) {
    tone(2,1000);   // Send 1kHz sound signal
    delay(500);    // for 1 sec
    noTone(2);      // Stop sound...
    delay(500);    // for 1 sec
  }
  
  //delay(250);

  /*// 4-digit 7-segment
  if (digitcount == 1 && start_num != 0 && workcount == 1) {
    //long startTime = millis();
    if((millis()/1000) < start_num){
      displayNumber(start_num -(millis()/1000));
    }
    else {
      // reached zero, flash the display
      time=millis();
      while(millis() < time+200) {
        displayNumber(0);  // display 0 for 0.2 second
      }
      time=millis();    
      while(millis() < time+200) {
        lightNumber(10);  // Turn display off for 0.2 second
      }
    }  
    //while( (millis() - startTime) < 2000) {
    //displayNumber(1217);
    //}
    //delay(1000);  
  }*/
}

/*
void displayNumber(int toDisplay) {
#define DISPLAY_BRIGHTNESS  500

#define DIGIT_ON  HIGH
#define DIGIT_OFF  LOW

  long beginTime = millis();

  for(int digit = 4 ; digit > 0 ; digit--) {

    //Turn on a digit for a short amount of time
    switch(digit) {
    case 1:
      digitalWrite(digit1, DIGIT_ON);
      break;
    case 2:
      digitalWrite(digit2, DIGIT_ON);
      break;
    case 3:
      digitalWrite(digit3, DIGIT_ON);
      break;
    case 4:
      digitalWrite(digit4, DIGIT_ON);
      break;
    }

    //Turn on the right segments for this digit
    lightNumber(toDisplay % 10);
    toDisplay /= 10;

    delayMicroseconds(DISPLAY_BRIGHTNESS); 
    //Display digit for fraction of a second (1us to 5000us, 500 is pretty good)

    //Turn off all segments
    lightNumber(10); 

    //Turn off all digits
    digitalWrite(digit1, DIGIT_OFF);
    digitalWrite(digit2, DIGIT_OFF);
    digitalWrite(digit3, DIGIT_OFF);
    digitalWrite(digit4, DIGIT_OFF);
  }

  while( (millis() - beginTime) < 10) ; 
  //Wait for 20ms to pass before we paint the display again
}

//Given a number, turns on those segments
//If number == 10, then turn off number
void lightNumber(int numberToDisplay) {

#define SEGMENT_ON  LOW
#define SEGMENT_OFF HIGH

  switch (numberToDisplay){

  case 0:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_OFF);
    break;

  case 1:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    break;

  case 2:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 3:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 4:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 5:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 6:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 7:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    break;

  case 8:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 9:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 10:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    break;
  }
}
*/
