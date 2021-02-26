void CycleHandler()
{
  //MonitorCycleInput();

  if (IsCycleInputChanged())
    StartStopCycle();

  StopCycleIfAlarm();

  ManageOzoneCycle();
}



/* ----------------------------------------------------------------------------------------------------------- */
/* -------------------------------------------------- INPUT -------------------------------------------------- */
/* ----------------------------------------------------------------------------------------------------------- */
void MonitorCycleInput()
{
  if (!digitalRead(PIN_IN_START_CYCLE)) is_cycle_input_on = true;
  else is_cycle_input_on = false;
}

bool IsCycleInputChanged()
{
  if (is_cycle_input_on_prev != is_cycle_input_on)
  {
    is_cycle_input_on_prev = is_cycle_input_on;
    return true;
  }
  return false;
}

void StartStopCycle()
{
  if (is_cycle_input_on) StartCycleIfNotAlarm();
  else StopCycle();
}



/* ----------------------------------------------------------------------------------------------------------- */
/* -------------------------------------------------- START -------------------------------------------------- */
/* ----------------------------------------------------------------------------------------------------------- */
void StartCycleIfNotAlarm()
{
  if (!alarm_current)
  {
    is_on_current = 1;
    o3_gen_cycle_direction_current = 1;
    o3_gen_cycle_state_current = 1;
    o3_gen_cycle_current_millis = millis();
    s2_time_countdown = s2_time_current;
    start_countdown = 0;
    digitalWrite(PIN_OUT_END_CYCLE, LOW);
  }
}



/* ----------------------------------------------------------------------------------------------------------- */
/* -------------------------------------------------- STOP --------------------------------------------------- */
/* ----------------------------------------------------------------------------------------------------------- */
void StopCycleIfAlarm()
{
  if (IsAlarmStateChanged())
    if (alarm_current)
      StopCycle();
}

bool IsAlarmStateChanged()
{
  if (alarm_old != alarm_current)
  {
    alarm_old = alarm_current;
    return true;
  }
  return false;
}

void StopCycle()
{
  is_on_current = 0;
  s2_time_countdown = s2_time_current;

  digitalWrite(PIN_OUT_GENO3, LOW);
  o3_gen_cycle_state_old = o3_gen_cycle_state_current = 0;
  s1_settings_current = 1;
  s2_settings_current = 1;
  s3_settings_current = 1;
}



/* ----------------------------------------------------------------------------------------------------------- */
/* -------------------------------------------------- CYCLE -------------------------------------------------- */
/* ----------------------------------------------------------------------------------------------------------- */
void ManageOzoneCycle()
{
  if (is_on_current)
  {
    DecrementCountdown();

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
    if (o3_gen_cycle_direction_current) digitalWrite(PIN_OUT_GENO3, o3_gen_cycle_state_current);
    else digitalWrite(PIN_OUT_GENO3, LOW);
    s1_settings_current = 0;
    s2_settings_current = 0;
    s3_settings_current = 0;
  }
  else StopCycle();
}

void DecrementCountdown()
{
  if ((millis() - second_current_millis) > 1000)
  {
    second_current_millis = millis();

    if (s2_time_countdown - 1000 > 0) s2_time_countdown -= 1000;
    else CheckCountdown0();

    Serial.println(s2_time_countdown);
  }
}

void CheckCountdown0()
{
  digitalWrite(PIN_OUT_END_CYCLE, HIGH);
  StopCycle();
}
