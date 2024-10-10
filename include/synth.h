#ifndef SYNTH_H
#define SYNTH_H

#include <stdbool.h>
#include <stddef.h>

static const size_t operatorCount = 10;

static const size_t patchCount = 20;

typedef enum WaveType {
	OFF,
	SINE,
	SQUARE,
	TRIANGLE,
	SAWTOOTH,
} WaveType;

typedef struct Operator {
	WaveType type;
	float amplitude;
	float pitch;
	float phase;
	float offset;
	float pulseWidth; // Only used by square waves.
	float t; // [0, 2pi] regardless of wave type.
	float output;
} Operator;

typedef struct Patch {
	float *source;
	float *destination;
	float level;
	bool modulate; // Adds the source to the destination when true, overwrites the destination with the source when false.
} Patch;

typedef struct Synth {
	Operator operators[operatorCount];
	Patch patches[patchCount];
	float level;
	float output;
} Synth;

float stepSynth(Synth *synth, float sampleRate);

#endif // SYNTH_H
