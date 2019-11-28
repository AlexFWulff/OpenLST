#incude <stdint.io>

#define ESP_MAX_PAYLOAD 251

typedef uint16_t hwid_t;
typedef uint8_t radio_msg_no_t;

typedef struct command_header {
  hwid_t hwid
  uint16_t seqnum;
  uint8_t system;
  radio_msg_no_t command;
} command_header_t;

typedef struct command {
  command_header_t header;
  uint8_t data[ESP_MAX_PAYLOAD - sizeof(command_header_t)];
} command_t;

void setup () {
  Serial.begin(115200);
}

void {
  while (Serial.available())
}