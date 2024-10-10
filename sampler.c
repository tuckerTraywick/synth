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
	};
	assert(sample.samples && "Failed `malloc()`.");
	// TODO: Handle failed `malloc()`.
	
	float period = sampleRate/pitch;
	float increment = 2*M_PI/period;
	float t = 0;
	for (size_t i = 0; i < sampleCount; ++i) {
		sample.samples[i] = level*sinf(t + phase);
		t += increment;
		if (t > 2*M_PI) {
			t -= 2*M_PI;
		}
	}
	return sample;
}

uint16_t stepLane(Lane *lane) {
	if (lane->t < lane->sample.sampleCount) {
		uint16_t sample = lane->sample.samples[lane->t]*lane->level;
		++lane->t;
		return sample;
	}
	return 0;
}
