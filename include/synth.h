#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// The number of operators.
static const size_t operatorCount = 10;

// The number of notes that can be played at once.
static const size_t voiceCount = 10;

// The number of patches between operators that can be made.
static const size_t patchCount = 20;

// Produces a wave and follows an envelope.
typedef struct Operator {
	float index;
	float level;
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

typedef enum ConnectionType {
	FM,
	AM,
} ConnectionType;

typedef struct Connection {
	size_t source;
	size_t destination;
	float level;
	ConnectionType type;
} Connection;

typedef struct Synth {
	Voice voices[voiceCount];
	size_t nextVoice;
	Connection patches[patchCount];
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
