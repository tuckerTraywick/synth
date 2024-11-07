#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include "synth.h"

#include <stdio.h>

// Resets an oscillator's inputs to its parameters.
static void initializeOscillator(Synth *synth, Voice *voice, size_t oscillatorIndex) {
	OscillatorParameters *parameters = synth->oscillatorParameters + oscillatorIndex;
	Oscillator *oscillator = voice->oscillators + oscillatorIndex;
	oscillator->amplitude = parameters->amplitude;
	oscillator->frequency = parameters->frequencyCoarse*voice->frequency + parameters->frequencyFine;
	oscillator->offset = parameters->offset;
	oscillator->phase = parameters->phase;
	oscillator->pulseWidth = parameters->pulseWidth;
}

// Steps an oscillator 1 tick.
static void stepOscillator(Synth *synth, Voice *voice, size_t oscillatorIndex, float sampleRate) {
	OscillatorParameters *parameters = synth->oscillatorParameters + oscillatorIndex;
	Oscillator *oscillator = voice->oscillators + oscillatorIndex;
	float period = sampleRate/oscillator->frequency;
	float increment = 2.0f*M_PI/period;
	switch (parameters->type) {
		case SINE:
			oscillator->output = oscillator->amplitude*sinf(oscillator->t + oscillator->phase) + oscillator->offset;
			oscillator->t += increment;
			if (oscillator->t > 2.0f*M_PI) {
				oscillator->t -= 2.0f*M_PI;
			}
			break;
		case SQUARE:
			oscillator->output = (oscillator->t < 2.0*M_PI*oscillator->pulseWidth) ? -oscillator->amplitude : oscillator->amplitude;
			oscillator->t += increment;
			if (oscillator->t > 2.0f*M_PI) {
				oscillator->t = 0.0f;
			}
			break;
		case TRIANGLE:
			oscillator->output = (oscillator->t < M_PI) ? oscillator->amplitude*oscillator->t/M_PI : oscillator->amplitude*(2.0f*M_PI - oscillator->t)/M_PI;
			oscillator->t += increment;
			if (oscillator->t > 2.0f*M_PI) {
				oscillator->t = 0.0f;
			}
			break;
		case SAWTOOTH:
			oscillator->output = oscillator->t/2.0/M_PI;
			oscillator->t += increment;
			if (oscillator->t > 2.0f*M_PI) {
				oscillator->t = 0.0f;
			}
			break;
		case REVERSE_SAWTOOTH:
			oscillator->output = (2.0f*M_PI - oscillator->t)/2.0/M_PI;
			oscillator->t += increment;
			if (oscillator->t > 2.0f*M_PI) {
				oscillator->t = 0.0f;
			}
			break;
		default:
			assert(false && "Invalid enum value.");
	}
}

// Steps an evnelope 1 tick.
static void stepEnvelope(Synth *synth, Voice *voice, size_t envelopeIndex, float sampleRate) {
	EnvelopeParameters *parameters = synth->envelopeParameters + envelopeIndex;
	Envelope *envelope = voice->envelopes + envelopeIndex;
	if (voice->held) {
		// Attack phase.
		if (envelope->t < parameters->attack) {
			envelope->output += 1.0f/parameters->attack/sampleRate;
			envelope->t += 1.0f/sampleRate;
		// Decay phase.
		} else if (envelope->t < parameters->attack + parameters->decay) {
			if (parameters->decay != 0.0f) {
				envelope->output -= (1.0f - parameters->sustain)/parameters->decay/sampleRate;
			}
			envelope->t += 1.0f/sampleRate;
		// Sustain phase.
		} else {
			envelope->output = parameters->sustain;
		}
	} else if (envelope->t != 0.0f) {
		// If the note was just released, move t to the release phase.
		if (envelope->t < parameters->attack + parameters->decay) {
			// envelope->output = parameters->sustain;
			envelope->t = parameters->attack + parameters->decay;
		// If the output hasn't reached zero, continue releasing.
		} else if (envelope->output > 0.0f) {
			envelope->output -= parameters->sustain/parameters->release/sampleRate;
			envelope->t += 1.0f/sampleRate;
		// Else, the envelope is done so reset it.
		} else {
			envelope->output = 0.0f;
			envelope->t = 0.0f;
		}
	}
}

