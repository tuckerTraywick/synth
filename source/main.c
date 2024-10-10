#include <math.h>
#include <stdio.h>
#include "SDL.h"

const float sampleRate = 48000;

const float multiplier = 1000;

static void playSound(void *userData, uint8_t *stream, int length) {
	float *t = (float*)userData;
	uint16_t *output = (uint16_t*)stream;
	float frequency = 440;
	float period = sampleRate/frequency;
	float increment = 2*M_PI/period;
	for (int i = 0; i < length/sizeof *output; ++i) {
		output[i] = multiplier*sinf(*t);
		*t += increment;
		if (*t > 2*M_PI) {
			*t -= 2*M_PI;
		}
	}
}

int main(void) {
	// Initialization.
	if (SDL_Init(SDL_INIT_VIDEO) < 0 || SDL_Init(SDL_INIT_AUDIO) < 0) {
		printf("Error initializing SDL.\n");
		return 1;
	}

	// Open an audio device.
	float t = 0;
	SDL_AudioSpec audioSpec = {
		.freq = sampleRate,
		.format = AUDIO_S16,
		.channels = 1,
		.samples = 2048,
		.callback = &playSound,
        .userdata = &t,
	};
	SDL_AudioDeviceID playbackDevice = SDL_OpenAudioDevice(NULL, 0, &audioSpec, &audioSpec, 0);
	if (playbackDevice == 0) {
		printf("Error opening playback device.\n");
		SDL_Quit();
		return 1;
	}

	// Begin playback.
	SDL_PauseAudioDevice(playbackDevice, 0);

	SDL_Delay(2000);

	// Cleanup.
	SDL_CloseAudioDevice(playbackDevice);
	SDL_Quit();
	return 0;
}
