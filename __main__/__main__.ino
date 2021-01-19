#include <EEPROM.h>

#include <SPI.h>
#include <SD.h>
File myFile;

#include "RTClib.h"
RTC_DS3231 rtc;

uint8_t is_on_temp = 0;
uint8_t is_on_current = 0;
uint8_t is_on_old = 0;

uint8_t o3_gen_cycle_state_current = 0;
uint8_t o3_gen_cycle_state_old = 0;
uint8_t o3_gen_cycle_direction_current = 0;
uint8_t o3_gen_cycle_direction_old = 0;

unsigned long o3_gen_cycle_current_millis = 0;


int s1_ppb_current = 0;
int s1_ppb_old = 0;
int s1_min_current = 0;
int s1_min_temp = 0;
int s1_min_old = 0;
int s1_max_current = 500;
int s1_max_temp = 500;
int s1_max_old = 500;
unsigned long s1_time_current = 1200000;
unsigned long s1_time_temp = 1200000;
unsigned long s1_time_old = 1200000;
long s1_time_countdown = 1200000;
uint8_t s1_color_current = 0;
uint8_t s1_color_old = 0;

int s2_ppb_current = 0;
int s2_ppb_old = 0;
int s2_min_current = 4500;
int s2_min_temp = 4500;
int s2_min_old = 4500;
int s2_max_current = 5000;
int s2_max_temp = 5000;
int s2_max_old = 5000;
unsigned long s2_time_current = 1200000;
unsigned long s2_time_temp = 1200000;
unsigned long s2_time_old = 1200000;
long s2_time_countdown = 1200000;
uint8_t s2_color_current = 0;
uint8_t s2_color_old = 0;

int s3_ppb_current = 0;
int s3_ppb_old = 0;
int s3_min_current = 0;
int s3_min_temp = 0;
int s3_min_old = 0;
int s3_max_current = 500;
int s3_max_temp = 500;
int s3_max_old = 500;
unsigned long s3_time_current = 1200000;
unsigned long s3_time_temp = 1200000;
unsigned long s3_time_old = 1200000;
long s3_time_countdown = 1200000;
uint8_t s3_color_current = 0;
uint8_t s3_color_old = 0;

uint8_t page_current = 0;
uint8_t page_old = 0;

uint8_t alarm_current = 0;
uint8_t alarm_old = 0;

uint8_t start_countdown = 0;


/* dummy stuff */
unsigned long second_current_millis = 0;
uint8_t test_counter_current = 20;
uint8_t test_counter_old = 20;
uint8_t dir = 1;

#define RELAY_PIN 25

const unsigned long O3_CYCLE_WORKING_TIMER_MILLIS = 2700000;
const unsigned long O3_CYCLE_RESTING_TIMER_MILLIS = 900000;


uint8_t s1_settings_current = 1;
uint8_t s1_settings_old = 1;
uint8_t s2_settings_current = 1;
uint8_t s2_settings_old = 1;
uint8_t s3_settings_current = 1;
uint8_t s3_settings_old = 1;

bool RTC_inizialized = false;
bool SD_inizialized = false;

/* rtc */

int rtc_year_current = 0;
int rtc_year_old = 0;
int rtc_month_current = 0;
int rtc_month_old = 0;
int rtc_day_current = 0;
int rtc_day_old = 0;
int rtc_hour_current = 0;
int rtc_hour_old = 0;
int rtc_minute_current = 0;
int rtc_minute_old = 0;
int rtc_second_current = 0;
int rtc_second_old = 0;

void setup() {
  digitalWrite(25, LOW);
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  //pinMode(8, OUTPUT);
  //digitalWrite(8, LOW);

  delay(1000);
  Serial.begin(9600);
  delay(1000);
  Serial1.begin(9600);
  delay(1000);
  Serial2.begin(9600);
  delay(1000);
  Serial3.begin(9600);
  delay(1000);

  /*
    if (EEPROM.read(0) != 123)
      for (int i = 0; i < 256; i++) EEPROM.update(i, 0);
  */
  manageEEPROM();
  delay(1000);

  if (SD.begin(4))
    SD_inizialized = true;
  else {
    SD_inizialized = false;
    //Serial.println("initialization done.");
    digitalWrite(26, HIGH);
    delay(100);
    digitalWrite(26, LOW);
    delay(100);
    digitalWrite(26, HIGH);
    delay(100);
    digitalWrite(26, LOW);
    delay(100);
    digitalWrite(26, HIGH);
    delay(100);
    digitalWrite(26, LOW);
    delay(100);
  }

  if (rtc.begin())
    RTC_inizialized = true;
  else
    RTC_inizialized = false;

  if (rtc.lostPower()) {
    //Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  page_current = 1;
}

void loop() {
  RTC_Handler();
  SD_Handler();
  
  updateSensorsVal();
  checkSensorsAlarm();
  checkMainSensor();

  listenNextion();
  updateNextion();

  manageOzoneCycle();
}

void startOzoneIfNotAlarm()
{
  if (is_on_temp != is_on_current)
  {
    if (!alarm_current)
    {
      is_on_current = is_on_temp;
      o3_gen_cycle_direction_current = 1;
      o3_gen_cycle_state_current = 1;
      o3_gen_cycle_current_millis = millis();
      s2_time_countdown = s2_time_current;
      start_countdown = 0;
      /*dir = 1; /* ----- To remove ----- */
    }
    else
    {
      is_on_temp = is_on_current;
    }
  }
}

void stopOzoneIfAlarm()
{
  if (alarm_old != alarm_current)
  {
    alarm_old = alarm_current;
    if (alarm_current)
    {
      is_on_current = is_on_temp = 0;
      s2_time_countdown = s2_time_current;
    }
  }
}

void manageOzoneCycle()
{
  stopOzoneIfAlarm();
  startOzoneIfNotAlarm();

  if (is_on_current)
  {
    if ((millis() - second_current_millis) > 1000)
    {
      second_current_millis = millis();
      if (start_countdown)
      {
        if (s2_time_countdown - 1000 > 0)
          s2_time_countdown -= 1000;
        else
          is_on_temp = 0;
      }
    }
    if (o3_gen_cycle_state_current)
    {
      if ((millis() - o3_gen_cycle_current_millis) > O3_CYCLE_WORKING_TIMER_MILLIS)
      {
        o3_gen_cycle_current_millis = millis();
        o3_gen_cycle_state_current = !o3_gen_cycle_state_current;
      }
    }
    else
    {
      if ((millis() - o3_gen_cycle_current_millis) > O3_CYCLE_RESTING_TIMER_MILLIS)
      {
        o3_gen_cycle_current_millis = millis();
        o3_gen_cycle_state_current = !o3_gen_cycle_state_current;
      }
    }
    if (o3_gen_cycle_direction_current)
      digitalWrite(RELAY_PIN, o3_gen_cycle_state_current);
    else
      digitalWrite(RELAY_PIN, LOW);
    s1_settings_current = 0;
    s2_settings_current = 0;
    s3_settings_current = 0;
  }
  else
  {
    digitalWrite(RELAY_PIN, LOW);
    o3_gen_cycle_state_old = o3_gen_cycle_state_current = 0;
    s1_settings_current = 1;
    s2_settings_current = 1;
    s3_settings_current = 1;
  }
}
