#include <math.h>
#include <stdio.h>
#include "SDL.h"

typedef struct SineWave {
    float phase;
} SineWave;

static const int sampleRate = 44100;

// Generates a sine wave and writes it to an audio stream.
static void sineCallback(void *userData, Uint8 *stream, int length) {
	Sint16 *output = (Sint16*)stream;
    SineWave *wave = (SineWave*)userData;
	int amplitude = 28000;
	int frequency = 440;
	for (int i = 0; i < length/2; ++i) {
		output[i] = (Sint16)(amplitude*sinf(2.0f*M_PI*wave->phase));
		wave->phase += (float)frequency/sampleRate;
		if (wave->phase >= 1.0f) {
			wave->phase -= 1.0f;
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
    SineWave wave = {.phase = 0.0f};
	SDL_AudioSpec audioSpec = {
		.freq = sampleRate,
		.format = AUDIO_S16SYS,
		.channels = 1,
		.samples = 1024,
		.callback = &sineCallback,
        .userdata = &wave,
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
