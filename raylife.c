#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#include <stdio.h>
#include <stdlib.h>

#define SCREEN_HEIGHT 900
#define SCREEN_WIDTH 1000
#define INITIAL_CAMERA_ZOOM 1.0f
#define ZOOM_SCALE 0.25f
#define GRID_SIZE 100
#define GRID_SPACING 50
#define TOTAL_CELLS 10

typedef struct {
    int isAlive;
    Vector2 pos;
    Vector2 size;
} Cell;

Cell *createCell(int x, int y, int height, int width) {
    Cell *cell = malloc(sizeof(Cell));
    cell->isAlive = 1;
    cell->pos.x = x;
    cell->pos.y = y;
    cell->size.x = width;
    cell->size.y = height;
    return cell;
}

void storeCell(Cell ***cells, int *cellCount, Vector2 pos, int width, int height) {
    // Create a new cell
    Cell *newCell = createCell(pos.x, pos.y, height, width);

    // Reallocate memory if needed
    (*cells) = realloc(*cells, sizeof(Cell*) * (*cellCount + 1));
    
    // Add the new cell to the array
    (*cells)[*cellCount] = newCell;
    (*cellCount)++;
}

void deleteCell(Cell ***cells, int *cellCount, Vector2 *pos) {
    if (*cellCount == 0 || !cells || !(*cells)) return;

    for (int i = 0; i < *cellCount; i++) {
        if ((*cells)[i]->pos.x == pos->x && (*cells)[i]->pos.y == pos->y) {
            free((*cells)[i]);
            
            for (int j = i; j < *cellCount - 1; j++)
                (*cells)[j] = (*cells)[j + 1];

            *cells = realloc(*cells, sizeof(Cell*) * (*cellCount - 1));
            *cellCount--;
            return;
        }
    }
}

void freeCells(Cell **cells, int *cellCount) {
    if (*cellCount == 0 || !cells) return;

    for (int i = 0; i < *cellCount; i++) {
        free(cells[i]);
        *cellCount--;
    }

    free(cells);
}

void drawCells(Cell **cells, int *cellCount) {
    if (!cells || *cellCount == 0) return;
    for (int i = 0; i < *cellCount; i++) {
        if (cells[i]->isAlive)
            DrawRectangleV(cells[i]->pos, cells[i]->size, WHITE);
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

    Cell **cells = malloc(sizeof(Cell*) * TOTAL_CELLS);
    int cellCount = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_SPACE)) playMode = !playMode;

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))  {
            moveCamera(&camera);
        }

        float wheel = GetMouseWheelMove();
        if (wheel)
            setCamera(&camera);
            zoomIncrement(&camera, &wheel);

        BeginDrawing();
            ClearBackground(BLACK);
 
            const char *text = playMode ? "Play mode" : "Draw mode";
            DrawText(text, 10, 10, 20, WHITE);
 
            BeginMode2D(camera);
                rlPushMatrix();
                    rlTranslatef(0, 25*50, 0);
                    rlRotatef(90, 1, 0, 0);
                    DrawGrid(GRID_SIZE, GRID_SPACING);
                rlPopMatrix();

                // Draw cells
                drawCells(cells, &cellCount);

                if (!playMode && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
                    int alignedX = ((int)mousePos.x / GRID_SPACING) * GRID_SPACING;
                    int alignedY = ((int)mousePos.y / GRID_SPACING) * GRID_SPACING;
                    Vector2 pos = { alignedX, alignedY };
                    // Add a new cell when clicking
                    storeCell(&cells, &cellCount, pos, GRID_SIZE / 2, GRID_SIZE / 2);
                }

                if (!playMode && IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
                    Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
                    int alignedX = ((int)mousePos.x / GRID_SPACING) * GRID_SPACING;
                    int alignedY = ((int)mousePos.y / GRID_SPACING) * GRID_SPACING;
                    Vector2 pos = { alignedX, alignedY };
                    // Delete a cell when clicking
                    deleteCell(&cells, &cellCount, &pos);
                }

            EndMode2D();
        EndDrawing();
    }

    freeCells(cells, &cellCount);
    CloseWindow();

    return 0;
}