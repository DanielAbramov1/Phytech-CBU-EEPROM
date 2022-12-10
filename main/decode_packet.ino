
void decode_packet(String packet, int sender)
{

  // 1min for deep sleep
  activity_detected();
  char firstchar = packet[0];
  char second_char = packet[1];

  if (ALLOW_DEBUG && packet[0] != 'S') Serial.printf("DATA RECIEVED: %s from %d\n", packet, sender);

  switch (firstchar)
  {
    // reset EEPROM
    case '?':
      print_all_commands();
      break;

    // save to EEPROM
    case '-':
      clear_eeprom();
      break;

    // save to EEPROM
    case 'S':
      split_string(packet);
      break;

    // get sensor parameters from EEPROM (G0/1/2)
    case 'G':
      //      get_sensor(second_char - '0');
      break;

    // Get parameters from CBU
    case 'K':
      Mode = 'A';
      send_conUNIT_parameters();
      break;

    // Reset CBU system
    case 'L':
      Mode = 'A';
      send_conUNIT_reset();
      break;

    // Send ping to CBU
    case 'N':
      Mode = 'A';
      send_conUNIT_ping_to_conUNIT();
      break;

    // Open\close latch relay
    case 'O':
      Mode = 'A';
      packet.remove(0, 1); //remove 1 character starting at index 0
      pump_func(packet);
      break;

    // Get latch relay state
    case 'P':
      Mode = 'A';
      get_pump();
      break;

    // Open\close relay
    case 'Q':
      Mode = 'A';
      packet.remove(0, 1); //remove 1 character starting at index 0
      valve_func(packet);
      break;

    // Get relay state
    case 'R':
      Mode = 'A';
      get_valve();
      break;

    // CBU shows the PWM raw value (probably frequency)
    case 'V':
      Mode = 'A';
      send_conUNIT_pulse_raw_value();
      break;

    // Received from the CBU, when was the last activity recorded
    case 'W':
      Mode = 'A';
      send_conUNIT_pulse_last_activity();
      break;

    // 4-20mA settings read from CBU unit
    case 'X':
      Mode = 'A';
      send_conUNIT_pulse_analog_raw_value();
      break;

    case 'Y':
      Mode = 'R';
      break;

    case 'Z':
      Mode = 'A';
      break;

    // App send ping every 1s
    case '+':
      activity_detected();
      break;
  }
}

// Example: 'S0,after,10,0,PSI,10,5,2'
const String* split_string(String packet)
{
  String strs[20];
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

  if (ALLOW_DEBUG)
  {
    // Show the resulting substrings
    for (int i = 0; i < StringCount; i++)
    {
      Serial.print(i);
      Serial.print(": \"");
      Serial.print(strs[i]);
      Serial.println("\"");
    }
    Serial.println();
  }

  // String -> char array

  //         Index  , name   ,            port,   units,           range,  threshold_high, threshold_low
  set_sensor(strs[0], strs[1], strs[2].toInt(), strs[4], strs[5].toInt(), strs[6].toInt(), strs[7].toInt());
}
