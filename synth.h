#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// The number of oscillators, envlopes, and filters the synth can have.
static const size_t synthSize = 8;

// The number of connections that can be made between components.
static const size_t patchCount = 20;

// The number of notes that can be played at once.
static const size_t voiceCount = 10;

// The number of samples played per second.
static const float sampleRate = 48000.0f;

// The type of wave an oscillator produces.
typedef enum WaveType {
	SINE,
	SQUARE,
	TRIANGLE,
	SAWTOOTH,
	REVERSE_SAWTOOTH,
} WaveType;

// Produces a wave and follows an envelope.
typedef struct OscillatorParameters {
	WaveType type;
	float amplitude;
	float frequencyCoarse;
	float frequencyFine;
	float phase;
	float offset;
	float pulseWidth;
} OscillatorParameters;

typedef struct Oscillator {
	float amplitude;
	float frequency;
	float phase;
	float offset;
	float pulseWidth;
	float output;
	float t;
} Oscillator;

typedef struct FilterParamters {
	float cutoff;
	float resonance;
} FilterParameters;

typedef struct Filter {
	float input;
	float cutoff;
	float resonance;
	float output;
} Filter;

typedef struct EnvelopeParameters {
	float attack;
	float decay;
	float sustain;
	float release;
} EnvelopeParameters;

typedef struct Envelope {
	float output;
	float t;
} Envelope;

typedef enum PatchSourceType {
	OSCILLATOR,
	ENVELOPE,
	FILTER,
} PatchSourceType;

typedef enum PatchDestinationType {
	OUTPUT = -1,
	OSCILLATOR_AMPLITUDE = offsetof(Oscillator, amplitude),
	OSCILLATOR_FREQUENCY = offsetof(Oscillator, frequency),
	OSCILLATOR_PHASE = offsetof(Oscillator, phase),
	OSCILLATOR_OFFSET = offsetof(Oscillator, offset),
	OSCILLATOR_PULSEWIDTH = offsetof(Oscillator, pulseWidth),
	FILTER_INPUT = 200 + offsetof(Filter, input),
	FILTER_CUTOFF = 200 + offsetof(Filter, cutoff),
} PatchDestinationType;

typedef struct Patch {
	float level;
	PatchSourceType sourceType;
	size_t sourceIndex;
	PatchDestinationType destinationType;
	size_t destinationIndex;
} Patch;

typedef struct Voice {
	Oscillator oscillators[synthSize];
	Envelope envelopes[synthSize];
	Filter filters[synthSize];
	bool held; // Whether the note is being held.
	float frequency;
	float output;
} Voice;

typedef struct Synth {
	OscillatorParameters oscillatorParameters[synthSize];
	size_t oscillatorCount;
	EnvelopeParameters envelopeParameters[synthSize];
	size_t envelopeCount;
	FilterParameters filterParameters[synthSize];
	size_t filterCount;
	Patch patches[patchCount];
	size_t patchCount;
	Voice voices[voiceCount];
	size_t nextVoice;
	float volume;
	float modulation;
	float output;
} Synth;

float stepSynth(Synth *synth, float sampleRate);

size_t startNote(Synth *synth, float frequency);

void stopNote(Synth *synth, size_t note);

#endif // SYNTH_H
