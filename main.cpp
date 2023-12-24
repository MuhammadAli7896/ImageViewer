#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING // to silent the warning of depracation of filesystem class that in C++ 17 it is removed from experimental class
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include "LinkedList.h"
#include <experimental/filesystem> // used to extract the images from the folder

namespace fs = std::experimental::filesystem; 

// global variables for window and surface so that they can be used in any function
SDL_Window* window = nullptr;
SDL_Surface* surface = nullptr;
// LinkedList in which all image paths are stored
LinkedList<std::string> ImagePaths;
Node<std::string> *currentImg; // image path of the current image which is to be displayed
const int size_of_arrows = 50; // height and width of arrows
int windowHeight = 0;
int windowWidth = 0;
std::string imageFolder = "C:\\Users\\Dev\\Pictures"; // Path for the folder from where the images will be extracted
// Define a variable to track the zoom state
bool zoomedIn = false;
// Declare a boolean variable to track whether the cursor is over the arrows
bool cursorOverArrowLeft = false;
bool cursorOverArrowRight = false;
// Declare arrow surface pointers with global scope
SDL_Surface* arrow_left_default = nullptr;
SDL_Surface* arrow_right_default = nullptr;
SDL_Surface* arrow_left_hover = nullptr;
SDL_Surface* arrow_right_hover = nullptr;
// Declare TTF font and color
TTF_Font* font = nullptr;
SDL_Color textColor = { 161, 158, 160, 0.8 };  // Gray color
// These are used for the number of the image
int index = 0;
std::string value; // The final text that is displayed on the bottom right of screen


void LoadImagesFromFolder() {

	// Check if the specified folder exists
	if (!fs::exists(imageFolder) || !fs::is_directory(imageFolder)) {
		// If the folder doesn't exist then it will go to the subfolder which is in the same directory
		imageFolder = fs::current_path().string() + "/images"; // Use "images" as the default subfolder
		std::cout << "Specified folder not found. Using default folder path: " << imageFolder << std::endl;
	}

	// Initialize SDL_image
	int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
		return;
	}

	for (const auto& entry : fs::directory_iterator(imageFolder)) {
		if (fs::is_regular_file(entry)) {
			// Open the file to check if it's a valid image using SDL_image
			SDL_RWops* rwops = SDL_RWFromFile(entry.path().string().c_str(), "rb");
			if (rwops != nullptr) {
				// loading all types of extensions for an image
				if (IMG_isPNG(rwops) || IMG_isJPG(rwops) || IMG_isBMP(rwops) ||
					IMG_isGIF(rwops) || IMG_isICO(rwops) || IMG_isLBM(rwops) ||
					IMG_isPCX(rwops) || IMG_isPNM(rwops) || IMG_isTIF(rwops) ||
					IMG_isXCF(rwops) || IMG_isXPM(rwops) || IMG_isXV(rwops)) {
					ImagePaths.append(entry.path().string());
				}
				SDL_RWclose(rwops);
			}
		}
	}
	// Assigning current image the path of first image in the folder
	currentImg = ImagePaths.head;
	// Quit SDL_image
	IMG_Quit();
}

void LoadArrowImages() {
	// Load default arrow images
	arrow_left_default = IMG_Load("arrow_left.png");
	arrow_right_default = IMG_Load("arrow_right.png");

	// Load hover arrow images
	arrow_left_hover = IMG_Load("arrow_left_hover.jpg");
	arrow_right_hover = IMG_Load("arrow_right_hover.jpg");

	// Check if all images are loaded successfully
	if (!arrow_left_default || !arrow_right_default || !arrow_left_hover || !arrow_right_hover) {
		std::cerr << "Error loading arrow images: " << IMG_GetError() << std::endl;
		SDL_Quit();
		exit(-1);
	}
}

