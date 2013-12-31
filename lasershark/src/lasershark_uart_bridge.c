/*
 lasershark_uart_bridge.h - Lasershark firmware component to allow for
 limited uart communication through lasershark.
 Copyright (C) 2012 Jeffrey Nelson <nelsonjm@macpod.net>

 This file is part of Lasershark's Firmware.

 Lasershark is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 2 of the License, or
 (at your option) any later version.

 Lasershark is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Lasershark. If not, see <http://www.gnu.org/licenses/>.
 */

#include "lasershark_uart_bridge.h"
#include "lasershark.h"
#include "uart.h"
#include <string.h>
#include <stdbool.h>

void lasershark_process_uart_bridge_command() {
	uint32_t u32_temp;
	uint8_t u8_temp;
	//bool lasershark_output_was_enabled;

	IN2Packet[0] = OUT2Packet[0]; // Put the command sent in the "IN" buffer
	IN2Packet[1] = LASERSHARK_UB_CMD_SUCCESS; // Assume output will be success

	//lasershark_output_was_enabled = lasershark_output_is_enabled();
	//if (lasershark_output_was_enabled) {
	//	// Disable output
	//	lasershark_output_enable(false);
	//}

	switch (OUT2Packet[0]) {
	case LASERSHARK_UB_CMD_TX:
		u8_temp = uart_tx_nonblock(OUT2Packet + 2, OUT2Packet[1]);
		IN2Packet[2] = u8_temp; // Return number of bytes copied.
		break;
	case LASERSHARK_UB_CMD_RX:
		u8_temp = uart_rx_nonblock(IN2Packet + 3, OUT2Packet[1]);
		IN2Packet[2] = u8_temp; // Return number of bytes copied.
		break;
	case LASERSHARK_UB_CMD_RX_READY:
		u8_temp =
				uart_rx_ready() ?
						LASERSHARK_UB_RX_READY : LASERSHARK_UB_RX_NOT_READY;
		IN2Packet[2] = u8_temp;
		break;
	case LASERSHARK_UB_CMD_MAX_RX:
		u8_temp = LASERSHARK_UB_MAX_RX_SIZE;
		IN2Packet[2] = u8_temp;
		break;
	case LASERSHARK_UB_CMD_MAX_TX:
		u8_temp = LASERSHARK_UB_MAX_TX_SIZE;
		IN2Packet[2] = u8_temp;
		break;
	case LASERSHARK_UB_CMD_CLEAR_RX_FIFO:
		uart_rx_clear_fifo();
		break;
	case LASERSHARK_UB_CMD_VERSION:
		u32_temp = LASERSHARK_UB_VERSION;
		memcpy(IN2Packet + 2, &u32_temp, sizeof(uint32_t));
		break;
	default:
		IN2Packet[1] = LASERSHARK_UB_CMD_UNKNOWN;
		break;
	}

	//if (!lasershark_output_was_enabled) {
	//	// Re-enable output.
	//	lasershark_output_enable(true);
	//}
	return;

}

void lasershark_uart_bridge_init() {
	UARTInit(115200);
}

