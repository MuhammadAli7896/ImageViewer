#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING // Don't remove this line pls comment this
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <experimental/filesystem> // Don't remove this line pls comment this
//#include <filesystem>

namespace fs = std::experimental::filesystem; // Don't remove this line pls comment this
//namespace fs = std::filesystem;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
std::vector<std::string> imagePaths;
int currentImageIndex = 0;
const int windowWidth = 800;  // Adjust the window width
const int windowHeight = 600; // Adjust the window height
const int size_of_arrows = 50;
const int gap_in_x = 7;
const int gap_in_y = 0;
std::string imageFolder = "C:\\Users\\Dev\\source\\repos\\Classic-Fumigation\\src\\images"; // path for the folder from where the images will be extracted

void LoadImagesFromFolder() {
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
					imagePaths.push_back(entry.path().string());
				}
				SDL_RWclose(rwops);
			}
		}
	}

	// Quit SDL_image
	IMG_Quit();
}


bool LoadCurrentImage(SDL_Surface *window_surface) {

if (currentImageIndex >= 0 && currentImageIndex < static_cast<int>(imagePaths.size())) {
		SDL_Surface* surface = IMG_Load(imagePaths[currentImageIndex].c_str());
		SDL_Rect rect;
		rect.x = rect.y = 0;
        rect.w = windowWidth; rect.h = windowHeight;
		if (surface != nullptr) {
            SDL_BlitScaled(surface, NULL, window_surface, &rect);
//          SDL_BlitSurface(surface, NULL, window_surface, &rect);
            SDL_UpdateWindowSurface(window);
//			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
//			SDL_FreeSurface(surface);
//			if (texture != nullptr) {
//				// Clear previous textures
//				SDL_RenderClear(renderer);
//				SDL_RenderCopy(renderer, texture, nullptr, nullptr);
//				SDL_RenderPresent(renderer);
//				SDL_DestroyTexture(texture);
//				return true;
//			}
		}
	}
    SDL_Surface *arrow_left = IMG_Load("arrow_left.jpg");
	SDL_Surface *arrow_right = IMG_Load("arrow_right.jpg");
	SDL_Rect rect;
	rect.x = gap_in_x; rect.y = windowHeight / 2 - (size_of_arrows / 2);
	SDL_BlitSurface(arrow_left, NULL, window_surface, &rect);
	rect.x = windowWidth - size_of_arrows - gap_in_x;
	SDL_BlitSurface(arrow_right, NULL, window_surface, &rect);
	SDL_UpdateWindowSurface(window);
	return false;
}

void on_click(SDL_MouseButtonEvent &button, SDL_Surface *window_surface)
{
    if(button.button == SDL_BUTTON_LEFT)
    {
        if (button.x >= gap_in_x && button.x<= gap_in_x + size_of_arrows && button.y >= (windowHeight / 2 - (size_of_arrows / 2)) && button.y <= (windowHeight / 2 + (size_of_arrows / 2))) {
            currentImageIndex = (currentImageIndex - 1 + imagePaths.size()) % imagePaths.size();
            LoadCurrentImage(window_surface);
        }
        else if (button.x >= windowWidth - size_of_arrows && button.x <= windowWidth && button.y >= (windowHeight / 2 - (size_of_arrows / 2)) && button.y <= (windowHeight / 2 + (size_of_arrows / 2))) {
            currentImageIndex = (currentImageIndex + 1) % imagePaths.size();
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

	// Create a window
	window = SDL_CreateWindow("Image Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window creation failed: %s", SDL_GetError());
		SDL_Quit();
		return -1;
	}


//	// Create a renderer
//	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
//	if (renderer == nullptr) {
//		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer creation failed: %s", SDL_GetError());
//		SDL_DestroyWindow(window);
//		SDL_Quit();
//		return -1;
//	}

	// Load images from a folder
	LoadImagesFromFolder();

	SDL_Surface *window_surface = SDL_GetWindowSurface(window);

	// Load the first image
	if (!imagePaths.empty()) {
		LoadCurrentImage(window_surface);
	}

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
					currentImageIndex = (currentImageIndex - 1 + imagePaths.size()) % imagePaths.size();
					LoadCurrentImage(window_surface);
					break;
				case SDLK_RIGHT:
					currentImageIndex = (currentImageIndex + 1) % imagePaths.size();
					LoadCurrentImage(window_surface);
					break;
				default:
					break;
				}
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN) {
				on_click(e.button, window_surface); // Check if the mouse click is within the next button area
			}
		}
	}

	// Clean up
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
