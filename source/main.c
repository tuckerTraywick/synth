#include <math.h>
#include <stdio.h>
#include "SDL.h"
#include "synth.h"
#include "gui.h"

static const int sampleRate = 44100;

static const float multiplier = 100;

static void playSound(void *userData, uint8_t *stream, int length) {
	Synth *synth = (Synth*)userData;
	uint16_t *output = (uint16_t*)stream;
	for (size_t i = 0; i < length/sizeof (uint16_t); ++i) {
		output[i] = multiplier*stepSynth(synth, sampleRate);
	}
}

static void drawOscillator(gui_Window *window, unsigned int x, unsigned int y, float *level) {
	gui_drawHorizontalSlider(window, level, true, x, y, 100, 15);
}

static void drawOscillators(gui_Window *window, unsigned int x, unsigned int y, float *levels, float *octaves) {
	gui_setAlignment(window, gui_MIDDLE_LEFT);
	gui_drawText(window, "Oscillator Levels", x + 1, y);
	char number[] = "1";
	for (size_t i = 0; i < SYNTH_SIZE; ++i) {
		gui_drawText(window, number, x + 3, y + 2 + 2*i);
		drawOscillator(window, x + 5, y + 2 + 2*i, levels + i);
		++number[0];
	}

	gui_drawText(window, "Oscillator Octaves", x + 17, y);
	number[0] = '1';
	for (size_t i = 0; i < SYNTH_SIZE; ++i) {
		gui_drawText(window, number, x + 3 + 17, y + 2 + 2*i);
		drawOscillator(window, x + 5 + 17, y + 2 + 2*i, octaves + i);
		++number[0];
	}
}

static void drawPatches(gui_Window *window, unsigned int x, unsigned int y, bool patches[][SYNTH_SIZE + 1]) {
	gui_setAlignment(window, gui_MIDDLE_LEFT);
	gui_drawText(window, "Patches", x + 2, y - 1);
	gui_setAlignment(window, gui_CENTER);

	char number[] = "1";
	for (size_t i = 0; i < SYNTH_SIZE; ++i) {
		gui_drawText(window, number, x - 3, y + 2*i + 3);
		gui_drawText(window, number, x + 2*i - 1, y + 1);
		++number[0];
	}
	// gui_drawText(window, "out", x - 3, y + 2*SYNTH_SIZE + 3);
	gui_drawText(window, "out", x + 2*SYNTH_SIZE - 1, y + 1);

	for (size_t i = 0; i < SYNTH_SIZE; ++i) {
		for (size_t j = 0; j <= SYNTH_SIZE; ++j) {
			gui_drawCheckbox(window, patches[i] + j, 20, x + 2*j - 1, y + 2*i + 3);
		}
	}
}

static void drawFrequency(gui_Window *window, unsigned int x, unsigned int y, float *frequency) {
	gui_setAlignment(window, gui_CENTER);
	gui_drawText(window, "Frequency", x, y);
	gui_drawHorizontalSlider(window, frequency, true, x, y + 2, 100, 15);
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
			{2, 0},
			{1, 0},
			{0, 100},
		},
		.patchCount = 3,
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

	float baseFrequency = 440;
	float unison = 0.5;
	float levels[SYNTH_SIZE] = {[0 ... SYNTH_SIZE-1] = 0.5};
	float octaves[SYNTH_SIZE] = {0};
	bool patches[][SYNTH_SIZE + 1] = {[0 ... SYNTH_SIZE+1] = {0}};
	synth.operators[0].level = 100;
	synth.operators[0].frequency = 440;

	for (size_t i = 0; i < SYNTH_SIZE; ++i) {
		synth.operators[i].frequency = (unison + 0.5)*baseFrequency + (int)(4*octaves[i])*baseFrequency;
		synth.operators[i].level = 10*levels[i];
	}

	// Run the gui.
	gui_setBackgroundColor(&window, 0, 0, 0, 255);
	gui_setDrawColor(&window, 255, 255, 255, 255);
	gui_setFillColor(&window, 90, 90, 90, 255);
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
			drawOscillators(&window, 3, 1, levels, octaves);
			// drawPatches(&window, 14, 18, patches);
			drawFrequency(&window, 19, 35, &unison);
		gui_endDrawing(&window);

		for (size_t i = 0; i < SYNTH_SIZE; ++i) {
			synth.operators[i].frequency = (unison + 0.5)*baseFrequency + (int)(4*octaves[i])*baseFrequency;
			synth.operators[i].level = 10*levels[i];
		}
	}
	
	// Cleanup.
	SDL_CloseAudioDevice(playbackDevice);
	gui_teardown();
	// SDL_Quit();
	return 0;
}
