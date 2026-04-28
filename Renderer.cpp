#include "Renderer.h"
#include <cstdio>

namespace {
constexpr int WINDOW_W = 800;
constexpr int WINDOW_H = 600;

const Color BG_TOP = {236, 242, 248, 255};
const Color BG_BOTTOM = {215, 224, 236, 255};
const Color BOARD_FILL = {237, 241, 247, 255};
const Color BOARD_GRID = {100, 123, 158, 255};
const Color TITLE_COLOR = {38, 49, 66, 255};
const Color TEXT_SOFT = {79, 94, 116, 255};

const char* UI_FONT_PATH = "resources/fonts/Science_Gothic/ScienceGothic-VariableFont_CTRS,slnt,wdth,wght.ttf";
const char* TITLE_FONT_PATH = "resources/fonts/Audiowide/Audiowide-Regular.ttf";
const char* INFO_FONT_PATH = "resources/fonts/Tilt_Neon/TiltNeon-Regular-VariableFont_XROT,YROT.ttf";
constexpr bool START_IN_FULLSCREEN = true;

bool roundOver(const Game& game) {
    return game.getWinnerInfo().hasWinner() || game.getMoveCount() >= ROWS * COLS;
}
}

Renderer::Renderer(int screenW, int screenH, const char* title, int targetFPS) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenW, screenH, title);
    SetWindowMinSize(screenW, screenH);
    SetTargetFPS(targetFPS);

    if (START_IN_FULLSCREEN && !IsWindowFullscreen()) {
        int monitor = GetCurrentMonitor();
        int monitorW = GetMonitorWidth(monitor);
        int monitorH = GetMonitorHeight(monitor);
        SetWindowSize(monitorW, monitorH);
        ToggleFullscreen();
    }

    if (FileExists(UI_FONT_PATH)) {
        uiFont_ = LoadFontEx(UI_FONT_PATH, 64, nullptr, 0);
        uiFontLoaded_ = (uiFont_.texture.id != 0);
    }
    if (FileExists(TITLE_FONT_PATH)) {
        titleFont_ = LoadFontEx(TITLE_FONT_PATH, 96, nullptr, 0);
        titleFontLoaded_ = (titleFont_.texture.id != 0);
    }
    if (FileExists(INFO_FONT_PATH)) {
        infoFont_ = LoadFontEx(INFO_FONT_PATH, 56, nullptr, 0);
        infoFontLoaded_ = (infoFont_.texture.id != 0);
    }
}

Renderer::~Renderer() {
    if (uiFontLoaded_) UnloadFont(uiFont_);
    if (titleFontLoaded_) UnloadFont(titleFont_);
    if (infoFontLoaded_) UnloadFont(infoFont_);
    CloseWindow();
}

void Renderer::drawUiText(const char* text, float x, float y, float size, Color color) const {
    if (uiFontLoaded_) {
        DrawTextEx(uiFont_, text, Vector2{x, y}, size, 1.0f, color);
    } else {
        DrawText(text, static_cast<int>(x), static_cast<int>(y), static_cast<int>(size), color);
    }
}

void Renderer::drawTitleText(const char* text, float x, float y, float size, Color color) const {
    if (titleFontLoaded_) {
        DrawTextEx(titleFont_, text, Vector2{x + 2.0f, y + 2.0f}, size, 1.0f, Fade(BLACK, 0.28f));
        DrawTextEx(titleFont_, text, Vector2{x, y}, size, 1.0f, color);
    } else {
        DrawText(text, static_cast<int>(x + 2.0f), static_cast<int>(y + 2.0f), static_cast<int>(size), Fade(BLACK, 0.28f));
        DrawText(text, static_cast<int>(x), static_cast<int>(y), static_cast<int>(size), color);
    }
}

void Renderer::drawInfoText(const char* text, float x, float y, float size, Color color) const {
    if (infoFontLoaded_) {
        DrawTextEx(infoFont_, text, Vector2{x, y}, size, 1.0f, color);
    } else {
        DrawText(text, static_cast<int>(x), static_cast<int>(y), static_cast<int>(size), color);
    }
}

