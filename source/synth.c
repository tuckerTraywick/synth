#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include "synth.h"

#include <stdio.h>

float stepSynth(Synth *synth, float sampleRate) {
	synth->output = 0.0f;
	// Compute the outputs of the operators.
	for (size_t i = 0; i < operatorCount; ++i) {
		Operator *source = synth->operators + i;

		if (source->type == OFF) {
			continue;
		}

		// Compute the operator output.
		float period = sampleRate/source->pitch;
		float increment = 2.0f*M_PI/period;
		if (source->type == SINE) {
			source->output = source->amplitude*sinf(source->t + source->phase) + source->offset;
			source->t += increment;
			if (source->t > 2.0f*M_PI) {
				source->t -= 2.0f*M_PI;
			}
		} else if (source->type == SQUARE) {
			source->output = (source->t + source->phase < 2.0f*M_PI*source->pulseWidth) ? source->offset - 1.0f : source->offset + 1.0f;
			source->t += increment;
			if (source->t > 2.0f*M_PI) {
				source->t = 0.0f;
			}
		} else if (source->type == TRIANGLE) {
			float tNorm = (source->t + source->phase)/2.0f/M_PI;
			source->output = (tNorm < 0.5) ? tNorm - 0.5f + source->offset : 1.0f - tNorm - 0.5f + source->offset;
			source->t += increment;
			if (source->t > 2.0f*M_PI) {
				source->t = 0.0f;
			}
		} else if (source->type == SAWTOOTH) {
			float tNorm = (source->t + source->phase)/2.0f/M_PI;
			source->output = tNorm - 0.5f + source->offset;
			source->t += increment;
			if (source->t > 2.0f*M_PI) {
				source->t = 0.0f;
			}
		}
	}

	// Route the patches.
	for (size_t i = 0; i < patchCount; ++i) {
		if (synth->patches[i].source == NULL) {
			break;
		}

		if (synth->patches[i].modulate) {
			*synth->patches[i].destination += *synth->patches[i].source*synth->patches[i].level;
		} else {
			*synth->patches[i].destination = *synth->patches[i].source*synth->patches[i].level;
		}
	}

	return synth->level*synth->output;
}
