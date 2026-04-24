#include "FuncDef.h"

Scene scene;

int main () {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Role-Playing Game");
    SetTargetFPS(60);
    
    scene.sceneCamera.speed = 1.0f;
    scene.sceneCamera.sensitivity = 0.5f;

    scene.AddObject(new Box("Box 1", {0, 0, 0}, 1, GRAY));
    scene.AddObject(new Box("Box 2", {2, 0, 2}, 1, RED));

    string text = "0", speedText = "";
    float dT = GetFrameTime();

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
            scene.sceneCamera.SpeedScroll();
        }
        else 
        {
            scene.SpeedScroll();
            scene.SelectionMove(dT);
        }

        if (IsKeyPressed(SELECTION_KEY))
            scene.SelectObject(GetScreenToWorldRay(GetMousePosition(), scene.sceneCamera.Camera()));
        

        BeginDrawing();
        BeginMode3D(scene.sceneCamera.Camera());

            ClearBackground(RAYWHITE);

            DrawGrid(100, 1.0f);
            scene.DrawScene();

        EndMode3D();

            DrawText("Camera", 10, 20, 20, DARKGREEN);
            DrawText(TextFormat("X: %.2f Y: %.2f Z: %.2f", scene.sceneCamera.Camera().position.x, scene.sceneCamera.Camera().position.y, scene.sceneCamera.Camera().position.z), 10, 50, 20, DARKBROWN);
            DrawText(text.c_str(), 10, 80, 20, DARKGRAY);

            // Selected Info
            if (scene.selected != nullptr)
            {
                DrawText("Selected", 10, 110, 20, DARKGREEN);
                DrawText(TextFormat("Name: %s", scene.selected->Name().c_str()), 10, 140, 20, DARKGREEN);
                DrawText(TextFormat("X: %.2f Y: %.2f Z: %.2f", scene.selected->Position().x, scene.selected->Position().y, scene.selected->Position().z), 10, 170, 20, DARKGREEN);
            }

            DrawText(speedText.c_str(), SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 40, DARKGRAY);

        EndDrawing();

    }

    CloseWindow();
}