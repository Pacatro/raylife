#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCREEN_HEIGHT 900
#define SCREEN_WIDTH 1000
#define INITIAL_CAMERA_ZOOM 1.0f
#define ZOOM_SCALE 0.25f
#define GRID_SPACING 50
#define GRID_ROWS 50
#define GRID_COLS 50

// TODO: FIX GENERATIONS

typedef struct {
    int isAlive;
    Vector2 pos;
    Vector2 size;
    int aliveNeighbors;
} Cell;

Cell grid[GRID_ROWS][GRID_COLS];
int aliveCells = 0;

void initGrid() {
    for (int i = 0; i < GRID_ROWS; i++) {
        for (int j = 0; j < GRID_COLS; j++) {
            grid[i][j].isAlive = 0;
            grid[i][j].pos.x = j * GRID_SPACING;
            grid[i][j].pos.y = i * GRID_SPACING;
            grid[i][j].size.x = GRID_SPACING;
            grid[i][j].size.y = GRID_SPACING;
            grid[i][j].aliveNeighbors = 0;
        }
    }
}

void drawGrid() {
    for (int i = 0; i <= GRID_ROWS; i++) {
        Vector2 start = {0, i * GRID_SPACING};
        Vector2 end = {GRID_COLS * GRID_SPACING, i * GRID_SPACING};
        DrawLineV(start, end, GRAY);
    }

    for (int j = 0; j <= GRID_COLS; j++) {
        Vector2 start = {j * GRID_SPACING, 0};
        Vector2 end = {j * GRID_SPACING, GRID_ROWS * GRID_SPACING};
        DrawLineV(start, end, GRAY);
    }
}

Cell *createCell(int x, int y, int height, int width) {
    Cell *cell = malloc(sizeof(Cell));
    cell->isAlive = 1;
    cell->pos.x = x;
    cell->pos.y = y;
    cell->size.x = width;
    cell->size.y = height;
    cell->aliveNeighbors = 0;
    return cell;
}

int countAliveNeighbors(int x, int y) {
    int count = 0;

    // This is going to check the 8 surrounding cells
    // If any of them are alive, then the cell is alive
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;

            // Calculate the neighbor's coordinates
            int nx = x + dx;
            int ny = y + dy;

            if (nx >= 0 && nx < GRID_COLS && ny >= 0 && ny < GRID_ROWS) {
                if (grid[ny][nx].isAlive) {
                    count++;
                }
            }
        }   
    }

    return count;
}

void toggleCells(int x, int y) {
    if (x >= 0 && x < GRID_COLS && y >= 0 && y < GRID_ROWS) {
        grid[y][x].isAlive = !grid[y][x].isAlive;
        aliveCells = aliveCells + (grid[y][x].isAlive ? 1 : -1);
    }
}

void updateGrid() {
    Cell nextGrid[GRID_ROWS][GRID_COLS];
    memcpy(nextGrid, grid, sizeof(nextGrid));

    for (int i = 0; i < GRID_ROWS; i++) {
        for (int j = 0; j < GRID_COLS; j++) {
            int aliveNeighbors = countAliveNeighbors(i, j);
            
            // Game of life rules
            if (grid[i][j].isAlive) {
                if (aliveNeighbors < 2 || aliveNeighbors > 3) {
                    nextGrid[i][j].isAlive = 0;
                    aliveCells--;
                }
            } else {
                if (aliveNeighbors == 3) {
                    nextGrid[i][j].isAlive = 1;
                    aliveCells++;
                }
            }
        }
    }

    memcpy(grid, nextGrid, sizeof(grid));
}

void drawCells() {
    for(int i = 0; i < GRID_ROWS; i++) {
        for(int j = 0; j < GRID_COLS; j++) {
            if (grid[i][j].isAlive)
                DrawRectangleV(grid[i][j].pos, grid[i][j].size, WHITE);
        }
    }
}

void moveCamera(Camera2D *camera) {
    Vector2 delta = GetMouseDelta();
    delta = Vector2Scale(delta, -INITIAL_CAMERA_ZOOM / camera->zoom);
    camera->target = Vector2Add(camera->target, delta);
}

void setCamera(Camera2D *camera) {
    Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), *camera);
    camera->offset = GetMousePosition();
    camera->target = mousePos;
}

void zoomIncrement(Camera2D *camera, float *wheel) {
    float scaleFactor = INITIAL_CAMERA_ZOOM + (ZOOM_SCALE * fabsf(*wheel));
    if (*wheel < 0) scaleFactor = INITIAL_CAMERA_ZOOM / scaleFactor;
    camera->zoom = Clamp(camera->zoom*scaleFactor, 0.125f, 64.0f);
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "RayLife");

    Camera2D camera = { 0 };
    camera.zoom = INITIAL_CAMERA_ZOOM;

    int playMode = 0;

    initGrid();
    printf("Alive cells: %d\n", aliveCells);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (aliveCells == 0) playMode = 0;
        if (IsKeyPressed(KEY_SPACE)) playMode = !playMode;

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))  {
            moveCamera(&camera);
        }

        float wheel = GetMouseWheelMove();
        if (wheel){
            setCamera(&camera);
            zoomIncrement(&camera, &wheel);
        }

        BeginDrawing();
            ClearBackground(BLACK);
 
            const char *text = playMode ? "Play mode" : "Draw mode";
            DrawText(text, 10, 10, 20, WHITE);
 
            BeginMode2D(camera);
                drawGrid();
                // Draw cells
                drawCells();

                // Draw mode actions
                if (!playMode && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
                    int gridX = (int)(mousePos.x / GRID_SPACING);
                    int gridY = (int)(mousePos.y / GRID_SPACING);
                    toggleCells(gridX, gridY);
                }

                if (playMode && aliveCells > 0) {
                    updateGrid();
                }

            EndMode2D();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}