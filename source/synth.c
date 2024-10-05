#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include "synth.h"

#include <stdio.h>

float stepSynth(Synth *synth, size_t sampleRate) {
	float synthOutput = 0;
	for (size_t i = 0; i < operatorCount; ++i) {
		Operator *source = synth->operators + i;

		if (source->level == 0) {
			continue;
		}

		// Compute the operator output.
		size_t period = sampleRate/source->pitch;
		size_t t = synth->t%period;
		if (source->type == SINE) {
			source->output = source->level*sinf(2*M_PI*t/period + source->input + source->phase) + source->offset;
		} else if (source->type == SQUARE) {
			source->output = source->level*sinf(2*M_PI*t/period + source->input + source->phase) + source->offset;
			if (source->output < 0) {
				source->output = -source->level;
			} else if (source->output > 0) {
				source->output = source->level;
			}
			// source->output = (t < period/2) ? source->level : -source->level;
		}

		// Route the output to the other operators.
		for (size_t j = 0; j < operatorCount; ++j) {
			if (synth->patches[i][j]) {
				synth->operators[j].input += source->output;
			}
		}

		// Route the output to the synth output.
		if (synth->patches[i][operatorCount]) {
			synthOutput += source->output;
		}

		// Clear the operator's input.
		source->input = 0;
	}

	++synth->t;
	if (synth->t >= sampleRate) {
		synth->t = 0;
	}
	return synthOutput;
}
