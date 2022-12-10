/*
   System Modes:
      'A' - Active Mode                 (42mA)
      'R' - Real Time Monitor Mode      (mA)
      'S' - Deep Sleep Mode             (0.15mA)
*/
typedef struct               // 44 bytes
{
  //TODO must be char array
  char sensor_name[20];                 // Sensor Name (after/before/pond)
  int16_t port;                         // (10, 11, 12)
  char units[5];                        // PSI
  int16_t range;                        //  0 - 10
  int16_t threshold_high;               // 5
  int16_t threshold_low;                // 2
} sensor_metadata_t;

typedef struct
{
  sensor_metadata_t sensor_metadata;

  int16_t raw_reading;
  int16_t conversion;
  int16_t reading_value;

} mySensor_t;


mySensor_t sensors[3];

char Mode = 'A';
#define ALLOW_BT 1
#define ALLOW_DEBUG 1

void setup()
{
  Serial.begin(115200);

  setup_ble("Phytec CBU");
  rs485_config();
  IO_config();
  ads_config();
  init_frequencyMeter();
  setup_eeprom();

  get_sensor();
}

void loop()
{
  switch (Mode)
  {
    case 'A':
      listener();
      blynk_LED(100);
      //      deep_sleep_timer(); // MCU goes into sleep mode after a minute of inactivity.
      break;

    case 'R':
      listener();
      send_comUNIT_RealTime();
      blynk_LED(50);
      //      deep_sleep_timer(); // MCU goes into sleep mode after a minute of inactivity.
      break;

    case 'S':
      deep_sleep();
      break;
  }
}
