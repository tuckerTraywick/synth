#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include "synth.h"

#include <stdio.h>

float stepSynth(Synth *synth, float sampleRate) {
	float synthOutput = 0.0f;
	for (size_t i = 0; i < operatorCount; ++i) {
		Operator *source = synth->operators + i;

		if (source->level == 0) {
			continue;
		}

		// Compute the operator output.
		if (source->type == SINE) {
			float period = sampleRate/source->pitch;
			source->output = source->level*sinf(source->t + source->input + source->phase);
			source->t += 2.0f*M_PI/period;
			if (source->t > 2.0f*M_PI) {
				source->t -= 2.0f*M_PI;
			}
		} else if (source->type == SQUARE) {
			float period = sampleRate/source->pitch;
			source->output = source->level*sinf(source->t + source->input + source->phase);
			source->output = (source->output < 0) ? -source->level : source->level;
			source->t += 2.0f*M_PI/period;
			if (source->t > 2.0f*M_PI) {
				source->t =- 0.0f;
			}
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
		source->input = 0.0f;
	}

	// synth->t += 1.0f/sampleRate;
	// if (synth->t >= 1.0f) {
	// 	synth->t -= 1.0f;
	// }
	return synth->level*synthOutput;
}
