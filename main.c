#include "game_logic.h"
#include <raylib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum { MAIN_MENU, CPU_MENU, PVP_GAME, CPU_GAME, SHOW_RULES, SHOW_SCORES, EXIT_GAME, END_SCREEN } GameState;
typedef enum { EASY, MEDIUM, HARD, PVP } Difficulty;

typedef struct {
    int col;
    int targetRow;
    char type;
    float y;
    bool active;
} FallingPiece;

typedef struct {
    Difficulty diff;
    int moves;
    int score;
} ScoreEntry;

#define MAX_SCORES 20
#define SCORE_FILE "scores.txt"

GameState current_state = MAIN_MENU;
Difficulty diff;

const int MAX_MOVES = ROWS * COLS;
int boardX = 120, boardY = 50, cellSize = 80, hoveredcol;

WinnerInfo gameWinner;
int movesCount;
char currentPlayer;
float aiDelay;
FallingPiece fallingPiece = {0,0,' ',0,false};

int backButton(float x, float y) {
    int textWidth = MeasureText("BACK", 30);
    Rectangle button = (Rectangle){(800 - textWidth - 20)/2, y, textWidth + 20, 40};
    DrawRectangleRounded(button, 0.25f, 8, (Color){0,0,120,255});
    if(CheckCollisionPointRec(GetMousePosition(), button)){
        DrawRectangleRounded(button,0.25f,8,(Color){255,0,0,100});
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return 1;
    }
    DrawText("BACK", button.x+10, button.y+5, 30, (Color){231,231,231,255});
    return 0;
}

void saveScore(Difficulty diff, int moves){
    ScoreEntry scores[MAX_SCORES+1];
    int count = 0;
    FILE *file = fopen(SCORE_FILE, "r");
    if (file) {
        while(fscanf(file,"%d %d %d\n",&scores[count].diff,&scores[count].moves,&scores[count].score) == 3){
            count++;
            if(count >= MAX_SCORES) break;
        }
        fclose(file);
    }
    int base, penalty;
    if(diff == EASY){ base=1000; penalty=10; }
    else if(diff == MEDIUM){ base=1200; penalty=12; }
    else { base=1500; penalty=15; }
    ScoreEntry newScore;
    newScore.diff = diff;
    newScore.moves = moves;
    newScore.score = base - moves * penalty;
    if(newScore.score < 0) newScore.score = 0;
    if (gameWinner.winner == 1){
        if(count < MAX_SCORES){
            for(int i=count; i>0; i--) scores[i] = scores[i-1];
            scores[0] = newScore;
            count++;
        } else {
            for(int i=MAX_SCORES-1; i>0; i--) scores[i] = scores[i-1];
            scores[0] = newScore;
        }
        file = fopen(SCORE_FILE,"w");
        if(!file) return;
        for(int i=0;i<count;i++){
            fprintf(file,"%d %d %d\n",scores[i].diff,scores[i].moves,scores[i].score);
        }
        fclose(file);
    }
    
}

void showScores() {
    ClearBackground(RAYWHITE);
    DrawText("Last Scores (CPU Games)", 200, 50, 30, DARKBLUE);
    FILE *file = fopen(SCORE_FILE,"r");
    if(!file){
        DrawText("No scores yet.", 300, 200, 20, RED);
    } else {
        int y = 100;
        Difficulty d;
        int moves, score;
        while(fscanf(file,"%d %d %d\n",&d,&moves,&score) == 3){
            const char *diffName = (d==EASY)?"Easy":(d==MEDIUM)?"Medium":(d==HARD)?"Hard":"Unknown";
            char line[100];
            sprintf(line,"Diff: %s | Moves: %d | Score: %d", diffName, moves, score);
            DrawText(line, 150, y, 20, BLACK);
            y += 25;
        }
        fclose(file);
    }
    if(backButton(500,550)) current_state = MAIN_MENU;
}

void startGame(){
    gameWinner = (WinnerInfo){0, -1, -1, -1, -1};
    movesCount = 0;
    currentPlayer = 'X';
    aiDelay = 0;
    fallingPiece.active = false;
    initBoard();
}

