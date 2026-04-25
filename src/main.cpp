#include "FuncDef.h"

GlobalInfo gI;
SaveSystem save(gI.SAVE_FOLDER_PATH+R"(\scene.txt)");
float dT;
string text, speedText;

void Start()
{
    gI.scene.sceneCamera.speed = 1.0f;
    gI.scene.sceneCamera.sensitivity = 0.5f;
    gI.scene.AddUIObject(new Button("Button 1", "Click To Add Box", {gI.SCREEN_WIDTH - 200, 30}, {200, 60}, 20, MAROON));
    gI.LoadThings();
    save.LoadScene(gI.scene);
}

void Update()
{
    dT = GetFrameTime();
    gI.Shade();
    if (IsMouseButtonPressed(gI.FREE_CAMERA_KEY)) DisableCursor(); 
    else if (IsMouseButtonReleased(gI.FREE_CAMERA_KEY)) EnableCursor(); 
    
    speedText = "";
    text = "Camera Off";
    if (IsMouseButtonDown(gI.FREE_CAMERA_KEY))
    {   
        text = "Camera Mode";
        
        gI.scene.sceneCamera.CameraFreeMove(dT);
        gI.scene.sceneCamera.SpeedScroll();
    }
    else 
    {
        gI.scene.SpeedScroll();
        gI.scene.SelectionMove(dT);
    }

    if (IsMouseButtonDown(gI.SELECTION_KEY))
        gI.scene.SelectObject(GetScreenToWorldRay(GetMousePosition(), gI.scene.sceneCamera.Camera()));
    
    Button* button = dynamic_cast<Button*>(gI.scene.ui[0]);
    if (button) if (button->IsClicked()) gI.scene.AddObject(new Box());
    gI.scene.ObjectSpawn();
}

int main () {

    InitWindow(gI.SCREEN_WIDTH, gI.SCREEN_HEIGHT, "Role-Playing Game");
    SetTargetFPS(60);
    Start();

    while (WindowShouldClose() == false){
   
        Update();

        BeginDrawing();

            BeginMode3D(gI.scene.sceneCamera.Camera());

                ClearBackground(SKYBLUE);
                DrawGrid(10000, 1.0f);
                gI.scene.DrawScene();
                if (gI.scene.selected) DrawBoundingBox(gI.scene.selected->Boundary(), GREEN);

            EndMode3D();

            DrawText("Camera", 10, 20, 20, DARKGREEN);
            DrawText(TextFormat("X: %.2f Y: %.2f Z: %.2f", gI.scene.sceneCamera.Camera().position.x, gI.scene.sceneCamera.Camera().position.y, gI.scene.sceneCamera.Camera().position.z), 10, 50, 20, DARKBROWN);
            DrawText(text.c_str(), 10, 80, 20, DARKGRAY);

            if (gI.scene.selected != nullptr)
            {
                DrawText("Selected", 10, 110, 20, DARKGREEN);
                DrawText(TextFormat("Name: %s", gI.scene.selected->Name().c_str()), 10, 140, 20, DARKGREEN);
                DrawText(TextFormat("X: %.2f Y: %.2f Z: %.2f", gI.scene.selected->Position().x, gI.scene.selected->Position().y, gI.scene.selected->Position().z), 10, 170, 20, DARKGREEN);
            }

            DrawText(speedText.c_str(), gI.SCREEN_WIDTH/2, gI.SCREEN_HEIGHT/2, 40, DARKGRAY);
            
            gI.scene.DrawSceneUI();

        EndDrawing();

    }

    save.SaveScene(gI.scene);
    gI.UnloadThings();

    CloseWindow();
}