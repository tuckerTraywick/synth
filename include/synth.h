#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>
#include <stddef.h>

#define SYNTH_SIZE 8

// A typical ADSR envelope.
typedef struct Envelope {
	float attack;
	float decay;
	float sustain;
	float release;
	float t;
	float *output;
} Envelope;

// The type of wave an oscillator produces.
typedef enum WaveType {
	SINE,
	TRIANGLE,
	SAWTOOTH,
	SQUARE,
	// WHITE_NOISE,
} WaveType;

// Produces a wave each time it is stepped.
typedef struct Oscillator {
	WaveType type;
	float frequency;
	float amplitude;
	float phase;
	float offset;
	float t;
	float *outputA;
	float *outputB;
} Oscillator;

// The type of filtering a filter does.
typedef enum FilterType {
	LOW_PASS,
	// HIGH_PASS,
	// BAND_PASS,
} FilterType;

// A filter that alters a signal.
typedef struct Filter {
	float leftCutoff;
	float rightCutoff;
	float previousSample;
	float currentSample;
	float *output;
} Filter;

// A collection of modules that can be connected to eachother to make sounds.
typedef struct Synth {
	Envelope envelopes[SYNTH_SIZE];
	Oscillator oscillators[SYNTH_SIZE];
	Filter filters[SYNTH_SIZE];
	float outputs[SYNTH_SIZE];
} Synth;

// Steps an evnelope forward one tick. Returns the sample it generates.
float stepEnvelope(Envelope *envelope, float sampleRate);

// Steps an oscillator forward one tick. Returns the sample it generates.
float stepOscillator(Oscillator *oscillator, float sampleRate);

// Steps a filter forward one tick. Returns the sample it generates.
float stepFilter(Filter *filter, float sampleRate);

// Steps a synth forward one tick. Returns the sum of its outputs.
float stepSynth(Synth *synth, float sampleRate);

#endif // SYNTH_H
