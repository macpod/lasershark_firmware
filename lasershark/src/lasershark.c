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

static bool lasershark_output_enabled;

static bool lasershark_bulk_interrupt_retrigger;


static __INLINE void lasershark_set_interlock_a(bool val)
{
	GPIOSetValue(LASERSHARK_INTL_A_PORT, LASERSHARK_INTL_A_PIN, val);
}

static __INLINE bool lasershark_get_interlock_b()
{
	return GPIOGetValue(LASERSHARK_INTL_B_PORT, LASERSHARK_INTL_B_PIN);
}

static __INLINE void lasershark_set_c(bool val)
{
	GPIOSetValue(LASERSHARK_C_PORT, LASERSHARK_C_PIN, val);
}

void lasershark_init()
{
	int i, j = j;
	lasershark_output_enabled = false;
	lasershark_bulk_interrupt_retrigger = false;
	lasershark_ringbuffer_head = 0;
	lasershark_ringbuffer_tail = 0;

	// Set lasershark pins to be inputs/outputs as appropriate ASAP!
	GPIOSetDir(LASERSHARK_C_PORT, LASERSHARK_C_PIN, 1); // Output
	GPIOSetDir(LASERSHARK_INTL_A_PORT, LASERSHARK_INTL_A_PIN, 1); // Output
	GPIOSetDir(LASERSHARK_INTL_B_PORT, LASERSHARK_INTL_B_PIN, 0); // Input

	// Unlike the DAC output, these are not pulled down at start so we need to pull down these pins ASAP!
	lasershark_set_interlock_a(0);
	lasershark_set_c(0);

	SSPInit();

	lasershark_ilda_rate_max = LASERSHARK_USB_SOF_RATE
			* LASERSHARK_USB_DATA_ISO_PACKET_SAMPLE_COUNT;

	// This is buffer sent when the system is off
	lasershark_blankingbuffer[0] = DAC124S085_DAC_VAL_MIN; // A
	lasershark_blankingbuffer[1] = DAC124S085_DAC_VAL_MIN; // B
	lasershark_blankingbuffer[2] = DAC124S085_INPUT_REG_DATA_MASK
			& DAC124S085_DAC_VAL_MID; // INTL_A, C, X
	lasershark_blankingbuffer[3] = DAC124S085_DAC_VAL_MID; // Y

	for (i = 0; i < LASERSHARK_RINGBUFFER_SAMPLES; i++) {
		lasershark_ringbuffer[i][0] = DAC124S085_DAC_VAL_MIN; // A
		lasershark_ringbuffer[i][1] = DAC124S085_DAC_VAL_MIN; // B
		lasershark_ringbuffer[i][2] = DAC124S085_INPUT_REG_DATA_MASK
				& DAC124S085_DAC_VAL_MID; // INTL_A, C, X
		lasershark_ringbuffer[i][3] = DAC124S085_DAC_VAL_MID; // Y
	}

	// dummy code to blink LEDS.. woo
#if (0)

	while (1) {
		// Poor mans debounce
		GPIOToggleValue(LED_PORT, USR2_LED_BIT);
		GPIOToggleValue(LED_PORT, USR1_LED_BIT);

		for (i = 0; i < 10000000; i++) {
			asm("nop");
		}
	}
#endif

	// Really awful code block used to calibrate X and Y pots
#if (0)

	GPIOSetDir(LASERSHARK_PGM_BUTTON_PORT, LASERSHARK_PGM_BUTTON_PIN, 0); // Input

	while (1) {
		lasershark_blankingbuffer[0] = DAC124S085_DAC_VAL_MAX; // A
		lasershark_blankingbuffer[1] = DAC124S085_DAC_VAL_MAX; // B
		lasershark_blankingbuffer[2] = LASERSHARK_INTL_A_BITMASK
		| LASERSHARK_C_BITMASK | (DAC124S085_INPUT_REG_DATA_MASK
				& DAC124S085_DAC_VAL_MAX); // INTL_A, C, X
		lasershark_blankingbuffer[3] = DAC124S085_DAC_VAL_MAX; // Y
		lasershark_set_interlock_a(true);
		dac124s085_dac(lasershark_blankingbuffer);
		lasershark_set_c(true);
		GPIOToggleValue(LED_PORT, USR2_LED_BIT);
		GPIOToggleValue(LED_PORT, USR1_LED_BIT);

		while (GPIOGetValue(LASERSHARK_PGM_BUTTON_PORT,
						LASERSHARK_PGM_BUTTON_PIN)) {
			asm("nop");
		}

		// Poor mans debounce
		for (i = 0; i < 10000000; i++) {
			asm("nop");
		}

		lasershark_blankingbuffer[0] = DAC124S085_DAC_VAL_MID; // A
		lasershark_blankingbuffer[1] = DAC124S085_DAC_VAL_MID; // B
		lasershark_blankingbuffer[2] = DAC124S085_INPUT_REG_DATA_MASK
		& DAC124S085_DAC_VAL_MID; // INTL_A, C, X
		lasershark_blankingbuffer[3] = DAC124S085_DAC_VAL_MID; // Y
		lasershark_set_interlock_a(false);
		dac124s085_dac(lasershark_blankingbuffer);
		lasershark_set_c(false);
		GPIOToggleValue(LED_PORT, USR2_LED_BIT);
		GPIOToggleValue(LED_PORT, USR1_LED_BIT);

		while (GPIOGetValue(LASERSHARK_PGM_BUTTON_PORT,
						LASERSHARK_PGM_BUTTON_PIN)) {
			asm("nop");
		}

		// Poor mans debounce
		for (i = 0; i < 10000000; i++) {
			asm("nop");
		}

		lasershark_blankingbuffer[0] = DAC124S085_DAC_VAL_MIN; // A
		lasershark_blankingbuffer[1] = DAC124S085_DAC_VAL_MIN; // B
		lasershark_blankingbuffer[2] = DAC124S085_INPUT_REG_DATA_MASK
		& DAC124S085_DAC_VAL_MIN; // INTL_A, C, X
		lasershark_blankingbuffer[3] = DAC124S085_DAC_VAL_MIN; // Y
		lasershark_set_interlock_a(false);
		dac124s085_dac(lasershark_blankingbuffer);
		lasershark_set_c(false);
		GPIOToggleValue(LED_PORT, USR2_LED_BIT);
		GPIOToggleValue(LED_PORT, USR1_LED_BIT);

		while (GPIOGetValue(LASERSHARK_PGM_BUTTON_PORT,
						LASERSHARK_PGM_BUTTON_PIN)) {
			asm("nop");
		}

		// Poor mans debounce
		for (i = 0; i < 10000000; i++) {
			asm("nop");
		}

	}
#endif

	TIMER32_1_IRQHandler(); // The output is disabled, so the blank buffer will be sent.

	init_timer32(1, lasershark_core_duration);
	lasershark_set_ilda_rate(LASERSHARK_ILDA_RATE_DEFAULT);
	NVIC_SetPriority(TIMER_32_1_IRQn, 1);
	enable_timer32(1);
}

