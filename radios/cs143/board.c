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

#include <cc1110.h>
#include "cc1110_regs.h"

#include "commands.h"
#include "board.h"
#include "board_defaults.h"
#include "hwid.h"

#include "stringx.h"
#include "uart1.h"

uint8_t role; // satellite (1) or groundstation (0)?                                                                                                                                        
int last_sent;
uint16_t data_len;
__xdata uint8_t data_q[SAMPLE_DATA_LEN];

__xdata uint16_t dropped_packets[WS];
uint8_t drop_count;

void board_init(void) {
	// LED0 setup - just turn it on
	P0SEL &= ~(1<<6);  // GPIO not peripheral
	P0DIR |= 1<<6;  // Output not input
	P0_6 = 1;

	// LED1 setup
	P0SEL &= ~(1<<7);  // GPIO not peripheral
	P0DIR |= 1<<7;  // Output not input

	// Power amplifier bias control on P2.0
	P2SEL &= ~(1<<0);  // GPIO not peripheral
	P2DIR |= 1<<0;  // Output not input
	P2_0 = 0;

	/* Set up radio "test" signal pins.  We're setting these up to be
	   outputs from the radio core indicating when the radio is
	   transmitting and receiving, to control off-chip, on-board and
	   off-board RF path switches.  Once set up, they are automatically
	   toggled appropriately by the radio core whenever the radio state
	   changes. */
        // P1_7 = LNA_PD (asserted high in TX) = LST_TX_MODE
        IOCFG2 = IOCFG2_GDO2_INV_ACTIVE_HIGH | IOCFG_GDO_CFG_PA_PD;
        // P1_6 = PA_PD  (asserted low in RX) = !LST_RX_MODE
        IOCFG1 = IOCFG1_GDO1_INV_ACTIVE_LOW | IOCFG_GDO_CFG_LNA_PD;
        // No special function for P1_5 (IOCFG0)

  // set sample data to 0
  data_len = SAMPLE_DATA_LEN;
  memsetx((char __xdata *) data_q, 0, SAMPLE_DATA_LEN);
}

uint8_t custom_commands(const __xdata command_t *cmd, uint8_t len, __xdata command_t *reply) {
  
  uint8_t reply_length;
  
  __xdata hsat_status_t *status_data;
  __xdata hsat_status_header_t *header;
  __xdata uint8_t *payload;

  __xdata hsat_status_ack_t *status_ack_data;
  
  
  switch (cmd->header.command) {
    // dump status data
    case hsat_dump_status_cmd:
      // just a header so no data here
      role = ROLE_SAT;
      last_sent = -1;
      drop_count = 0;

      // send first data window
      send_next_window(reply);
      
      break;

    // received status data  
    case hsat_status_cmd:
      role = ROLE_GND;
      
      status_data = (__xdata hsat_status_t *) cmd->data;
      // #TODO: this might have to be &(status_data->header) since we don't want to reference status_data again
      header = &status_data->header;
      payload = status_data->payload;

      // ... deal with data
      
      break;

    // received status acknowledgement
    case hsat_status_ack_cmd:
      status_ack_data = (__xdata hsat_status_ack_t *) cmd->data;
      drop_count = status_ack_data->num_lost;
      // #TODO: I don't think dropped_packets should be referenced here - it's already a pointer
      memcpyx((__xdata void *) &dropped_packets,
	      (__xdata void *) status_ack_data->lost_packets,
	      drop_count * sizeof(uint16_t));

      // send next window after parsing dropped packets
      send_next_window(reply);
      
      break;

    default:
      break;
  }

  // #TODO - use len here
  len; reply; reply_length = 0;
  return reply_length;
}


void send_next_window(__xdata command_t *status_cmd) {
  uint8_t done = 0;
  uint8_t i; uint8_t drop_index = 0; uint8_t data_index = 0;
  uint8_t payload_len;

  // don't know why we need the -1, but apparently it's necessary otherwise 1 extra byte is sent
  uint8_t command_header_size = sizeof(hsat_status_header_t) + sizeof(command_header_t) - 1;
  
  __xdata command_buffer_t *buf = (__xdata command_buffer_t *) status_cmd;
  hsat_status_t *status = (__xdata hsat_status_t *) status_cmd->data;

  // #TODO why not combine this with the next two lines?
  uint16_t seqnum_start = 0;
  uint16_t seqnum_finish = 0;
   
  // drop count < WS because if all packets in the window are dropped the groundstation won't ACK
  seqnum_start = last_sent + 1;
  seqnum_finish = last_sent + WS - drop_count;

  // package and send the window
  for (i = 0; i < WS; i++) {
    // while there are still drops to retransmit ...
    if (drop_index < drop_count) {
      data_index = dropped_packets[drop_index];
      drop_index++;
    }

    // otherwise just send the next new packet
    else {
      last_sent++;
      data_index = last_sent;
    }

    // check if it's the last packet
    if (data_index * PAYLOAD_SIZE > data_len) {
      payload_len = data_index * PAYLOAD_SIZE - data_len;
      done = 1;
    }

    else {
      payload_len = PAYLOAD_SIZE;
    }

    // eventually need to pull dest hwid from the dump_status packet
    status_cmd->header.hwid = hwid_flash;
    status_cmd->header.seqnum = data_index;
    status_cmd->header.system = MSG_TYPE_RADIO_OUT;
    status_cmd->header.command = hsat_status_cmd;
    
    status->header.len = payload_len;
    status->header.seqnum_start = seqnum_start;
    status->header.seqnum_finish = seqnum_finish;
    status->header.window_size = WS;

    // actually copy the payload data into the packet
    memcpyx((__xdata void *) status->payload,
	    // #TODO: I also think the indexing should be PAYLOAD_SIZE * data_index otherwise it fucks up on the last packet
	    (__xdata void *) (&data_q[payload_len * data_index]),
	    // #TODO: sizeof seems wrong here, right? - almost positive this should be payload_len
	    sizeof(payload_len));

    // send over UART instead of radio for now
    uart1_send_message(buf->msg, payload_len + command_header_size);
    
    if (done) {
      // send a done packet here
      // ...
      return;
    }
  }
 
  return;
}


void board_led_set(__bit led_on) {
  P0_7 = led_on;
}

