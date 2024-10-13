#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// The number of operators.
static const size_t operatorCount = 10;

// The number of notes that can be played at once.
static const size_t voiceCount = 10;

// Produces a wave and follows an envelope.
typedef struct Operator {
	float level;
	float index;
	float offset;
	float attack;
	float sustain;
	float release;
	float fm;
	float am;
	float output;
	float oscillatorT; // [0, 2pi].
	float envelopeT; // [0, inf] in seconds.
} Operator;

typedef struct Voice {
	Operator operators[operatorCount];
	bool held; // Whether the note is being held.
	float frequency;
	float output;
} Voice;

typedef struct Synth {
	Voice voices[voiceCount];
	size_t nextVoice;
	float fmPatches[operatorCount][operatorCount + 1];
	float amPatches[operatorCount][operatorCount + 1];
	size_t patchCount;
	size_t operatorCount;
	float level;
	float modulation;
	float output;
} Synth;

float stepSynth(Synth *synth, float sampleRate);

void beginNote(Synth *synth, float frequency);

void endNote(Synth *synth, float frequency);

#endif // SYNTH_H
