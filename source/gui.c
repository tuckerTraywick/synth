// TODO: Fix bounds checking for offscreen widgets.
// TODO: Add drawing borders aligned around grid spaces.
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include "gui.h"

// The default number of columns.
static unsigned int defaultGridWidth = 10;

// The default number of rows.
static unsigned int defaultGridHeight = 10;

// The width of the border around a button's text.
static unsigned int buttonBorderWidth = 2;

// The font text is rendered with.
static TTF_Font *font;

// The path to the font file.
static char *fontPath = "font.ttf";

// The point size of the font.
static unsigned int fontSize = 20;

// A sentinel value representing nothing being selected.
static SDL_Rect nothingSelected = {0, 0, 0, 0};

// A sentinel value representing the background being selected.
static SDL_Rect backgroundSelected = {-1, -1, -1, -1};

// Returns true if the mouse is in the given rectangle.
static bool inRectangle(unsigned int mouseX, unsigned int mouseY, unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
	return x <= mouseX && mouseX <= x + width && y <= mouseY && mouseY <= y + height;
}

// Returns true if two rectangles are equal.
static bool rectanglesEqual(SDL_Rect *a, SDL_Rect *b) {
	return a->x == b->x && a->y == b->y && a->w == b->w && a->h == b->h;
}

// Clamps an unsigned int between two values.
static unsigned int clamp(unsigned int x, unsigned int min, unsigned int max) {
	if (x <= min) {
		return min;
	} else if (x >= max) {
		return max;
	} else {
		return x;
	}
}

// Draws text at a pixel coordinate.
static void drawTextAndGetSize(gui_Window *window, char *text, unsigned int x, unsigned int y, int *width, int *height) {
	// Create a surface from the text.
	SDL_Surface *surface = TTF_RenderText_Solid(font, text, window->drawColor);
	if (!surface) {
		return; // TODO: Handle error.
	}
	
	// Create a texture from the surface.
	SDL_Texture *texture = SDL_CreateTextureFromSurface(window->sdlRenderer, surface);
	SDL_QueryTexture(texture, NULL, NULL, width, height);
	
	// Put the texture on the screen.
	gui_getPixelCoordinates(window, x, y, *width, *height, &x, &y);
	SDL_Rect rectangle = {x, y, *width, *height};
	SDL_RenderCopy(window->sdlRenderer, texture, NULL, &rectangle);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

bool gui_setup(void) {
	// Setup SDL.
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return true;
	}
	if (TTF_Init() < 0) {
		SDL_Quit();
		return true;
	}
	
	// Open the font.
	font = TTF_OpenFont(fontPath, fontSize);
	if (!font) {
		TTF_Quit();
		SDL_Quit();
		return true;
	}
	return false;
}

void gui_teardown(void) {
	TTF_Quit();
	SDL_Quit();
}

bool gui_intializeWindow(gui_Window *window, char *title, unsigned int width, unsigned int height) {
	*window = (gui_Window){
		.width = width,
		.height = height,
		.useGrid = true,
		.gridWidth = defaultGridWidth,
		.gridHeight = defaultGridHeight,
		.isOpen = true,
	};

	// Initialize the SDL window.
	window->sdlWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	if (!window->sdlWindow) {
		return true;
	}

	// Initialize the SDL renderer.
	window->sdlRenderer = SDL_CreateRenderer(window->sdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!window->sdlRenderer) {
		SDL_DestroyWindow(window->sdlWindow);
		return true;
	}
	return false;
}

void gui_destroyWindow(gui_Window *window) {
	SDL_DestroyWindow(window->sdlWindow);
	SDL_DestroyRenderer(window->sdlRenderer);
	*window = (gui_Window){0};
}

bool gui_windowIsOpen(gui_Window *window) {
	return window->isOpen;
}

void gui_beginDrawing(gui_Window *window) {
	// Flush the events.
	SDL_Event event = {0};
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			window->isOpen = false;
		}
	}

	// Draw the background.
	SDL_SetRenderDrawColor(
		window->sdlRenderer,
		window->backgroundColor.r,
		window->backgroundColor.g,
		window->backgroundColor.b,
		window->backgroundColor.a
	);
	SDL_RenderClear(window->sdlRenderer);
}

void gui_endDrawing(gui_Window *window) {
	// Detect clicks on the background.
	uint32_t leftDown = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT);
	if (rectanglesEqual(&window->mouseSelection, &backgroundSelected) && !leftDown) {
		window->mouseSelection = nothingSelected;
	} else if (rectanglesEqual(&window->mouseSelection, &nothingSelected) && leftDown) {
		window->mouseSelection = backgroundSelected;
	}

	// Draw the window.
	SDL_RenderPresent(window->sdlRenderer);
}