// Resets a filter's inputs.
static void initializeFilter(Synth *synth, Voice *voice, size_t filterIndex) {
	FilterParameters *parameters = synth->filterParameters + filterIndex;
	Filter *filter = voice->filters + filterIndex;
	filter->cutoff = parameters->cutoff;
	filter->resonance = parameters->resonance;
	filter->input = 0.0f;
}

// Steps a filter 1 tick.
static void stepFilter(Voice *voice, size_t filterIndex) {
	Filter *filter = voice->filters + filterIndex;
	filter->output = filter->cutoff*filter->output + (1.0f - filter->cutoff)*filter->input;
}

// Adds or multiplies the input of a patch's destination with the value of the output of a patch's
// source, depending on the type of source.
// TODO: Make this function route the oscillator patches first, then the envelope patches so inputs
// get added then multiplied.
static void routePatch(Voice *voice, Patch *patch, float modulation) {
	// Find the destination first.
	float *destination = NULL;
	switch (patch->destinationType) {
		case OUTPUT:
			destination = &voice->output;
			break;
		case OSCILLATOR_AMPLITUDE ... OSCILLATOR_PULSEWIDTH:
			// Add the right offset to point to the right field of the oscillator.
			destination = (float*)((char*)(voice->oscillators + patch->destinationIndex) + patch->destinationType);
			break;
		case FILTER_INPUT ... FILTER_CUTOFF:
			// Add the right offset to point to the right field of the filter.
			destination = (float*)((char*)(voice->filters + patch->destinationIndex) + patch->destinationType - 200);
			break;
		default:
			assert(false && "Invalid enum value.");
	}

	// Route the source to the destination.
	switch (patch->sourceType) {
		case OSCILLATOR:
			*destination += ((patch->destinationType == OUTPUT) ? 1.0f : modulation)*patch->level*voice->oscillators[patch->sourceIndex].output;
			break;
		case ENVELOPE:
			*destination *= patch->level*voice->envelopes[patch->sourceIndex].output;
			break;
		case FILTER:
			*destination += ((patch->destinationType == OUTPUT) ? 1.0f : modulation)*patch->level*voice->filters[patch->sourceIndex].output;
			break;
		default:
			assert(false && "Invalid enum value.");
	}
}

float stepSynth(Synth *synth, float sampleRate) {
	synth->output = 0.0f;
	for (size_t voiceIndex = 0; voiceIndex < voiceCount; ++voiceIndex) {
		Voice *voice = synth->voices + voiceIndex;
		voice->output = 0.0f;

		// Skip dead voices.
		if (voice->frequency == 0.0f) {
			continue;
		}

		// Step the components and reset their inputs.
		for (size_t componentIndex = 0; componentIndex < synthSize; ++componentIndex) {
			if (componentIndex < synth->oscillatorCount) {
				stepOscillator(synth, voice, componentIndex, sampleRate);
				initializeOscillator(synth, voice, componentIndex);
			}

			if (componentIndex < synth->envelopeCount) {
				stepEnvelope(synth, voice, componentIndex, sampleRate);
			}

			if (componentIndex < synth->filterCount) {
				stepFilter(voice, componentIndex);
				initializeFilter(synth, voice, componentIndex);
			}
		}

		// Route the patches.
		for (size_t patchIndex = 0; patchIndex < synth->patchCount; ++patchIndex) {
			routePatch(voice, synth->patches + patchIndex, synth->modulation);
		}

		synth->output += voice->output;
	}
	return synth->volume*synth->output;
}

size_t startNote(Synth *synth, float frequency) {
	Voice *voice = synth->voices + synth->nextVoice;
	*voice = (Voice){.frequency = frequency, .held = true};
	// Initialize the components.
	for (size_t componentIndex = 0; componentIndex < synthSize; ++componentIndex) {
		initializeOscillator(synth, voice, componentIndex);
	}
	synth->nextVoice = (synth->nextVoice + 1)%voiceCount;
	return voice - synth->voices;
}

void stopNote(Synth *synth, size_t noteIndex) {
	synth->voices[noteIndex].held = false;
}
