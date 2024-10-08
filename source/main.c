#include <math.h>
#include <stdio.h>
#include "SDL.h"
#include "synth.h"

// The number of samples played per second.
const float sampleRate = 48000.0f;

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
		.level = 4000.0f,
		.oscillators = {
			{.type = SINE, .pitch = 440.0f, .amplitude = 1.0f, .offset = 0.0f, .pulseWidth = 0.5},
			{.type = SINE, .pitch = 220.0f, .amplitude = 1.0f, .offset = 0.0f, .pulseWidth = 0.5},
			{.type = SINE, .pitch = 440.0f, .amplitude = 1.0f, .offset = 0.0f, .pulseWidth = 0.5},
		},
		.connections = {
			{.source = &synth.oscillators[0].output, .destination = &synth.output, .level = 1.0f, .type = ADD},
			{.source = &synth.oscillators[1].output, .destination = &synth.output, .level = 1.0f, .type = MULTIPLY},
			{.source = &synth.oscillators[2].output, .destination = &synth.oscillators[0].phase, .level = 1.0f, .type = SET},
		},
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

	// Wait to quit.
	printf("Press enter to stop.\n");
	getchar();

	// Cleanup.
	SDL_CloseAudioDevice(playbackDevice);
	SDL_Quit();
	return 0;
}
