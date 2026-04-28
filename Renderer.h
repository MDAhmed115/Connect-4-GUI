#pragma once
#include <raylib.h>
#include "Game.h"

class Renderer {
public:
    Renderer(int screenW, int screenH, const char* title, int targetFPS);
    ~Renderer();

    void draw(Game& game);          // one call per frame; reads game state
    bool shouldClose() const;

private:
    // Layout constants
    static constexpr int BOARD_X    = 120;
    static constexpr int BOARD_Y    = 50;
    static constexpr int CELL_SIZE  = 80;

    // Drawing sub-routines
    void drawMainMenu   (Game& game);
    void drawCpuMenu    (Game& game);
    void drawBoard      (const Game& game);
    void drawPieces     (const Game& game);
    void drawHover      (const Game& game);
    void drawWinLine    (const Game& game);
    void drawUI         (Game& game);
    void drawEndScreen  (Game& game);
    void drawRules      (Game& game);
    void drawScores     (Game& game);
    void drawCopyrightFooter();

    // Input helpers (return true if clicked)
    bool drawButton(const char* label, float x, float y, Color base);

    // Typography helpers
    void drawUiText(const char* text, float x, float y, float size, Color color) const;
    void drawTitleText(const char* text, float x, float y, float size, Color color) const;
    void drawInfoText(const char* text, float x, float y, float size, Color color) const;

    // Mouse input routing
    void handleMouseInput(Game& game);

    Font uiFont_{};
    Font titleFont_{};
    Font infoFont_{};
    bool uiFontLoaded_ = false;
    bool titleFontLoaded_ = false;
    bool infoFontLoaded_ = false;

    bool  winnerAnimActive_ = false;
    float winnerAnimTime_ = 0.0f;

    Vector2 virtualMouse_ = {0.0f, 0.0f};
    Camera2D layoutCamera_{};
};