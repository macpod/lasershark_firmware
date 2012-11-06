/*
dac123s085.h - Lasershark firmware.
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
#ifndef DAC124S085_H
#define DAC124S085_H

#include <stdint.h>
#include <stdbool.h>

#define DAC124S085_INPUT_REG_DATA_MASK 0x0FFF

#define DAC124S085_INPUT_REG_A 0x0000
#define DAC124S085_INPUT_REG_B 0x4000
#define DAC124S085_INPUT_REG_C 0x8000
#define DAC124S085_INPUT_REG_D 0xC000

#define DAC124S085_OP_WRITE_NO_UPDATE 		0x0000
#define DAC124S085_OP_WRITE_UPDATE_OUTPUTS 	0x1000
#define DAC124S085_OP_WRITE_ALL_UPDATE		0x2000
#define DAC124S085_OP_POWER_DOWN 			0x3000

#define DAC124S085_DAC_VAL_MIN 0
#define DAC124S085_DAC_VAL_MID 0x7FF
#define DAC124S085_DAC_VAL_MAX 0xFFF

void dac124s085_init(void);
__inline void dac124s085_dac(volatile const uint16_t *abcd);
__inline void dac124s085_dac_chn_set(uint16_t reg, uint16_t val, bool update_outputs);
#endif