int finishButton(float x, float y){
    int textWidth = MeasureText("FINISH", 30);
    Rectangle button = (Rectangle){(800 - textWidth - 20)/2, y, textWidth + 20, 40};
    DrawRectangleRounded(button,0.25f,8,(Color){0,0,120,255});
    if(CheckCollisionPointRec(GetMousePosition(), button)){
        DrawRectangleRounded(button,0.25f,8,(Color){255,0,0,100});
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return 1;
    }
    DrawText("FINISH", button.x+10, button.y+5,30,(Color){231,231,231,255});
    return 0;
}

void menu(){
    DrawText("Connect 4", 275, 100, 50, (Color){80,0,80,255});
    const char* options[] = {"Player vs Player","Player vs CPU","Show Rules","Show Scores","Exit"};
    GameState nextStates[] = {PVP_GAME, CPU_MENU, SHOW_RULES, SHOW_SCORES, EXIT_GAME};
    for(int i=0;i<5;i++){
        int textWidth = MeasureText(options[i],30);
        Rectangle button = (Rectangle){(800-textWidth-20)/2, 230 + i*60, textWidth+20, 40};
        DrawRectangleRounded(button,0.25f,8,(Color){0,0,120,255});
        if(CheckCollisionPointRec(GetMousePosition(),button)){
            DrawRectangleRounded(button,0.25f,8,(Color){255,0,0,100});
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) current_state = nextStates[i];
        }
        DrawText(options[i], button.x+10, button.y+5, 30, (Color){231,231,231,255});
    }
}

void cpuMenu(){
    DrawText("Choose your Difficulty", 200, 100, 40, (Color){80,0,80,255});
    const char* levels[] = {"Easy","Medium","Hard"};
    Difficulty diffs[] = {EASY, MEDIUM, HARD};
    for(int i=0;i<3;i++){
        int textWidth = MeasureText(levels[i],30);
        Rectangle button = (Rectangle){(800-textWidth-20)/2,200+i*70, textWidth+20,40};
        if(CheckCollisionPointRec(GetMousePosition(),button)){
            Color hoverColor = (diffs[i]==EASY)?GREEN:(diffs[i]==MEDIUM)?ORANGE:RED;
            DrawRectangleRounded(button,0.25f,8,hoverColor);
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                startGame();
                diff = diffs[i];
                current_state = CPU_GAME;
            }
        } else DrawRectangleRounded(button,0.25f,8,(Color){0,0,120,255});
        DrawText(levels[i], button.x+10, button.y+5,30,(Color){231,231,231,255});
    }
    if(backButton(300,550)) current_state = MAIN_MENU;
}

void endScreen(){
    DrawText("Game Over", 275, 100, 50, (Color){80,0,80,255});
    const char* options[] = {"Restart","Main Menu"};
    for(int i=0;i<2;i++){
        int textWidth = MeasureText(options[i],30);
        Rectangle button = (Rectangle){(800-textWidth-20)/2,230+i*100,textWidth+20,40};
        DrawRectangleRounded(button,0.25f,8,(Color){0,0,120,255});
        if(CheckCollisionPointRec(GetMousePosition(),button)){
            DrawRectangleRounded(button,0.25f,8,(Color){255,0,0,100});
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                if(i==0){
                    if(diff==PVP) { startGame(); current_state = PVP_GAME; }
                    else { startGame(); current_state = CPU_MENU; }
                } else { startGame(); current_state = MAIN_MENU; }
            }
        }
        DrawText(options[i], button.x+10, button.y+5,30,(Color){231,231,231,255});
    }
}

void drawBoard(){
    DrawRectangleRounded((Rectangle){boardX, boardY, cellSize*COLS, cellSize*ROWS},0.1f,8,(Color){192,192,192,255});
    DrawRectangleRoundedLines((Rectangle){boardX,boardY,cellSize*COLS,cellSize*ROWS},0.1f,100,(Color){0,0,128,255});
    for(int i=1;i<COLS;i++) DrawLineEx((Vector2){boardX+i*cellSize,boardY},(Vector2){boardX+i*cellSize,boardY+ROWS*cellSize},3,(Color){0,0,128,255});
    for(int j=1;j<ROWS;j++) DrawLineEx((Vector2){boardX,boardY+j*cellSize},(Vector2){boardX+COLS*cellSize,boardY+j*cellSize},3,(Color){0,0,128,255});
}

