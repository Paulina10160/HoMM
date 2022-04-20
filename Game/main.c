#include <stdio.h>
#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "SDL_image.h"

// Structure that contains data about the object texture, position, speed and its size
struct ObjectData {
	SDL_Texture *texture;
	int positionX;
	int positionY;
	int width;
	int height;
	int speed; // speed is the amount of pixels that the object will travel in a single frame
};
typedef struct ObjectData ObjectData; // Creating shortcut for the struct that is declared above

// Global variables that contains window size 
int WINDOW_WIDTH = 1920;
int WINDOW_HEIGHT = 1080;

int main()
{
	// Init SDL libraries
	SDL_SetMainReady(); // Just leave it be
	int result = 0;
	result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO); // Init of the main SDL library
	if (result) // SDL_Init returns 0 (false) when everything is OK
	{
		printf("Can't initialize SDL. Error: %s", SDL_GetError()); // SDL_GetError() returns a string (as const char*) which explains what went wrong with the last operation
		return -1;
	}

	result = IMG_Init(IMG_INIT_PNG); // Init of the Image SDL library. We only need to support PNG for this project
	if (!(result & IMG_INIT_PNG)) // Checking if the PNG decoder has started successfully
	{
		printf("Can't initialize SDL image. Error: %s", SDL_GetError());
		return -1;
	}

	// Creating the window 1920x1080 (could be any other size)
	SDL_Window* window = SDL_CreateWindow("FirstSDL",
		0, 0,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN);

	if (!window)
		return -1;

	// Setting the window to fullscreen 
	if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) < 0) {
		printf("Could not set up the fullscreen\n");
		return -1;
	}

	// Creating a renderer which will draw things on the screen
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
		return -1;

	// Setting the color of an empty window (RGBA). You are free to adjust it.
	SDL_SetRenderDrawColor(renderer, 20, 150, 39, 255);

	// Loading an image
	char image_path[] = "image.png";
	// Here the surface is the information about the image. It contains the color data, width, height and other info.
	SDL_Surface* surface = IMG_Load(image_path);
	if (!surface)
	{
		printf("Unable to load an image %s. Error: %s", image_path, IMG_GetError());
		return -1;
	}

	// Now we use the renderer and the surface to create a texture which we later can draw on the screen.
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture)
	{
		printf("Unable to create a texture. Error: %s", SDL_GetError());
		return -1;
	}

	// Creating the game object (car)
	ObjectData car;
	car.texture = texture;
	car.positionX = WINDOW_WIDTH / 2;
	car.positionY = WINDOW_HEIGHT / 2;
	car.width = surface->w;
	car.height = surface->h;
	car.speed = 1;

	// Creating the destiny position for the car image (once the mouse button is clicked those variables will change)
	int carDestinyX = car.height;
	int carDestinyY = car.width;

	// Bye-bye the surface
	SDL_FreeSurface(surface);

	int done = 0;
	SDL_Event sdl_event;

	// The main loop
	// Every iteration is a frame
	while (!done)
	{
		// Polling the messages from the OS.
		// That could be key downs, mouse movement, ALT+F4 or many others
		while (SDL_PollEvent(&sdl_event))
		{
			if (sdl_event.type == SDL_QUIT) // The user wants to quit
			{
				done = 1;
			}
			else if (sdl_event.type == SDL_KEYDOWN) // A key was pressed
			{
				switch (sdl_event.key.keysym.sym) // Which key?
				{
				case SDLK_ESCAPE: { // Posting a quit message to the OS queue so it gets processed on the next step and closes the game
					SDL_Event event;
					event.type = SDL_QUIT;
					event.quit.type = SDL_QUIT;
					event.quit.timestamp = SDL_GetTicks();
					SDL_PushEvent(&event); 
				}
				break;
				// Other keys here
				default:
					break;
				}
			}
			else if (sdl_event.type == SDL_MOUSEBUTTONDOWN) { // Mouse button was clicked
				switch (sdl_event.button.button) { // Check the type of button clicked
				case SDL_BUTTON_LEFT: { // Left mouse button
					// Get the current position of the mouse and update the destiny position for the car
					SDL_GetMouseState(&carDestinyX, &carDestinyY);
				}
				break;
				default: // any other button
					break;
				}
			}
		}

		// Clearing the screen
		SDL_RenderClear(renderer);

		// All drawing goes here

		// Here is the rectangle where the image will be on the screen
		SDL_Rect rect;
		rect.x = (int)round(car.positionX - car.width / 2); // Counting from the image's center but that's up to you
		rect.y = (int)round(car.positionY - car.height / 2); // Counting from the image's center but that's up to you
		rect.w = (int)car.width;
		rect.h = (int)car.height;

		SDL_RenderCopyEx(renderer, // Already know what is that
			car.texture, // The image
			0, // A rectangle to crop from the original image. Since we need the whole image that can be left empty (nullptr [0])
			&rect, // The destination rectangle on the screen.
			0, // An angle in degrees for rotation
			0, // The center of the rotation (when nullptr [0], the rect center is taken)
			SDL_FLIP_NONE); // We don't want to flip the image

		// Showing the screen to the player
		SDL_RenderPresent(renderer);

		// Prepare the next frame. In our case we need to move the current car position to get closer to the destiny car position
		// Check whether the X current and destiny position differes (it can only be moved if the difference between destiny and current X position is bigger or equal to car.speed
		// otherwise we would notice weird movement for the car - to see that weird movement replace the car.speed with 0 in the if statement)
		if (abs(car.positionX - carDestinyX) >= car.speed) { // abs() returns the absolute value
			// if different then move the car to get closer to its destiny postion
			if (car.positionX > carDestinyX) {
				car.positionX -= car.speed;
			}
			else {
				car.positionX += car.speed;
			}
		}
		// Check whether the Y current and destiny position differes
		if (abs(car.positionY - carDestinyY) >= car.speed) {
			if (car.positionY > carDestinyY) {
				car.positionY -= car.speed;
			}
			else {
				car.positionY += car.speed;
			}
		}
	}

	// If we reached here then the main loop stoped
	// That means the game wants to quit

	// Shutting down the renderer
	SDL_DestroyRenderer(renderer);

	// Shutting down the window
	SDL_DestroyWindow(window);

	// Quitting the Image SDL library
	IMG_Quit();
	// Quitting the main SDL library
	SDL_Quit();

	// Done.
	return 0;
}