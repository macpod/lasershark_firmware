/*
usbdesc.c - Lasershark firmware.
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
#include "usbdesc.h"
#include "config.h"
#include "lasershark.h"
#include "iap.h"
 
/* USB Standard Device Descriptor */
const uint8_t USB_DeviceDescriptor[] = {
  USB_DEVICE_DESC_SIZE,              /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,        /* bDescriptorType */
  WBVAL(0x0200), /* 2.0 */           /* bcdUSB */
  USB_DEVICE_CLASS_RESERVED,	     /* bDeviceClass CDC*/
  0x00,                              /* bDeviceSubClass */
  0x00,                              /* bDeviceProtocol */
  USB_MAX_PACKET0,                   /* bMaxPacketSize0 */
  WBVAL(USB_VENDOR_ID),                     /* idVendor */
  WBVAL(USB_PROD_ID),                     /* idProduct */
  WBVAL(USB_DEVICE), /* 1.00 */          /* bcdDevice */
  0x01,                              /* iManufacturer */
  0x02,                              /* iProduct */
  0x03,                              /* iSerialNumber */
  0x01                               /* bNumConfigurations: one possible configuration*/
};

/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t USB_ConfigDescriptor[] = {
/* Configuration 1 */
  USB_CONFIGUARTION_DESC_SIZE,       /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(                             /* wTotalLength */
    1*USB_CONFIGUARTION_DESC_SIZE +
    3*USB_INTERFACE_DESC_SIZE     +  /* interfaces */
    4*USB_ENDPOINT_DESC_SIZE         /* endpoints */
      ),
  0x02,                              /* bNumInterfaces */
  0x01,                              /* bConfigurationValue: 0x01 is used to select this configuration */
  0x00,                              /* iConfiguration: no string to describe this configuration */
  USB_CONFIG_BUS_POWERED /*|*/       /* bmAttributes */
/*USB_CONFIG_REMOTE_WAKEUP*/,
  USB_CONFIG_POWER_MA(500),          /* bMaxPower, device power consumption is 500 mA */


/* Interface 0, Alternate Setting 0 interface descriptor */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  0,			                     /* bInterfaceNumber: Number of Interface */
  0x00,                              /* bAlternateSetting: Alternate setting */
  0x02,                              /* bNumEndpoints: Two endpoints used */
  0xFF, 							 /* bInterfaceClass: Vendor specific */
  0xFF,						         /* bInterfaceSubClass: Vendor specific */
  0x00,                              /* bInterfaceProtocol: no protocol used */
  0x00,                              /* iInterface: */

  /* Endpoint, EP1 Bulk Out */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_OUT(1),               /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(64),            			 /* wMaxPacketSize */
  0x00,                              /* bInterval: ignore for Bulk transfer */

  /* Endpoint, EP1 Bulk In */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_IN(1),                /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(LASERSHARK_USB_CTRL_SIZE),            			 /* wMaxPacketSize */
  0x00,                              /* bInterval: ignore for Bulk transfer */

/* Interface 1, Alternate Setting 0, Data class interface descriptor*/
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  1,                 				 /* bInterfaceNumber: Number of Interface */
  0x00,                              /* bAlternateSetting: no alternate setting */
  0x01,                              /* bNumEndpoints: one endpoint used */
  0xFF,          					 /* bInterfaceClass: Data Interface Class */
  0xFF,                              /* bInterfaceSubClass: no subclass available */
  0x00,                              /* bInterfaceProtocol: no protocol used */
  0x00,                              /* iInterface: */

  /* Endpoint, EP4 Isochronous Out */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_OUT(4),               /* bEndpointAddress */
  USB_ENDPOINT_TYPE_ISOCHRONOUS,            /* bmAttributes */
  WBVAL(LASERSHARK_USB_DATA_ISO_SIZE),            /* wMaxPacketSize */
  0x01,                              /* bInterval: ignore for Bulk transfer */

  /* Interface 1, Alternate Setting 1, Data class interface descriptor*/
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  1,                 				 /* bInterfaceNumber: Number of Interface */
  0x01,                              /* bAlternateSetting: no alternate setting */
  0x01,                              /* bNumEndpoints: one endpoint used */
  0xFF,          					 /* bInterfaceClass: Data Interface Class */
  0xFF,                              /* bInterfaceSubClass: no subclass available */
  0x00,                              /* bInterfaceProtocol: no protocol used */
  0x00,                              /* iInterface: */

  /* Endpoint, EP3 Bulk Out */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_OUT(3),               /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(64),            			 /* wMaxPacketSize */
  0x00,                              /* bInterval: ignore for Bulk transfer */

  /* Terminator */
  0                                  /* bLength */
};






/* USB String Descriptor (optional) */
uint8_t USB_StringDescriptor[] = {
/* Index 0x00: LANGID Codes */
  0x04,                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  WBVAL(0x0409), /* US English */    /* wLANGID */
/* Index 0x01: Manufacturer */
  (6*2 + 2),                        /* bLength (13 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'M',0,
  'a',0,
  'c',0,
  'p',0,
  'o',0,
  'd',0,
/* Index 0x02: Product */
  (10*2 + 2),                        /* bLength ( 17 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'L',0,
  'a',0,
  's',0,
  'e',0,
  'r',0,
  's',0,
  'h',0,
  'a',0,
  'r',0,
  'k',0,
/* Index 0x03: Serial Number */
  (16*2 + 2),                        /* bLength (12 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
};





void usb_populate_serialno() {
	int i, j, temp;
	unsigned int result_table[5];

	uint8_t* curr_stringdescriptor_ptr = USB_StringDescriptor
			+ USB_StringDescriptor[0]; // Skip header
	uint8_t iserial_stringlist_pos =
			USB_DeviceDescriptor[USB_DEVICE_DESCRIPTOR_ISERIAL_POS];

	// Jump to iSerialNumber (the start)
	for (i = 0; i < iserial_stringlist_pos - 1; i++) {
		curr_stringdescriptor_ptr += *curr_stringdescriptor_ptr;
	}

	if (*curr_stringdescriptor_ptr < 16 * 2) { // Verify the length to make sure we have enough room to write the SN
		return;
	}

	curr_stringdescriptor_ptr++;
	if (*curr_stringdescriptor_ptr != USB_STRING_DESCRIPTOR_TYPE) { // Verify this is a a string descriptor.
		return;
	}

	iap_read_serial_number(result_table);
	if (result_table[0] != IAP_CMD_SUCCESS) {
		return; // Couldn't read serial number
	}

	curr_stringdescriptor_ptr++; // We are on the first string char.
	for (i = 0; i < 4; i++) {
		for (j = 0; j < sizeof(unsigned) * 2; j++) {
			temp = (uint8_t) (result_table[1 + i] >> (4 * j)) & 0x0F;
			if (temp < 10) {
				*curr_stringdescriptor_ptr = '0' + temp;
			} else {
				*curr_stringdescriptor_ptr = 'A' + temp - 10;
			}
			curr_stringdescriptor_ptr += 2;
		}
	}
}

