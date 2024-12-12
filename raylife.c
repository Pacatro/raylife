#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#define SCREEN_HEIGHT 900
#define SCREEN_WIDTH 1000
#define INITIAL_CAMERA_ZOOM 1.0f
#define ZOOM_SCALE 0.25f
#define GRID_SPACING 50
#define GRID_ROWS 100
#define GRID_COLS 100
#define INIT_INTERVAL 0.2f
#define MAX_GENERATIONS 1000

typedef struct {
    int isAlive;
    Vector2 pos;
    Vector2 size;
} Cell;

Cell grid[GRID_ROWS][GRID_COLS];
int aliveCells = 0;
int generation = 0;

void initGrid() {
    for (int i = 0; i < GRID_ROWS; i++) {
        for (int j = 0; j < GRID_COLS; j++) {
            grid[i][j].isAlive = 0;
            grid[i][j].pos.x = j * GRID_SPACING;
            grid[i][j].pos.y = i * GRID_SPACING;
            grid[i][j].size.x = GRID_SPACING;
            grid[i][j].size.y = GRID_SPACING;
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

int countAliveNeighbors(int x, int y) {
    int count = 0;

    // This is going to check the 8 surrounding cells
    // If any of them are alive, then the cell is alive
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;

            // Calculate the neighbor's coordinates
            int nx = (x + dx + GRID_COLS) % GRID_COLS;
            int ny = (y + dy + GRID_ROWS) % GRID_ROWS;

            count += grid[ny][nx].isAlive;
        }   
    }

    return count;
}

void toggleCells(int x, int y) {
    if (x >= 0 && x < GRID_COLS && y >= 0 && y < GRID_ROWS) {
        grid[y][x].isAlive = !grid[y][x].isAlive;
        aliveCells += (grid[y][x].isAlive ? 1 : -1);
    }
}

void updateGrid() {
    Cell nextGrid[GRID_ROWS][GRID_COLS];
    int newAliveCells = 0;

    for (int i = 0; i < GRID_ROWS; i++) {
        for (int j = 0; j < GRID_COLS; j++) {
            int aliveNeighbors = countAliveNeighbors(j, i);
            
            // Copy position and size
            nextGrid[i][j].pos = grid[i][j].pos;
            nextGrid[i][j].size = grid[i][j].size;

            // Game of Life rules
            if ((grid[i][j].isAlive && (aliveNeighbors == 2 || aliveNeighbors == 3)) || 
                (!grid[i][j].isAlive && aliveNeighbors == 3)) {
                nextGrid[i][j].isAlive = 1;
                newAliveCells++;
            } else {
                nextGrid[i][j].isAlive = 0;
            }
        }
    }

    memcpy(grid, nextGrid, sizeof(grid));
    aliveCells = newAliveCells;
}

void drawCells() {
    for(int i = 0; i < GRID_ROWS; i++) {
        for(int j = 0; j < GRID_COLS; j++) {
            if (grid[i][j].isAlive){
                DrawRectangleV(grid[i][j].pos, grid[i][j].size, WHITE);
                DrawRectangleLinesEx(
                    (Rectangle) {
                        grid[i][j].pos.x,
                        grid[i][j].pos.y,
                        grid[i][j].size.x,
                        grid[i][j].size.y
                    },
                    1,
                    BLACK
                );
            }
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
    
    camera.offset.x = SCREEN_WIDTH / 2.0f;
    camera.offset.y = SCREEN_HEIGHT / 2.0f;
    
    float gridCenterX = (GRID_COLS * GRID_SPACING) / 2.0f;
    float gridCenterY = (GRID_ROWS * GRID_SPACING) / 2.0f;
    
    camera.target.x = gridCenterX;
    camera.target.y = gridCenterY;

    int playMode = 0;
    double lastGenerationTime = 0.0;
    float generation_interval = INIT_INTERVAL;

    initGrid();

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (aliveCells == 0) playMode = 0;
        if (IsKeyPressed(KEY_SPACE) && aliveCells > 0) playMode = !playMode;

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
            DrawRectangle(5, 5, 300, 110, Fade(RAYWHITE, 0.8f));
            DrawText(playMode ? "Play mode" : "Draw mode", 10, 10, 20, BLACK);
            DrawText(TextFormat("Generation: %d (Max: %d)", generation, MAX_GENERATIONS), 10, 35, 20, BLACK);
            DrawText(TextFormat("Cells: %d", aliveCells), 10, 60, 20, BLACK);
            DrawText(TextFormat("Generation interval: %.1fs", generation_interval), 10, 85, 20, BLACK);
 
            BeginMode2D(camera);
                drawGrid();
                drawCells();

                // Reset all with R
                if (IsKeyPressed(KEY_R)) {
                    playMode = 0;
                    generation = 0;
                    aliveCells = 0;
                    generation_interval = INIT_INTERVAL;
                    initGrid();
                }

                if (IsKeyPressed(KEY_UP)) {
                    generation_interval += 0.1f;
                    if (generation_interval > MAX_GENERATIONS) generation_interval = MAX_GENERATIONS;
                }

                if (IsKeyPressed(KEY_DOWN)) {
                    generation_interval -= 0.1f;
                    if (generation_interval < 0.0) generation_interval = 0.0;
                }

                // Draw mode actions with left mouse button
                if (!playMode && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
                    int gridX = (int)(mousePos.x / GRID_SPACING);
                    int gridY = (int)(mousePos.y / GRID_SPACING);
                    toggleCells(gridX, gridY);
                }

                // Update grid with space
                if (playMode && aliveCells > 0) {
                    double time = GetTime();

                    if (time - lastGenerationTime >= generation_interval) {
                        updateGrid();
                        generation++;
                        lastGenerationTime = time;

                        if (generation >= MAX_GENERATIONS) 
                            playMode = 0;
                    }
                }

            EndMode2D();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}