void Renderer::drawBoard(const Game& game) {
    (void)game;
    Rectangle boardRect = {(float)BOARD_X, (float)BOARD_Y, (float)(CELL_SIZE * COLS), (float)(CELL_SIZE * ROWS)};
    DrawRectangleRounded(Rectangle{boardRect.x - 10, boardRect.y - 10, boardRect.width + 20, boardRect.height + 20}, 0.08f, 8, Fade((Color){76, 104, 145, 255}, 0.22f));
    DrawRectangleRounded(boardRect, 0.08f, 8, (Color){227, 236, 248, 255});
    DrawRectangleGradientV((int)boardRect.x, (int)boardRect.y, (int)boardRect.width, (int)boardRect.height, (Color){238, 245, 255, 255}, (Color){209, 222, 241, 255});
    DrawRectangleRoundedLines(boardRect, 0.08f, 8, BOARD_GRID);

    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            float cx = static_cast<float>(BOARD_X + c * CELL_SIZE + CELL_SIZE / 2);
            float cy = static_cast<float>(BOARD_Y + r * CELL_SIZE + CELL_SIZE / 2);
            DrawCircleV(Vector2{cx, cy}, CELL_SIZE * 0.39f, Fade((Color){79, 97, 126, 255}, 0.16f));
            DrawCircleV(Vector2{cx, cy}, CELL_SIZE * 0.34f, (Color){244, 248, 255, 255});
        }
    }
    for(int i=1;i<COLS;i++) {
        float x = static_cast<float>(BOARD_X + i * CELL_SIZE);
        float yTop = static_cast<float>(BOARD_Y);
        float yBottom = static_cast<float>(BOARD_Y + ROWS * CELL_SIZE);
        DrawLineEx(Vector2{x, yTop}, Vector2{x, yBottom}, 2, Fade(BOARD_GRID, 0.8f));
    }
    for(int j=1;j<ROWS;j++) {
        float y = static_cast<float>(BOARD_Y + j * CELL_SIZE);
        float xLeft = static_cast<float>(BOARD_X);
        float xRight = static_cast<float>(BOARD_X + COLS * CELL_SIZE);
        DrawLineEx(Vector2{xLeft, y}, Vector2{xRight, y}, 2, Fade(BOARD_GRID, 0.8f));
    }
}

void Renderer::drawPieces(const Game& game){
    const Board& board = game.getBoard();
    const FallingPiece& fallingPiece = game.getFallingPiece();

    for(int c=0;c<COLS;c++){
        for(int r=0;r<ROWS;r++){
            char cell = board.getCell(r, c);
            if(cell!=' ' && !(fallingPiece.active && fallingPiece.col==c && fallingPiece.targetRow==r)){
                float cx = static_cast<float>(BOARD_X + c * CELL_SIZE + CELL_SIZE / 2);
                float cy = static_cast<float>(BOARD_Y + r * CELL_SIZE + CELL_SIZE / 2);
                Color piece = (cell=='X') ? (Color){37, 99, 235, 255} : (Color){225, 29, 72, 255};
                Color light = (cell=='X') ? (Color){147, 197, 253, 255} : (Color){251, 113, 133, 255};
                DrawCircleV(Vector2{cx, cy}, CELL_SIZE * 0.38f, Fade(piece, 0.18f));
                DrawCircleV(Vector2{cx, cy}, CELL_SIZE * 0.34f, piece);
                DrawCircleV(Vector2{cx - 6.0f, cy - 8.0f}, CELL_SIZE * 0.12f, Fade(light, 0.60f));
                DrawCircleLines((int)cx, (int)cy, CELL_SIZE * 0.34f, Fade(WHITE, 0.35f));
            }
        }
    }
    if(fallingPiece.active){
        float cx = static_cast<float>(BOARD_X + fallingPiece.col * CELL_SIZE + CELL_SIZE / 2);
        float cy = fallingPiece.y;
        Color piece = (fallingPiece.piece=='X') ? (Color){37, 99, 235, 255} : (Color){225, 29, 72, 255};
        Color light = (fallingPiece.piece=='X') ? (Color){147, 197, 253, 255} : (Color){251, 113, 133, 255};
        DrawCircleV(Vector2{cx, cy}, CELL_SIZE * 0.38f, Fade(piece, 0.18f));
        DrawCircleV(Vector2{cx, cy}, CELL_SIZE * 0.34f, piece);
        DrawCircleV(Vector2{cx - 6.0f, cy - 8.0f}, CELL_SIZE * 0.12f, Fade(light, 0.60f));
        DrawCircleLines((int)cx, (int)cy, CELL_SIZE * 0.34f, Fade(WHITE, 0.35f));
    }
}

