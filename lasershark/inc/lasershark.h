/*
lasershark.h - Lasershark firmware.
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

#ifndef LASERSHARK_H_
#define LASERSHARK_H_

#include <stdbool.h>
#include <stdint.h>

#define LASERSHARK_CMD_SUCCESS 0x00
#define LASERSHARK_CMD_FAIL 0x01
#define LASERSHARK_CMD_UNKNOWN 0xFF

// Set/Get output commands
#define LASERSHARK_CMD_SET_OUTPUT 0x80
#define LASERSHARK_CMD_GET_OUTPUT 0x81
#define LASERSHARK_CMD_OUTPUT_ENABLE 0x01
#define LASERSHARK_CMD_OUTPUT_DISABLE 0x00

// Set/get current ilda rate
#define LASERSHARK_CMD_SET_ILDA_RATE 0x82
#define LASERSHARK_CMD_GET_ILDA_RATE 0x83

// Get max ilda rate
#define LASERSHARK_CMD_GET_MAX_ILDA_RATE 0X84

// Get number of ilda elements per sample
#define LASERSHARK_CMD_GET_SAMP_ELEMENT_COUNT 0X85
// Get number of ilda samples that can be transmitter per iso packet.
#define LASERSHARK_CMD_GET_ISO_PACKET_SAMP_COUNT 0x86

// Get min dac value
#define LASERSHARK_CMD_GET_DAC_MIN 0x87
// Get max dac value
#define LASERSHARK_CMD_GET_DAC_MAX 0x88


// Get the number of samples the ring buffer is able to store
#define LASERSHARK_CMD_GET_RINGBUFFER_SAMPLE_COUNT 0X89

// Get the number of samples that are unfilled in the ring buffer
#define LASERSHARK_CMD_GET_RINGBUFFER_EMPTY_SAMPLE_COUNT 0X8A

// Version Info
#define LASERSHARK_FW_MAJOR_VERSION 2
#define LASERSHARK_FW_MINOR_VERSION 2
#define LASERSHARK_CMD_GET_LASERSHARK_FW_MAJOR_VERSION 0X8B
#define LASERSHARK_CMD_GET_LASERSHARK_FW_MINOR_VERSION 0X8C

// Clears ring buffer
#define LASERSHARK_CMD_CLEAR_RINGBUFFER 0x8D

// Get number of ilda samples that can be transmitter per bulk packet.
#define LASERSHARK_CMD_GET_BULK_PACKET_SAMP_COUNT 0x8E

#define LASERSHARK_X_DAC_REG DAC124S085_INPUT_REG_C
#define LASERSHARK_Y_DAC_REG DAC124S085_INPUT_REG_D
#define LASERSHARK_A_DAC_REG DAC124S085_INPUT_REG_A
#define LASERSHARK_B_DAC_REG DAC124S085_INPUT_REG_B

#define LASERSHARK_C_PORT 1
#define LASERSHARK_C_PIN 1
#define LASERSHARK_C_BITMASK 0x4000

#define LASERSHARK_INTL_A_PORT 1
#define LASERSHARK_INTL_A_PIN 2
#define LASERSHARK_INTL_A_BITMASK 0x8000

#define LASERSHARK_INTL_B_PORT 1
#define LASERSHARK_INTL_B_PIN 0


#define LASERSHARK_PGM_BUTTON_PORT 0
#define LASERSHARK_PGM_BUTTON_PIN 1

#define LASERSHARK_USB_CTRL_SIZE 64
#define LASERSHARK_USB_DATA_BULK_SIZE 64
#define LASERSHARK_USB_DATA_ISO_SIZE 512
#define LASERSHARK_USB_SOF_RATE 1000
unsigned char OUT1Packet[LASERSHARK_USB_CTRL_SIZE]; //User application buffer for receiving and holding OUT packets sent from the host
unsigned char IN1Packet[LASERSHARK_USB_CTRL_SIZE]; //User application buffer for sending IN packets to the host


#define LASERSHARK_RINGBUFFER_SAMPLES 768
#define LASERSHARK_ILDA_CHANNELS 4
#define LASERSHARK_SAMPLE_SIZE 8
#define LASERSHARK_USB_DATA_ISO_PACKET_SAMPLE_COUNT 64
#define LASERSHARK_USB_DATA_BULK_PACKET_SAMPLE_COUNT 8
volatile uint16_t lasershark_ringbuffer[LASERSHARK_RINGBUFFER_SAMPLES][LASERSHARK_ILDA_CHANNELS];
volatile uint16_t lasershark_blankingbuffer[LASERSHARK_ILDA_CHANNELS];

volatile uint32_t lasershark_ringbuffer_head;
volatile uint32_t lasershark_ringbuffer_tail;

#define LASERSHARK_ILDA_RATE_DEFAULT 1000
uint32_t lasershark_ilda_rate_max;
uint32_t lasershark_curr_ilda_rate;
uint32_t lasershark_core_duration;

void lasershark_init();

void lasershark_output_enable(bool enable);

bool lasershark_output_is_enabled();


void lasershark_process_command();

bool lasershark_set_ilda_rate(uint32_t ilda_rate);

__inline uint32_t lasershark_get_empty_sample_count();

__inline void lasershark_process_data(uint32_t cnt);

void lasershark_set_bulk_data_interrupt_needs_retrigger();

void lasershark_handle_bulk_data_interrupt_retrigger();

void TIMER32_1_IRQHandler(void);

#endif /* LASERSHARK_H_ */
