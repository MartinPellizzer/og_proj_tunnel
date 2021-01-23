void RTC_Handler()
{
  DateTime now = rtc.now();

  rtc_year_current = now.year();
  rtc_month_current = now.month();
  rtc_day_current = now.day();
  rtc_hour_current = now.hour();
  rtc_minute_current = now.minute();
  rtc_second_current = now.second();
}
