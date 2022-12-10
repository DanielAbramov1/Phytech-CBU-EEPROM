#include <SoftwareSerial.h>
#define RS485transmit HIGH
#define RS485receive LOW
#define rs485_dir1 5

void rs485_config()
{
  // Define pin modes for TX and RX
  pinMode(18, INPUT);
  pinMode(19, OUTPUT);

  Serial1.begin(19200, SERIAL_8N1, 18, 19);  //RX, TX
  digitalWrite(rs485_dir1, RS485receive);
}

void send_RS485_01(String message)
{
  // message
  byte PACKET[100] = {0};

  digitalWrite(rs485_dir1, RS485transmit);
  delayMicroseconds(100);

  unsigned int message_length = message.length();
  if (ALLOW_DEBUG)
  {
    Serial.print("RS485_send: ");
    Serial.print(message);
    Serial.printf(", length: %d\n", message_length);
  }

  byte payload[message_length];
  message.getBytes(payload, message_length + 1);

  // sender ID - 1 byte
  uint8_t sender_ID = 0xF0; // DEC 240
  //  Serial.print("senderID: ");
  //  Serial.println(sender_ID, HEX);
  PACKET[0] = sender_ID;

  // sizeof - 1 byte
  //  Serial.printf("sizeof %d\n", sizeof payload);
  PACKET[1] = sizeof payload;

  // checksum - 2 bytes
  uint16_t checksum = CRC16_CCITT(payload, message_length, 0xFFFF);
  //  Serial.printf("payload checksum: %u\n", checksum);

  PACKET[3] = (checksum >> 8) & 0xFF;
  PACKET[2] = (checksum) & 0xFF;

  //  Serial.printf("1: %u\n", PACKET[2]);
  //  Serial.printf("2: %u\n", PACKET[3]);

  // payload
  //  Serial.printf("payload: %s\n", message);
  for (int i = 0; i < message_length; i++)
  {
    PACKET[i + 4] = payload[i];
  }

  //  Serial.printf("%d, %d, %u, %u", sender_ID, sizeof payload, PACKET[2], PACKET[3]);

  //  for (int i = 0; i < message.length() + 4; i++) {
  //    Serial.print(PACKET[i]);
  //    Serial.print(",");
  //  }
  //  Serial.println('\n');

  // Msg ['SenderID' 'size' 'payload checksum' 'payload' '\n']
  Serial1.write(PACKET, message.length() + 4);
  Serial1.write('\n');
  Serial1.flush();

  digitalWrite(rs485_dir1, RS485receive);
}

//packets received via BT are sender = 0, via Serial = 1, com unit = 2
void listener()
{
  /*
    if (SerialBT.available())
    {
    String str_from_serial = SerialBT.readStringUntil('\n');
    decode_packet(str_from_serial, 0);
    }
  */

  if (Serial.available())
  {
    String str_from_serial = Serial.readStringUntil('\n');
    decode_packet(str_from_serial, 1);
  }

  if (Serial1.available() > 0)
  {
    char buf[100] = {0};

    // read the incoming bytes:
    int rlen = Serial1.readBytesUntil('\n', buf, 100);
    print_packet_received(buf, rlen);

    // CRC validation
    crc_validation(buf, rlen);
  }
}

void print_packet_received(char *buf, int rlen)
{
  if (ALLOW_DEBUG)
  {
    Serial.println("==");
    Serial.printf("packet length received: %d\n", rlen);
    Serial.printf("I received: ");
  }

  for (int i = 0; i < rlen; i++)
  {
    Serial.print(buf[i], DEC);
    Serial.print(", ");
  }
  Serial.println();
}
