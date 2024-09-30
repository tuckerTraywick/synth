#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include "synth.h"

#include <stdio.h>


float stepSynth(Synth *synth, float sampleRate) {
	// Zero the inputs.
	for (size_t i = 0; i < SYNTH_SIZE; ++i) {
		synth->operators[i].input = 0;
	}

	// Route the sources to the destinations.
	float synthOutput = 0;
	for (size_t i = 0; i < PATCH_SIZE; ++i) {
		Patch *patch = synth->patches + i;
		Operator *source = synth->operators + patch->source;
		if (patch->destination >= SYNTH_SIZE) {
			// If the destination is not a real operator, route it to the output of the synth.
			synthOutput += source->output;
		} else {
			Operator *destination = synth->operators + patch->destination;
			destination->input += source->output;
		}
	}

	// Compute the output of each operator.
	for (size_t i = 0; i < SYNTH_SIZE; ++i) {
		Operator *operator = synth->operators + i;
		operator->output = operator->level*(sinf(2*M_PI*operator->frequency*synth->t + operator->input + operator->phase) + operator->offset);
	}

	synth->t += 1.0f/sampleRate;
	if (synth->t >= 1.0f) {
		synth->t = 0.0f;
	}
	return synthOutput;
}
