#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include "synth.h"

#include <stdio.h>

float stepSynth(Synth *synth, float sampleRate) {
	synth->output = 0.0f;
	for (size_t i = 0; i < voiceCount; ++i) {
		Voice *voice = synth->voices + i;
		voice->output = 0.0f;

		if (voice->frequency == 0.0f) {
			continue;
		}

		// Step the envelopes.
		for (size_t j = 0; j < synth->envelopeCount; ++j) {
			Envelope *envelope = voice->envelopes + j;
			if (voice->held) {
				// Attack phase.
				if (envelope->t < envelope->attack) {
					envelope->output += envelope->level/envelope->attack/sampleRate;
					envelope->t += 1.0f/sampleRate;
				// Decay phase.
				} else if (envelope->t < envelope->attack + envelope->decay) {
					if (envelope->decay != 0.0f) {
						envelope->output -= (envelope->level - envelope->sustain)/envelope->decay/sampleRate;
					}
					envelope->t += 1.0f/sampleRate;
				// Sustain phase.
				} else {
					envelope->output = envelope->sustain;
				}
			} else if (envelope->t != 0.0f) {
				// If the note was just released, move t to the release phase.
				if (envelope->t < envelope->attack + envelope->decay) {
					// envelope->output = envelope->sustain;
					envelope->t = envelope->attack + envelope->decay;
				// If the output hasn't reached zero, continue releasing.
				} else if (envelope->output > 0.0f) {
					envelope->output -= envelope->sustain/envelope->release/sampleRate;
					envelope->t += 1.0f/sampleRate;
				// Else, the envelope is done so reset it.
				} else {
					envelope->output = 0.0f;
					envelope->t = 0.0f;
				}
			}
		}

		// Step the oscillators.
		for (size_t j = 0; j < synth->oscillatorCount; ++j) {
			// Compute the sine wave and the output of the envelope.
			Oscillator *operator = voice->oscillators + j;
			operator->output = (operator->level + synth->modulation*operator->am)*sinf(operator->t + synth->modulation*operator->fm);

			// Update the oscillator's t.
			float period = sampleRate/operator->index/(voice->frequency + operator->frequencyOffset);
			float increment = 2.0f*M_PI/period;
			operator->t += increment;
			if (operator->t > 2.0f*M_PI) {
				operator->t -= 2.0f*M_PI;
			}
		}

		// Zero the oscillators' inputs.
		for (size_t j = 0; j < synth->oscillatorCount; ++j) {
			voice->oscillators[j].fm = 0.0f;
			voice->oscillators[j].am = 0.0f;
			voice->oscillators[j].frequencyOffset = 0.0f;
		}
		
		// Do oscillator frequency modulation.
		for (size_t j = 0; j < synth->oscillatorCount; ++j) {
			for (size_t k = 0; k < synth->oscillatorCount; ++k) {
				if (synth->fmPatches[j][k] != 0.0f) {
					Oscillator *source = voice->oscillators + j;
					Oscillator *destination = voice->oscillators + k;
					destination->fm += synth->fmPatches[j][k]*source->output;
				}
			}
		}
		for (size_t j = 0; j < synth->oscillatorCount; ++j) {
			if (synth->fmPatches[j][oscillatorCount] != 0.0f) {
				Oscillator *source = voice->oscillators + j;
				voice->output += source->output;
			}
		}

		// Do oscillator amplitude modulation.
		for (size_t j = 0; j < synth->oscillatorCount; ++j) {
			for (size_t k = 0; k < synth->oscillatorCount; ++k) {
				if (synth->amPatches[j][k] != 0.0f) {
					Oscillator *source = voice->oscillators + j;
					Oscillator *destination = voice->oscillators + k;
					destination->am += synth->amPatches[j][k]*source->output;
				}
			}
		}
		for (size_t j = 0; j < synth->oscillatorCount; ++j) {
			if (synth->amPatches[j][oscillatorCount] != 0.0f) {
				Oscillator *source = voice->oscillators + j;
				voice->output += synth->amPatches[j][oscillatorCount]*source->output;
			}
		}

		// Do envelope amplitude modulation.
		for (size_t j = 0; j < synth->envelopeCount; ++j) {
			for (size_t k = 0; k < synth->oscillatorCount; ++k) {
				if (synth->envelopeAmPatches[j][k] != 0.0f) {
					Envelope *envelope = voice->envelopes + j;
					Oscillator *oscillator = voice->oscillators + k;
					oscillator->level = synth->envelopeAmPatches[j][k]*envelope->output;
				}
			}
		}

		// Do envelope frequency modulation.
		for (size_t j = 0; j < synth->envelopeCount; ++j) {
			for (size_t k = 0; k < synth->oscillatorCount; ++k) {
				if (synth->envelopeFmPatches[j][k] != 0.0f) {
					Envelope *envelope = voice->envelopes + j;
					Oscillator *oscillator = voice->oscillators + k;
					oscillator->frequencyOffset += synth->envelopeFmPatches[j][k]*envelope->output;
				}
			}
		}

		synth->output += voice->output;
	}
	return (synth->level*synth->output >= synth->level) ? synth->level : synth->level*synth->output;
}
