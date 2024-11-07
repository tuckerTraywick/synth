#ifndef SYNTH_H
#define SYNTH_H

#include <stddef.h>
#include <stdbool.h>

const size_t synthSize = 4;

const size_t voiceCount = 16;

typedef enum WaveType {
	SINE,
	SQUARE,
	TRIANGLE,
	SAWTOOTH,
	REVERSE_SAWTOOTH,
} WaveType;

typedef struct OscillatorParameters {
	WaveType type;
	float frequencyCoarse;
	float frequencyFine;
	float amplitude;
	float pulseWidth;
	float follow;
} OscillatorParameters;

typedef struct Oscillator {
	float amplitudeModulation;
	float frequencyModulation;
	float pulseWidthModulation;
	float output;
	float t;
} Oscillator;

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

typedef struct FilterParameters {
	float cutoff;
	float resonance;
} FilterParameters;

typedef struct Filter {
	float input;
	float cutoff;
	float resonance;
	float output;
	float t;
	float preivousOutput;
} Filter;

typedef struct Voice {
	bool held;
	float note;
	float output;
	Oscillator oscillators[synthSize];
	Envelope envelopes[synthSize];
	Filter filter;
} Voice;

typedef struct Synth {
	float volume;
	float modulation;
	float output;
	OscillatorParameters oscillatorParameters[synthSize];
	EnvelopeParameters envelopeParameters[synthSize];
	FilterParameters filterParamters;
	Voice voices[voiceCount];	
} Synth;

float stepOscillator(Oscillator *oscillator, float sampleRate);

float stepEnvelope(Envelope *envelope, float sampleRate);

float stepFilter(Filter *filter, float sampleRate);

float stepSynth(Synth *synth, float sampleRate);

void beginNote(Synth *synth, float pitch);

void endNote(Synth *synth, float pitch);

#endif // SYNTH_H
