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

int a_count = 0; // Counter for A button (Need to push 2 times)
int c_count = 0; // Counter for C button (Need to push 2 times)
int kcount = 0; // Counter show put colour
int dcount = 0; // Counter show end colour
int a_start_count = 0; // Counter for %RH mode
int c_start_count = 0; // Counter for water level value mode
int start_count = 0; // Counter for turn on/off mode
int whilecount = 0; // Counter to display lcd
char oldkey = 'E';  // Input old key

String numInput = "";   // Input data for * button type string
String a_numInput = ""; // Input data for A button type string
String c_numInput = ""; // Input data for C button type string
int numcheck = 0;   // Stop loop input number time
int errordetect =0; // Detect error input

int state_ref_humidity = 0; // Set state to know ref_humidity have value or not
int state_ref_waterlevel = 0; // Set state to know ref_humidity have value or not
int digitcount = 0; // Set digitcount for start countdown time
int workcount = 0; // Set workcount for correct start countdown time

int comp_ref_humidity; // Comparation State between humidity with ref_humidity value
int comp_ref_waterlevel; // Comparation State between humidity with ref_humidity value

int digit1 = 53; //PWM Display pin 1 
int digit2 = 50; //PWM Display pin 2 
int digit3 = 49; //PWM Display pin 6 
int digit4 = 47; //PWM Display pin 8 
int segA = A10; //Display pin 14
int segB = 48; //Display pin 16 
int segC = 45; //Display pin 13
int segD = 43; //Display pin 3
int segE = A11; //Display pin 5
int segF = 51; //Display pin 11 
int segG = 46; //Display pin 15

int start_num ;       // Number to countdown from input time
int ref_humidity;     // Reference of humudity value
int ref_waterlevel;   // Reference of water level value
unsigned long time;

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

  
  //4-digit 7-segmen Set-up
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

  start_num = numInput.toInt();
  

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
  delay(2000);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("*-Timer");
  lcd.setCursor(0,1);
  lcd.print("#-Turn On/Off");
}

