/*
lasershark.c - Lasershark firmware.
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

#include <string.h>
#include "lasershark.h"
#include "gpio.h"
#include "config.h"
#include "ssp.h"
#include "timer32.h"
#include "usbreg.h"
#include "dac124s085.h"

void lasershark_init() {
	int i, j = j;
	lasershark_output_enabled = false;
	lasershark_ringbuffer_head = 0;
	lasershark_ringbuffer_tail = 0;

	lasershark_usb_data_packet_size = LASERSHARK_USB_DATA_SIZE
			- (LASERSHARK_USB_DATA_SIZE % (LASERSHARK_ILDA_CHANNELS
					* sizeof(uint16_t)));
	lasershark_usb_data_packet_samp_count = lasershark_usb_data_packet_size
			/ (LASERSHARK_ILDA_CHANNELS * sizeof(uint16_t));

	SSPInit();
	init_timer32(1, lasershark_core_duration);
	lasershark_set_ilda_rate(LASERSHARK_ILDA_RATE_DEFAULT);
	enable_timer32(1);

	lasershark_ilda_rate_max = LASERSHARK_USB_SOF_RATE
			* lasershark_usb_data_packet_samp_count;

	// This is buffer sent when the system is off
	lasershark_blankingbuffer[0] = DAC124S085_DAC_VAL_MID; // X
	lasershark_blankingbuffer[1] = DAC124S085_DAC_VAL_MID; // Y
	for (j = 2; j < LASERSHARK_ILDA_CHANNELS; j++) { // All dem lasers
		lasershark_blankingbuffer[j] = DAC124S085_DAC_VAL_MIN;
	}

	for (i = 0; i < LASERSHARK_RINGBUFFER_SAMPLES; i++) {
		lasershark_ringbuffer[i][0] = DAC124S085_DAC_VAL_MID; // X
		lasershark_ringbuffer[i][1] = DAC124S085_DAC_VAL_MID; // Y
		for (j = 2; j < LASERSHARK_ILDA_CHANNELS; j++) { // All dem lasers
			lasershark_ringbuffer[i][j] = DAC124S085_DAC_VAL_MIN;
		}
	}
}

void lasershark_process_command() {
	uint32_t temp;
	IN1Packet[0] = OUT1Packet[0]; // Put the command sent in the "IN" buffer
	IN1Packet[1] = LASERSHARK_CMD_SUCCESS; // Assume output will be success

	switch (OUT1Packet[0]) {
	case LASERSHARK_CMD_SET_OUTPUT: //Enable/Disable output
		switch (OUT1Packet[1]) {
		case LASERSHARK_CMD_OUTPUT_DISABLE: // Disable output
			GPIOSetValue(LED_PORT, USR1_LED_BIT, 1); // 1 makes voltage across diode 0v
			lasershark_output_enabled = false;
			break;
		case LASERSHARK_CMD_OUTPUT_ENABLE: // Enable output
			GPIOSetValue(LED_PORT, USR1_LED_BIT, 0); // 0 makes voltage across diode >  0v
			lasershark_output_enabled = true;
			break;
		default:
			IN1Packet[1] = LASERSHARK_CMD_FAIL;
			break;
		}
	case LASERSHARK_CMD_GET_OUTPUT: // Get enabled state
		if (lasershark_output_enabled) {
			IN1Packet[2] = LASERSHARK_CMD_OUTPUT_ENABLE;
		} else {
			IN1Packet[2] = LASERSHARK_CMD_OUTPUT_DISABLE;
		}
		break;
	case LASERSHARK_CMD_SET_ILDA_RATE:
		memcpy(&temp, OUT1Packet + 1, sizeof(uint32_t));
		if (!lasershark_set_ilda_rate(temp)) { // Note: I tried calling without temp on a pic32, but discovered a weird MC bug.
			IN1Packet[1] = LASERSHARK_CMD_FAIL;
		}
		break;
	case LASERSHARK_CMD_GET_ILDA_RATE:
		memcpy(IN1Packet + 2, &lasershark_curr_ilda_rate, sizeof(uint32_t));
		break;
	case LASERSHARK_CMD_GET_MAX_ILDA_RATE:
		temp = lasershark_ilda_rate_max;
		memcpy(IN1Packet + 2, &temp, sizeof(uint32_t));
		break;
	case LASERSHARK_CMD_GET_SAMP_ELEMENT_COUNT:
		temp = LASERSHARK_ILDA_CHANNELS;
		memcpy(IN1Packet + 2, &temp, sizeof(uint32_t));
		break;
	case LASERSHARK_CMD_GET_PACKET_SAMP_COUNT:
		memcpy(IN1Packet + 2, &lasershark_usb_data_packet_samp_count,
				sizeof(uint32_t));
		break;
	case LASERSHARK_CMD_GET_DAC_MIN:
		temp = DAC124S085_DAC_VAL_MIN;
		memcpy(IN1Packet + 2, &temp, sizeof(uint32_t));
		break;
	case LASERSHARK_CMD_GET_DAC_MAX:
		temp = DAC124S085_DAC_VAL_MAX;
		memcpy(IN1Packet + 2, &temp, sizeof(uint32_t));
		break;
	default:
		IN1Packet[1] = LASERSHARK_CMD_UNKNOWN;
		break;
	}

}

bool lasershark_set_ilda_rate(uint32_t ilda_rate) {
	if (ilda_rate > lasershark_ilda_rate_max || ilda_rate == 0) {
		return false;
	}
	lasershark_curr_ilda_rate = ilda_rate;
	// Remember ILDA rate will be 1/2 Frequency (i.e. 50Khz = 100Kpps)
	lasershark_core_duration = SystemCoreClock / lasershark_curr_ilda_rate - 1;

	update_timer32(1, lasershark_core_duration);

	return true;
}

__inline void lasershark_process_data(uint32_t cnt) {
	uint32_t dat, n, cntmod = (cnt + 3) / 4;
	uint32_t *pData;

	GPIOToggleValue(LED_PORT, USR2_LED_BIT);

	for (n = 0; n < cntmod; n++) {
		dat = LPC_USB->RxData;
		pData
				= ((uint32_t __attribute__((packed)) *) lasershark_ringbuffer[lasershark_ringbuffer_tail]);
		if (n % 2) { // Odd
			pData[1] = dat;
			lasershark_ringbuffer_tail = (lasershark_ringbuffer_tail + 1)
					% LASERSHARK_RINGBUFFER_SAMPLES;
		} else { // Even
			pData[0] = dat;
		}
	}
}

void TIMER32_1_IRQHandler(void) {
	LPC_TMR32B1->IR = 1; /* clear interrupt flag */

	if (!lasershark_output_enabled) {
		dac124s085_dac(lasershark_blankingbuffer);
		return;
	}

	// If the head and tail are the same, don't play the sample, it can make the galvos/lasers lose sanity.
	// This also has the desirable side effect of turning off the laser once all the buffer samples are used up (i.e. in the even USB comms stop).
	if (lasershark_ringbuffer_head == lasershark_ringbuffer_tail) {
		dac124s085_dac_chn_set(DAC124S085_INPUT_REG_C, DAC124S085_DAC_VAL_MIN,
				false);
		dac124s085_dac_chn_set(DAC124S085_INPUT_REG_D, DAC124S085_DAC_VAL_MIN,
				true);
		return;
	}

	dac124s085_dac(lasershark_ringbuffer[lasershark_ringbuffer_head]);

	lasershark_ringbuffer_head = (lasershark_ringbuffer_head + 1)
			% LASERSHARK_RINGBUFFER_SAMPLES;

}

