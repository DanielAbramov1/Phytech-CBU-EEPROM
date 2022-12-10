#define sleep_timer 60000 // 1 min

unsigned long loop_time;
unsigned long sleep_time;


void blynk_LED(int timer)
{
  if (millis() - loop_time > timer)
  {
    digitalWrite(LED1, !digitalRead(LED1));
    loop_time = millis();
  }
}

void activity_detected()
{
  sleep_time = millis();
}

void deep_sleep_timer()
{
  if (millis() - sleep_time > sleep_timer)
  {
    Mode = 'S';
    sleep_time = millis();
  }
}

void deep_sleep()
{
  if (ALLOW_DEBUG) Serial.println("Deep sleep activate");
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_36, 0); // wakeup when button state is low
  esp_deep_sleep_start();
}
