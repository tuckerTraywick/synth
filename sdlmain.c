#include <math.h>
#include <stdio.h>
#include "SDL.h"
#include "synth.h"

// SDL callback to play the synth.
static void playSound(void *userData, uint8_t *stream, int length) {
	Synth *synth = (Synth*)userData;
	uint16_t *output = (uint16_t*)stream;
	for (int i = 0; i < length/sizeof *output; ++i) {
		output[i] = stepSynth(synth, sampleRate);
	}
}

int main(void) {
	// Initialization.
	if (SDL_Init(SDL_INIT_VIDEO) < 0 || SDL_Init(SDL_INIT_AUDIO) < 0) {
		printf("Error initializing SDL.\n");
		return 1;
	}

	Synth synth = {
		.volume = 1000.0f,
		.modulation = 1.0f,
		
		.oscillatorParameters = {
			{.type = SQUARE, .amplitude = 1.0f, .frequencyCoarse = 1.0f, .pulseWidth = 0.5f},
			{.type = TRIANGLE, .amplitude = 0.9f, .frequencyCoarse = 0.0f, .frequencyFine = 0.5f, .offset = 0.1f, .pulseWidth = 0.5f},
		},
		.oscillatorCount = 2,

		.envelopeParameters = {
			{.attack = 1.0f, .decay = 0.0f, .sustain = 1.0f, .release = 1.0f},
		},
		.envelopeCount = 1,

		.filterParameters = {
			{.cutoff = 0.0f},
		},
		.filterCount = 1,
		
		.patches = {
			{.level = 1.0f, .sourceType = OSCILLATOR, .sourceIndex = 0, .destinationType = FILTER_INPUT, .destinationIndex = 0},
			{.level = 1.0f, .sourceType = OSCILLATOR, .sourceIndex = 1, .destinationType = FILTER_CUTOFF, .destinationIndex = 0},
			{.level = 1.0f, .sourceType = FILTER, .sourceIndex = 0, .destinationType = OUTPUT, .destinationIndex = 0},
		},
		.patchCount = 3,
	};

	// Open an audio device.
	SDL_AudioSpec audioSpec = {
		.freq = sampleRate,
		.format = AUDIO_S16,
		.channels = 1,
		.samples = 2048,
		.callback = &playSound,
        .userdata = &synth,
	};
	SDL_AudioDeviceID playbackDevice = SDL_OpenAudioDevice(NULL, 0, &audioSpec, &audioSpec, 0);
	if (playbackDevice == 0) {
		printf("Error opening playback device.\n");
		SDL_Quit();
		return 1;
	}

	// Begin playback.
	SDL_PauseAudioDevice(playbackDevice, 0);

	// TODO: DO SYNTH UPDATE IN GUI LOOP INSTEAD OF CALLBACK SO CONTROLS RESPOND IMMEDIATELY.

	size_t note = startNote(&synth, 440.0f);
	printf("Press enter to stop holding the note.\n");
	getchar();
	stopNote(&synth, note);

	// Wait to quit.
	printf("Press enter to stop.\n");
	getchar();

	// Cleanup.
	SDL_CloseAudioDevice(playbackDevice);
	SDL_Quit();
	return 0;
}
