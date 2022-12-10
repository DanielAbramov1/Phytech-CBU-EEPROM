// 'senderID', 'payload_size', 'CRC_LSB', 'CRC_MSB', 'payload[0]', 'payload[1]', 'payload[2]'...
void crc_validation(char *buf, int rlen)
{
  // senderID
  uint8_t senderID = buf[0];
  if (ALLOW_DEBUG) Serial.printf("senderID : %d\n", buf[0]);

  // payload size
  unsigned int payload_size = buf[1];
  if (ALLOW_DEBUG) Serial.printf("payload size: %d\n", buf[1]);

  // CRC LSB
  uint8_t CRC_LSB = buf[2];
  if (ALLOW_DEBUG) Serial.printf("CRC LSB: %d\n", CRC_LSB);

  // CRC MSB
  uint8_t CRC_MSB = buf[3];
  if (ALLOW_DEBUG) Serial.printf("CRC MSB: %d\n", CRC_MSB);

  // payload
  byte payload[100] = {0};
  for (int i = 0; i < payload_size; i++)
  {
    payload[i] = buf[i + 4];
  }

  String payload_for_print = "";
  for (int i = 0; i < payload_size; i++)
  {
    payload_for_print += String(buf[i + 4]);
  }
  if (ALLOW_DEBUG) Serial.printf("payload: %s\n", payload_for_print);

  uint16_t checksum = CRC16_CCITT(payload, payload_size, 0xFFFF);
  if (ALLOW_DEBUG) Serial.printf("calculated checksum: %d\n", checksum);

  uint16_t msb = CRC_MSB << 8;
  uint16_t sender_checksum = (msb | CRC_LSB);
  if (ALLOW_DEBUG) Serial.printf("sender checksum: %d\n", sender_checksum);

  char payload_for_decode[100] = {0};
  for (int i = 0; i < payload_size; i++)
  {
    payload_for_decode[i] = buf[i + 4];
  }
  if (ALLOW_DEBUG) Serial.printf("payload: %s\n", payload);

  // CRC validation
  if (checksum == sender_checksum)
  {
    decode_packet(payload_for_decode, 2);
  }
}

// calculates CRC code of the specified buffer
unsigned short CRC16_CCITT(const unsigned char *data, unsigned short length, unsigned short crc)
{
  unsigned char b;

  while (length > 0)
  {
    b = *data;
    crc = (crc >> 8) | (crc << 8);
    crc ^= b;
    crc ^= (crc & 0xff) >> 4;
    crc ^= crc << 12;
    crc ^= (crc & 0xff) << 5;

    ++data;
    --length;
  }

  return crc;
}
