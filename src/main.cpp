#include "FuncDef.h"

Scene scene;

int main () {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Role-Playing Game");
    SetTargetFPS(60);
    
    scene.sceneCamera.speed = 1.0f;
    scene.sceneCamera.sensitivity = 0.5f;

    scene.AddObject(new Box({0, 0, 0}, 1));
    scene.AddObject(new Box({2, 0, 2}, 1));

    Ray tRay = {scene.sceneCamera.Camera().position, {0, 0, 0}};
    RayCollision rC = {false, 0, {0, 0, 0}, {0, 0, 0}};

    Box* selected = nullptr;

    string text = "0", speedText = "";
    float dT = GetFrameTime(), selectionSpeed = 1.0f;

    while (WindowShouldClose() == false){
   
        dT = GetFrameTime();

        if (IsKeyPressed(FREE_CAMERA_KEY)) DisableCursor(); 
        else if (IsKeyReleased(FREE_CAMERA_KEY)) EnableCursor(); 
        
        speedText = "";
        text = "Camera Off";
        if (IsKeyDown(FREE_CAMERA_KEY))
        {   
            text = "Camera Mode";
            
            scene.sceneCamera.CameraFreeMove(dT);
            if (GetMouseWheelMoveV().y != 0)
            {
                scene.sceneCamera.speed += GetMouseWheelMoveV().y * 0.5f;
                scene.sceneCamera.speed = Clamp(scene.sceneCamera.speed, 0.1f, MAX_SPEED);
                speedText = to_string(scene.sceneCamera.speed);
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
                selected->Position({selected->Position().x + GetDirectionalInputV(KEY_W, KEY_S, KEY_A, KEY_D).x * dT * selectionSpeed, selected->Position().y  + GetInputODFrom(KEY_Q, KEY_E) * dT * selectionSpeed, selected->Position().z + GetDirectionalInputV(KEY_W, KEY_S, KEY_A, KEY_D).y * selectionSpeed * dT});
                selected->Rotation({selected->Rotation().x + GetDirectionalInputV(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT).y * dT * selectionSpeed, selected->Rotation().y + GetDirectionalInputV(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT).x * dT * selectionSpeed, selected->Rotation().z});
            }
        }

        tRay = GetScreenToWorldRay(GetMousePosition(), scene.sceneCamera.Camera());
        if (IsKeyPressed(SELECTION_KEY))
        {
            selected = nullptr;
            for (int i = 0; i < scene.ObjectCount(); i++)
            {
                Box* box = scene.Objects()[i];
                rC = GetRayCollisionBox(tRay, box->Boundary());
                if (rC.hit) {
                    selected = box;
                    break;
                }
            } 
        }

        BeginDrawing();
        BeginMode3D(scene.sceneCamera.Camera());

            ClearBackground(RAYWHITE);

            DrawGrid(100, 1.0f);
            for (int i = 0; i < scene.ObjectCount(); i++)
            {
                Box* box = scene.Objects()[i];
                DrawModel(box->Model(), box->Position(), box->Size(), DARKBLUE);
            }
            DrawCubeWires((Vector3){ 2.0f, 0.5f, 2.0f }, 1.0f, 1.0f, 1.0f, DARKGRAY);

        EndMode3D();

            DrawText("Camera", 10, 20, 20, DARKGREEN);
            DrawText(TextFormat("X: %.2f Y: %.2f Z: %.2f", scene.sceneCamera.Camera().position.x, scene.sceneCamera.Camera().position.y, scene.sceneCamera.Camera().position.z), 10, 50, 20, DARKBROWN);
            DrawText(text.c_str(), 10, 80, 20, DARKGRAY);

            // Selected Info
            if (selected != nullptr)
            {
                DrawText("Selected", 10, 110, 20, DARKGREEN);
                DrawText(TextFormat("X: %.2f Y: %.2f Z: %.2f", selected->Position().x, selected->Position().y, selected->Position().z), 10, 140, 20, DARKGREEN);
            }
            DrawText(speedText.c_str(), SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 40, DARKGRAY);

        EndDrawing();

    }

    CloseWindow();
}