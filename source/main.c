#include <math.h>
#include <stdio.h>
#include "SDL.h"
#include "synth.h"
#include "gui.h"

const size_t sampleRate = 44100;

const float multiplier = 1000;

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
	for (size_t i = 0; i < operatorCount; ++i) {
		gui_drawText(window, number, x + 3, y + 2 + 2*i);
		drawOscillator(window, x + 5, y + 2 + 2*i, levels + i);
		++number[0];
	}

	gui_drawText(window, "Oscillator Octaves", x + 17, y);
	number[0] = '1';
	for (size_t i = 0; i < operatorCount; ++i) {
		gui_drawText(window, number, x + 3 + 17, y + 2 + 2*i);
		drawOscillator(window, x + 5 + 17, y + 2 + 2*i, octaves + i);
		++number[0];
	}
}

static void drawPatches(gui_Window *window, unsigned int x, unsigned int y, bool patches[][operatorCount + 1]) {
	gui_setAlignment(window, gui_MIDDLE_LEFT);
	gui_drawText(window, "Patches", x + 2, y - 1);
	gui_setAlignment(window, gui_CENTER);

	char number[] = "1";
	for (size_t i = 0; i < operatorCount; ++i) {
		gui_drawText(window, number, x - 3, y + 2*i + 3);
		gui_drawText(window, number, x + 2*i - 1, y + 1);
		++number[0];
	}
	gui_drawText(window, "out", x + 2*operatorCount - 1, y + 1);

	for (size_t i = 0; i < operatorCount; ++i) {
		for (size_t j = 0; j <= operatorCount; ++j) {
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
	Synth synth = {0};
	// memset(&synth, 0, sizeof (Synth));
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

	// Create a window.
	gui_Window window = {0};
	if (gui_intializeWindow(&window, "Synth", 600, 600)) {
		printf("Error initializing window.\n");
		SDL_CloseAudioDevice(playbackDevice);
		gui_teardown();
		return 1;
	}

	// Initial setup.
	synth.patches[0][operatorCount] = true;
	synth.operators[0].type = SQUARE;
	synth.operators[0].level = 1;
	synth.operators[0].pitch = 440;

	// synth.patches[1][0] = true;
	// synth.operators[1].type = SQUARE;
	// synth.operators[1].level = 1;
	// synth.operators[1].pitch = 400;
	
	// Run the gui.
	// gui_setBackgroundColor(&window, 0, 0, 0, 255);
	// gui_setDrawColor(&window, 255, 255, 255, 255);
	// gui_setFillColor(&window, 90, 90, 90, 255);
	// gui_setAccentColor(&window, 255, 0, 0, 255);
	// gui_setGridSize(&window, 40, 40);
	// while (gui_windowIsOpen(&window)) {
	// 	gui_beginDrawing(&window);
	// 		#if 0
	// 			// Draw vertical and horizontal rulers.
	// 			gui_setAlignment(&window, gui_CENTER);
	// 			for (unsigned int i = 0; i < window.gridWidth; ++i)
	// 				gui_drawRectangle(&window, i, 0, 10, 10);
	// 			for (unsigned int i = 0; i < window.gridHeight; ++i)
	// 				gui_drawRectangle(&window, 0, i, 10, 10);
	// 		#endif
	// 		drawOscillators(&window, 3, 1, levels, octaves);
	// 		drawPatches(&window, 14, 18, synth.patches);
	// 		gui_setAlignment(&window, gui_CENTER);
	// 		// if (gui_drawButton(&window, "play", 19, 35)) {
	// 		// 	startNote(&synth, 440);
	// 		// }
	// 	gui_endDrawing(&window);

	// 	// for (size_t i = 0; i < operatorCount; ++i) {
	// 	// 	synth.operators[i].octave = (int)(3*octaves[i] + 1);
	// 	// 	synth.operators[i].level = levels[i];
	// 	// }
	// }

	// size_t sampleCount = sampleRate*10;
	// uint16_t *samples = malloc(sampleCount*sizeof *samples);
	// if (!samples) {
	// 	printf("Bad allocation.\n");
	// 	return 1;
	// }

	// // Generate a sample.
	// for (size_t i = 0; i < sampleCount; ++i) {
	// 	samples[i] = multiplier*stepSynth(&synth, sampleRate);
	// }
	
	// // Queue the sample.
	// SDL_QueueAudio(playbackDevice, samples, sampleCount*sizeof *samples);

	printf("Press enter to quit.\n");
	getchar();
	
	// Cleanup.
	SDL_CloseAudioDevice(playbackDevice);
	gui_teardown();
	return 0;
}
