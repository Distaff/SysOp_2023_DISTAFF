#pragma once
#include <stdbool.h>

static const int grid_width = 30;
static const int grid_height = 30;
static const int grid_size = grid_height * grid_width;

char* create_grid();
void destroy_grid(char* grid);
void draw_grid(char* grid);
void init_grid(char* grid);
bool is_alive(int row, int col, char* grid);
void update_grid(char* src, char* dst);