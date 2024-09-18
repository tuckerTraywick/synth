#include <math.h>
#include <stdio.h>
#include "SDL.h"
#include "synth.h"

static const int sampleRate = 44100;

static void playSound(void *userData, uint8_t *stream, int length) {
	Synth *synth = (Synth*)userData;
	uint16_t *output = (uint16_t*)stream;
	for (size_t i = 0; i < length/sizeof (uint16_t); ++i) {
		output[i] = stepSynth(synth, (float)sampleRate);
	}
}

int main(void) {
	// Initialize SDL.
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		printf("Error initializing SDL.\n");
		return 1;
	}

	// Open an audio device.
    Synth synth = {
		.oscillators[0] = {.type = SINE, .frequency = 440, .amplitude = 5000, .output = &synth.outputs[0]},
		.oscillators[1] = {.type = SINE, .frequency = 10, .amplitude = 10, .offset = 440, .output = &synth.oscillators[0].frequency},
	};
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

	// Begin playback and wait until the user quits.
	SDL_PauseAudioDevice(playbackDevice, 0);
	printf("Press enter to quit.\n");
	getchar();
	
	// Cleanup.
	SDL_CloseAudioDevice(playbackDevice);
	SDL_Quit();
	return 0;
}
