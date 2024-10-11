#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include "synth.h"

#include <stdio.h>

static float getEnvelopeLevel(Operator *operator, float sampleRate) {
	float envelopeLength = operator->attack + operator->release;
	float output = 0.0f;
	if (operator->envelopeT < operator->attack) {
		output = operator->sustain/operator->attack*operator->envelopeT;
	} else {
		output = operator->sustain - operator->sustain/operator->release*(operator->envelopeT - operator->attack);
	}

	if (operator->envelopeT < envelopeLength) {
		operator->envelopeT += 1.0/sampleRate;
	}
	return 1.0f; // output;
}

float stepSynth(Synth *synth, float sampleRate) {
	synth->output = 0.0f;
	for (size_t i = 0; i < voiceCount; ++i) {
		Voice *voice = synth->voices + i;
		voice->output = 0.0f;

		// Calculate the output of each operator.
		for (size_t j = 0; j < operatorCount; ++j) {
			// Compute the sine wave and the output of the envelope.
			Operator *operator = voice->operators + j;
			operator->output = getEnvelopeLevel(operator, sampleRate)*(operator->level + operator->am)*sinf(operator->oscillatorT + synth->modulation*operator->fm);

			// Update the oscillator's t.
			float period = sampleRate/operator->index/voice->frequency;
			float increment = 2.0f*M_PI/period;
			operator->oscillatorT += increment;
			if (operator->oscillatorT > 2.0f*M_PI) {
				operator->oscillatorT -= 2.0f*M_PI;
			}
		}

		// Zero the operators' inputs.
		for (size_t j = 0; j < operatorCount; ++j) {
			voice->operators[j].fm = 0.0f;
			voice->operators[j].am = 0.0f;
		}
		
		// Route the output of the operators.
		for (size_t j = 0; j < patchCount; ++j) {
			Connection *patch = synth->patches + j;
			if (patch->level == 0) {
				continue;
			}
			
			Operator *source = voice->operators + patch->source;
			if (patch->destination >= operatorCount) {
				voice->output += patch->level*source->output;
				continue;
			}

			Operator *destination = voice->operators + patch->destination;
			if (patch->type == FM) {
				destination->fm = patch->level*source->output;
			} else {
				destination->am = patch->level*source->output;
			}
		}

		synth->output += voice->output;
	}
	return synth->level*synth->output;
}
