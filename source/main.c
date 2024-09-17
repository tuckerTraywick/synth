#include <math.h>
#include <stdio.h>
#include "SDL.h"
#include "synth.h"

#define arrayLength(array) (sizeof(array)/sizeof((array)[0]))

static const int sampleRate = 44100;

int main(void) {
	// Initialize SDL.
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		printf("Error initializing SDL.\n");
		return 1;
	}
	
	// Generate a sine wave sample.
	size_t sampleCapacity = 2048;
	float *samples = malloc(sampleCapacity*sizeof (float));
	size_t sampleLength = generateSineSample(440, 5000, 0, sampleRate, samples);

	// Open an audio device.
    Sample sample = {
		.length = sampleLength,
		.samples = samples,
	};
	SDL_AudioSpec audioSpec = {
		.freq = sampleRate,
		.format = AUDIO_S16,
		.channels = 1,
		.samples = 2048,
		.callback = &loopSample,
        .userdata = &sample,
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
	free(samples);
	SDL_CloseAudioDevice(playbackDevice);
	SDL_Quit();
	return 0;
}
