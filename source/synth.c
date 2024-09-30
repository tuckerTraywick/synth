#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include "synth.h"

float stepSynth(Synth *synth, float sampleRate) {
	float synthOutput = 0;
	for (long i = SYNTH_SIZE - 1; i >= 0; --i) {
		Operator *source = synth->operators + i;
		source->output = source->level*(sinf(2*M_PI*source->frequency*synth->t + source->input + source->phase) + source->offset);
		// Route the output to the other operators.
		for (size_t j = 0; j < SYNTH_SIZE; ++j) {
			if (synth->patches[i][j]) {
				Operator *destination = synth->operators + j;
				destination->input = source->output;
			}
		}
		// Route the output to the synth output.
		if (synth->patches[i][SYNTH_SIZE]) {
			synthOutput += source->output;
		}
	}

	synth->t += 1.0f/sampleRate;
	if (synth->t >= 1.0f) {
		synth->t = 0.0f;
	}
	return synthOutput;
}
