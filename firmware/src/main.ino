#include <TimeLib.h>
#include <TimeAlarms.h>
#include <TinyGPS.h>
#include <SoftwareSerial.h>

SoftwareSerial SerialGPS = SoftwareSerial(2, 3); // rx, tx
TinyGPS gps; 

const int offset = 9;
const int trig_pin = 9;
const int led_pin = 13;
const int switch_pin = 8;

volatile bool is_cancel = false;

void setup() {
  pinMode(trig_pin, OUTPUT);
  digitalWrite(trig_pin, LOW);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
  pinMode(switch_pin, INPUT_PULLUP);

  Serial.begin(9600);
  SerialGPS.begin(9600);
  Serial.println("Waiting for GPS time ... ");
  getTimeFromGPS();

  Alarm.alarmRepeat(9, 25, 0, customAlarm);
  Alarm.timerRepeat(5, TimerRepeat);
}

void loop() {
  for(int i=0 ; i<10 ; i++){
    Alarm.delay(100);
    if (digitalRead(switch_pin) == LOW){
      is_cancel = true;
      digitalWrite(trig_pin, LOW);
    }
    digitalWrite(led_pin, !is_cancel);
  }
  getTimeFromGPS();
}

// Functions -------------------------------------------------------------------
// GPSからの情報で時刻補正
void getTimeFromGPS() {
  bool is_done = true;
  while(is_done){
    while (SerialGPS.available()) {
      if (gps.encode(SerialGPS.read())) {
        unsigned long age;
        int Year;
        byte Month, Day, Hour, Minute, Second;
        gps.crack_datetime(&Year, &Month, &Day, &Hour, &Minute, &Second, NULL, &age);
        if (age < 500) {
          setTime(Hour, Minute, Second, Day, Month, Year);
          adjustTime(offset * SECS_PER_HOUR);
          is_done = false;
        }
      }
    }
    delay(10);
  }
}

// Alarm Action
void customAlarm() {
  Serial.print("Alarm:\t");
  if (is_cancel){
    Serial.println("cancelled");
    is_cancel = false;
    return;
  }
  digitalClockDisplay();
  digitalWrite(trig_pin, HIGH);
  Alarm.timerOnce(600, OnceTimer);
}

// Alarm Destructor
void OnceTimer() {
  Serial.print("OnceTimer:\t");
  digitalClockDisplay();
  digitalWrite(trig_pin, LOW);
}

// Timer Action
void TimerRepeat(){
  digitalClockDisplay();
}

// time writer
void digitalClockDisplay() {
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());

  Serial.println(); 
}

void printDigits(int digits) {
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