void Renderer::drawHover(const Game& game){
    const Board& board = game.getBoard();
    Vector2 mouse = virtualMouse_;
    if (mouse.x < BOARD_X || mouse.x >= BOARD_X + COLS * CELL_SIZE || mouse.y < BOARD_Y || mouse.y >= BOARD_Y + ROWS * CELL_SIZE) {
        return;
    }

    int hoveredCol = static_cast<int>((mouse.x - BOARD_X) / CELL_SIZE);
    if(hoveredCol<0) hoveredCol=0;
    if(hoveredCol>=COLS) hoveredCol=COLS-1;

    float colX = static_cast<float>(BOARD_X + hoveredCol * CELL_SIZE);
    float colY = static_cast<float>(BOARD_Y);
    float colH = static_cast<float>(ROWS * CELL_SIZE);

    for (int i = 0; i < 4; ++i) {
        float expand = static_cast<float>(10 + i * 4);
        DrawRectangleRounded(Rectangle{colX - expand * 0.5f, colY - 8.0f - i, (float)CELL_SIZE + expand, colH + 16.0f + i * 2.0f},
                             0.45f, 16, Fade((Color){66, 153, 225, 255}, 0.09f - i * 0.015f));
    }

    DrawRectangleRounded(Rectangle{colX - 3.0f, colY - 4.0f, (float)CELL_SIZE + 6.0f, colH + 8.0f}, 0.38f, 16, Fade((Color){67, 146, 255, 255}, 0.18f));
    DrawLineEx(Vector2{colX - 1.0f, colY}, Vector2{colX - 1.0f, colY + colH}, 3.0f, Fade((Color){49, 130, 206, 255}, 0.55f));
    DrawLineEx(Vector2{colX + CELL_SIZE + 1.0f, colY}, Vector2{colX + CELL_SIZE + 1.0f, colY + colH}, 3.0f, Fade((Color){49, 130, 206, 255}, 0.55f));
    DrawCircle(BOARD_X + hoveredCol * CELL_SIZE + CELL_SIZE / 2, BOARD_Y - 12, 9, Fade((Color){49, 130, 206, 255}, 0.9f));

    int filledStartRow=ROWS;
    for(int r=ROWS-1;r>=0;r--) if(board.getCell(r, hoveredCol)!=' ') filledStartRow=r;
    int filledCount = ROWS - filledStartRow;
    int emptyCount = ROWS - filledCount;
    if(filledCount>0) {
        float x = colX;
        float y = static_cast<float>(BOARD_Y + filledStartRow * CELL_SIZE);
        float w = static_cast<float>(CELL_SIZE);
        float h = static_cast<float>(filledCount * CELL_SIZE);
        DrawRectangleRounded(Rectangle{x, y, w, h}, 0.4f, 12, Fade((Color){239, 68, 68, 255}, 0.20f));
    }
    if(emptyCount>0) {
        float x = colX;
        float y = colY;
        float w = static_cast<float>(CELL_SIZE);
        float h = static_cast<float>(emptyCount * CELL_SIZE);
        DrawRectangleRounded(Rectangle{x, y, w, h}, 0.4f, 12, Fade((Color){250, 204, 21, 255}, 0.16f));
    }
}

