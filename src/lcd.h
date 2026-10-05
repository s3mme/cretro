#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "cpu.h"

void lcd_init(void);
bool lcd_process_input(void);
void lcd_step(void const *buffer, int pitch);

#endif
