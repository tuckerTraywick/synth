#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// The number of operators.
static const size_t operatorCount = 6;

// The number of notes that can be played at once.
static const size_t voiceCount = 12;

typedef enum OperatorType {
	MODULATOR,
	CARRIER,
	LFO,
} OperatorType;

// Produces a wave and follows an envelope.
typedef struct Operator {
	OperatorType type;
	float index;
	float level;
	float feedback;
	float attack;
	float sustain;
	float decay;
	float release;
	float input;
	float output;
	float t; // [0, 2pi].
} Operator;

typedef struct Voice {
	Operator operators[operatorCount];
	float frequency;
	float output;
} Voice;

typedef struct Synth {
	Voice voices[voiceCount];
	size_t nextVoice;
	bool patches[operatorCount][operatorCount];
	float level;
	float intensity;
	float carrierAttack;
	float carrierDecay;
	float modulatorAttack;
	float modulatorDecay;
	float lfoRate;
	float lfoDepth;
	float output;
} Synth;

float stepSynth(Synth *synth, float sampleRate);

void beginNote(Synth *synth, float frequency);

void endNote(Synth *synth, float frequency);

#endif // SYNTH_H