void Renderer::drawWinLine(const Game& game){
    const WinnerInfo& gameWinner = game.getWinnerInfo();
    const int movesCount = game.getMoveCount();
    constexpr int MAX_MOVES = ROWS * COLS;

    if (!gameWinner.hasWinner()) {
        winnerAnimActive_ = false;
        winnerAnimTime_ = 0.0f;
    }

    if (gameWinner.hasWinner()) {
        if (!winnerAnimActive_) {
            winnerAnimActive_ = true;
            winnerAnimTime_ = 0.0f;
        }
        winnerAnimTime_ += GetFrameTime();
        const float animDuration = 0.35f;
        float t = winnerAnimTime_ / animDuration;
        if (t > 1.0f) t = 1.0f;

        int startX = BOARD_X + gameWinner.startCol * CELL_SIZE + CELL_SIZE / 2;
        int startY = BOARD_Y + gameWinner.startRow * CELL_SIZE + CELL_SIZE / 2;
        int endX = BOARD_X + gameWinner.endCol * CELL_SIZE + CELL_SIZE / 2;
        int endY = BOARD_Y + gameWinner.endRow * CELL_SIZE + CELL_SIZE / 2;
        float startXF = static_cast<float>(startX);
        float startYF = static_cast<float>(startY);
        float endXF = static_cast<float>(endX);
        float endYF = static_cast<float>(endY);
        float midX = (startXF + endXF) * 0.5f;
        float midY = (startYF + endYF) * 0.5f;
        float curStartX = midX + (startXF - midX) * t;
        float curStartY = midY + (startYF - midY) * t;
        float curEndX = midX + (endXF - midX) * t;
        float curEndY = midY + (endYF - midY) * t;
        Color winColor = gameWinner.winner==1 ? (Color){34, 197, 94, 255} : (Color){245, 158, 11, 255};
        DrawLineEx(Vector2{curStartX, curStartY}, Vector2{curEndX, curEndY}, 14, Fade(winColor, 0.25f));
        DrawLineEx(Vector2{curStartX, curStartY}, Vector2{curEndX, curEndY}, 8, gameWinner.winner==1?GREEN:ORANGE);

        int stepRow = (gameWinner.endRow > gameWinner.startRow) ? 1 : (gameWinner.endRow < gameWinner.startRow ? -1 : 0);
        int stepCol = (gameWinner.endCol > gameWinner.startCol) ? 1 : (gameWinner.endCol < gameWinner.startCol ? -1 : 0);
        for (int i = 0; i < 4; ++i) {
            int r = gameWinner.startRow + i * stepRow;
            int c = gameWinner.startCol + i * stepCol;
            float cx = static_cast<float>(BOARD_X + c * CELL_SIZE + CELL_SIZE / 2);
            float cy = static_cast<float>(BOARD_Y + r * CELL_SIZE + CELL_SIZE / 2);
            DrawCircleV(Vector2{cx, cy}, CELL_SIZE * 0.44f, Fade(winColor, 0.10f + 0.06f * t));
            DrawCircleLines(static_cast<int>(cx), static_cast<int>(cy), CELL_SIZE * 0.42f, Fade(winColor, 0.45f + 0.30f * t));
        }
    }

    const char* resultText = nullptr;
    Color resultColor = {75, 85, 99, 255};
    if (gameWinner.winner == 1) {
        resultText = "X wins!";
        resultColor = (Color){39, 108, 255, 255};
    } else if (gameWinner.winner == 2) {
        resultText = "O wins!";
        resultColor = (Color){225, 29, 72, 255};
    } else if (movesCount == MAX_MOVES) {
        resultText = "Draw!";
        resultColor = (Color){107, 114, 128, 255};
    }

    if (resultText) {
        Font resultFont = titleFontLoaded_ ? titleFont_ : GetFontDefault();
        const float resultSize = 52.0f;
        Vector2 resultPos = {static_cast<float>(BOARD_X + CELL_SIZE * COLS + 26), static_cast<float>(BOARD_Y + CELL_SIZE * ROWS - 8)};
        DrawTextPro(resultFont, resultText, resultPos, Vector2{0.0f, 0.0f}, -90.0f, resultSize, 1.0f, Fade(BLACK, 0.22f));
        DrawTextPro(resultFont, resultText, Vector2{resultPos.x - 2.0f, resultPos.y - 2.0f}, Vector2{0.0f, 0.0f}, -90.0f, resultSize, 1.0f, resultColor);
    }

    if (game.isVsAI() && gameWinner.winner == 1) {
        int score = game.getScoreManager().computeScore(game.getDifficulty(), game.getMoveCount());
        char scoreText[64];
        std::snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
        drawUiText(scoreText, BOARD_X + CELL_SIZE * COLS - 190.0f, BOARD_Y - 56.0f, 30, TEXT_SOFT);
    }
}

