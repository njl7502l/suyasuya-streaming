#include <TimeLib.h>
#include <TimeAlarms.h>
#include <TinyGPS.h>
#include <SoftwareSerial.h>

SoftwareSerial SerialGPS = SoftwareSerial(2, 3);
TinyGPS gps; 

const int offset = 9;

void setup() {
  Serial.begin(9600);
  SerialGPS.begin(9600);
  Serial.println("Waiting for GPS time ... ");
  getTimeFromGPS();

  Alarm.alarmRepeat(10, 56, 0, Alarm);
  Alarm.timerRepeat(5, digitalClockDisplay);
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

// Timer Actions
void Alarm() {
  Serial.print("Alarm:\t");
  digitalClockDisplay();
  Alarm.timerOnce(60, OnceTimer);
}

void OnceTimer() {
  Serial.print("OnceTimer:\t");
  digitalClockDisplay();
}

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
