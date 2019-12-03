// OpenLST
// Copyright (C) 2018 Planet Labs Inc.
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef _BOARD_H
#define _BOARD_H

// We use a 27MHz clock
#define F_CLK 27000000

#define CUSTOM_BOARD_INIT 1
#define BOARD_HAS_TX_HOOK 1
#define BOARD_HAS_RX_HOOK 1
#define CONFIG_CAPABLE_RF_RX 1
#define CONFIG_CAPABLE_RF_TX 1

// Enable the power supply sense lines AN0 and AN1
#define ADCCFG_CONFIG 0b00000011

#define RADIO_RANGING_RESPONDER 1

void board_init(void);

#define BOARD_HAS_LED 1
void board_led_set(__bit led_on);

// These are macros to save space in the bootloader
// Enable bias to on-board 1W RF power amp (RF6504)
#define board_pre_tx() P2_0 = 1;
// Disable on-board power amp bias, to save power
#define board_pre_rx() P2_0 = 0;

// ############ Custom Implementation #############

// configuration flags
#define FORWARD_MESSAGES_RF 1
#define CUSTOM_COMMANDS 1

// functions
uint8_t custom_commands(const __xdata command_t *cmd, uint8_t len, __xdata command_t *reply);
uint8_t send_next_window();

// custom parameters
#define ROLE_GND 0
#define ROLE_SAT 1
#define WS 4 // window size
#define SAMPLE_DATA_LEN 64

// comand structs
typedef struct hsat_status_ack {
  uint8_t num_lost;
  uint16_t lost_packets[120]; // ~ data size / 2
} hsat_status_ack_t;

typedef struct hsat_status_header {
  uint16_t seqnum_start;
  uint16_t sequnum_finish;
  uint8_t window_size;
  uint16_t len;
} hsat_status_header_t;

//#define PAYLOAD_SIZE ESP_MAX_PAYLOAD - sizeof(command_header_t) - sizeof(hsat_status_header_t)
#define PAYLOAD_SIZE 2 // nice and easy test ;)

typedef struct hsat_status {
  hsat_status_header_t header;
  uint8_t payload[PAYLOAD_SIZE];
} hsat_status_t;
  
typedef enum {
  hsat_status_ack_cmd = 0x7F,
  hsat_dump_status_cmd = 0x80,
  hsat_status_cmd = 0x81,
} hsat_msg_type;

// application variables
uint8_t role; // satellite (1) or groundstation (0)?
uint16_t last_sent;
__xdata uint16_t data_len = SAMPLE_DATA_LEN;
__xdata uint8_t data_q[SAMPLE_DATA_LEN];
static __xdata hsat_status_t status; // not positive about status here

#endif