void Renderer::handleMouseInput(Game& game) {
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;

    if (game.getState() != GameState::PvpGame && game.getState() != GameState::CpuGame) {
        return;
    }
    if (roundOver(game)) {
        return;
    }

    Vector2 mousePos = virtualMouse_;
    float boardRight = static_cast<float>(BOARD_X + COLS * CELL_SIZE);
    float boardBottom = static_cast<float>(BOARD_Y + ROWS * CELL_SIZE);
    if (mousePos.x < static_cast<float>(BOARD_X) || mousePos.x >= boardRight ||
        mousePos.y < static_cast<float>(BOARD_Y) || mousePos.y >= boardBottom) {
        return;
    }

    int clickedCol = static_cast<int>((mousePos.x - static_cast<float>(BOARD_X)) / static_cast<float>(CELL_SIZE));
    game.processInput(clickedCol);
}

void Renderer::draw(Game& game) {
    if (IsKeyPressed(KEY_F11)) {
        if (IsWindowFullscreen()) {
            ToggleFullscreen();
            MaximizeWindow();
        } else {
            int monitor = GetCurrentMonitor();
            int monitorW = GetMonitorWidth(monitor);
            int monitorH = GetMonitorHeight(monitor);
            SetWindowSize(monitorW, monitorH);
            ToggleFullscreen();
        }
    }

    const int screenW = GetScreenWidth();
    const int screenH = GetScreenHeight();
    const float scaleX = static_cast<float>(screenW) / static_cast<float>(WINDOW_W);
    const float scaleY = static_cast<float>(screenH) / static_cast<float>(WINDOW_H);
    const float scale = (scaleX < scaleY) ? scaleX : scaleY;
    const float scaledW = static_cast<float>(WINDOW_W) * scale;
    const float scaledH = static_cast<float>(WINDOW_H) * scale;
    const float offsetX = (static_cast<float>(screenW) - scaledW) * 0.5f;
    const float offsetY = (static_cast<float>(screenH) - scaledH) * 0.5f;

    layoutCamera_.target = Vector2{0.0f, 0.0f};
    layoutCamera_.offset = Vector2{offsetX, offsetY};
    layoutCamera_.rotation = 0.0f;
    layoutCamera_.zoom = scale;
    virtualMouse_ = GetScreenToWorld2D(GetMousePosition(), layoutCamera_);

    BeginDrawing();
    ClearBackground(BG_BOTTOM);
    DrawRectangleGradientV(0, 0, screenW, screenH, BG_TOP, BG_BOTTOM);
    BeginMode2D(layoutCamera_);

    switch (game.getState()) {
        case GameState::MainMenu:   drawMainMenu(game); break;
        case GameState::CpuMenu:    drawCpuMenu(game); break;
        case GameState::PvpGame:
        case GameState::CpuGame:
            drawBoard(game);
            if (!roundOver(game)) {
                drawHover(game);
            }
            drawPieces(game);
            drawWinLine(game);
            drawUI(game);
            break;
        case GameState::ShowRules:  drawRules(game); break;
        case GameState::ShowScores: drawScores(game); break;
        case GameState::EndScreen:  drawEndScreen(game); break;
        default: break;
    }

    drawCopyrightFooter();
    EndMode2D();

    EndDrawing();
    handleMouseInput(game);
}

bool Renderer::shouldClose() const {
    return WindowShouldClose();
}

