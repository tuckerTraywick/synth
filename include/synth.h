#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define OPERATOR_COUNT 6

#define MAX_POLYPHONY 24

#define MAX_NOTE_LENGTH 1

typedef struct Operator {
	float level;
	float octave;
	float phase;
	float offset;
	float attack; // In seconds.
	float decay; // In seconds.
	float sustain;
	float release; // In seconds.
	float envelopeLength; // In seconds.
} Operator;

typedef struct Lane {
	bool pressed;
	float pitch; // In Hz.
	float t;
	float tNorm; // t normalized between 0 and 1.
	float output;
	float operatorInputs[OPERATOR_COUNT];
	float operatorOutputs[OPERATOR_COUNT];
} Lane;

typedef struct Synth {
	Operator operators[OPERATOR_COUNT];
	bool patches[OPERATOR_COUNT][OPERATOR_COUNT + 1];
	Lane lanes[MAX_POLYPHONY];
	size_t nextLane;
} Synth;

void startNote(Synth *synth, float pitch);

void stopNote(Synth *synth, float pitch);

float stepSynth(Synth *synth, float step);

#endif // SYNTH_H
