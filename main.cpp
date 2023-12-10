#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
std::vector<std::string> imagePaths;
int currentImageIndex = 0;

void LoadImagesFromFolder(const std::string& folderPath) {
	for (const auto& entry : fs::directory_iterator(folderPath)) {
		if (fs::is_regular_file(entry) && (entry.path().extension() == ".jpeg" || entry.path().extension() == ".jpg" || entry.path().extension() == ".png" || entry.path().extension() == ".jpeg")) {
			imagePaths.push_back(entry.path().string());
		}
	}
}

bool LoadCurrentImage() {
	if (currentImageIndex >= 0 && currentImageIndex < static_cast<int>(imagePaths.size())) {
		SDL_Surface* surface = IMG_Load(imagePaths[currentImageIndex].c_str());
		if (surface != nullptr) {
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);
			if (texture != nullptr) {
				// Clear previous textures
				SDL_RenderClear(renderer);
				SDL_RenderCopy(renderer, texture, nullptr, nullptr);
				SDL_RenderPresent(renderer);
				SDL_DestroyTexture(texture);
				return true;
			}
		}
	}
	return false;
}

int main(int argc, char* argv[]) {
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL initialization failed: %s", SDL_GetError());
		return -1;
	}

	// Create a window
	window = SDL_CreateWindow("Image Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window creation failed: %s", SDL_GetError());
		SDL_Quit();
		return -1;
	}

	// Create a renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer creation failed: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	// Load images from a folder
	std::string imageFolder = "C:\\Users\\Dev\\source\\repos\\Classic-Fumigation\\src\\images"; // path for the folder from where the images will be extracted
	LoadImagesFromFolder(imageFolder);

	// Load the first image
	if (!imagePaths.empty()) {
		LoadCurrentImage();
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
					LoadCurrentImage();
					break;
				case SDLK_RIGHT:
					currentImageIndex = (currentImageIndex + 1) % imagePaths.size();
					LoadCurrentImage();
					break;
				default:
					break;
				}
			}
		}
	}

	// Clean up
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
