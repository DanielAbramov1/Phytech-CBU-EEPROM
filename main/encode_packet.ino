unsigned long loop_time2;
bool should_calc_frequency = false;
String APN;

//convert volt to bar scale for pressure sensor
float convert4_20mA(float x)
{
  if (x < 0.41) x = 0.4;
  return x * 6.25 - 2.5;
}

// ************************
// ***** Control unit *****
// ************************

// ?
void print_all_commands()
{
  if (ALLOW_DEBUG)
  {
    Serial.println("-                         : reset EEPROM");
    Serial.println("K                         : Get system parameters from CBU");
    Serial.println("L                         : Reset CBU system");
    Serial.println("N                         : Send ping to CBU ");
    Serial.println("O0/1                      : Open/close pump");
    Serial.println("P                         : Get pump state");
    Serial.println("Q0/1                      : Open/close valve");
    Serial.println("R                         : Get valve");
    Serial.println("V                         : CBU shows the PWM raw value ");
    Serial.println("W                         : Last activity recorded");
    Serial.println("X                         : 4-20mA settings read from CBU unit");
    Serial.println("Y                         : Read sensors (Real-time) - BLE App");
    Serial.println("Z                         : Exit from real-time mode");
    Serial.println("+                         : App send ping every 1s");
    Serial.println("G0                        : read sensor0 parameters from EEPROM");
    Serial.println("G1                        : read sensor1 parameters from EEPROM");
    Serial.println("G2                        : read sensor2 parameters from EEPROM");
    Serial.println("S0,after,10,PSI,10,5,2    : Save sensor0 parameters in EEPROM");
    Serial.println("S1,before,10,PSI,10,5,2   : Save sensor1 parameters in EEPROM");
    Serial.println("S2,pond,10,PSI,10,5,2     : Save sensor2 parameters in EEPROM");
  }
}

// K
void send_conUNIT_parameters() {
  String Index = "k", HW_version = "02", SW_version = "03", Battery_is_charging = "04";
  String MAC_address = WiFi.macAddress();

  String buf;
  buf += Index, buf += ", ", buf += MAC_address, buf += ", ", buf += HW_version, buf += ", ", buf += SW_version, buf += ", ", buf += Battery_is_charging, buf += ", ";

  // Send ACK via RS485 and application
  send_RS485_01(buf);
  if (ALLOW_BT) update_BLE_data(buf, 0);
}

// L
void send_conUNIT_reset() {
  String Index = "l", STATUS = "OK";

  String buf;
  buf += Index, buf += ", ", buf += STATUS;

  // Send ACK via RS485 and application
  send_RS485_01(buf);
  if (ALLOW_BT) update_BLE_data(buf, 0);

  ESP.restart();
}

//N
void send_conUNIT_ping_to_conUNIT() {
  String Index = "n", ping_from_comUNIT = "OK";

  String buf;
  buf += Index, buf += ", ", buf += ping_from_comUNIT;

  // Send ACK via RS485 and application
  send_RS485_01(buf);
  if (ALLOW_BT) update_BLE_data(buf, 0);
}

// O0/1
void pump_func(String packet) {

  if (packet.toInt() == 1)
  {
    digitalWrite(L_Relay_S_1, HIGH);
    delay(10);
    digitalWrite(L_Relay_S_1, LOW);
    pump_relay_state = "ON";
  }
  else
  {
    digitalWrite(L_Relay_R_1, HIGH);
    delay(10);
    digitalWrite(L_Relay_R_1, LOW);
    pump_relay_state = "OFF";
  }

  String Index = "o";

  String buf;
  buf += Index, buf += ", ", buf += pump_relay_state;

  // Send ACK via RS485 and application
  send_RS485_01(buf);
  if (ALLOW_BT) update_BLE_data(buf, 0);
}

// P
void get_pump() {
  String Index = "p";

  String buf;
  buf += Index, buf += ", ", buf += pump_relay_state;

  // Send ACK via RS485 and application
  send_RS485_01(buf);
  if (ALLOW_BT) update_BLE_data(buf, 0);
}

