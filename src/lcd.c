#include <stdio.h>
#include <stdbool.h>

#include "lcd.h"

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;

void lcd_init(void) {
	SDL_Init(SDL_INIT_EVERYTHING);

	window = SDL_CreateWindow("cretro - Chip 8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
							  640, 320, SDL_WINDOW_INPUT_FOCUS);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture	 = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
}

static void process_key(SDL_Event const *e, bool value) {
	switch (e->key.keysym.sym) {
		case SDLK_x:
			m.keys[0] = value;
			break;
		case SDLK_1:
			m.keys[1] = value;
			break;
		case SDLK_2:
			m.keys[2] = value;
			break;
		case SDLK_3:
			m.keys[3] = value;
			break;
		case SDLK_q:
			m.keys[4] = value;
			break;
		case SDLK_w:
			m.keys[5] = value;
			break;
		case SDLK_e:
			m.keys[6] = value;
			break;
		case SDLK_a:
			m.keys[7] = value;
			break;
		case SDLK_s:
			m.keys[8] = value;
			break;
		case SDLK_d:
			m.keys[9] = value;
			break;
		case SDLK_y:
			m.keys[0xA] = value;
			break;
		case SDLK_c:
			m.keys[0xB] = value;
			break;
		case SDLK_4:
			m.keys[0xC] = value;
			break;
		case SDLK_r:
			m.keys[0xD] = value;
			break;
		case SDLK_f:
			m.keys[0xE] = value;
			break;
		case SDLK_v:
			m.keys[0xF] = value;
			break;
		default:
			// Ignore unknown keys
			break;
	}
}

bool lcd_process_input(void) {
	SDL_Event e;
	bool running = true;

	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_QUIT:
				running = SDL_FALSE;
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_ESCAPE) {
					running = SDL_FALSE;
					break;
				}
				process_key(&e, 1);
				break;
			case SDL_KEYUP:
				process_key(&e, 0);
				break;
			default:
				// Ignore unknown key events
				break;
		}
	}

	return running;
}

void lcd_step(void const *buffer, int pitch) {
	SDL_UpdateTexture(texture, NULL, buffer, pitch);
	SDL_SetTextureColorMod(texture, 69, 170, 242);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}