void Renderer::drawCopyrightFooter() {
    const char* ownerText = "Muhammed Ahmed Ashfaq";
    const float nameSize = 16.0f;
    const float symbolRadius = 8.0f;
    const float symbolDiameter = symbolRadius * 2.0f;
    const float gap = 7.0f;

    Font nameFont = uiFontLoaded_ ? uiFont_ : GetFontDefault();
    Vector2 nameDims = MeasureTextEx(nameFont, ownerText, nameSize, 1.0f);

    float totalWidth = symbolDiameter + gap + nameDims.x;
    float x = static_cast<float>(WINDOW_W) - totalWidth - 12.0f;
    float y = static_cast<float>(WINDOW_H) - nameDims.y - 10.0f;

    Vector2 center = {x + symbolRadius, y + nameDims.y * 0.5f + 0.5f};
    DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), symbolRadius, (Color){71, 85, 105, 255});

    Font cFont = GetFontDefault();
    const float cSize = 13.0f;
    Vector2 cDims = MeasureTextEx(cFont, "C", cSize, 1.0f);
    Vector2 cPos = {center.x - cDims.x * 0.5f, center.y - cDims.y * 0.5f};
    DrawTextEx(cFont, "C", Vector2{cPos.x + 1.0f, cPos.y + 1.0f}, cSize, 1.0f, Fade(BLACK, 0.30f));
    DrawTextEx(cFont, "C", cPos, cSize, 1.0f, (Color){71, 85, 105, 255});

    float nameX = x + symbolDiameter + gap;
    DrawTextEx(nameFont, ownerText, Vector2{nameX + 1.0f, y + 1.0f}, nameSize, 1.0f, Fade(BLACK, 0.30f));
    DrawTextEx(nameFont, ownerText, Vector2{nameX, y}, nameSize, 1.0f, (Color){71, 85, 105, 255});
}

bool Renderer::drawButton(const char* label, float x, float y, Color base) {
    const int textSize = 30;
    const int textWidth = MeasureText(label, textSize);
    const Rectangle bounds = {x, y, static_cast<float>(textWidth + 26), 44.0f};
    bool hovered = CheckCollisionPointRec(virtualMouse_, bounds);

    if (hovered) {
        for (int i = 0; i < 4; ++i) {
            float expand = static_cast<float>(8 + i * 4);
            DrawRectangleRounded(Rectangle{bounds.x - expand * 0.5f, bounds.y - expand * 0.5f,
                                           bounds.width + expand, bounds.height + expand},
                                 0.26f, 8, Fade(base, 0.10f - i * 0.02f));
        }
        DrawRectangleRounded(Rectangle{bounds.x - 4, bounds.y - 4, bounds.width + 8, bounds.height + 8}, 0.22f, 8, Fade(base, 0.20f));
    }

    DrawRectangleRounded(bounds, 0.22f, 8, base);
    DrawRectangleRoundedLines(bounds, 0.22f, 8, Fade(BLACK, 0.2f));
    Font font = uiFontLoaded_ ? uiFont_ : GetFontDefault();
    Vector2 txt = MeasureTextEx(font, label, static_cast<float>(textSize), 1.0f);
    float tx = x + (bounds.width - txt.x) * 0.5f;
    float ty = y + (bounds.height - txt.y) * 0.5f;
    drawUiText(label, tx, ty, static_cast<float>(textSize), (Color){248,250,252,255});

    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hovered;
}

void Renderer::drawUI(Game& game) {
    const char* modeText = game.isVsAI() ? "Mode: vs CPU" : "Mode: PvP";
    char modeBuffer[64];
    if (game.isVsAI()) {
        const char* diffText = "Easy";
        if (game.getDifficulty() == Difficulty::Medium) diffText = "Medium";
        else if (game.getDifficulty() == Difficulty::Hard) diffText = "Hard";
        std::snprintf(modeBuffer, sizeof(modeBuffer), "Mode: vs CPU (%s)", diffText);
        modeText = modeBuffer;
    }

    drawInfoText(game.getCurrentPlayer() == 'X' ? "Turn: X" : "Turn: O", BOARD_X, BOARD_Y - 38.0f, 26,
                 game.getCurrentPlayer() == 'X' ? (Color){30, 64, 175, 255} : (Color){190, 24, 93, 255});
    Vector2 modePosition = {static_cast<float>(BOARD_X - 70), static_cast<float>(BOARD_Y + (ROWS * CELL_SIZE) / 2 + 90)};
    Font modeFont = uiFontLoaded_ ? uiFont_ : GetFontDefault();
    DrawTextPro(modeFont, modeText, modePosition, Vector2{0.0f, 0.0f}, -90.0f, 24.0f, 1.0f, TEXT_SOFT);

    if (!roundOver(game)) {
        float backX = static_cast<float>((WINDOW_W - (MeasureText("BACK", 30) + 20)) / 2);
        float backY = 550.0f;
        if (drawButton("BACK", backX, backY, (Color){73, 85, 107, 255})) {
            game.startNewGame(game.getDifficulty(), game.isVsAI());
            game.setState(game.isVsAI() ? GameState::CpuMenu : GameState::MainMenu);
        }
    } else {
        float finishX = static_cast<float>((WINDOW_W - (MeasureText("FINISH", 30) + 20)) / 2);
        if (drawButton("FINISH", finishX, 550.0f, (Color){22, 163, 74, 255})) {
            game.setState(GameState::EndScreen);
        }
    }
}

