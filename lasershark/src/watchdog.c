/*
watchdog.c - Lasershark firmware.
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

#include "watchdog.h"
#include "config.h"
#ifdef __USE_CMSIS
#include "LPC13xx.h"
#endif

#if (!WATCHDOG_ENABLED)
#warning Watchdog is disabled!
#endif

__inline void watchdog_feed() {
#if (WATCHDOG_ENABLED)
	LPC_WDT->FEED = 0xAA;
	LPC_WDT->FEED = 0x55;
#endif
}


// Feeds watchdog or if disabled just nops (useful for loops when code is optimized.)
__inline void watchdog_feed_or_nop() {
#if (WATCHDOG_ENABLED)
	LPC_WDT->FEED = 0xAA;
	LPC_WDT->FEED = 0x55;
#else
	asm("nop");
#endif
}


void watchdog_init() {
#if (WATCHDOG_ENABLED)

	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 15); // Power on WTD peripheral
	//	LPC_SYSCON->CLKOUTCLKSEL = 0x02; // Use WDTOSC for CLKOUT pin

	LPC_SYSCON->WDTOSCCTRL = 0x1 << 5 | 0xF; // FREQSEL = 1.4Mhz, 64 =  ~9.375kHz
	LPC_SYSCON->PDRUNCFG &= ~(0x1 << 6); // Power WDTOSC

	LPC_SYSCON->WDTCLKSEL = 0x02; // Use WDTOSC as the WTD clock source

	LPC_SYSCON->WDTCLKUEN = 0x01; // Update clock source
	// Write 0 then 1 to apply
	LPC_SYSCON->WDTCLKUEN = 0x00;
	LPC_SYSCON->WDTCLKUEN = 0x01;
	while (!(LPC_SYSCON->WDTCLKUEN & 0x01))
		; // Wait for update to occur.

	LPC_SYSCON->WDTCLKDIV = 0x01; // Enabled WDTCLK and set it to divide by 1 (7.8KHz)

	NVIC_EnableIRQ(WDT_IRQn);
	LPC_WDT->TC = 256; // Delay = <276 (minimum of 256, max of 2^24)>*4 / 9.375khz(WDTCLK) =0.10922666666 s
	LPC_WDT->MOD = WDEN | WDRESET; // Cause reset to occur when WDT hits.

	watchdog_feed();
#endif
}

