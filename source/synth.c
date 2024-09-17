#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include "synth.h"

void loopSample(void *userData, uint8_t* stream, int length) {
	Sample *sample = (Sample*)userData;
	uint16_t *output = (uint16_t*)stream;
	for (size_t i = 0; i < length/sizeof (uint16_t); ++i) {
		output[i] = sample->samples[sample->position];
		sample->position = (sample->position + 1)%sample->length;
	}
}

size_t generateSineSample(float frequency, float amplitude, float offset, uint32_t sampleRate, float *samples) {
	float period = sampleRate/frequency; // In samples.
	float step = 2.0f*M_PI/period; // [0, 2pi] is the domain.
	float t = 0.0f;
	for (size_t i = 0; i < period; ++i) {
		samples[i] = amplitude*sinf(t + offset);
		t += step;
	}
	return period;
}

size_t generateSquareSample(float frequency, float amplitude, float offset, uint32_t sampleRate, float *samples) {
	float period = sampleRate/frequency; // In samples.
	float step = 1/period; // [0, 1] is the domain.
	float t = 0.0f;
	for (size_t i = 0; i < period; ++i) {
		samples[i] = (t < 0.5) ? 0 : amplitude;
		t += step;
	}
	return period;
}

size_t generateTriangleSample(float frequency, float amplitude, float offset, uint32_t sampleRate, float *samples) {
	float period = sampleRate/frequency; // In samples.
	float step = 1/period;
	float t = 0.0f;
	for (size_t i = 0; i < period; ++i) {
		samples[i] = (t < 0.5) ? t*amplitude : (1 - t)*amplitude;
		t += step;
	}
	return period;
}

float getAlpha(float cutoffFrequency, float sampleRate) {
	// Simplified Butterworth approximation.
    float omega = 2.0f*M_PI*cutoffFrequency/sampleRate;
    return omega/(omega + 1.0f);
}

void applyLowPassFilter(float cutoffFrequency, uint32_t sampleRate, float *samples, size_t length) {
	float alpha = getAlpha(cutoffFrequency, sampleRate);
	for (size_t i = 1; i < length; ++i) {
		samples[i] = alpha*samples[i] + (1.0f - alpha)*samples[i - 1];
	}
}




// Generates a sine wave and writes it to an audio stream.
// static void sineCallback(void *userData, Uint8 *stream, int length) {
// 	int16_t *output = (int16_t*)stream;
// 	Oscillator *oscillator = (Oscillator*)userData;
// 	float frequency = 440.0;
// 	float amplitude = 1000;
// 	float step = 2*M_PI*frequency/sampleRate;
// 	// Compute the wave.
// 	for (size_t i = 0; i < length/sizeof (int16_t); ++i) {
// 		output[i] = amplitude*sinf(oscillator->t);
// 		oscillator->t += step;
// 		if (oscillator->t >= 2*M_PI) {
// 			oscillator->t -= 2*M_PI; // `-= 2pi` instead of `= 0` to preserve how far t is over 2*pi.
// 		}
// 	}
// }
