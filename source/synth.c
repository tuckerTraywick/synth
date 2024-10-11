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

		// Zero the operators' inputs.
		for (size_t j = 0; j < operatorCount; ++j) {
			voice->operators[j].input = 0.0f;
		}
		
		// Calculate the output of each operator.
		for (size_t j = 0; j < operatorCount; ++j) {
			// Compute the sine wave.
			Operator *operator = voice->operators + j;
			operator->output = sinf(operator->t + operator->input);
			
			// Update t.
			float period = sampleRate/operator->index/voice->frequency;
			float increment = 2.0f*M_PI/period;
			operator->t += increment;
			if (operator->t > 2.0f*M_PI) {
				operator->t -= 2.0f*M_PI;
			}

			// Route the output of the operator.
			if (operator->type == CARRIER) {
				voice->output += operator->level*operator->output;
			}
		}
		// Route the output of the voice.
		synth->output += voice->output;
	}
	return synth->level*synth->output;
}
