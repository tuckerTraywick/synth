#ifndef SYNTH_H
#define SYNTH_H

#include <stdbool.h>
#include <stddef.h>

static const size_t operatorCount = 4;

typedef enum WaveType {
	OFF,
	SINE,
	SQUARE,
	TRIANGLE,
	SAWTOOTH,
} WaveType;

typedef struct Operator {
	WaveType type;
	float level;
	float pitch;
	float phase;
	float offset;
	float pulseWidth;
	float input;
	float output;
	float t; // [0, 2pi].
} Operator;

typedef struct Synth {
	Operator operators[operatorCount];
	bool patches[operatorCount][operatorCount + 1];
	float level;
} Synth;

float stepSynth(Synth *synth, float sampleRate);

#endif // SYNTH_H
