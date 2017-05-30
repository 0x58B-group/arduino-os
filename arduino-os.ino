// Libraries
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Ultrasonic.h>

// Programs
#define PROGRAMS 4

#define MENU 100
#define VOLTM 0
#define TEMPM 1
#define US_DIST 2
#define BEEPY 3

String programs[PROGRAMS] = {"Volt Meter",
                             "Temp Meter",
                             "Ultrasonic",
                             "Beepy!"
                            };

#define RUN(PROG) running_prog = (PROG)

// UI pins
#define BUT_A 2
#define BUT_B 3
#define ROT A0

// Sensor pins
#define DHT_PIN 4
#define US_TRIG 6
#define US_ECHO 7
#define US_TIMEOUT 40000UL

// Output pins
#define BEEPER 5
#define LCD_ADDR 0x27

// Sensor/LCD object
DHT dht(DHT_PIN, DHT11);
LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);
Ultrasonic us(US_TRIG, US_ECHO);

bool is_pressed(int pin);
void menu_loop();
void voltm_setup();
void voltm_loop();
void tempm_setup();
void tempm_loop();
void us_dist_setup();
void us_dist_loop();
void beepy_setup();
void beepy_loop();

int running_prog = 0;

// Menu program varies
int prog = 0;

void setup() {
  // put your setup code here, to run once:
  // Init LCD
  delay(500);
  lcd.begin();
  lcd.clear();
  lcd.print("LCD OK");
  lcd.setCursor(0, 1);
  // Finished initing LCD
  // Init DHT11
  dht.begin();
  // Try to read temp
  if (isnan(dht.readTemperature())) {
    lcd.print("DHT Failed!");
    delay(2000);
    lcd.clear();
    lcd.print("System halt");
    while (true);
  } else { // Or succeed
    lcd.print("DHT OK");
  }
  delay(1000);
  lcd.clear();
  // Finished initing DHT11
  // Init I/O btw test
  pinMode(BUT_A, INPUT);
  pinMode(BUT_B, INPUT);
  pinMode(BEEPER, OUTPUT);
  lcd.print("ROT < 10 ");
  while ((1024 - analogRead(ROT)) > 10);
  lcd.print("PASS");
  delay(1000);
  lcd.clear();
  lcd.print("ROT > 1014 ");
  while ((1024 - analogRead(ROT)) < 1014);
  lcd.print("Pass");
  delay(1000);
  lcd.clear();
  lcd.print("PRESS A ");
  while (!is_pressed(BUT_A));
  delay(25);
  lcd.print("PASS");
  delay(1000);
  lcd.clear();
  lcd.print("PRESS B ");
  while (!is_pressed(BUT_B));
  delay(25);
  lcd.print("PASS");
  delay(1000);
  lcd.clear();
  lcd.print("BEEP");
  tone(BEEPER, 1760, 500);
  delay(500);
  lcd.clear();
  // Finished initing
  lcd.print("Complete.");
  lcd.setCursor(0, 1);
  lcd.print("Booting...");
  delay(750);
  RUN(MENU);
}

void loop() {
  // put your main code here, to run repeatedly:
  switch (running_prog) {
    case MENU:
      menu_loop();
      break;
    case VOLTM:
      voltm_loop();
      break;
    case TEMPM:
      tempm_loop();
      break;
    case US_DIST:
      us_dist_loop();
      break;
    case BEEPY:
      beepy_loop();
      break;
    default:
      RUN(MENU);
      break;
  }
}

// Built-in API
bool is_pressed(int pin) {
  if (digitalRead(pin) == HIGH) {
    return true;
  } else if (digitalRead(pin) == LOW) {
    return false;
  } else {
    return false;
  }
}

// Menu as an app
void menu_loop() {
  int deg = analogRead(A0);
  prog = (1024 - deg) / (1024 / PROGRAMS);
  if (prog > PROGRAMS - 1) {
    prog = PROGRAMS - 1;
  }
  if (prog < 0) {
    prog = 0;
  }
  lcd.clear();
  lcd.print("Programs:");
  lcd.setCursor(0, 1);
  lcd.print(prog + 1);
  lcd.print(":");
  lcd.print(programs[prog]);
  if (is_pressed(BUT_A)) {
    delay(25);
    switch (prog) {
      case VOLTM:
        voltm_setup();
        break;
      case TEMPM:
        tempm_setup();
        break;
      case US_DIST:
        us_dist_setup();
        break;
      case BEEPY:
        beepy_setup();
        break;
    }
    RUN(prog);
  }
  while ((abs(analogRead(A0) - deg) < 10) && (!is_pressed(BUT_A)));
}

// App: voltage meter
void voltm_setup() {
  lcd.clear();
}
void voltm_loop() {
  lcd.clear();
  int adc_value = analogRead(A1);
  lcd.print(adc_value);
  float volt = adc_value / (1024 / 5.0);
  lcd.setCursor(0, 1);
  lcd.print(volt);
  lcd.print("V");
  if (is_pressed(BUT_B)) {
    delay(25);
    RUN(MENU);
    return;
  }
  while (abs(analogRead(A0) - adc_value) < 10 && is_pressed(BUT_B) == false);
}

// App: temp meter
void tempm_setup() {
  lcd.clear();
}

void tempm_loop() {
  if (millis() % 500 == 0) {
    lcd.clear();
    int temp = dht.readTemperature();
    lcd.print("Temp = ");
    lcd.print(temp);
    if (is_pressed(BUT_B)) {
      delay(25);
      RUN(MENU);
      return;
    }
  }
  if (is_pressed(BUT_B)) {
    delay(25);
    RUN(MENU);
    return;
  }
}

// App: Ultrasonic
void us_dist_setup() {
  lcd.clear();
}

void us_dist_loop() {
  if (millis() % 50 == 0) {
    int dist = us.distanceRead();
    if (dist < 1100) {
      lcd.clear();
      lcd.print("DIST = ");
      lcd.print(dist);
      lcd.print(" cm");
    } else {
      lcd.clear();
      lcd.print("Failed!");
    }
  }
  if (is_pressed(BUT_B)) {
    delay(25);
    RUN(MENU);
    return;
  }
}

// App: Beepy
void beepy_setup() {
  lcd.clear();
  lcd.print("Beepy!");
  while (is_pressed(BUT_A));
  delay(25);
}

void beepy_loop() {
  if (is_pressed(BUT_A)) {
    tone(BEEPER, 1760, 500);
  }
  if (is_pressed(BUT_B)) {
    RUN(MENU);
  }
}
