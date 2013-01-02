/*
 iap.c - Lasershark firmware.
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

#include "iap.h"

const IAP iap_entry = (IAP) IAP_LOCATION;

// First result entry is the result, last 4 are the 128 bit UID.
void iap_read_serial_number(unsigned int result_table[]) {
	unsigned int param_table[5];
	param_table[0] = IAP_READ_UID;
	iap_entry(param_table, result_table);
}