void hover(){
    Vector2 mouse = GetMousePosition();
    hoveredcol = (mouse.x - boardX)/cellSize;
    if(hoveredcol<0) hoveredcol=0;
    if(hoveredcol>=COLS) hoveredcol=COLS-1;
    int filledStartRow=ROWS;
    for(int r=ROWS-1;r>=0;r--) if(board[r][hoveredcol]!=' ') filledStartRow=r;
    int filledCount = ROWS - filledStartRow;
    int emptyCount = ROWS - filledCount;
    if(filledCount>0) DrawRectangleRounded((Rectangle){boardX+hoveredcol*cellSize, boardY+filledStartRow*cellSize, cellSize, filledCount*cellSize},0.5f,100,(Color){255,0,0,75});
    if(emptyCount>0) DrawRectangleRounded((Rectangle){boardX+hoveredcol*cellSize, boardY, cellSize, emptyCount*cellSize},0.5f,100,(Color){255,255,0,75});
}

int addPieceAnimated(int col,char type){
    if(col<0||col>=COLS) return -1;
    int row=-1;
    for(int r=ROWS-1;r>=0;r--) if(board[r][col]==' '){ row=r; break; }
    if(row!=-1){
        fallingPiece.col = col;
        fallingPiece.targetRow = row;
        fallingPiece.type = type;
        fallingPiece.y = (float)boardY - cellSize;
        fallingPiece.active = true;
    }
    return row;
}

void drawPieces(){
    for(int c=0;c<COLS;c++){
        for(int r=0;r<ROWS;r++){
            if(board[r][c]!=' ' && !(fallingPiece.active && fallingPiece.col==c && fallingPiece.targetRow==r)){
                DrawCircle(boardX+c*cellSize+cellSize/2, boardY+r*cellSize+cellSize/2, cellSize*3/8+4, BLACK);
                DrawCircle(boardX+c*cellSize+cellSize/2, boardY+r*cellSize+cellSize/2, cellSize*3/8, board[r][c]=='X'?(Color){0,0,255,255}:(Color){255,0,0,255});
            }
        }
    }
    if(fallingPiece.active){
        float speed=1500.0f;
        fallingPiece.y += speed*GetFrameTime();
        float targetY = boardY+fallingPiece.targetRow*cellSize+cellSize/2;
        if(fallingPiece.y>=targetY){
            fallingPiece.y=targetY;
            board[fallingPiece.targetRow][fallingPiece.col]=fallingPiece.type;
            fallingPiece.active=false;
            movesCount++;
            gameWinner=checkWinner(fallingPiece.type);
            currentPlayer = (fallingPiece.type=='X')?'O':'X';
            if(gameWinner.winner!=0 && diff!=PVP) saveScore(diff, movesCount);
        }
        DrawCircle(boardX+fallingPiece.col*cellSize+cellSize/2, fallingPiece.y, cellSize*3/8+4, BLACK);
        DrawCircle(boardX+fallingPiece.col*cellSize+cellSize/2, fallingPiece.y, cellSize*3/8, fallingPiece.type=='X'?(Color){0,0,255,255}:(Color){255,0,0,255});
    }
}

void aiLogic(){
    int col;
    if(gameWinner.winner!=0 || movesCount>=MAX_MOVES || fallingPiece.active) return;
    if(currentPlayer=='X' && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) addPieceAnimated(hoveredcol,'X');
    else if(currentPlayer=='O'){
        aiDelay+=GetFrameTime();
        if(aiDelay>=0.45f){
            switch(diff){
                case EASY: col=cpuMoveEasy('O','X'); break;
                case MEDIUM: col=cpuMoveMedium('O','X'); break;
                case HARD: col=cpuMoveHard('O','X'); break;
                default: col=-1; break;
            }
            addPieceAnimated(col,'O');
            aiDelay=0;
        }
    }
}

