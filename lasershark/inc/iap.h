/*
iap.h - Lasershark firmware.
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
#ifndef IAP_H
#define IAP_H


#define IAP_LOCATION 0x1fff1ff1

#define IAP_CMD_SUCCESS 0x00000000

#define IAP_READ_UID 58

typedef void (*IAP)(unsigned int [],unsigned int[]);


void iap_read_serial_number(unsigned int result_table[]);

#endif
