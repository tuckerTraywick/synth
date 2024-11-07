#include <math.h>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// 0 <= t <= 2*pi
float f(float t) {
	return sin(t);
}

int main(void) {
	// Setup.
	InitWindow(600, 600, "test");
	SetTargetFPS(60);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
	GuiSetStyle(DEFAULT, BACKGROUND_COLOR, 0);

	// Main loop.
	while (!WindowShouldClose()) {
		int width = GetScreenWidth();
		int height = GetScreenHeight();
		BeginDrawing();
			ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
			DrawLine(0, height/2, width, height/2, WHITE);
			DrawLine(width/2, height/2 - 10, width/2, height/2 + 10, WHITE);
			DrawText("0", 0, height/2 + 5, 20, (Color){255, 0, 0, 255});
			DrawText("pi", width/2 - 10, height/2 + 5, 20, (Color){255, 0, 0, 255});
			DrawText("2pi", width - 30, height/2 + 5, 20, (Color){255, 0, 0, 255});
			for (int x = 0; x < width; ++x) {
				float t = (float)x/(float)width*2*PI;
				int y = 100*f(t);
				DrawPixel(x, height/2 - y, RAYWHITE);
			}
		EndDrawing();
	}

	// Teardown.
	CloseWindow();
	return 0;
}