void Renderer::drawMainMenu(Game& game) {
    drawTitleText("CONNECT 4", 86, 86, 84, TITLE_COLOR);
    drawUiText("Classic strategy. modern controls.", 92, 166, 22, TEXT_SOFT);
    drawUiText("F11: Toggle Fullscreen", 92, 196, 18, (Color){100, 116, 139, 255});

    const char* options[] = {"Player vs Player", "Player vs CPU", "Show Rules", "Show Scores", "Exit"};
    const Color colors[] = {
        (Color){37, 99, 235, 255},
        (Color){14, 116, 144, 255},
        (Color){217, 119, 6, 255},
        (Color){124, 58, 237, 255},
        (Color){220, 38, 38, 255}
    };

    const float panelX = 90.0f;
    for (int i = 0; i < 5; ++i) {
        float y = static_cast<float>(220 + i * 62);
        if (!drawButton(options[i], panelX, y, colors[i])) {
            continue;
        }

        if (i == 0) {
            game.startNewGame(Difficulty::Easy, false);
        } else if (i == 1) {
            game.setState(GameState::CpuMenu);
        } else if (i == 2) {
            game.setState(GameState::ShowRules);
        } else if (i == 3) {
            game.setState(GameState::ShowScores);
        } else {
            game.setState(GameState::ExitGame);
        }
    }
}

void Renderer::drawCpuMenu(Game& game) {
    drawTitleText("CPU MODE", 88, 96, 64, TITLE_COLOR);
    drawTitleText("Choose Difficulty", 90, 170, 34, (Color){66, 78, 95, 255});

    struct CpuOption { const char* label; Difficulty diff; Color hover; };
    const CpuOption options[] = {
        {"Easy", Difficulty::Easy, (Color){22, 163, 74, 255}},
        {"Medium", Difficulty::Medium, (Color){217, 119, 6, 255}},
        {"Hard", Difficulty::Hard, (Color){220, 38, 38, 255}}
    };

    const float panelX = 90.0f;
    for (int i = 0; i < 3; ++i) {
        float y = static_cast<float>(230 + i * 74);
        if (drawButton(options[i].label, panelX, y, options[i].hover)) {
            game.startNewGame(options[i].diff, true);
        }
    }

    float backX = panelX;
    if (drawButton("BACK", backX, 460.0f, (Color){73, 85, 107, 255})) {
        game.setState(GameState::MainMenu);
    }
}

void Renderer::drawRules(Game& game) {
    Rectangle card = {90, 60, 620, 460};
    DrawRectangleRounded(Rectangle{card.x - 6, card.y - 6, card.width + 12, card.height + 12}, 0.06f, 10, Fade((Color){100, 116, 139, 255}, 0.18f));
    DrawRectangleRounded(card, 0.05f, 10, Fade(WHITE, 0.92f));
    DrawRectangleRoundedLines(card, 0.05f, 10, Fade((Color){148, 163, 184, 255}, 0.7f));

    drawTitleText("RULES", 300, 84, 58, TITLE_COLOR);

    const char* lines[] = {
        "Drop your piece into any of the 7 columns.",
        "Pieces always fall to the lowest empty slot.",
        "Connect 4 of your pieces to win:",
        "Horizontal, Vertical, or Diagonal.",
        "If the board fills with no winner, it is a draw."
    };

    const int lineCount = static_cast<int>(sizeof(lines) / sizeof(lines[0]));
    const float lineSize = 26.0f;
    const float lineSpacing = 58.0f;
    const float startY = 180.0f;

    Font info = infoFontLoaded_ ? infoFont_ : GetFontDefault();
    for (int i = 0; i < lineCount; ++i) {
        Vector2 dims = MeasureTextEx(info, lines[i], lineSize, 1.0f);
        float x = (WINDOW_W - dims.x) * 0.5f;
        drawInfoText(lines[i], x, startY + i * lineSpacing, lineSize, (Color){71, 85, 105, 255});
    }

    float backX = static_cast<float>((WINDOW_W - (MeasureText("BACK", 30) + 26)) / 2);
    if (drawButton("BACK", backX, 534.0f, (Color){73, 85, 107, 255})) {
        game.setState(GameState::MainMenu);
    }
}

