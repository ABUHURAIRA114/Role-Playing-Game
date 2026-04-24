#include "FuncDef.h"

Scene scene;

int main () {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Role-Playing Game");
    SetTargetFPS(60);
    
    scene.sceneCamera.speed = 1.0f;
    scene.sceneCamera.sensitivity = 0.5f;

    scene.AddObject(new Box("Box 1", {0, 0, 0}, 1, GRAY));
    scene.AddObject(new Box("Box 2", {2, 0, 2}, 1, RED));
    scene.AddUIObject(new Button("Button 1", "Click To Add Box", {SCREEN_WIDTH - 200, 30}, {200, 60}, 20, DARKBLUE));
 
    UIGrid grid({SCREEN_WIDTH/2-150, 30}, 30);
    
    FilePathList files = LoadDirectoryFiles(MODELS_FOLDER_PATH.c_str());
    for (int i = 0; i<files.count; i++)
    {
        if (IsFileExtension(files.paths[i], ".obj") || IsFileExtension(files.paths[i], ".gltf"))
        {
            scene.AddUIObject(new Button("Button"+to_string(i), files.paths[i], {0,0}, {300, 40}, 20, DARKBLUE));
            grid.AddElement(scene.ui[scene.uiCount-1]);
        }
    }
    grid.OrderUI(VERTICAL);

    UnloadDirectoryFiles(files);

    // model.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = texture;
    string text = "0", speedText = "", buttonText = "";
    float dT = GetFrameTime();

    while (WindowShouldClose() == false){
   
        dT = GetFrameTime();

        if (IsMouseButtonPressed(FREE_CAMERA_KEY)) DisableCursor(); 
        else if (IsMouseButtonReleased(FREE_CAMERA_KEY)) EnableCursor(); 
        
        speedText = "";
        text = "Camera Off";
        if (IsMouseButtonDown(FREE_CAMERA_KEY))
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

        if (IsMouseButtonDown(SELECTION_KEY))
            scene.SelectObject(GetScreenToWorldRay(GetMousePosition(), scene.sceneCamera.Camera()));
        
        Button* button = dynamic_cast<Button*>(scene.ui[0]);
        if (button) if (button->IsClicked()) scene.AddObject(new Box());

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
            
            scene.DrawSceneUI();

        EndDrawing();

    }

    scene.UnloadThings();

    CloseWindow();
}