// Update arrows based on hover state
void UpdateArrows(SDL_Surface* window_surface) {
	SDL_Rect rect;

	// Load appropriate arrow images based on hover state
	SDL_Surface* current_left_arrow = cursorOverArrowLeft ? arrow_left_hover : arrow_left_default;
	SDL_Surface* current_right_arrow = cursorOverArrowRight ? arrow_right_hover : arrow_right_default;

	// Blit the left arrow onto the window_surface
	rect.x = 0;
	rect.y = windowHeight / 2 - (size_of_arrows / 2);
	SDL_BlitSurface(current_left_arrow, nullptr, window_surface, &rect);

	// Blit the right arrow onto the window_surface
	rect.x = windowWidth - size_of_arrows;
	SDL_BlitSurface(current_right_arrow, nullptr, window_surface, &rect);

	// Update the window surface
	SDL_UpdateWindowSurface(window);
}

void RenderText(SDL_Surface* window_surface, const std::string& text1, const std::string& text2) {
	if (font == nullptr) {
		std::cerr << "Font not loaded! Cannot render text." << std::endl;
		return;
	}

	// Render text surface
	SDL_Surface* textSurface1 = TTF_RenderText_Solid(font, text1.c_str(), textColor);
	SDL_Surface* textSurface2 = TTF_RenderText_Solid(font, text2.c_str(), textColor);

	if (textSurface1 == nullptr) {
		std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
		return;
	}

	// Create rect to hold text
	SDL_Rect textRect1, textRect2; // 1 for the image name and 2 for the image number / number of images in the folder
	textRect1.x = (windowWidth - textSurface1->w) / 2;  // Center horizontally
	textRect1.y = windowHeight - textSurface1->h - 5;  // 10 pixels from the top
	textRect1.w = textSurface1->w;
	textRect1.h = textSurface1->h;
	textRect2.x = (windowWidth - textSurface2->w) - 15; // Center horizontally
	textRect2.y = windowHeight - textSurface2->h - 5;  // 10 pixels from the top
	textRect2.w = textSurface2->w;
	textRect2.h = textSurface2->h;

	// Blit the text surface onto the window_surface
	SDL_BlitSurface(textSurface1, nullptr, window_surface, &textRect1);
	SDL_BlitSurface(textSurface2, nullptr, window_surface, &textRect2);

	// Update the window surface
	SDL_UpdateWindowSurface(window);

	// Free the text surface
	SDL_FreeSurface(textSurface1);
	SDL_FreeSurface(textSurface2);
}

