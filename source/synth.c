#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include "synth.h"

// Returns the alpha parameter to tune a lowpass filter to the given cutoff frequency.
static float getAlpha(float cutoffFrequency, float sampleRate) {
    float omega = 2.0f*M_PI*cutoffFrequency/sampleRate;
    return omega/(omega + 1.0f); // Simplified Butterworth approximation.
}

float stepOscillator(Oscillator *oscillator, float sampleRate) {
	float frequency = oscillator->frequency;
	float amplitude = oscillator->amplitude;
	float phase = oscillator->phase;
	float offset = oscillator->offset;
	float t = oscillator->t;
	float period = sampleRate/frequency;
	float range = 0.0f;
	float sample = 0.0f;

	// Generate the sample.
	switch (oscillator->type) {
		case SINE:
			range = 2.0f*M_PI;
			sample = amplitude*sinf(t + phase) + offset;
			break;
		case TRIANGLE:
			range = 1.0f;
			sample = (t < 0.5) ? amplitude*t + offset : amplitude*(1.0f - t) + offset;
			break;
		case SAWTOOTH:
			range = 1.0f;
			sample = amplitude*t + offset;
			break;
		case SQUARE:
			range = 1.0f;
			sample = (t < 0.5) ? offset : amplitude + offset;
			break;
	}

	// Update t.
	float step = range/period;
	oscillator->t += step;
	if (oscillator->t >= range) {
		oscillator->t = (oscillator->type == SINE) ? oscillator->t - range : 0.0f;
	}

	// Deliver the output.
	if (oscillator->outputA)
		*oscillator->outputA = sample;
	if (oscillator->outputB)
		*oscillator->outputB = sample;
	return sample;
}

float stepFilter(Filter *filter, float sampleRate) {
	float alpha = getAlpha(filter->leftCutoff, sampleRate);
	float output = alpha*filter->currentSample + (1.0f - alpha)*filter->previousSample;
	filter->previousSample = output;
	if (filter->output)
		*filter->output = output;
	return output;
}

float stepSynth(Synth *synth, float sampleRate) {
	for (size_t i = 0; i < SYNTH_SIZE; ++i) {
		stepOscillator(synth->oscillators + i, sampleRate);
	}
	for (size_t i = 0; i < SYNTH_SIZE; ++i) {
		stepFilter(synth->filters + i, sampleRate);
	}

	float output = 0.0f;
	for (size_t i = 0; i < SYNTH_SIZE; ++i) {
		output += synth->outputs[i];
	}
	return output;
}
