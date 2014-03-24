/*
 main.c - Lasershark firmware.
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

#ifdef __USE_CMSIS
#include "LPC13xx.h"
#endif

#include <cr_section_macros.h>
#include <NXP/crp.h>
#include <stdbool.h>
#include "config.h"
#include "watchdog.h"
#include "gpio.h"
#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "usbdesc.h"
#include "type.h"
#include "lasershark.h"
#include "lasershark_uart_bridge.h"

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP;


int main(void) {
	/* Basic chip initialization is taken care of in SystemInit() called
	 * from the startup code. SystemInit() and chip settings are defined
	 * in the CMSIS system_<part family>.c file.
	 */

	/* Initialize GPIO (sets up clock) */
	GPIOInit();

	/* Set pin C, INTL A and INTL B function and state ASAP!
	 * If pin C and INTL A are set as inputs, it will cause the amplifiers to output > 10v
	 * which is NOT GOOD! There is no real way to avoid this issue unfortunately other than
	 * to insist the board is not connected when being programmed.
	 */
	/* Make pin functions digital IOs, pulldowns enabled, ADMODE=digital. */
	LPC_IOCON->R_PIO1_1 = (1 << 0) | (0x1 << 3) | (1 << 7); // C
	LPC_IOCON->R_PIO1_2 = (1 << 0) | (0x1 << 3) | (1 << 7); // INTL A
	LPC_IOCON->R_PIO1_0 = (1 << 0) | (0x1 << 3) | (1 << 7); // INTL B

	/* Set LED port pin to output */
	GPIOSetDir(LED_PORT, USR1_LED_BIT, 1);
	GPIOSetDir(LED_PORT, USR2_LED_BIT, 1);

	/* Enable IOCON blocks for io pin multiplexing.*/
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 16);

	// Set initial state of lasershark ASAP!
	lasershark_init();
	lasershark_uart_bridge_init();

	USBIOClkConfig();

	usb_populate_serialno(); // Populate the devices serial number
	USB_Init(); // USB Initialization
	// Make USB a lower priority than the timer used for output.
	NVIC_SetPriority(USB_IRQn, 2);
	USB_Connect(TRUE); // USB Connect

	watchdog_init();

	while (!USB_Configuration) // wait until USB is configured
	{
		watchdog_feed_or_nop();
	}

	while (1) {
		watchdog_feed_or_nop();
		lasershark_handle_bulk_data_interrupt_retrigger();
	}
	// TODO handle usb disconnects better instead of resetting (i.e. for externally powered LaserSharks).

	return 0;
}
