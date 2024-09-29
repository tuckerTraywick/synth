#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include "synth.h"

float stepSynth(Synth *synth) {
	// Clear the inputs.
	for (size_t i = 0; i < SYNTH_SIZE; ++i) {
		synth->operators[i].input = 0;
		synth->operators[i].output = 0;
	}

	// Step each patch and compute the synth's output.
	float synthOutput = 0;
	for (size_t i = 0; i < synth->patchCount; ++i) {
		Patch *patch = synth->patches + i;
		float previousOutput = 0;
		// Step each operator and compute the patch's output.
		for (size_t j = 0; j < patch->operatorCount; ++j) {
			if (patch->operators[j] >= SYNTH_SIZE) {
				synthOutput += previousOutput;
			} else {
				Operator *operator = synth->operators + patch->operators[j];
				operator->input += previousOutput;
				operator->output = operator->level*sinf(2*M_PI*operator->frequency*synth->t + operator->input + operator->phase) + operator->offset;
				previousOutput = operator->output;
			}
		}
	}

	synth->t += 1.0f/44100.0f;
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
