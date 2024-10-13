#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include "synth.h"

#include <stdio.h>

// static float getEnvelopeLevel(Operator *operator, float sampleRate, bool held) {
// 	float envelopeLength = operator->attack + operator->release;
// 	float output = 0.0f;
// 	if (operator->envelopeT <= operator->attack) {
// 		output = operator->sustain/operator->attack*operator->envelopeT;
// 	} else if (held) {
// 		return operator->sustain;
// 	} else {
// 		output = operator->sustain - operator->sustain/operator->release*(operator->envelopeT - operator->attack);
// 	}

// 	if (operator->envelopeT < envelopeLength) {
// 		operator->envelopeT += 1.0/sampleRate;
// 	}
// 	return output;
// }

float stepSynth(Synth *synth, float sampleRate) {
	synth->output = 0.0f;
	for (size_t i = 0; i < voiceCount; ++i) {
		Voice *voice = synth->voices + i;
		voice->output = 0.0f;

		if (voice->frequency == 0.0f) {
			continue;
		}

		// Calculate the output of each operator.
		for (size_t j = 0; j < synth->operatorCount; ++j) {
			// Compute the sine wave and the output of the envelope.
			Operator *operator = voice->operators + j;
			operator->output = (operator->level + synth->modulation*operator->am)*sinf(operator->oscillatorT + synth->modulation*operator->fm);

			// Update the oscillator's t.
			float period = sampleRate/operator->index/voice->frequency;
			float increment = 2.0f*M_PI/period;
			operator->oscillatorT += increment;
			if (operator->oscillatorT > 2.0f*M_PI) {
				operator->oscillatorT -= 2.0f*M_PI;
			}
		}

		// Zero the operators' inputs.
		for (size_t j = 0; j < synth->operatorCount; ++j) {
			voice->operators[j].fm = 0.0f;
			voice->operators[j].am = 0.0f;
		}
		
		// Do frequency modulation.
		for (size_t j = 0; j < synth->operatorCount; ++j) {
			for (size_t k = 0; k < synth->operatorCount; ++k) {
				if (synth->fmPatches[j][k] != 0.0f) {
					Operator *source = voice->operators + j;
					Operator *destination = voice->operators + k;
					destination->fm += synth->fmPatches[j][k]*source->output;
				}
			}
		}
		for (size_t j = 0; j < synth->operatorCount; ++j) {
			if (synth->fmPatches[j][operatorCount] != 0.0f) {
				Operator *source = voice->operators + j;
				voice->output += source->output;
			}
		}

		// Do amplitude modulation.
		for (size_t j = 0; j < synth->operatorCount; ++j) {
			for (size_t k = 0; k < synth->operatorCount; ++k) {
				if (synth->amPatches[j][k] != 0.0f) {
					Operator *source = voice->operators + j;
					Operator *destination = voice->operators + k;
					destination->am += synth->amPatches[j][k]*source->output;
				}
			}
		}
		for (size_t j = 0; j < synth->operatorCount; ++j) {
			if (synth->amPatches[j][operatorCount] != 0.0f) {
				Operator *source = voice->operators + j;
				voice->output += synth->amPatches[j][operatorCount]*source->output;
			}
		}

		synth->output += voice->output;
	}
	return synth->level*synth->output;
}
