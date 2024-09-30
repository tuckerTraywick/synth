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
		output[i] = stepSynth(synth, sampleRate);
	}
}

static void drawOscillator(gui_Window *window, unsigned int x, unsigned int y, float *level) {
	gui_drawHorizontalSlider(window, level, true, x, y, 100, 15);
}

static void drawOscillators(gui_Window *window, unsigned int x, unsigned int y, float *levels) {
	gui_setAlignment(window, gui_MIDDLE_LEFT);
	gui_drawText(window, "Oscillators", x, y);
	char number[] = "0";
	for (size_t i = 0; i < SYNTH_SIZE; ++i) {
		gui_drawText(window, number, x, y + 2 + 2*i);
		drawOscillator(window, x + 2, y + 2 + 2*i, levels + i);
		number[0] += 1;
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
		.patches = {
			{0, 100, 1000},
		},
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

	float frequency = 440.0f;
	float levels[SYNTH_SIZE] = {0};//{[0 ... SYNTH_SIZE-1] = 0.5f};
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
			drawOscillators(&window, 1, 1, levels);
		gui_endDrawing(&window);

		for (size_t i = 0; i < SYNTH_SIZE; ++i) {
			synth.operators[i].frequency = ((int)(8*levels[i] + 1))*frequency;
		}
	}
	
	// Cleanup.
	SDL_CloseAudioDevice(playbackDevice);
	gui_teardown();
	// SDL_Quit();
	return 0;
}
