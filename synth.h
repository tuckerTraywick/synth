#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

static const size_t operatorCount = 4;

typedef enum WaveType {
	SINE,
	SQUARE,
} WaveType;

typedef struct Operator {
	WaveType type;
	float level;
	float pitch;
	float phase;
	float offset;
	float input;
	float output;
} Operator;

typedef struct Synth {
	Operator operators[operatorCount];
	bool patches[operatorCount][operatorCount + 1];
	size_t t;
} Synth;

float stepSynth(Synth *synth, size_t sampleRate);

#endif // SYNTH_H
