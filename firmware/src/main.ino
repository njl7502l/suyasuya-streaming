#include <TimeLib.h>
#include <TimeAlarms.h>
#include <TinyGPS.h>
#include <SoftwareSerial.h>

SoftwareSerial SerialGPS = SoftwareSerial(2, 3); // rx, tx
TinyGPS gps; 

const int offset = 9;

void setup() {
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);

  Serial.begin(9600);
  SerialGPS.begin(9600);
  Serial.println("Waiting for GPS time ... ");
  getTimeFromGPS();

  Alarm.alarmRepeat(14, 57, 0, customAlarm);
  Alarm.timerRepeat(5, TimerRepeat);
}

void loop() {
  Alarm.delay(1000);
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
  digitalClockDisplay();
  digitalWrite(8, HIGH);
  Alarm.timerOnce(60, OnceTimer);
}

// Alarm Destructor
void OnceTimer() {
  Serial.print("OnceTimer:\t");
  digitalClockDisplay();
  digitalWrite(8, LOW);
}

// Timer Action
void TimerRepeat(){
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
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
