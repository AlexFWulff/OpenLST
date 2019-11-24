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
#include "uart0.h"

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
}


uint8_t custom_commands(const __xdata command_t *cmd, uint8_t len, __xdata command_t *reply) {
  uint8_t reply_length;
  
  // ensure that this is a message we expect
  if (cmd->header.command == hsat_msg && len == sizeof(command_header_t) + sizeof(hsat_msg_data_t)) {
      __xdata hsat_msg_data_t *reply_data = (__xdata hsat_msg_data_t *) cmd->data;
      // could sniff data here if desired
      // ...
      
      // for now just send the message data out to UART0

      // I don't see any problems with typecasting the pointer to the struct to a byte array
      // but it's worth checking if this is right
      uart0_send_message((__xdata const unsigned char *)reply_data, sizeof(hsat_msg_data_t));
      reply->header.command = common_msg_ack;
  }

  else {
    // this is the default but it's nice to have it here
    reply->header.command = common_msg_nack;
  }

  // just going to reply with an ACK for now
  reply_length = (uint8_t) sizeof(command_header_t);
  return reply_length;
}


void board_led_set(__bit led_on) {
  P0_7 = led_on;
}
