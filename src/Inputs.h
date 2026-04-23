#include "ClassDef.h"

Vector2 GetDirectionalInputV() {
    Vector2 input = {0.0f, 0.0f};

    if (IsKeyDown(KEY_W)) {
        input.y -= 1.0f; // Move up
    }
    if (IsKeyDown(KEY_S)) {
        input.y += 1.0f; // Move down
    }
    if (IsKeyDown(KEY_A)) {
        input.x -= 1.0f; // Move left
    }
    if (IsKeyDown(KEY_D)) {
        input.x += 1.0f; // Move right
    }

    return input;
}

Vector2 GetDirectionalInputV2() {
    Vector2 input = {0.0f, 0.0f};

    if (IsKeyDown(KEY_UP)) {
        input.y -= 1.0f; // Move up
    }
    if (IsKeyDown(KEY_DOWN)) {
        input.y += 1.0f; // Move down
    }
    if (IsKeyDown(KEY_LEFT)) {
        input.x -= 1.0f; // Move left
    }
    if (IsKeyDown(KEY_RIGHT)) {
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