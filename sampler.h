#ifndef SAMPLE_H
#define SAMPLE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct Sample {
	size_t sampleCount;
	uint16_t *samples;
	float level;
} Sample;

typedef struct Lane {
	Sample sample;
	size_t t;
} Lane;

Sample sineSample(float pitch, float level, float phase, float duration, float sampleRate);

uint16_t stepLane(Lane *lane);

#endif // SAMPLE_H
