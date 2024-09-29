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
		if (patch->source == 0 && patch->destination == 0 && patch->level == 0) {
			break; // If we encounter an empty patch, there's no use continuing.
		}

		Operator *source = synth->operators + patch->source;
		if (patch->destination >= SYNTH_SIZE) {
			// If the destination is not a real operator, route it to the output of the synth.
			synthOutput += patch->level*source->output;
		} else {
			Operator *destination = synth->operators + patch->destination;
			destination->input += patch->level*source->output;
		}
	}

	// Compute the output of each operator.
	for (size_t i = 0; i < SYNTH_SIZE; ++i) {
		Operator *operator = synth->operators + i;
		operator->output = sinf(2*M_PI*operator->frequency*synth->t + operator->input + operator->phase) + operator->offset;
	}

	// Clear the inputs.
	// for (size_t i = 0; i < SYNTH_SIZE; ++i) {
	// 	synth->operators[i].input = 0;
	// 	synth->operators[i].output = 0;
	// }

	// // Step each patch and compute the synth's output.
	// float synthOutput = 0;
	// for (size_t i = 0; i < PATCH_SIZE; ++i) {
	// 	Patch *patch = synth->patches + i;
	// 	if (patch->source == 0 && patch->destination == 0 && patch->level == 0) {
	// 		break; // If we encounter an empty patch, there's no use continuing.
	// 	}

	// 	// Compute the output of the source.
	// 	Operator *source = synth->operators + patch->source;
	// 	source->output = sinf(2*M_PI*source->frequency*synth->t + source->input + source->phase) + source->offset;
	// 	// printf("output %zu: %f\n", patch->source, source->output);
	// 	if (patch->destination >= SYNTH_SIZE) {
	// 		// If the destination is not a real operator, route it to the output of the synth.
	// 		synthOutput += patch->level*source->output;
	// 	} else {
	// 		Operator *destination = synth->operators + patch->destination;
	// 		destination->input += patch->level*source->output;
	// 	}
	// }

	synth->t += 1.0f/sampleRate;
	if (synth->t >= 1.0f) {
		synth->t = 0.0f;
	}
	return synthOutput;
}

// float stepOscillator(Oscillator *oscillator, float sampleRate) {
// 	float frequency = oscillator->frequency;
// 	float amplitude = oscillator->amplitude;
// 	float phase = oscillator->phase;
// 	float offset = oscillator->offset;
// 	float t = oscillator->t;
// 	float period = sampleRate/frequency;
// 	float range = 0.0f;
// 	float sample = 0.0f;

// 	// Generate the sample.
// 	range = 2.0f*M_PI;
// 	sample = amplitude*sinf(t + phase) + offset;

// 	// Update t.
// 	float step = range/period;
// 	oscillator->t += step;
// 	if (oscillator->t >= range) {
// 		oscillator->t = oscillator->t - range;
// 	}
// 	return sample;
// }

// float stepSynth(Synth *synth, float sampleRate) {
// 	for (size_t i = 0; i < SYNTH_SIZE; ++i) {
// 		stepOscillator(synth->oscillators + i, sampleRate);
// 	}

// 	float output = 0.0f;
// 	for (size_t i = 0; i < SYNTH_SIZE; ++i) {
// 		output += synth->outputs[i];
// 	}
// 	return output;
// }
