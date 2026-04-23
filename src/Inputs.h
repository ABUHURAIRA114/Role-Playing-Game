#include "ClassDef.h"

Vector2 GetDirectionalInputV(int up, int down, int left, int right) {
    Vector2 input = {0.0f, 0.0f};

    if (IsKeyDown(up)) {
        input.y -= 1.0f; // Move up
    }
    if (IsKeyDown(down)) {
        input.y += 1.0f; // Move down
    }
    if (IsKeyDown(left)) {
        input.x -= 1.0f; // Move left
    }
    if (IsKeyDown(right)) {
        input.x += 1.0f; // Move right
    }

    return input;
}

float GetInputODFrom(int a, int b) {
    float input = 0.0f;

    if (IsKeyDown(a)) {
        input -= 1.0f; // Move up
    }
    if (IsKeyDown(b)) {
        input += 1.0f; // Move down
    }

    return input;
}