#include <math.h>
#include <stdio.h>
#include "SDL.h"

typedef struct Oscillator {
	double t;
} Oscillator;

static const int sampleRate = 44100;

// Generates a sine wave and writes it to an audio stream.
static void sineCallback(void *userData, Uint8 *stream, int length) {
	Sint16 *output = (Sint16*)stream;
	Oscillator *oscillator = (Oscillator*)userData;
	double frequency = 440.0;
	double amplitude = 1000;
	double step = 2*M_PI*frequency/sampleRate;
	// Compute the wave.
	for (int i = 0; i < length/sizeof (Sint16); ++i) {
		output[i] = amplitude*sinf(oscillator->t);
		oscillator->t += step;
		if (oscillator->t >= 2*M_PI) {
			oscillator->t -= 2*M_PI; // `-= 2pi` instead of `= 0` to preserve how far t is over 2*pi.
		}
	}
}

int main(void) {
	// Initialize SDL.
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		printf("Error initializing SDL.\n");
		return 1;
	}
	
	// Open an audio device.
    Oscillator oscillator = {.t=0.0};
	SDL_AudioSpec audioSpec = {
		.freq = sampleRate,
		.format = AUDIO_S16,
		.channels = 1,
		.samples = 2048,
		.callback = &sineCallback,
        .userdata = &oscillator,
	};
	SDL_AudioDeviceID playbackDevice = SDL_OpenAudioDevice(NULL, 0, &audioSpec, &audioSpec, 0);
	if (playbackDevice == 0) {
		printf("Error opening playback device.\n");
		SDL_Quit();
		return 1;
	}

	// Begin playback and wait until the user quits.
	SDL_PauseAudioDevice(playbackDevice, 0);
	printf("Press Enter to quit.\n");
	getchar();
	
	// Cleanup.
	SDL_CloseAudioDevice(playbackDevice);
	SDL_Quit();
	return 0;
}
