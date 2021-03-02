void DataLoggerHandler()
{
  byte val = map(s2_ppb_current, 0, 10000, 0, 255);
  analogWrite(PIN_OUT_DATA, val);
}
