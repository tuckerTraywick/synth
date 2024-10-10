#ifndef SYNTH_H
#define SYNTH_H

#include <stdbool.h>
#include <stddef.h>

// How many oscillators the synth has.
static const size_t oscillatorCount = 10;

// How many connections between components can be made.
static const size_t connectionCount = 20;

// The type of wave an oscillator produces.
typedef enum WaveType {
	OFF,
	SINE,
	SQUARE,
	TRIANGLE,
	SAWTOOTH,
	REVERSE_SAWTOOTH,
} WaveType;

// Produces wave.
typedef struct Oscillator {
	WaveType type;
	float amplitude;
	float pitch;
	float phase;
	float offset;
	float pulseWidth; // Only used by square waves.
	float t; // [0, 2pi] regardless of wave type.
	float output;
} Oscillator;

// Produces an ADSR envelope.
typedef struct Envelope {
	float attack;
	float decay;
	float sustain;
	float release;
	float t; // [0, 1].
	float output; // [0, 1].
} Envelope;

// The way a connection interacts with the desintaion.
typedef enum ConnectionType {
	SET, // Overwrite the destination with the source.
	ADD, // Add the source to the desintation.
	MULTIPLY, // Multiply the destination by the source.
} ConnectionType;

// A connection between two parts of the synth.
typedef struct Connection {
	ConnectionType type;
	float *source;
	float *destination;
	float level;
} Connection;

// A collection of oscillators and connections.
typedef struct Synth {
	Oscillator oscillators[oscillatorCount];
	Connection connections[connectionCount];
	float level;
	float output;
} Synth;

// Steps each part of the synth forward one tick and returns the output.
float stepSynth(Synth *synth, float sampleRate);

#endif // SYNTH_H
