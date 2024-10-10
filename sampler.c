#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "sampler.h"

#include <stdio.h>

Sample sineSample(float pitch, float level, float phase, float duration, float sampleRate) {
	size_t sampleCount = sampleRate*duration;
	Sample sample = {
		.sampleCount = sampleCount,
		.samples = malloc(sampleCount*sizeof *sample.samples),
		.level = 1,
	};
	assert(sample.samples && "Failed `malloc()`.");
	// TODO: Handle failed `malloc()`.
	
	float period = sampleRate/pitch;
	for (size_t i = 0; i < sampleCount; ++i) {
		float t = 2*M_PI*fmodf(i, period)/period;
		sample.samples[i] = level*sinf(t + phase);
	}
	return sample;
}

uint16_t stepLane(Lane *lane) {
	if (lane->t < lane->sample.sampleCount) {
		uint16_t sample = lane->sample.samples[lane->t]*lane->sample.level;
		++lane->t;
		return sample;
	}
	return 0;
}