// Q0/1
void valve_func(String packet) {

  if (packet.toInt() == 1)
  {
    digitalWrite(L_Relay_S_2, HIGH);
    delay(10);
    digitalWrite(L_Relay_S_2, LOW);
    valve_relay_state = "ON";
  }
  else
  {
    digitalWrite(L_Relay_R_2, HIGH);
    delay(10);
    digitalWrite(L_Relay_R_2, LOW);
    valve_relay_state = "OFF";
  }

  String Index = "q";

  String buf;
  buf += Index, buf += ", ", buf += valve_relay_state;

  // Send ACK via RS485 and application
  send_RS485_01(buf);
  if (ALLOW_BT) update_BLE_data(buf, 0);

}

// R
void get_valve() {

  String Index = "r";

  String buf;
  buf += Index, buf += ", ", buf += valve_relay_state;

  // Send ACK via RS485 and application
  send_RS485_01(buf);
  if (ALLOW_BT) update_BLE_data(buf, 0);

}

// U
void send_conUNIT_get_pump() {
  String Index = "u";

  String buf;
  buf += Index, buf += ", ", buf += pump_relay_state;

  // Send ACK via RS485 and application
  send_RS485_01(buf);
  if (ALLOW_BT) update_BLE_data(buf, 0);
}

// V
void send_conUNIT_pulse_raw_value() {
  String Index = "v", frequency = "01";

  String buf;
  buf += Index, buf += ", ", buf += frequency;

  // Send ACK via RS485 and application
  send_RS485_01(buf);
  if (ALLOW_BT) update_BLE_data(buf, 0);

}

// W
void send_conUNIT_pulse_last_activity() {
  String Index = "w", last_activity = "21/12/21";

  String buf;
  buf += Index, buf += ", ", buf += last_activity;

  // Send ACK via RS485 and application
  send_RS485_01(buf);
  if (ALLOW_BT) update_BLE_data(buf, 0);
}

// X
void send_conUNIT_pulse_analog_raw_value() {
  String Index = "x", raw_value = "01";

  String buf;
  buf += Index, buf += ", ", buf += raw_value;

  // Send ACK via RS485 and application
  send_RS485_01(buf);
  if (ALLOW_BT) update_BLE_data(buf, 0);
}

// realtime ADC and flow sens
void send_comUNIT_RealTime() {

  if (millis() - loop_time2 > 1000)
  {
    String index = "y";
    int16_t adc0, adc1, adc2, adc3;
    float volts0, volts1, volts2, volts3, flow_sens;

    // ADS1015
    adc0 = ads.readADC_SingleEnded(0);
    adc1 = ads.readADC_SingleEnded(1);
    adc2 = ads.readADC_SingleEnded(2);
    adc3 = ads.readADC_SingleEnded(3);

    volts0 = ads.computeVolts(adc0);
    volts1 = ads.computeVolts(adc1);
    volts2 = ads.computeVolts(adc2);
    volts3 = ads.computeVolts(adc3);

    // freq measure for flow sensor
    if (should_calc_frequency) flow_sens = calc_freq();

    //    SerialBT.printf("%s, %.3f, %.3f, %.3f, %.3f, %.3f\n", index, convert4_20mA(volts0), convert4_20mA(volts1), volts2, volts3, flow_sens);
    if (ALLOW_DEBUG) Serial.printf("%s, %.3f, %.3f, %.3f, %.3f, %.3f\n", index, volts0, volts1, volts2, volts3, flow_sens);

    String buf;
    //    buf += index, buf += ", ", buf += String(convert4_20mA(volts0)), buf += ", ", buf += String(convert4_20mA(volts1)), buf += ", ", buf += String(volts2), buf += ", ", buf += String(volts3), buf += ", ", buf += String(flow_sens);
    buf += index, buf += ", ", buf += String(volts0), buf += ", ", buf += String(volts1), buf += ", ", buf += String(volts2), buf += ", ", buf += String(volts3), buf += ", ", buf += String(flow_sens);
    if (ALLOW_BT) update_BLE_data(buf, 0);

    loop_time2 = millis();
  }
}