void gui_getPixelCoordinates(gui_Window *window, unsigned int gridX, unsigned int gridY, unsigned int width, unsigned int height, unsigned int *pixelX, unsigned int *pixelY) {
	if (window->useGrid) {
		*pixelX = window->width/window->gridWidth*gridX + window->width/window->gridWidth/2;
		*pixelY = window->height/window->gridHeight*gridY + window->height/window->gridHeight/2;
	} else {
		*pixelX = gridX;
		*pixelY = gridY;
	}

	if (window->alignment == gui_CENTER) {
		*pixelX -= width/2;
		*pixelY -= height/2;
	} else if (window->alignment == gui_MIDDLE_LEFT) {
		*pixelY -= height/2;
	} else if (window->alignment == gui_MIDDLE_RIGHT) {
		*pixelX -= width;
		*pixelY -= height/2;
	}
}

void gui_setCoordinateMode(gui_Window *window, bool useGrid) {
	window->useGrid = useGrid;
}

void gui_setGridSize(gui_Window *window, unsigned int gridWidth, unsigned int gridHeight) {
	window->gridWidth = gridWidth;
	window->gridHeight = gridHeight;
}

void gui_setDrawColor(gui_Window *window, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	window->drawColor = (SDL_Color){r, g, b, a};
}

void gui_setAccentColor(gui_Window *window, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	window->accentColor = (SDL_Color){r, g, b, a};
}

void gui_setFillColor(gui_Window *window, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	window->fillColor = (SDL_Color){r, g, b, a};
}

void gui_setBackgroundColor(gui_Window *window, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	window->backgroundColor = (SDL_Color){r, g, b, a};
}

void gui_setAlignment(gui_Window *window, gui_AlignmentType alignment) {
	window->alignment = alignment;
}

void gui_drawRectangle(gui_Window *window, unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
	SDL_SetRenderDrawColor(
		window->sdlRenderer,
		window->drawColor.r,
		window->drawColor.g,
		window->drawColor.b,
		window->drawColor.a
	);
	gui_getPixelCoordinates(window, x, y, width, height, &x, &y);
	SDL_Rect rectangle = {
		.x = x,
		.y = y,
		.w = width,
		.h = height,
	};
	SDL_RenderDrawRect(window->sdlRenderer, &rectangle);
}

void gui_drawAccentRectangle(gui_Window *window, unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
	SDL_SetRenderDrawColor(
		window->sdlRenderer,
		window->accentColor.r,
		window->accentColor.g,
		window->accentColor.b,
		window->accentColor.a
	);
	gui_getPixelCoordinates(window, x, y, width, height, &x, &y);
	SDL_Rect rectangle = {
		.x = x,
		.y = y,
		.w = width,
		.h = height,
	};
	SDL_RenderDrawRect(window->sdlRenderer, &rectangle);
}

void gui_drawFilledRectangle(gui_Window *window, unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
	SDL_SetRenderDrawColor(
		window->sdlRenderer,
		window->fillColor.r,
		window->fillColor.g,
		window->fillColor.b,
		window->fillColor.a
	);
	gui_getPixelCoordinates(window, x, y, width, height, &x, &y);
	SDL_Rect rectangle = {
		.x = x,
		.y = y,
		.w = width,
		.h = height,
	};
	SDL_RenderFillRect(window->sdlRenderer, &rectangle);
}

void gui_drawAccentFilledRectangle(gui_Window *window, unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
	SDL_SetRenderDrawColor(
		window->sdlRenderer,
		window->accentColor.r,
		window->accentColor.g,
		window->accentColor.b,
		window->accentColor.a
	);
	gui_getPixelCoordinates(window, x, y, width, height, &x, &y);
	SDL_Rect rectangle = {
		.x = x,
		.y = y,
		.w = width,
		.h = height,
	};
	SDL_RenderFillRect(window->sdlRenderer, &rectangle);
}

void gui_drawText(gui_Window *window, char *text, unsigned int x, unsigned int y) {
	int width = 0;
	int height = 0;
	drawTextAndGetSize(window, text, x, y, &width, &height);
}

