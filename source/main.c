#include <stdint.h>
#include <math.h>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

const int sampleRate = 44100;

const int audioBufferSize = 1024;

const float volume = 5000;

float t;

float a, b, c, d;

float wave[audioBufferSize];

unsigned int waveLength;

float x = 0.0;
float f(float t) {
	if (x + a > -100.0 && x + a < 100.0) {
		x += a;
	}
	return sinf(440.0*t*t*c)*sinf(123.0*t) + sinf(b*440.0*t + a*sinf(234.0*t) + 60*c*t*t)*sinf(b*123.0*t);
}

void audioCallback(void *buffer, unsigned int bufferSize) {
	uint16_t *samples = buffer;
	waveLength = bufferSize;
	for (size_t i = 0; i < bufferSize; ++i) {
		wave[i] = f(t);
		samples[i] = (float)volume*wave[i];
		float step = 2.0*PI/(float)sampleRate;
		t += step;
	}
}

int main(void) {
	// Setup audio.
	SetAudioStreamBufferSizeDefault(audioBufferSize);
	InitAudioDevice();
	AudioStream stream = LoadAudioStream(sampleRate, 16, 1);
	SetAudioStreamCallback(stream, audioCallback);
	PlayAudioStream(stream);

	// Setup window.
	InitWindow(600, 600, "Synthesizer");
	SetTargetFPS(60);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
	GuiSetStyle(DEFAULT, BACKGROUND_COLOR, 0);

	// Main loop.
	while (!WindowShouldClose()) {
		int width = GetScreenWidth();
		int height = GetScreenHeight();
		Color red = (Color){255, 0, 0, 255};

		BeginDrawing();
			ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
			DrawLine(0, height/2, width, height/2, WHITE);
			// DrawLine(width/2, 0, width/2, height, WHITE);
			// DrawText("0", 0, height/2 + 5, 20, red);
			// DrawText("pi", width/2 - 10, height/2 + 5, 20, red);
			// DrawText("2pi", width - 30, height/2 + 5, 20, red);

			a = GuiSlider((Rectangle){25, 20, 200, 13}, "a", NULL, a, -20, 20);
			b = GuiSlider((Rectangle){25, 40, 200, 13}, "b", NULL, b, 0, 1.0);
			c = GuiSlider((Rectangle){25, 60, 200, 13}, "c", NULL, c, 0, 1.0);

			int previousY = 20*wave[0];
			for (int x = 0; x < width; ++x) {
				int y = 20.0*wave[(int)((float)x/(float)width*waveLength)];
				DrawLine((x - 1), height/2 - previousY, x, height/2 - y, red);
				previousY = y;
			}
		EndDrawing();
	}

	// Teardown.
	CloseAudioDevice();
	CloseWindow();
	return 0;
}
