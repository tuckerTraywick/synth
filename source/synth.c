#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include "synth.h"

void startNote(Synth *synth, float pitch) {
	synth->lanes[synth->nextLane] = (Lane){.pressed = true, .pitch = pitch};
	synth->nextLane = 0;
	// Find the next free lane.
	for (size_t i = 0; i < MAX_POLYPHONY; ++i) {
		Lane *lane = synth->lanes + i;
		// If there is an open lane, use that one next.
		if (lane->pitch == 0) {
			synth->nextLane = i;
			break;
		}
		// Else, overwrite the oldest lane next.
		if (lane->t > synth->lanes[synth->nextLane].t) {
			synth->nextLane = i;
		}
	}
}

void stopNote(Synth *synth, float pitch) {
	// Find the note with the given pitch and kill it.
	for (size_t i = 0; i < MAX_POLYPHONY; ++i) {
		Lane *lane = synth->lanes + i;
		if (lane->pitch - pitch <= 0.5) {
			synth->lanes[i] = (Lane){0};
			synth->nextLane = i;
		}
	}
}

float stepSynth(Synth *synth, float step) {
	float synthOutput = 0;
	// Compute the output of each lane.
	for (size_t i = 0; i < MAX_POLYPHONY; ++i) {
		Lane *lane = synth->lanes + i;
		// Compute the output of each operator.
		for (long i = OPERATOR_COUNT - 1; i >= 0; --i) {
			Operator *source = synth->operators + i;
			
			// Compute the operator output and route it to the other operators.
			lane->operatorOutputs[i] = sinf(2*M_PI*source->octave*lane->pitch*lane->tNorm + lane->operatorInputs[i] + source->phase) + source->offset;
			for (size_t j = 0; j < OPERATOR_COUNT; ++j) {
				if (synth->patches[i][j]) {
					lane->operatorInputs[j] += synth->patches[i][j]*lane->operatorOutputs[i];
				}
			}

			// Route the operator output to the synth output.
			if (synth->patches[i][OPERATOR_COUNT]) {
				lane->output += synth->patches[i][OPERATOR_COUNT]*lane->operatorOutputs[i];
			}

			// Clear the operator's input.
			lane->operatorInputs[i] = 0;
		}
		synthOutput += lane->output;

		// Update t.
		if (lane->t >= MAX_NOTE_LENGTH) {
			*lane = (Lane){0};
			synth->nextLane = i;
		} else {
			lane->t += step;
			if (lane->tNorm >= 1) {
				lane->tNorm = 0;
			} else {
				lane->tNorm += step;
			}
		}
	}

	return synthOutput;
}
