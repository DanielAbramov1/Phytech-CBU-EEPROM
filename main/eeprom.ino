#include <EEPROM.h>

#define EEPROM_SIZE 512



void setup_eeprom()
{
  EEPROM.begin(EEPROM_SIZE);
}

void clear_eeprom()
{
  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    EEPROM.write(i, 255);
  }
  if (ALLOW_DEBUG) Serial.println("EEPROM has been erased successfully");
}

void set_sensor(String index, String sensor_name, int port, String units, int range, int threshold_high, int threshold_low)
{
  //update whatever needed

  sensor_metadata_t tmp_array[] = {sensors[0].sensor_metadata, sensors[1].sensor_metadata, sensors[2].sensor_metadata};
  EEPROM.put(0, tmp_array);


  //  index.remove(0, 1); //remove 1 character starting at index 0
  //  int sensor_number = index.toInt();
  //
  //
  //
  //  if (ALLOW_DEBUG)
  //  {
  //    Serial.println("PARAMETERS TO SAVE");
  //    Serial.printf("index: %d\n", sensor_number);
  //    Serial.printf("sensor_name: %s\n", sensor_name);
  //    Serial.printf("port: %d\n", port);
  //    Serial.printf("functionality: %d\n", functionality);
  //    Serial.printf("units: %s\n", units);
  //    Serial.printf("range: %d\n", range);
  //    Serial.printf("threshold_high: %d\n", threshold_high);
  //    Serial.printf("threshold_low: %d\n", threshold_low);
  //  }
  //
  //  //Data to store.
  //  sensors[sensor_number] =
  //  {
  //    sensor_name,       // Sensor Name (after, before, pond)
  //    port,              // (10, 11, 12)
  //    units,             // PSI
  //    range,             //  0 - 10
  //    threshold_high,    // 5
  //    threshold_low      // 2
  //  };
  //
  //  if (sensor_number == 0) EEPROM.put(0, sensors[0]);
  //  else if (sensor_number == 1) EEPROM.put(44, sensors[1]);
  //  else if (sensor_number == 2) EEPROM.put(88, sensors[2]);
  //  EEPROM.commit();
  //
  //  if (ALLOW_DEBUG) Serial.printf("EEPROM has been saved successfully, size: %d\n", sizeof(sensors));
}

void get_sensor()
{
  sensor_metadata_t tmp_array[] = {sensors[0].sensor_metadata, sensors[1].sensor_metadata, sensors[2].sensor_metadata};
  EEPROM.get(0, tmp_array);
  //  EEPROM_SESNOR sensors[3];
  //
  //  if (sensor_number == 0) EEPROM.get(0, sensors[0]);
  //  else if (sensor_number == 1) EEPROM.get(44, sensors[1]);
  //  else if (sensor_number == 2) EEPROM.get(88, sensors[2]);
  //
  //  if (ALLOW_DEBUG)
  //  {
  //    Serial.printf("sensor_name: %s\n", sensors[sensor_number].sensor_name);
  //    Serial.printf("port: %d\n", sensors[sensor_number].port);
  //    Serial.printf("units: %s\n", sensors[sensor_number].units);
  //    Serial.printf("range: %d\n", sensors[sensor_number].range);
  //    Serial.printf("threshold_high: %d\n", sensors[sensor_number].threshold_high);
  //    Serial.printf("threshold_low: %d\n", sensors[sensor_number].threshold_low);
  //  }
}