void Renderer::drawScores(Game& game) {
    Rectangle card = {100, 50, 600, 470};
    DrawRectangleRounded(Rectangle{card.x - 6, card.y - 6, card.width + 12, card.height + 12}, 0.06f, 10, Fade((Color){100, 116, 139, 255}, 0.18f));
    DrawRectangleRounded(card, 0.05f, 10, Fade(WHITE, 0.92f));
    DrawRectangleRoundedLines(card, 0.05f, 10, Fade((Color){148, 163, 184, 255}, 0.7f));

    drawTitleText("SCORES", 296, 74, 58, TITLE_COLOR);
    drawInfoText("CPU Mode History", 274, 132, 28, (Color){100, 116, 139, 255});
    const auto& scores = game.getScoreManager().getAll();

    if (scores.empty()) {
        Font info = infoFontLoaded_ ? infoFont_ : GetFontDefault();
        const char* msg = "No scores yet.";
        Vector2 dims = MeasureTextEx(info, msg, 30.0f, 1.0f);
        drawInfoText(msg, (WINDOW_W - dims.x) * 0.5f, 280, 30, (Color){185, 28, 28, 255});
    } else {
        Font info = infoFontLoaded_ ? infoFont_ : GetFontDefault();
        float y = 186.0f;
        for (size_t i = 0; i < scores.size() && i < 9; ++i) {
            const char* diffText = (scores[i].difficulty == Difficulty::Easy) ? "Easy" :
                                   (scores[i].difficulty == Difficulty::Medium) ? "Medium" : "Hard";
            char line[128];
            std::snprintf(line, sizeof(line), "%d) %s   Moves: %d   Score: %d", static_cast<int>(i + 1), diffText, scores[i].moves, scores[i].score);
            Vector2 dims = MeasureTextEx(info, line, 24.0f, 1.0f);
            drawInfoText(line, (WINDOW_W - dims.x) * 0.5f, y, 24, (Color){51, 65, 85, 255});
            y += 34.0f;
        }
    }

    float backX = static_cast<float>((WINDOW_W - (MeasureText("BACK", 30) + 26)) / 2);
    if (drawButton("BACK", backX, 534.0f, (Color){73, 85, 107, 255})) {
        game.setState(GameState::MainMenu);
    }
}

void Renderer::drawEndScreen(Game& game) {
    DrawRectangleRounded(Rectangle{190, 70, 420, 420}, 0.08f, 10, Fade(WHITE, 0.56f));
    DrawRectangleRoundedLines(Rectangle{190, 70, 420, 420}, 0.08f, 10, Fade((Color){148, 163, 184, 255}, 0.7f));
    Font title = titleFontLoaded_ ? titleFont_ : GetFontDefault();
    const char* over = "GAME OVER";
    const float overSize = 58.0f;
    Vector2 overDims = MeasureTextEx(title, over, overSize, 1.0f);
    float overX = 190.0f + (420.0f - overDims.x) * 0.5f;
    drawTitleText(over, overX, 114.0f, overSize, TITLE_COLOR);

    const char* options[] = {"Restart", "Main Menu"};
    const Color endColors[] = {
        (Color){5, 150, 105, 255},
        (Color){99, 102, 241, 255}
    };
    for (int i = 0; i < 2; ++i) {
        int textWidth = MeasureText(options[i], 30);
        float x = static_cast<float>((WINDOW_W - textWidth - 20) / 2);
        float y = static_cast<float>(250 + i * 104);

        if (!drawButton(options[i], x, y, endColors[i])) {
            continue;
        }

        if (i == 0) {
            if (game.isVsAI()) {
                game.startNewGame(game.getDifficulty(), true);
                game.setState(GameState::CpuMenu);
            } else {
                game.startNewGame(Difficulty::Easy, false);
            }
        } else {
            game.startNewGame(Difficulty::Easy, false);
            game.setState(GameState::MainMenu);
        }
    }
}
