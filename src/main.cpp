#include "FuncDef.h"

int main () {

    CameraMI camera({ 0.0f, 10.0f, 10.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
    camera.speed = 1.0f;
    camera.sensitivity = 0.5f;
    camera.Target({0, 0, 0});

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "My first RAYLIB program!");
    SetTargetFPS(60);
    Box box[2] = {Box({10, 10, 10}, 2), Box({-10, -10, -10}, 2)};
    Ray tRay = {camera.Camera().position, {0, 0, 0}};
    RayCollision rC = {false, 0, {0, 0, 0}, {0, 0, 0}};
    Box* selected = nullptr;

    string text = "0", speedText = "";
    float dT = GetFrameTime(), selectionSpeed = 1.0f;


    while (WindowShouldClose() == false){
   
        // camTarget.x += GetDirectionalInputV().x;
        // camTarget.z += GetDirectionalInputV().y;

        dT = GetFrameTime();

        if (IsKeyPressed(FREE_CAMERA_KEY))
        {
            DisableCursor();
        }
        else if (IsKeyReleased(FREE_CAMERA_KEY))
        {
            EnableCursor();
        }
        
        speedText = "";
        text = "Camera Off";
        if (IsKeyDown(FREE_CAMERA_KEY))
        {   
            text = "Camera Mode";
            
            camera.CameraFreeMove(dT);
            if (GetMouseWheelMoveV().y != 0)
            {
                camera.speed += GetMouseWheelMoveV().y * 0.5f;
                camera.speed = Clamp(camera.speed, 0.1f, MAX_SPEED);
                speedText = to_string(camera.speed);
            }
        }
        else 
        {
            if (GetMouseWheelMoveV().y != 0)
            {
                selectionSpeed += GetMouseWheelMoveV().y * 0.5f;
                selectionSpeed = Clamp(selectionSpeed, 0.1f, MAX_SPEED);
                speedText = to_string(selectionSpeed);
            }
            if (selected != nullptr)
            {
                selected->Size(selected->Size() + GetInputODFrom(KEY_MINUS, KEY_EQUAL) * dT * selectionSpeed);
                selected->Position({selected->Position().x + GetDirectionalInputV().x * dT * selectionSpeed, selected->Position().y  + GetInputODFrom(KEY_Q, KEY_E) * dT * selectionSpeed, selected->Position().z + GetDirectionalInputV().y * selectionSpeed * dT});
                selected->Rotation({selected->Rotation().x + GetDirectionalInputV2().y * dT * selectionSpeed, selected->Rotation().y + GetDirectionalInputV2().x * dT * selectionSpeed, selected->Rotation().z});
            }
        }

        tRay = GetScreenToWorldRay(GetMousePosition(), camera.Camera());
        if (IsKeyPressed(SELECTION_KEY))
        {
            selected = nullptr;
            for (int i = 0; i < 2; i++)
                {
                    rC = GetRayCollisionBox(tRay, box[i].Boundary());
                    if (rC.hit) {
                        selected = &box[i];
                        break;
                    }
                }
        }

        BeginDrawing();
        BeginMode3D(camera.Camera());

            ClearBackground(RAYWHITE);

            DrawGrid(100, 1.0f);
            for (int i = 0; i < 2; i++)
            {
                DrawModel(box[i].Model(), box[i].Position(), box[i].Size(), DARKBLUE);
            }
            // DrawSphere(camTarget, 0.5f, RED);
            DrawCubeWires((Vector3){ 2.0f, 0.5f, 2.0f }, 1.0f, 1.0f, 1.0f, DARKGRAY);

        EndMode3D();

            DrawText("Camera", 10, 20, 20, DARKGREEN);
            DrawText(TextFormat("X: %.2f", camera.Camera().position.x), 10, 40, 20, DARKGRAY);
            DrawText(TextFormat("Y: %.2f", camera.Camera().position.y), 10, 70, 20, DARKGRAY);
            DrawText(TextFormat("Z: %.2f", camera.Camera().position.z), 10, 100, 20, DARKGRAY);
            DrawText(text.c_str(), 10, 120, 20, DARKGRAY);
            DrawText((rC.hit?"Yes": "NO"), 10, 150, 20, DARKBLUE);

            // Selected Info
            if (selected != nullptr)
            {
                DrawText("Selected", 10, 200, 20, DARKGREEN);
                DrawText(TextFormat("X: %.2f", selected->Position().x), 10, 230, 20, DARKGREEN);
                DrawText(TextFormat("Y: %.2f", selected->Position().y), 10, 280, 20, DARKGREEN);
                DrawText(TextFormat("Z: %.2f", selected->Position().z), 10, 250, 20, DARKGREEN);
            }
            DrawText(speedText.c_str(), SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 20, DARKGRAY);


        EndDrawing();

    }

    CloseWindow();
}