#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>
#include <stddef.h>

typedef struct Sample {
	float *samples;
	size_t length;
	size_t position;
} Sample;

// Callback function for SDL.
void loopSample(void *sample, uint8_t* stream, int length);

size_t generateSineSample(float frequency, float amplitude, float offset, uint32_t sampleRate, float *samples);

size_t generateSquareSample(float frequency, float amplitude, float offset, uint32_t sampleRate, float *samples);

size_t generateTriangleSample(float frequency, float amplitude, float offset, uint32_t sampleRate, float *samples);

void applyLowPassFilter(float cutoffFrequency, uint32_t sampleRate, float *samples, size_t length);

#endif // SYNTH_H