bool LoadCurrentImage(SDL_Surface* window_surface)
{	
	// Check if currentImg is nullptr
	if (currentImg == nullptr) {
		std::cerr << "Error: currentImg is nullptr." << std::endl;
		return false;
	}

	surface = IMG_Load(currentImg->data.c_str());

	// Check if surface is nullptr
	if (surface == nullptr) {
		std::cerr << "Error loading image: " << IMG_GetError() << std::endl;
		return false;
	}

	SDL_Rect rect;

	// Calculate the destination rectangle position and dimensions
	int imageWidth = surface->w;
	int imageHeight = surface->h;
	float min_size = 0.5;
	float increase_size = 2.5;

	// Check if the image dimensions are greater than the screen size - 30
	if (imageWidth >= (windowWidth - 30) || imageHeight >= (windowHeight - 30)) {
		// Decrease image dimensions
		float aspectRatio = static_cast<float>(imageWidth) / static_cast<float>(imageHeight);

		if (aspectRatio >= 1.0f) {
			imageWidth = static_cast<int>(windowWidth - (100 * aspectRatio));
			imageHeight = static_cast<int>(windowHeight - (50 * aspectRatio));
		}
		else {
			imageHeight = windowHeight;
			imageWidth = static_cast<int>(imageHeight * aspectRatio);
		}
	}

	int posX = (windowWidth - imageWidth) / 2;
	int posY = (windowHeight - imageHeight) / 2;

	if (zoomedIn) {
		imageHeight *= 1.4;
		imageWidth *= 1.4;
		posX = (windowWidth - imageWidth) / 2;
		posY = (windowHeight - imageHeight) / 2;
	}

	rect.x = posX;
	rect.y = posY;
	rect.w = imageWidth;
	rect.h = imageHeight;

	if (surface != nullptr) {
		// Clear the window
		SDL_FillRect(window_surface, nullptr, SDL_MapRGB(window_surface->format, 0, 0, 0));

		// Blit the scaled image onto the window_surface
		SDL_BlitScaled(surface, nullptr, window_surface, &rect);

		// Blit the arrows onto the window_surface
		SDL_Surface* arrow_left = IMG_Load("arrow_left.png");
		SDL_Surface* arrow_right = IMG_Load("arrow_right.png");
		SDL_Surface* arrow_left_hover = IMG_Load("arrow_left_hover.jpg");
		SDL_Surface* arrow_right_hover = IMG_Load("arrow_right_hover.jpg");
		rect.x = 0;
		rect.y = windowHeight / 2 - (size_of_arrows / 2);
		if (!(cursorOverArrowLeft || cursorOverArrowRight))
		{
			// if cursor is not on the arrows render the default images of arrow
			SDL_BlitSurface(arrow_left, nullptr, window_surface, &rect);
			rect.x = windowWidth - size_of_arrows;
			SDL_BlitSurface(arrow_right, nullptr, window_surface, &rect);
		}
		else if(cursorOverArrowLeft)
		{
			// if over left arrow render the hover image of left arrow and default right arrow
			SDL_BlitSurface(arrow_left_hover, nullptr, window_surface, &rect);
			rect.x = windowWidth - size_of_arrows;
			SDL_BlitSurface(arrow_right, nullptr, window_surface, &rect);
		}
		else {
			// else render image of right hover arrow and default left arrow
			SDL_BlitSurface(arrow_left, nullptr, window_surface, &rect);
			rect.x = windowWidth - size_of_arrows;
			SDL_BlitSurface(arrow_right_hover, nullptr, window_surface, &rect);
		}
		
		// taking the index of the image and adding one to it
		value = std::to_string(index + 1);
		value = value + " / " + std::to_string(ImagePaths.length); // final text 

		// Render Text
		RenderText(window_surface, currentImg->data.substr(imageFolder.length() + 1), value); // currentImg data is sliced to remove the folder and directory address and just show image name

		//Update the window surface
		SDL_UpdateWindowSurface(window);

		// Free the images and surfaces
		SDL_FreeSurface(arrow_left);
		SDL_FreeSurface(arrow_right);
		SDL_FreeSurface(arrow_left_hover);
		SDL_FreeSurface(arrow_right_hover);
		SDL_FreeSurface(surface);

		return true;
	}
}

void on_click(SDL_MouseButtonEvent &button, SDL_Surface *window_surface)
{
    if(button.button == SDL_BUTTON_LEFT)
    {
		// for left arrow
        if (button.x >= 0 && button.x<= size_of_arrows && button.y >= (windowHeight / 2 - (size_of_arrows / 2)) && button.y <= (windowHeight / 2 + (size_of_arrows / 2))) {
			currentImg = currentImg->prev;
			index = (index - 1 + ImagePaths.length) % ImagePaths.length;
            LoadCurrentImage(window_surface);
        }
		// for right arrow
        else if (button.x >= windowWidth - size_of_arrows && button.x <= windowWidth && button.y >= (windowHeight / 2 - (size_of_arrows / 2)) && button.y <= (windowHeight / 2 + (size_of_arrows / 2))) {
			currentImg = currentImg->next;
			index = (index + 1) % ImagePaths.length;
            LoadCurrentImage(window_surface);
       }
    }
}

void on_hover(SDL_Window* window) {
	// Check if the cursor is over the arrows
	SDL_Rect leftArrowRect = {0, windowHeight / 2 - (size_of_arrows / 2), size_of_arrows, size_of_arrows };
	SDL_Rect rightArrowRect = { windowWidth - size_of_arrows, windowHeight / 2 - (size_of_arrows / 2), size_of_arrows, size_of_arrows };

	int x, y;
	// getting mouse poition
	SDL_GetMouseState(&x, &y);
	SDL_Point p;
	p.x = x;
	p.y = y;

	// checking if it is over arrows
	cursorOverArrowLeft = SDL_PointInRect(&p, &leftArrowRect);
	cursorOverArrowRight = SDL_PointInRect(&p, &rightArrowRect);

	// Set cursor to pointer if over arrows
	if (cursorOverArrowLeft || cursorOverArrowRight) {
		SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
	}
	else {
		SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
	}
}


