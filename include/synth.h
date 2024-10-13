#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// The number of oscillators.
static const size_t oscillatorCount = 6;

// The number of enveloeps.
static const size_t envelopeCount = 8;

// The number of notes that can be played at once.
static const size_t voiceCount = 10;

// Produces a wave and follows an envelope.
typedef struct Oscillator {
	float level;
	float index;
	float offset;
	float fm;
	float am;
	float output;
	float t; // [0, 2pi].
} Oscillator;

typedef struct Envelope {
	float attack;
	float decay;
	float sustain;
	float release;
	float output; // [0, 1].
	float t;
} Envelope;

typedef struct Voice {
	Oscillator oscillators[oscillatorCount];
	Envelope envelopes[envelopeCount];
	bool held; // Whether the note is being held.
	float frequency;
	float output;
} Voice;

typedef struct Synth {
	Voice voices[voiceCount];
	size_t nextVoice;
	size_t oscillatorCount;
	size_t envelopeCount;
	float fmPatches[oscillatorCount][oscillatorCount + 1];
	float amPatches[oscillatorCount][oscillatorCount + 1];
	float envelopeAmPatches[envelopeCount][oscillatorCount + 1];
	float envelopeFmPatches[envelopeCount][oscillatorCount + 1];
	float level;
	float modulation;
	float output;
} Synth;

float stepSynth(Synth *synth, float sampleRate);

void beginNote(Synth *synth, float frequency);

void endNote(Synth *synth, float frequency);

#endif // SYNTH_H
