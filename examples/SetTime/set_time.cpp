/*
  Set the time of a DS3131
*/

// Get the board definitions...
#include <Arduino.h>

#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
#define Serial SERIAL_PORT_USBVIRTUAL
#endif

#include "RTClib.h"

RTC_DS3231 rtc;

char buffer[64];

int8_t time_zone = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println("Set DS3231 Time.");

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1)
      ;
  }

  if (rtc.lostPower()) {
    DateTime now = rtc.now();
    Serial.println("RTC had lost power!");
    Serial.print("It internal time is currently: ");
    now.toString(buffer);
    Serial.println(buffer);
  }

  Serial.println();
  Serial.println("Commands: T yy mm dd HH MM SS | Z offset");
  Serial.println("   T: Input the new time in UTC with the following format");
  Serial.println("      year month day hour minute second");
  Serial.println("      example: T 19 03 29 15 30 00");
  Serial.println("   Z: Input the Time Zone to use for the display");
  Serial.println("      TZ offset between -12 to +12");
  Serial.println("      example Z -8");
  Serial.println();
}

void adjust_time() {
  int year = Serial.parseInt();
  int month = Serial.parseInt();
  int day = Serial.parseInt();
  int hour = Serial.parseInt();
  int minute = Serial.parseInt();
  int second = Serial.parseInt();
  DateTime dt(year, month, day, hour, minute, second);
  dt.toString(buffer);
  rtc.adjust(dt);
  Serial.print("Time Adjusted to: ");
  dt.toString(buffer);
  Serial.println(buffer);
}

void set_timezone() {
  time_zone = Serial.parseInt();
  if (time_zone < -12 || time_zone > 12) {
    time_zone = 0;
    Serial.println("Entered time_zone is not valid!");
  }
  else {
    Serial.print("Display adjusted to Time Zone offset = ");
    Serial.println(time_zone);
  }
}

void loop() {
  String str;
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    // str = Serial.readStringUntil('\n');
    char cmd = Serial.read();
    switch (cmd) {
      case 'T':
        adjust_time();
        break;
      case 'Z':
        set_timezone();
        break;
      default:
        Serial.readBytesUntil(cmd, buffer, 64);
        Serial.println("Command not recognized!");
    }
  }

  delay(1000);
  DateTime utc = rtc.now();
  DateTime local = utc.getLocalTime(time_zone);
  local.toString(buffer);
  Serial.print("Current unix time = ");
  Serial.print(local.unixtime());
  Serial.print(" --> Local time = ");
  Serial.println(buffer);
}