void on_double_click(SDL_MouseButtonEvent& button, SDL_Surface* window_surface) {
	if (button.button == SDL_BUTTON_LEFT && button.clicks == 2) {
		// Toggle the zoom state if it is in the position of arrows
		if (!(button.x >= 0 && button.x <= size_of_arrows && button.y >= (windowHeight / 2 - (size_of_arrows / 2)) && button.y <= (windowHeight / 2 + (size_of_arrows / 2))) && !(button.x >= windowWidth - size_of_arrows && button.x <= windowWidth && button.y >= (windowHeight / 2 - (size_of_arrows / 2)) && button.y <= (windowHeight / 2 + (size_of_arrows / 2))))
		{
			zoomedIn = !zoomedIn;
			LoadCurrentImage(window_surface);
		}
		
	}
}


int main(int argc, char* argv[]) {

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL initialization failed: %s", SDL_GetError());
		return -1;
	}

	// Initialize SDL_ttf
	if (TTF_Init() == -1) {
		std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
		// Handle initialization error as needed
	}


	// Get the screen size
	SDL_DisplayMode dm;
	if (SDL_GetCurrentDisplayMode(0, &dm) != 0) {
		std::cerr << "SDL_GetCurrentDisplayMode failed: " << SDL_GetError() << std::endl;
		IMG_Quit();
		SDL_Quit();
		return -1;
	}

	// Load a font
	std::string FontsPath = fs::current_path().string() + "/Fonts/Inter/static/Inter-Medium.ttf";
	font = TTF_OpenFont(FontsPath.c_str(), 22); // 22 is the font size

	if (font == nullptr) {
		std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
		// Handle font loading error as needed
	}

	windowWidth = dm.w;
	windowHeight = dm.h - 60;


	// Create a window
	window = SDL_CreateWindow("Image Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
		IMG_Quit();
		SDL_Quit();
		return -1;
	}

	// Load arrow images
	LoadArrowImages();

	// Load images from a folder
	LoadImagesFromFolder();

	SDL_Surface *window_surface = SDL_GetWindowSurface(window);

	// Load the first image
	if (ImagePaths.length) {
		LoadCurrentImage(window_surface);
	}
	
	// same procedure to render the text on the first image
	// its name and index out of total number of images
	value = std::to_string(index + 1);
	value = value + " / " + std::to_string(ImagePaths.length);
	RenderText(window_surface, currentImg->data.substr(imageFolder.length() + 1), value); // currentImg data is sliced to remove the folder and directory address and just show image name

	// Main loop
	bool quit = false;
	SDL_Event e;
	while (!quit) {
		// Event handling
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_LEFT:
					// if left arrow key is pressed
					currentImg = currentImg->prev;
					index = (index - 1 + ImagePaths.length) % ImagePaths.length;
					LoadCurrentImage(window_surface);
					break;
				case SDLK_RIGHT:
					// if right arrow key is pressed
					currentImg = currentImg->next;
					index = (index + 1) % ImagePaths.length;
					LoadCurrentImage(window_surface);
					break;
				default:
					break;
				}
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN) {
				on_click(e.button, window_surface); // Check if the mouse click is within the next button area
				on_double_click(e.button, window_surface); // Check for double-click events
			}
			else if (e.type == SDL_MOUSEMOTION) {
				// Update cursor state on hover
				on_hover(window);
				// Update arrows based on hover state
				UpdateArrows(window_surface);
			}
		}
	}

	// Clean up arrow images
	SDL_FreeSurface(arrow_left_default);
	SDL_FreeSurface(arrow_right_default);
	SDL_FreeSurface(arrow_left_hover);
	SDL_FreeSurface(arrow_right_hover);

	// Close the font
	if (font != nullptr) {
		TTF_CloseFont(font);
		font = nullptr;
	}

	// Clean up
	SDL_DestroyWindow(window);
	// Quit SDL_ttf
	TTF_Quit();
	SDL_Quit();

	return 0;
}
