#ifndef GUI_H
#define GUI_H

#include <stdbool.h>
#include <stdint.h>
#include <SDL.h>

// Indicates how to align widgets in a window.
typedef enum gui_AlignmentType {
	gui_TOP_LEFT,
	// gui_TOP_RIGHT,
	// gui_BOTTOM_LEFT,
	// gui_BOTTOM_RIGHT,
	gui_MIDDLE_LEFT,
	gui_MIDDLE_RIGHT,
	// gui_MIDDLE_TOP,
	// gui_MIDDLE_BOTTOM,
	gui_CENTER,
} gui_AlignmentType;

// A window that can be drawn to. Must be initialized with `gui_initializeWindow()` and destroyed
// with `gui_destroyWindow()`.
typedef struct gui_Window {
	SDL_Window *sdlWindow;
	SDL_Renderer *sdlRenderer;
	SDL_Rect mouseSelection; // The bounds of the widget the mouse is currently interacting with.
	SDL_Color backgroundColor; // The color of the background.
	SDL_Color drawColor; // The primary line and text color.
	SDL_Color accentColor; // The secondary line and text color.
	SDL_Color fillColor; // The color to fill shapes with.
	unsigned int width; // The width of the window in pixels.
	unsigned int height; // The height of the window in pixels.
	gui_AlignmentType alignment; // How to align widgets with their coordinates.
	bool useGrid; // Whether to draw widgets on a grid or with raw coordinates.
	unsigned int gridWidth; // The number of columns in the grid.
	unsigned int gridHeight; // The number of rows in the grid.
	bool isOpen; // Whether the window is still open.
} gui_Window;

// Initializes the global state of the library. Sets up SDL.
bool gui_setup(void);

// Deinitializes the global state of the library. Quits SDL.
void gui_teardown(void);

// Initializes a window. Returns true if an error occurred, false if successful.
bool gui_intializeWindow(gui_Window *window, char *title, unsigned int width, unsigned int height);

// Destroys a window and frees its resources.
void gui_destroyWindow(gui_Window *window);

// Returns true if the window is still open.
bool gui_windowIsOpen(gui_Window *window);

// Starts drawing.
void gui_beginDrawing(gui_Window *window);

// Ends drawing.
void gui_endDrawing(gui_Window *window);

// Translates grid coordinates into pixel coordinates.
void gui_getPixelCoordinates(gui_Window *window, unsigned int gridX, unsigned int gridY, unsigned int width, unsigned int height, unsigned int *pixelX, unsigned int *pixelY);

// Uses a grid when passed true, uses raw coordinates when passed false.
void gui_setCoordinateMode(gui_Window *window, bool useGrid);

// Sets the dimensions of the grid.
void gui_setGridSize(gui_Window *window, unsigned int gridWidth, unsigned int gridHeight);

// Set the color for drawing lines and text.
void gui_setDrawColor(gui_Window *window, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

// Set the accent color for lines and filling shapes.
void gui_setAccentColor(gui_Window *window, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

// Set the color for filling shapes.
void gui_setFillColor(gui_Window *window, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

// Sets the color of the window's background.
void gui_setBackgroundColor(gui_Window *window, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

// Center widgets when passed true, draw widgets starting at top left corner when passed false.
void gui_setAlignment(gui_Window *window, gui_AlignmentType alignment);

// Draws an unfilled rectangle using the draw color.
void gui_drawRectangle(gui_Window *window, unsigned int x, unsigned int y, unsigned int width, unsigned int height);

// Draws an unfilled rectangle using the accent color.
void gui_drawAccentRectangle(gui_Window *window, unsigned int x, unsigned int y, unsigned int width, unsigned int height);

// Draws a filled rectangle using the fill color.
void gui_drawFilledRectangle(gui_Window *window, unsigned int x, unsigned int y, unsigned int width, unsigned int height);

// Draws a filled rectangle using the accent color.
void gui_drawAccentFilledRectangle(gui_Window *window, unsigned int x, unsigned int y, unsigned int width, unsigned int height);

// Draws a single line of text.
void gui_drawText(gui_Window *window, char *text, unsigned int x, unsigned int y);

// Draws a button. Returns true if the button was pressed.
bool gui_drawButton(gui_Window *window, char *text, unsigned int x, unsigned int y);

// Draws a checkbox. Returns true if the box's state changed. The status of the box is returned in
// `value`.
bool gui_drawCheckbox(gui_Window *window, bool *value, unsigned int width, unsigned int x, unsigned int y);

// Draws a horizontal slider. The position of the slider is returned in `value` as a decimal between
// 0 and 1. Returns true if the slider is currently selected.
bool gui_drawHorizontalSlider(gui_Window *window, float *value, bool fillLeft, unsigned int x, unsigned int y, unsigned int width, unsigned int height);

#endif // GUI_H
