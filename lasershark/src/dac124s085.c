/*
dac123s085 - Lasershark firmware.
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

#include <stdint.h>
#include "dac124s085.h"
#include "ssp.h"

void dac124s085_init(void) {
	// Nothing to do.
}


__inline void dac124s085_dac(volatile const uint16_t *abcd) {
    SSPSendC16(DAC124S085_INPUT_REG_A | DAC124S085_OP_WRITE_NO_UPDATE | (DAC124S085_INPUT_REG_DATA_MASK & abcd[0])); // A
    SSPSendC16(DAC124S085_INPUT_REG_B | DAC124S085_OP_WRITE_NO_UPDATE | (DAC124S085_INPUT_REG_DATA_MASK & abcd[1])); // B
    SSPSendC16(DAC124S085_INPUT_REG_C | DAC124S085_OP_WRITE_NO_UPDATE | (DAC124S085_INPUT_REG_DATA_MASK & abcd[2])); // C
    SSPSendC16(DAC124S085_INPUT_REG_D | DAC124S085_OP_WRITE_UPDATE_OUTPUTS | (DAC124S085_INPUT_REG_DATA_MASK & abcd[2])); // D
}

__inline void dac124s085_dac_chn_set(uint16_t reg, uint16_t val, bool update_outputs) {
	if (reg != DAC124S085_INPUT_REG_A  && reg != DAC124S085_INPUT_REG_B && reg != DAC124S085_INPUT_REG_C && reg != DAC124S085_INPUT_REG_D)
	{
		return;
	}
    SSPSendC16(reg | (update_outputs ? DAC124S085_OP_WRITE_UPDATE_OUTPUTS : DAC124S085_OP_WRITE_NO_UPDATE) | (DAC124S085_INPUT_REG_DATA_MASK & val)); // D
}