void loop()
{
  // Keypad Input //
  char key = keypad.getKey();
  if (key != NO_KEY) {
  }

  // Test Key * - Numpad Input //
  if (key == '*') {
    oldkey = '*';
    a_count = 0;
    c_count = 0;
    errordetect = 0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Time Mode");
    lcd.setCursor(0,1);
    lcd.print("Set Time(s): ");
    char n;
    while (1)
    {
      n = keypad.getKey();
      if(n == '*')
       {
        digitcount += 1;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Time Mode");
        lcd.setCursor(0,1);
        lcd.print("Counting down");
        break;
      }
      if(n){
        if (n == 'A') {
          errordetect += 1;
          break;
        }
        if (n == 'B') {
          errordetect += 1;
          break;
        }
        if (n == 'C') {
          errordetect += 1;
          break;
        }
        if (n == 'D') {
          errordetect += 1;
          break;
        }
        if (n == '#') {
          errordetect += 1;
          break;
        }
        numInput += n;
      }
    }
    if (errordetect == 1) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Error Input");
        numInput = "";
      }
    else {
      n = ' ';
      start_num = numInput.toInt();
    }
  }

  if (key == '#') {
    oldkey = '#';
    a_count = 0;
    c_count = 0;
    start_count += 1;
    
    if (start_count == 1) {
      digitalWrite(6,HIGH);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Turn On/Off Mode");
      lcd.setCursor(0,1);
      lcd.print("Mode :Turn On");
      
    }
    if (start_count == 2) {
      digitalWrite(6,LOW);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Turn On/Off Mode");
      lcd.setCursor(0,1);
      lcd.print("Mode: Turn Off");
      start_count = 0;
    }
  }

  // Key A - Relative Humidity Display //
  if (key == 'A' && oldkey != 'D') {
    oldkey = 'A';                
    a_count += 1;
    c_count = 0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("%RH Mode:");
    lcd.setCursor(0,1);
    lcd.print("%RH: ");
    lcd.setCursor(5,1);
    lcd.print(humidity);
    lcd.setCursor(11,1);
    lcd.print("%");
    if (a_count == 2) {
      errordetect = 0;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Setting %RH Mode");
      lcd.setCursor(0,1);
      lcd.print("%RH Value:");
      char a;
      while (1)
      {
        a = keypad.getKey();
        if(a == 'A')
        {
          break;
        }
        if(a){
          if (a == 'B') {
            errordetect += 1;
            break;
          }
          if (a == 'C') {
            errordetect += 1;
            break;
          }
          if (a == 'D') {
            errordetect += 1;
            break;
          }
          if (a == '*') {
            errordetect += 1;
            break;
          }
          if (a == '#') {
            errordetect += 1;
            break;
          }
          a_numInput += a;
        }
      }
      if (errordetect == 1) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Error Input");
        a_count -= 1;
        a_numInput = "";
      }
      else {
        a = ' ';
        state_ref_humidity = 1;
        comp_ref_humidity = 1;
        ref_humidity = a_numInput.toInt();
        a_count = 0;
        a_numInput = "";
      }
    }
    if (humidity < ref_humidity && state_ref_humidity == 1 && comp_ref_humidity == 1) {
      delay(250);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("%RH: ");
      lcd.setCursor(5,0);
      lcd.print(humidity);
      lcd.setCursor(0,1);
      lcd.print("%RH < Ref Value");
      comp_ref_humidity = 0;
    }
    if (humidity == ref_humidity && state_ref_humidity == 1) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("%RH: ");
      lcd.setCursor(5,0);
      lcd.print(humidity);
      lcd.setCursor(0,1);
      lcd.print("%RH = Ref Value");
      comp_ref_humidity = 0; 
    }
    if (humidity > ref_humidity && state_ref_humidity == 1) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("%RH: ");
      lcd.setCursor(5,0);
      lcd.print(humidity);
      lcd.setCursor(0,1);
      lcd.print("%RH > Ref Value");
      comp_ref_humidity = 0;
    }
  }

  // Key B - Temperature Display //
  if (key == 'B') {
    oldkey = 'E';
    a_count = 0;
    c_count = 0;
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
    oldkey = 'C';                          
    a_count = 0;
    c_count += 1;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Water Level Mode");
    lcd.setCursor(0,1);
    lcd.print("%WL: ");
    lcd.setCursor(5,1);
    lcd.print(newwaterlevel);
    lcd.setCursor(11,1);
    lcd.print("%");
    if (c_count == 2) {
      errordetect = 0;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Setting WL Mode");
      lcd.setCursor(0,1);
      lcd.print("WL Value:");
      char c;
      while (1)
      {
        c = keypad.getKey();
        if(c == 'C')
        {
          break;
        }
        if(c){
          if (c == 'A') {
            errordetect += 1;
            break;
          }
          if (c == 'B') {
            errordetect += 1;
            break;
          }
          if (c == 'D') {
            errordetect += 1;
            break;
          }
          if (c == '*') {
            errordetect += 1;
            break;
          }
          if (c == '#') {
            errordetect += 1;
            break;
          }
          c_numInput += c;
        }
      }
      if (errordetect == 1) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Error Input");
        c_count -= 1;
        c_numInput = "";
      }
      else {
        c = ' ';
        state_ref_waterlevel = 1;
        ref_waterlevel = c_numInput.toInt();
        c_count = 0;
        c_numInput = "";
        comp_ref_waterlevel = 1;
      }
    } 
    if (newwaterlevel < ref_waterlevel && state_ref_waterlevel == 1  && comp_ref_waterlevel == 1) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Water Level Mode");
      lcd.setCursor(0,1);
      lcd.print("Fill More Water");
      comp_ref_waterlevel = 0;
    }
    if (newwaterlevel >= ref_waterlevel && state_ref_waterlevel == 1 && comp_ref_waterlevel == 1) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Water Level Mode");
      lcd.setCursor(0,1);
      lcd.print("Water Is Enough");
      comp_ref_waterlevel = 0;
    }
  }

  // Key D - RGB LEDs Display //
  if (key == 'D') {
    oldkey = 'D';
    a_count = 0;
    c_count = 0;
    dcount += 1;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Colour Mode");
    lcd.setCursor(0,1);
    lcd.print("Set Colour:");
    if (kcount == 0) {
      kcount += 1;
    }
  }
  if (oldkey == 'D' && key == '1','2','3','4') {
    if (key == '1' && kcount == 1) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Colour: Green");
      digitalWrite(10,HIGH);
      digitalWrite(11,LOW);
      digitalWrite(12,LOW);
    }
    if (key == '2' && kcount == 1) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Colour: Yellow");
      digitalWrite(10,LOW);
      digitalWrite(11,HIGH);
      digitalWrite(12,LOW);
    }
    if (key == '3'&& kcount == 1) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Colour: Red");
      digitalWrite(10,LOW);
      digitalWrite(11,LOW);
      digitalWrite(12,HIGH);
    }
    if (key == '4' && kcount == 1) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Colour:");
      lcd.setCursor(0,1);
      lcd.print("No Colour");
      digitalWrite(10,LOW);
      digitalWrite(11,LOW);
      digitalWrite(12,LOW);
    }
  }
  if (dcount == 2 && key == 'D') {
      oldkey = 'E'; //// set any letter that isnt D
      kcount = 0;
      dcount = 0;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("End Colour Mode");
  }

  /// Humidity & Temperature Sensor ///
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  if (humidity > ref_humidity && state_ref_humidity == 1 && start_num != 0) {
    digitalWrite(6,LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("%RH: ");
    lcd.setCursor(5,0);
    lcd.print(humidity);
    lcd.setCursor(0,1);
    lcd.print("%RH > Ref Value");
  }

  /// Water Sensor ///
  digitalWrite(40,HIGH);
  delay(100);
  waterlevel = analogRead(A15);
  newwaterlevel = map(waterlevel, 0, 1023, 0, 100);
  digitalWrite(40,LOW);

  /// Buzzer ///
  if (newwaterlevel <= ref_waterlevel && state_ref_waterlevel == 1) {
    tone(2,1000);   // Send 1kHz sound signal
    delay(500);    // for 1 sec
    noTone(2);      // Stop sound...
    delay(500);    // for 1 sec
  }
  if (humidity > ref_humidity && state_ref_humidity == 1) {
    tone(2,1000);   // Send 1kHz sound signal
    delay(500);    // for 1 sec
    noTone(2);      // Stop sound...
    delay(500);    // for 1 sec
  }
  
  //delay(250);

  // 4-digit 7-segment
  if (digitcount == 1 && start_num != 0 && state_ref_humidity == 0) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Set %RH Ref Pls");
  }
  if (digitcount == 1 && start_num != 0 && humidity < ref_humidity) {
    //long startTime = millis();
    if(start_num > 0){
      digitalWrite(6,HIGH);
      while(whilecount<90) {
        displayNumber(start_num);
        whilecount +=1 ;
      }
      //delay(1000);
      start_num -= 1;
      whilecount = 0;
      if(start_num == 0) {
        digitalWrite(6,LOW);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Finish Time Mode");
        digitcount = 0;
        numInput = "";
      }
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
  }
}


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
