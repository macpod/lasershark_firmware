/*
lasershark_3d_printer.h - Lasershark firmware component for 3d printers.
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

#ifndef LASERSHARK_3D_PRINTER_H_
#define LASERSHARK_3D_PRINTER_H_
#include <stdint.h>



//===================================================
#define LASERSHARK_3D_PRINTER_CMD_START_RANGE 0xA0

#define LASERSHARK_3D_PRINTER_CMD_STEPPER_1_STEP_TOWARDS_HOME 0XA0
#define LASERSHARK_3D_PRINTER_CMD_STEPPER_1_STEP_AWAY_FROM_HOME 0XA1
#define LASERSHARK_3D_PRINTER_CMD_STEPPER_2_STEP_TOWARDS_HOME 0XA2
#define LASERSHARK_3D_PRINTER_CMD_STEPPER_2_STEP_AWAY_FROM_HOME 0XA3

#define LASERSHARK_3D_PRINTER_CMD_STEPPER_HOME 0XA4
#define LASERSHARK_3D_PRINTER_CMD_STEPPER_HOME_STEPPER_1 0x01
#define LASERSHARK_3D_PRINTER_CMD_STEPPER_HOME_STEPPER_2 0x02

#define LASERSHARK_3D_PRINTER_CMD_GET_R1 0xA5
#define LASERSHARK_3D_PRINTER_CMD_GET_R2 0xA6

#define LASERSHARK_3D_PRINTER_CMD_SET_STEPPER_1_HOME_DIR 0XA7
#define LASERSHARK_3D_PRINTER_CMD_SET_STEPPER_2_HOME_DIR 0XA8

#define LASERSHARK_3D_PRINTER_STEPPER_SET_SET_DELAY_MS 0XA9

#define LASERSHARK_3D_PRINTER_CMD_END_RANGE 0xA9
//===================================================


#define LASERSHARK_3D_PRINTER_STEPPER_STEP_DELAY_MS_MIN 1
#define LASERSHARK_3D_PRINTER_STEPPER_STEP_DELAY_MS_MAX 10000000


// CTS
#define LASERSHARK_3D_PRINTER_STEPPER_1_STEP_PORT 0
#define LASERSHARK_3D_PRINTER_STEPPER_1_STEP_PIN 7
// TX
#define LASERSHARK_3D_PRINTER_STEPPER_1_DIR_PORT 1
#define LASERSHARK_3D_PRINTER_STEPPER_1_DIR_PIN 7

// RX
#define LASERSHARK_3D_PRINTER_STEPPER_2_STEP_PORT 1
#define LASERSHARK_3D_PRINTER_STEPPER_2_STEP_PIN 6
// RTS
#define LASERSHARK_3D_PRINTER_STEPPER_2_DIR_PORT 1
#define LASERSHARK_3D_PRINTER_STEPPER_2_DIR_PIN 5


// The following pins do not have internal pullup resistors.
// They must be pulled up with external resistors for this to function properly.
// SDA
#define LASERSHARK_3D_PRINTER_R1_PORT 0
#define LASERSHARK_3D_PRINTER_R1_PIN 5
// SCL
#define LASERSHARK_3D_PRINTER_R2_PORT 0
#define LASERSHARK_3D_PRINTER_R2_PIN 4


void lasershark_3dprinter_init();

void lasershark_process_3d_printer_command();

#endif /* LASERSHARK_3D_PRINTER_H_ */