void lasershark_output_enable(bool enable)
{
	lasershark_output_enabled = enable;
}

bool lasershark_output_is_enabled()
{
	return lasershark_output_enabled;
}

void lasershark_clear_ringbuffer()
{
	enable_timer32(0);
	lasershark_ringbuffer_head = 0;
	lasershark_ringbuffer_tail = 0;
	enable_timer32(1);
}

void lasershark_process_command()
{
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
	case LASERSHARK_CMD_GET_ISO_PACKET_SAMP_COUNT:
		temp = LASERSHARK_USB_DATA_ISO_PACKET_SAMPLE_COUNT;
		memcpy(IN1Packet + 2, &temp,
				sizeof(uint32_t));
		break;
	case LASERSHARK_CMD_GET_BULK_PACKET_SAMP_COUNT:
		temp = LASERSHARK_USB_DATA_BULK_PACKET_SAMPLE_COUNT;
		memcpy(IN1Packet + 2, &temp,
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
	case LASERSHARK_CMD_GET_RINGBUFFER_SAMPLE_COUNT:
		temp = LASERSHARK_RINGBUFFER_SAMPLES;
		memcpy(IN1Packet + 2, &temp, sizeof(uint32_t));
		break;
	case LASERSHARK_CMD_GET_RINGBUFFER_EMPTY_SAMPLE_COUNT:
		temp = lasershark_get_empty_sample_count();
		memcpy(IN1Packet + 2, &temp, sizeof(uint32_t));
		break;
	case LASERSHARK_CMD_GET_LASERSHARK_FW_MAJOR_VERSION:
		temp = LASERSHARK_FW_MAJOR_VERSION;
		memcpy(IN1Packet + 2, &temp, sizeof(uint32_t));
		break;
	case LASERSHARK_CMD_GET_LASERSHARK_FW_MINOR_VERSION:
		temp = LASERSHARK_FW_MINOR_VERSION;
		memcpy(IN1Packet + 2, &temp, sizeof(uint32_t));
		break;
	case LASERSHARK_CMD_CLEAR_RINGBUFFER:
		lasershark_clear_ringbuffer();
		break;
	default:
		IN1Packet[1] = LASERSHARK_CMD_UNKNOWN;
		break;
	}

}