bool gui_drawButton(gui_Window *window, char *text, unsigned int x, unsigned int y) {
	// Draw the text to get the size.
	int width = 0;
	int height = 0;
	drawTextAndGetSize(window, text, x, y, &width, &height);
	width += buttonBorderWidth;
	height += buttonBorderWidth;

	// Get the pixel coordinates of the text.
	unsigned int pixelX = 0;
	unsigned int pixelY = 0;
	gui_getPixelCoordinates(window, x, y, width, height, &pixelX, &pixelY);
	SDL_Rect boundingBox = {pixelX, pixelY, width, height};

	// Respond to the state of the mouse.
	int mouseX = 0;
	int mouseY = 0;
	uint32_t leftDown = SDL_GetMouseState(&mouseX, &mouseY) & SDL_BUTTON(SDL_BUTTON_LEFT);
	bool hovering = inRectangle(mouseX, mouseY, pixelX, pixelY, width, height);
	bool clicked = false;

	if (rectanglesEqual(&window->mouseSelection, &boundingBox)) {
		window->mouseSelection = nothingSelected;
	} else if (rectanglesEqual(&window->mouseSelection, &nothingSelected) && hovering && leftDown) {
		window->mouseSelection = boundingBox;
		clicked = true;
	}

	// Draw the border.
	if (hovering) {
		gui_drawAccentRectangle(window, x, y, width, height);
	} else {
		gui_drawRectangle(window, x, y, width, height);
	}
	return clicked;
}

bool gui_drawCheckbox(gui_Window *window, bool *value, unsigned int width, unsigned int x, unsigned int y) {
	// Get the pixel coordinates of the checkbox.
	unsigned int pixelX = 0;
	unsigned int pixelY = 0;
	gui_getPixelCoordinates(window, x, y, width, width, &pixelX, &pixelY);

	// Respond to the state of the mouse.
	int mouseX = 0;
	int mouseY = 0;
	uint32_t leftDown = SDL_GetMouseState(&mouseX, &mouseY) & SDL_BUTTON(SDL_BUTTON_LEFT);
	bool hovering = inRectangle(mouseX, mouseY, pixelX, pixelY, width, width);
	bool clicked = false;

	if (rectanglesEqual(&window->mouseSelection, &nothingSelected) && hovering && leftDown) {
		*value = !*value;
		clicked = true;
	}

	// Save the drawing state so we can draw the rectangles using the pixel coordinates instead of
	// the grid coordinates.
	bool useGrid = window->useGrid;
	window->useGrid = false;
	gui_AlignmentType alignment = window->alignment;
	window->alignment = gui_TOP_LEFT;

	// Draw the checkbox.
	if (hovering) {
		gui_drawAccentRectangle(window, pixelX, pixelY, width, width);
	} else {
		gui_drawRectangle(window, pixelX, pixelY, width, width);
	}
	if (*value) {
		gui_drawFilledRectangle(window, pixelX + 1, pixelY + 1, width - 2, width - 2);
	}

	// Restore the previous settings for drawing.
	window->useGrid = useGrid;
	window->alignment = alignment;
	return clicked;
}

bool gui_drawHorizontalSlider(gui_Window *window, float *value, bool fillLeft, unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
	// Get the pixel coordinates of the slider.
	unsigned int pixelX = 0;
	unsigned int pixelY = 0;
	gui_getPixelCoordinates(window, x, y, width, height, &pixelX, &pixelY);

	// Respond to the state of the mouse.
	SDL_Rect boundingBox = {pixelX, pixelY, pixelX + width, pixelY + height};
	int mouseX = 0;
	int mouseY = 0;
	uint32_t leftDown = SDL_GetMouseState(&mouseX, &mouseY) & SDL_BUTTON(SDL_BUTTON_LEFT);
	bool hovering = inRectangle(mouseX, mouseY, pixelX, pixelY, width, height);
	bool selected = false;
	if (rectanglesEqual(&window->mouseSelection, &boundingBox)) {
		if (leftDown) {
			float position = (float)(clamp(mouseX, pixelX, pixelX + width) - pixelX)/(float)width;
			*value = position;
			selected = true;
		} else {
			window->mouseSelection = nothingSelected;
		}
	} else if (rectanglesEqual(&window->mouseSelection, &nothingSelected) && hovering && leftDown) {
		float position = (float)(clamp(mouseX, pixelX, pixelX + width) - pixelX)/(float)width;
		*value = position;
		window->mouseSelection = boundingBox;
		selected = true;
	}

	// Save the drawing state so we can draw the rectangles using the pixel coordinates instead of
	// the grid coordinates.
	bool useGrid = window->useGrid;
	window->useGrid = false;
	gui_AlignmentType alignment = window->alignment;
	window->alignment = gui_TOP_LEFT;

	// Draw the vertical bar.
	unsigned int scale = 7;
	gui_drawAccentFilledRectangle(window, pixelX + *value * (width - width/scale), pixelY, width/scale, height);
	// Draw the shading.
	if (fillLeft) {
		gui_drawFilledRectangle(window, pixelX, pixelY, *value * (width - width/scale), height);
	}
	// Draw the border.
	if (hovering || selected) {
		gui_drawAccentRectangle(window, pixelX, pixelY, width, height);
	} else {
		gui_drawRectangle(window, pixelX, pixelY, width, height);
	}

	// Restore the previous settings for drawing.
	window->useGrid = useGrid;
	window->alignment = alignment;
	return selected;
}
