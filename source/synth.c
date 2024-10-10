#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include "synth.h"

#include <stdio.h>

float stepSynth(Synth *synth, float sampleRate) {
	synth->output = 0.0f;
	// Compute the outputs of the oscillators.
	for (size_t i = 0; i < oscillatorCount; ++i) {
		Oscillator *source = synth->oscillators + i;

		if (source->type == OFF) {
			continue;
		}

		float period = sampleRate/source->pitch;
		float increment = 2.0f*M_PI/period;
		if (source->type == SINE) {
			source->output = source->amplitude*sinf(source->t + source->phase) + source->offset;
			source->t += increment;
			if (source->t > 2.0f*M_PI) {
				source->t -= 2.0f*M_PI;
			}
		} else if (source->type == SQUARE) {
			source->output = (fmodf(source->t + source->phase, 2.0f*M_PI) < 2.0f*M_PI*source->pulseWidth) ? source->offset - source->amplitude/2.0f : source->offset + source->amplitude/2.0f;
			source->t += increment;
			if (source->t > 2.0f*M_PI) {
				source->t = 0.0f;
			}
		} else if (source->type == TRIANGLE) {
			float tNorm = fmodf(source->t + source->phase, 2.0f*M_PI)/2.0f/M_PI;
			source->output = (tNorm < 0.5) ? source->amplitude*tNorm/2.0f + source->offset : source->amplitude*(1.0f - tNorm)/2.0f + source->offset;
			source->t += increment;
			if (source->t > 2.0f*M_PI) {
				source->t = 0.0f;
			}
		} else if (source->type == SAWTOOTH) {
			float tNorm = (source->t + source->phase)/2.0f/M_PI;
			source->output = source->amplitude*(tNorm - 0.5f) + source->offset;
			source->t += increment;
			if (source->t > 2.0f*M_PI) {
				source->t = 0.0f;
			}
		} else if (source->type == REVERSE_SAWTOOTH) {
			float tNorm = (source->t + source->phase)/2.0f/M_PI;
			source->output = source->amplitude*(1.0f - tNorm - 0.5f) + source->offset;
			source->t += increment;
			if (source->t > 2.0f*M_PI) {
				source->t = 0.0f;
			}
		}
	}

	// Route the connections.
	for (size_t i = 0; i < connectionCount; ++i) {
		if (synth->connections[i].source == NULL) {
			break;
		}

		if (synth->connections[i].type == SET) {
			*synth->connections[i].destination = *synth->connections[i].source*synth->connections[i].level;
		} else if (synth->connections[i].type == ADD) {
			*synth->connections[i].destination += *synth->connections[i].source*synth->connections[i].level;
		} else if (synth->connections[i].type == MULTIPLY) {
			*synth->connections[i].destination *= *synth->connections[i].source*synth->connections[i].level;
		}
	}

	return synth->level*synth->output;
}
