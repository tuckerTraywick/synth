#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define SYNTH_SIZE 6

// #define PATCH_SIZE 24

typedef struct Operator {
	float frequency;
	float phase;
	float offset;
	// float attack;
	// float decay;
	// float sustain;
	// float release;
	float level;
	float input;
	float output;
} Operator;

typedef struct Synth {
	Operator operators[SYNTH_SIZE];
	bool patches[SYNTH_SIZE][SYNTH_SIZE + 1];
	float t;
} Synth;

float stepSynth(Synth *synth, float sampleRate);

#endif // SYNTH_H
