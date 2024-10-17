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
			{.amplitude = 1.0f, .frequencyCoarse = 1.0f},
		},
		.oscillatorCount = 1,
		
		.patches = {
			{.level = 1.0f, .sourceType = OSCILLATOR, .sourceIndex = 0, .destinationType = OUTPUT, .destinationIndex = 0},
		},
		.patchCount = 1,
	};
	beginNote(&synth, 440.0f);

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

	// Wait to quit.
	printf("Press enter to stop.\n");
	getchar();

	// Cleanup.
	SDL_CloseAudioDevice(playbackDevice);
	SDL_Quit();
	return 0;
}