void winCheck(){
    if(gameWinner.winner!=0){
        int startX=boardX+gameWinner.startcol*cellSize+cellSize/2;
        int startY=boardY+gameWinner.startrow*cellSize+cellSize/2;
        int endX=boardX+gameWinner.endcol*cellSize+cellSize/2;
        int endY=boardY+gameWinner.endrow*cellSize+cellSize/2;
        DrawLineEx((Vector2){startX,startY},(Vector2){endX,endY},8,gameWinner.winner==1?GREEN:ORANGE);
    }
    if(gameWinner.winner==1) DrawText("X wins!", boardX, boardY-50, 40, (Color){0,0,255,255});
    else if(gameWinner.winner==2) DrawText("O wins!", boardX, boardY-50, 40, (Color){255,0,0,255});
    else if(movesCount==MAX_MOVES) DrawText("Draw!", boardX, boardY-50, 40, BLACK);

    if(diff != PVP && gameWinner.winner == 1){
        int base, penalty;
        if(diff == EASY){ base=1000; penalty=10; }
        else if(diff == MEDIUM){ base=1200; penalty=12; }
        else { base=1500; penalty=15; }

        int score = base - movesCount * penalty;
        if(score < 0) score = 0;

        char scoreText[50];
        sprintf(scoreText, "Score: %d", score);
        DrawText(scoreText, boardX + cellSize*COLS - 180, boardY - 50, 30, DARKGRAY);
    }
}

void pvp(){
    drawBoard();
    if(gameWinner.winner==0){
        hover();
        DrawText(currentPlayer=='X'?"X Turn":"O Turn", boardX, boardY-50, 40, currentPlayer=='X'?(Color){0,0,255,255}:(Color){255,0,0,255});
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) addPieceAnimated(hoveredcol,currentPlayer);
        if(backButton(300,550)){ current_state=MAIN_MENU; startGame();}
    } else {
        if(finishButton(300,550)) current_state=END_SCREEN;
    }
    drawPieces();
    winCheck();
}

void cpuGame(){
    drawBoard();
    if(gameWinner.winner==0){
        hover();
        DrawText(currentPlayer=='X'?"X Turn":"O Turn", boardX, boardY-50, 40, currentPlayer=='X'?(Color){0,0,255,255}:(Color){255,0,0,255});
        aiLogic();
        if(backButton(300,550)) current_state=CPU_MENU;
    } else {
        if(finishButton(300,550)) current_state=END_SCREEN;
    }
    drawPieces();
    winCheck();
}

void cpuDiff(Difficulty d){
    switch(d){
        case EASY: case MEDIUM: case HARD: cpuGame(); break;
        case PVP: pvp(); break;
    }
}

void rules(){
    ClearBackground((Color){173,216,230});
    const char *lines[] = {
        "Rules:","",
        "Connect 4 is a two-player game.",
        "Player 1 uses 'X' and Player 2 uses 'O'.",
        "Players take turns dropping their piece into one of the 7 columns.",
        "Pieces stack from the bottom up in the selected column.",
        "The goal is to connect 4 of your pieces in a row:",
        "  - Horizontally",
        "  - Vertically",
        "  - Diagonally",
        "The game ends when:",
        "  - A player connects 4 of their pieces in a row -> that player wins",
        "  - The board is full -> the game is a draw"
    };
    int fontSize=20,lineSpacing=30,totalHeight=(sizeof(lines)/sizeof(lines[0]))*lineSpacing;
    int startY=(600-totalHeight)/2;
    for(int i=0;i<sizeof(lines)/sizeof(lines[0]);i++){
        int textWidth=MeasureText(lines[i],fontSize);
        if(i==0) DrawText(lines[i],330,70,50,(Color){80,0,80,255});
        else DrawText(lines[i],(800-textWidth)/2,startY+i*lineSpacing,fontSize,(Color){48,25,52,255});
    }
    if(backButton(500,550)) current_state=MAIN_MENU;
}

int main(void){
    InitWindow(800,600,"Connect 4");
    SetTargetFPS(90);
    startGame();
    while(!WindowShouldClose() && current_state!=EXIT_GAME){
        BeginDrawing();
        ClearBackground((Color){173,216,230});
        switch(current_state){
            case MAIN_MENU: menu(); break;
            case CPU_MENU: cpuMenu(); break;
            case CPU_GAME: cpuDiff(diff); break;
            case PVP_GAME: diff=PVP; cpuDiff(diff); break;
            case END_SCREEN: endScreen(); break;
            case SHOW_RULES: rules(); break;
            case SHOW_SCORES: showScores(); break;
            default: break;
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
