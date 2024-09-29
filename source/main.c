#include <math.h>
#include <stdio.h>
#include "SDL.h"
#include "synth.h"
#include "gui.h"

static const int sampleRate = 44100;

static void playSound(void *userData, uint8_t *stream, int length) {
	Synth *synth = (Synth*)userData;
	uint16_t *output = (uint16_t*)stream;
	for (size_t i = 0; i < length/sizeof (uint16_t); ++i) {
		output[i] = stepSynth(synth);
	}
}

int main(void) {
	// Initialization.
	if (gui_setup()) {
		printf("Error setting up GUI library.\n");
		return 1;
	}

	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		printf("Error initializing SDL.\n");
		return 1;
	}
	
	// Open an audio device.
	Synth synth = {
		.operators = {
			{.frequency = 440, .level = 1000},
			{.frequency = 440*4, .level = 1},
			{.frequency = 880, .level = 3},
		},
		.patches = {
			{.operators = {2, 1, 0, SYNTH_SIZE+1}, .operatorCount = 4}
		},
		.patchCount = 1,
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

	// Create a window.
	gui_Window window = {0};
	if (gui_intializeWindow(&window, "Synth", 600, 600)) {
		printf("Error initializing window.\n");
		SDL_CloseAudioDevice(playbackDevice);
		gui_teardown();
		return 1;
	}

	// Run the gui.
	gui_setBackgroundColor(&window, 0, 0, 0, 255);
	gui_setDrawColor(&window, 255, 255, 255, 255);
	gui_setFillColor(&window, 64, 64, 64, 255);
	gui_setAccentColor(&window, 255, 0, 0, 255);
	gui_setGridSize(&window, 40, 40);
	while (gui_windowIsOpen(&window)) {
		gui_beginDrawing(&window);
			// Draw vertical and horizontal rulers.
			#if 0
				gui_setAlignment(&window, gui_CENTER);
				for (unsigned int i = 0; i < window.gridWidth; ++i)
					gui_drawRectangle(&window, i, 0, 10, 10);
				for (unsigned int i = 0; i < window.gridHeight; ++i)
					gui_drawRectangle(&window, 0, i, 10, 10);
			#endif
		gui_endDrawing(&window);
	}
	
	// Cleanup.
	SDL_CloseAudioDevice(playbackDevice);
	gui_teardown();
	// SDL_Quit();
	return 0;
}