bool lasershark_set_ilda_rate(uint32_t ilda_rate)
{
	if (ilda_rate > lasershark_ilda_rate_max || ilda_rate == 0) {
		return false;
	}
	lasershark_curr_ilda_rate = ilda_rate;
	// Remember ILDA rate will be 1/2 Frequency (i.e. 50Khz = 100Kpps)
	lasershark_core_duration = SystemCoreClock / lasershark_curr_ilda_rate - 1;

	update_timer32(1, lasershark_core_duration);

	return true;
}

__inline uint32_t lasershark_get_empty_sample_count()
{
	return ((lasershark_ringbuffer_head > lasershark_ringbuffer_tail) ?
					lasershark_ringbuffer_head - lasershark_ringbuffer_tail :
					LASERSHARK_RINGBUFFER_SAMPLES - lasershark_ringbuffer_tail + lasershark_ringbuffer_head);
}

__inline void lasershark_process_data(uint32_t cnt)
{
	uint32_t dat, n, cntmod = (cnt + 3) / 4;
	uint32_t *pData;

	GPIOToggleValue(LED_PORT, USR2_LED_BIT);

	for (n = 0; n < cntmod; n++) {
		dat = LPC_USB->RxData;
		pData
				= ((uint32_t __attribute__((packed)) *) lasershark_ringbuffer[lasershark_ringbuffer_tail]);
		if (n & 0x1) { // Odd
			pData[1] = dat;
			lasershark_ringbuffer_tail = (lasershark_ringbuffer_tail + 1)
					% LASERSHARK_RINGBUFFER_SAMPLES;
		} else { // Even
			pData[0] = dat;
		}
	}
}

void  lasershark_set_bulk_data_interrupt_needs_retrigger(void)
{
	lasershark_bulk_interrupt_retrigger = true;
}

void lasershark_handle_bulk_data_interrupt_retrigger(void)
{
	if (lasershark_bulk_interrupt_retrigger) {
		if (LASERSHARK_USB_DATA_BULK_SIZE <= lasershark_get_empty_sample_count()) {
			lasershark_bulk_interrupt_retrigger = false;
			LPC_USB->DevIntSet = EP6_INT; // Physical EP 6 (Logical OUT EP 3). This is a write-only register. Don't OR-in value.
		}
	}
}

void TIMER32_1_IRQHandler(void)
{
	int32_t temp;

	LPC_TMR32B1->IR = 1; /* clear interrupt flag */

	if (!lasershark_output_enabled /*|| !lasershark_get_interlock_b()*/) {
		// This is buffer sent when the system is off
		lasershark_blankingbuffer[0] = DAC124S085_DAC_VAL_MIN; // A
		lasershark_blankingbuffer[1] = DAC124S085_DAC_VAL_MIN; // B
		lasershark_blankingbuffer[2] = DAC124S085_INPUT_REG_DATA_MASK
				& DAC124S085_DAC_VAL_MID; // INTL_A, C, X
		lasershark_blankingbuffer[3] = DAC124S085_DAC_VAL_MID; // Y

		lasershark_set_interlock_a(false);
		dac124s085_dac(lasershark_blankingbuffer);
		lasershark_set_c(false);
		return;
	}

	temp = (lasershark_ringbuffer_head + 1)
					% LASERSHARK_RINGBUFFER_SAMPLES;

	// If the head and tail are the same, don't play the sample, it can make the galvos/lasers lose sanity.
	// This also has the desirable side effect of turning off the laser once all the buffer samples are used up (i.e. in the even USB comms stop).
	if (temp == lasershark_ringbuffer_tail) {
		lasershark_set_interlock_a(false);
		dac124s085_dac_chn_set(LASERSHARK_A_DAC_REG, DAC124S085_DAC_VAL_MIN,
				false);
		dac124s085_dac_chn_set(LASERSHARK_B_DAC_REG, DAC124S085_DAC_VAL_MIN,
				true);
		lasershark_set_c(false);

		return;
	}

	lasershark_set_interlock_a(
			lasershark_ringbuffer[lasershark_ringbuffer_head][0]
					& LASERSHARK_INTL_A_BITMASK);
	dac124s085_dac(lasershark_ringbuffer[lasershark_ringbuffer_head]);
	lasershark_set_c(
			lasershark_ringbuffer[lasershark_ringbuffer_head][0]
					& LASERSHARK_C_BITMASK);

	lasershark_ringbuffer_head = temp;
}

