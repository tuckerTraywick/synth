#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>
#include <stddef.h>

#define SYNTH_SIZE 8

#define PATCH_SIZE (2*SYNTH_SIZE)

typedef struct Operator {
	float frequency;
	float phase;
	float offset;
	// float attack;
	// float decay;
	// float sustain;
	// float release;
	float level;
	// float feedback;
	float input;
	float output;
} Operator;

typedef struct Patch {
	size_t operators[PATCH_SIZE];
	size_t operatorCount;
} Patch;

typedef struct Synth {
	Operator operators[SYNTH_SIZE];
	Patch patches[SYNTH_SIZE];
	size_t patchCount;
	float t;
} Synth;

float stepSynth(Synth *synth);

#endif // SYNTH_H
