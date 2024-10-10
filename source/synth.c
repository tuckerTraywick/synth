#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include "synth.h"

#include <stdio.h>

float stepSynth(Synth *synth, float sampleRate) {
	float synthOutput = 0.0f;
	for (size_t i = 0; i < operatorCount; ++i) {
		Operator *source = synth->operators + i;

		if (source->type == OFF) {
			continue;
		}

		// Compute the operator output.
		float period = sampleRate/source->pitch;
		float increment = 2.0f*M_PI/period;
		if (source->type == SINE) {
			source->output = source->level*sinf(source->t + source->input + source->phase);
			source->t += increment;
			if (source->t > 2.0f*M_PI) {
				source->t -= 2.0f*M_PI;
			}
		} else if (source->type == SQUARE) {
			source->output = (source->t + source->input + source->phase < 2.0f*M_PI*source->pulseWidth) ? -source->level : source->level;
			source->t += increment;
			if (source->t > 2.0f*M_PI) {
				source->t = 0.0f;
			}
		} else if (source->type == TRIANGLE) {
			float tNorm = (source->t + source->input + source->phase)/2.0f/M_PI;
			source->output = (tNorm < 0.5) ? source->level*tNorm - source->level/2.0f : source->level*(1.0f - tNorm) - source->level/2.0f;
			source->t += increment;
			if (source->t > 2.0f*M_PI) {
				source->t = 0.0f;
			}
		} else if (source->type == SAWTOOTH) {
			float tNorm = (source->t + source->input + source->phase)/2.0f/M_PI;
			source->output = tNorm*source->level - source->level/2.0f;
			source->t += increment;
			if (source->t > 2.0f*M_PI) {
				source->t = 0.0f;
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
	return synth->level*synthOutput;
}
