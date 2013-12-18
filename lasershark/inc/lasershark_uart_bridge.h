/*
 lasershark_uart_bridge.h - Lasershark firmware component to allow for
 limited uart communication through a lasershark unit.
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

#ifndef LASERSHARK_UART_BRIDGE_H_
#define LASERSHARK_UART_BRIDGE_H_
#include <stdint.h>

#define LASERSHARK_UB_CMD_SUCCESS 0x00
#define LASERSHARK_UB_CMD_FAIL 0x01
#define LASERSHARK_UB_CMD_UNKNOWN 0xFF

#define LASERSHARK_UB_VERSION 1

#define LASERSHARK_UB_CMD_TX 0xA0
#define LASERSHARK_UB_CMD_RX 0xA1
#define LASERSHARK_UB_CMD_RX_READY 0xA2
#define LASERSHARK_UB_CMD_MAX_TX 0xA3
#define LASERSHARK_UB_CMD_MAX_RX 0xA4
#define LASERSHARK_UB_CMD_CLEAR_RX_FIFO 0xA5
#define LASERSHARK_UB_CMD_VERSION 0XA6

#define LASERSHARK_UB_RX_NOT_READY 0x0
#define LASERSHARK_UB_RX_READY 0x1

#define LASERSHARK_UB_MAX_RX_SIZE 16
#define LASERSHARK_UB_MAX_TX_SIZE 16

#define LASERSHARK_UB_USB_DATA_SIZE 64
unsigned char OUT2Packet[LASERSHARK_UB_USB_DATA_SIZE]; //User application buffer for receiving and holding OUT packets sent from the host
unsigned char IN2Packet[LASERSHARK_UB_USB_DATA_SIZE]; //User application buffer for sending IN packets to the host

void lasershark_uart_bridge_init();

void lasershark_process_uart_bridge_command();

#endif /* LASERSHARK_UART_BRIDGE_H_ */
