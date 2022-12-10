#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_SIZE 512 //bytes

typedef struct            
{
  char sensor_name[20];                 // Sensor Name (after/before/pond)
  int16_t port;                         // (10, 11, 12)
  float conversion;
  int16_t range;                        //  0 - 10
  int16_t threshold_high;               // 5
  int16_t threshold_low;                // 2

} sensor_metadata_t;

typedef struct
{
  sensor_metadata_t sensor_metadata;
  int16_t raw_reading_mV;
  float reading_value;
  
} mySensor_t;


mySensor_t sensors[3]; // 0 -after , 1-before , 2 - pond

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
  Serial.println("EEPROM was cleared!");
}


// New Addition
//-----------------------------------------

char* sensor_name_array (String sensor_name) 
{
  char* buff = (char*)malloc(20);
  memset(buff,0,20); // init malloc
  
  sensor_name.toCharArray(buff ,sensor_name.length() + 1);
  Serial.print("The name is : ");
  Serial.println(buff);

  return buff;
}

void array_to_struct (char * p, mySensor_t sensor[], int sensor_num)
{
  memcpy(sensor[sensor_num].sensor_metadata.sensor_name, p, strlen(p));
  Serial.println("The updated sensor name is struct: " + String(sensor[sensor_num].sensor_metadata.sensor_name));
  free(p);
}

void set_sensor(String name, int16_t port, float conversion, int16_t range, int16_t threshold_high, int16_t threshold_low)
{
  int sensor_to_update = -1; // init the index -> 0 -after , 1-before , 2 - pond
  
  if(name == "after")
  {
    sensor_to_update = 0;
  }
  else if(name == "before")
  {
    sensor_to_update = 1;
  }
  else if(name == "pond")
  {
    sensor_to_update = 2;
  }
  else
  {
    Serial.println("No such sensor. Update failed!");
    return;
  }

  array_to_struct(sensor_name_array(name),sensors,sensor_to_update);
  sensors[sensor_to_update].sensor_metadata.port = port;
  sensors[sensor_to_update].sensor_metadata.conversion = conversion;
  sensors[sensor_to_update].sensor_metadata.range = range;
  sensors[sensor_to_update].sensor_metadata.threshold_high = threshold_high;
  sensors[sensor_to_update].sensor_metadata.threshold_low = threshold_low;

  sensor_metadata_t tmp_array[] = {sensors[0].sensor_metadata, sensors[1].sensor_metadata, sensors[2].sensor_metadata};
  EEPROM.put(0, tmp_array);
  EEPROM.commit();
}

void get_sensor()
{
  sensor_metadata_t tmp_array[] = {sensors[0].sensor_metadata, sensors[1].sensor_metadata, sensors[2].sensor_metadata};
  EEPROM.get(0, tmp_array);
  sensors[0].sensor_metadata = tmp_array[0];
  sensors[1].sensor_metadata = tmp_array[1];
  sensors[2].sensor_metadata = tmp_array[2];

  //Print data from EEPROM
  for(int i=0 ; i<3 ; i++)
  {
    Serial.print("Sensor data: ");
    Serial.print(sensors[i].sensor_metadata.sensor_name);
    Serial.print(",");
    Serial.print(sensors[i].sensor_metadata.port);
    Serial.print(",");
    Serial.print(sensors[i].sensor_metadata.conversion);
    Serial.print(",");
    Serial.print(sensors[i].sensor_metadata.range);
    Serial.print(",");
    Serial.print(sensors[i].sensor_metadata.threshold_high);
    Serial.print(",");
    Serial.println(sensors[i].sensor_metadata.threshold_low);
  }
  
}

void split_string(String packet)
{
  String strs[7];
  int StringCount = 0;

  // Split the string into substrings
  while (packet.length() > 0)
  {
    int index = packet.indexOf(',');
    if (index == -1) // No comma found
    {
      strs[StringCount++] = packet;
      break;
    }
    else
    {
      strs[StringCount++] = packet.substring(0, index);
      packet = packet.substring(index + 1);
    }
  }

  // if (ALLOW_DEBUG)
  if(1)
  {
    // Show the resulting substrings
    for (int i = 0; i < StringCount; i++)
    {
      Serial.print(i);
      Serial.print(": \"");
      Serial.print(strs[i]);
      Serial.println("\"");
    }
  }

  //         name   , port           , conv             , range          , th high        , th low
  set_sensor(strs[1], strs[2].toInt(), strs[3].toFloat(), strs[4].toInt(), strs[5].toInt(), strs[6].toInt());
}

//-----------------------------------------

void setup()
{
  Serial.begin(115200);

  setup_eeprom();

  get_sensor();
}

void loop()
{
  // SerialCheck();
  if(Serial.available())
  { 
    String packet = Serial.readStringUntil('\n');
    Serial.println("Data from operator recieved : " + packet);
    switch(packet[0])
    {
        case 'S':
          split_string(packet);
          break;
          
        case 'G':
          get_sensor();
          break;

        case '-':
          clear_eeprom();
          break;


        
        // case '0':
        //   array_to_struct(sensor_name_array(arg),sensors,0);
        
        //   break;

        // case '1':
        //   array_to_struct(sensor_name_array(arg),sensors,1);
        
        // break;


    }
  }

  delay(200);
}
