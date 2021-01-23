void SD_Handler()
{
  if (SD_inizialized)
  {
    if (rtc_minute_old != rtc_minute_current)
    {
      rtc_minute_old = rtc_minute_current;

      myFile = SD.open("log.txt", FILE_WRITE);
      if (myFile)
      {
      digitalWrite(26, !digitalRead(26));
        myFile.print(rtc_year_current, DEC);
        myFile.print(',');
        myFile.print(rtc_month_current, DEC);
        myFile.print(',');
        myFile.print(rtc_day_current, DEC);
        myFile.print(',');
        myFile.print(rtc_hour_current, DEC);
        myFile.print(',');
        myFile.print(rtc_minute_current, DEC);
        myFile.print(',');
        myFile.print(rtc_second_current, DEC);
        myFile.print(',');
        myFile.print(s1_ppb_current, DEC);
        myFile.print(',');
        myFile.print(s2_ppb_current, DEC);
        myFile.print(',');
        myFile.print(s3_ppb_current, DEC);
        myFile.print(',');
        myFile.println();
        myFile.close();
      }
      else
      {
        //Serial.println("error opening test.txt");
      }
    }
  }
}
