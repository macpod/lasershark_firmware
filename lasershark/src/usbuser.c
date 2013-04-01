/*
 usbuser.c - Lasershark firmware.
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

#include "type.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "usbuser.h"
#include "lasershark.h"
#include "gpio.h"
#include "config.h"
#include "usbreg.h"

/*
 *  USB Power Event Callback
 *   Called automatically on USB Power Event
 *    Parameter:       power: On(TRUE)/Off(FALSE)
 */
#if USB_POWER_EVENT
void USB_Power_Event (uint32_t power) {
}
#endif

/*
 *  USB Reset Event Callback
 *   Called automatically on USB Reset Event
 */
#if USB_RESET_EVENT
void USB_Reset_Event(void) {
	USB_ResetCore();
}
#endif

/*
 *  USB Suspend Event Callback
 *   Called automatically on USB Suspend Event
 */
#if USB_SUSPEND_EVENT
void USB_Suspend_Event(void) {
}
#endif

/*
 *  USB Resume Event Callback
 *   Called automatically on USB Resume Event
 */
#if USB_RESUME_EVENT
void USB_Resume_Event(void) {
}
#endif

/*
 *  USB Remote Wakeup Event Callback
 *   Called automatically on USB Remote Wakeup Event
 */
#if USB_WAKEUP_EVENT
void USB_WakeUp_Event (void) {
}
#endif

/*
 *  USB Start of Frame Event Callback
 *   Called automatically on USB Start of Frame Event
 */
#if USB_SOF_EVENT
void USB_SOF_Event(void) {
	uint32_t cnt;
	if (USB_Configuration) {
		LPC_USB->Ctrl = ((USB_ENDPOINT_OUT(4) & 0x0F) << 2) | CTRL_RD_EN; // enable read
		// 3 clock cycles to fetch the packet length from RAM.
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");

		if ((cnt = LPC_USB->RxPLen) & PKT_DV) { // We have data...
			cnt &= PKT_LNGTH_MASK; // Get length in bytes
			lasershark_process_data(cnt);
			LPC_USB->Ctrl = 0;
		}
		LPC_USB->Ctrl = 0; // Disable read mode.. do this if you ever want to see a USB packet again
	}
}
#endif

/*
 *  USB Error Event Callback
 *   Called automatically on USB Error Event
 *    Parameter:       error: Error Code
 */

#if USB_ERROR_EVENT
void USB_Error_Event (uint32_t error) {
}
#endif

/*
 *  USB Set Configuration Event Callback
 *   Called automatically on USB Set Configuration Request
 */

#if USB_CONFIGURE_EVENT
void USB_Configure_Event(void) {

	if (USB_Configuration) { /* Check if USB is configured */
		/* add your code here */
	}
}
#endif

/*
 *  USB Set Interface Event Callback
 *   Called automatically on USB Set Interface Request
 */

#if USB_INTERFACE_EVENT
void USB_Interface_Event (void) {
}
#endif

/*
 *  USB Set/Clear Feature Event Callback
 *   Called automatically on USB Set/Clear Feature Request
 */

#if USB_FEATURE_EVENT
void USB_Feature_Event (void) {
}
#endif

#define P_EP(n) ((USB_EP_EVENT & (1 << (n))) ? USB_EndPoint##n : NULL)

/* USB Endpoint Events Callback Pointers */
void (* const USB_P_EP[USB_LOGIC_EP_NUM])(uint32_t event) = { P_EP(0), P_EP(1),
		P_EP(2), P_EP(3), P_EP(4), };

/*
 *  USB Endpoint 1 Event Callback
 *   Called automatically on USB Endpoint 1 Event
 *    Parameter:       event
 */
void USB_EndPoint1(uint32_t event) {
	switch (event) {
	case USB_EVT_OUT:
		USB_ReadEP(USB_ENDPOINT_OUT(1), OUT1Packet);
		lasershark_process_command();
		USB_WriteEP(USB_ENDPOINT_IN(1), IN1Packet, 64);
		break;
	case USB_EVT_IN:
		break;
	}
}

/*
 *  USB Endpoint 2 Event Callback
 *   Called automatically on USB Endpoint 2 Event
 *    Parameter:       event
 */
void USB_EndPoint2(uint32_t event) {
	switch (event) {
	case USB_EVT_IN:
		break;
	}
}

/*
 *  USB Endpoint 3 Event Callback
 *   Called automatically on USB Endpoint 3 Event
 *    Parameter:       event
 */
void USB_EndPoint3(uint32_t event) {
	uint32_t cnt;
	switch (event) {
	case USB_EVT_OUT:
		while (1) {
			if (LASERSHARK_USB_DATA_BULK_SIZE <= lasershark_get_empty_sample_count()) {
				break;
			}
		}

		LPC_USB->Ctrl = ((USB_ENDPOINT_OUT(3) & 0x0F) << 2) | CTRL_RD_EN; // enable read
		// 3 clock cycles to fetch the packet length from RAM.
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		

		if ((cnt = LPC_USB->RxPLen) & PKT_DV) { // We have data...
			cnt &= PKT_LNGTH_MASK; // Get length in bytes
			lasershark_process_data(cnt);
			LPC_USB->Ctrl = 0;
		}
		LPC_USB->Ctrl = 0; // Disable read mode.. do this if you ever want to see a USB packet again
	    WrCmdEP(USB_ENDPOINT_OUT(3), CMD_CLR_BUF);
		break;
	}
}

/*
 *  USB Endpoint 4 Event Callback
 *   Called automatically on USB Endpoint 3 Event
 *    Parameter:       event
 */
void USB_EndPoint4(uint32_t event) {
	switch (event) {
	case USB_EVT_IN:
		break;
	}
}

