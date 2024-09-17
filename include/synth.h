#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>
#include <stddef.h>

// The data passed to the `loopSample()` function to keep track of where in the sample it is.
typedef struct Sample {
	float *samples;
	size_t length;
	size_t position;
} Sample;

// Callback function for SDL.
void loopSample(void *sample, uint8_t* stream, int length);

// Generates a single sine wave oscillation.
size_t generateSineSample(float frequency, float amplitude, float offset, uint32_t sampleRate, float *samples);

// Generates a single square wave oscillation.
size_t generateSquareSample(float frequency, float amplitude, float offset, uint32_t sampleRate, float *samples);

// Generates a single triangle wave oscillation.
size_t generateTriangleSample(float frequency, float amplitude, float offset, uint32_t sampleRate, float *samples);

// Generates a single sawtooth wave oscillation.
size_t generateSawtoothSample(float frequency, float amplitude, float offset, uint32_t sampleRate, float *samples);

// Applies a lowpass filter to a sample.
void applyLowPassFilter(float cutoffFrequency, uint32_t sampleRate, float *samples, size_t length);

#endif // SYNTH_H
