#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>
#include <stddef.h>

#define SYNTH_SIZE 8

#define PATCH_SIZE 24

typedef struct Operator {
	float multiplier;
	float frequency;
	float phase;
	float offset;
	// float attack;
	// float decay;
	// float sustain;
	// float release;
	float input;
	float output;
} Operator;

typedef struct Patch {
	size_t source;
	size_t destination;
	float level;
} Patch;

typedef struct Synth {
	Operator operators[SYNTH_SIZE];
	Patch patches[PATCH_SIZE];
	float t;
} Synth;

float stepSynth(Synth *synth, float sampleRate);

#endif // SYNTH_H
