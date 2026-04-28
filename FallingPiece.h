#pragma once

struct FallingPiece {
    int   col       = 0;
    int   targetRow = 0;
    char  piece     = ' ';
    float y         = 0.0f;
    bool  active    = false;
};