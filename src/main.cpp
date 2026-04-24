#include "FuncDef.h"

GlobalInfo gI;
Scene scene;
SaveSystem save("./SaveFile.txt");

int main () {

    InitWindow(gI.SCREEN_WIDTH, gI.SCREEN_HEIGHT, "Role-Playing Game");
    SetTargetFPS(60);
    
    scene.sceneCamera.speed = 1.0f;
    scene.sceneCamera.sensitivity = 0.5f;

    // scene.AddObject(new Box("Box 1", {0, 0, 0}, 1));
    // scene.AddObject(new Box("Box 2", {2, 0, 2}, 1));
    scene.AddUIObject(new Button("Button 1", "Click To Add Box", {gI.SCREEN_WIDTH - 200, 30}, {200, 60}, 20, MAROON));
 
    UIGrid grid({gI.SCREEN_WIDTH-300, 100}, 45);
  
    FilePathList files = LoadDirectoryFiles(gI.MODELS_FOLDER_PATH.c_str());
    for (int i = 0; i<(int)files.count; i++)
    {
        if (IsFileExtension(files.paths[i], ".obj") || IsFileExtension(files.paths[i], ".gltf"))
        {
            try
            {
                string name = GetFileNameWithoutExt(files.paths[i]);
                gI.models[name] = LoadModel(files.paths[i]);
                gI.textures[name] = LoadTexture((gI.TEXTURES_FOLDER_PATH+"\\"+name+".png").c_str());
                
                scene.AddUIObject(new Button("Spawner"+to_string(i), name, {0,0}, {300, 40}, 20, DARKGRAY));
                grid.AddElement(scene.ui[scene.uiCount-1]);
            }
            catch(...)
            {
                cout<<"MASLA!!!\n";
            }
        }
    }
    grid.OrderUI(VERTICAL);

    UnloadDirectoryFiles(files);

    // model.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = texture;
    string text = "0", speedText = "", buttonText = "";
    float dT = GetFrameTime();
    save.LoadScene(scene);

    while (WindowShouldClose() == false){
   
        dT = GetFrameTime();

        if (IsMouseButtonPressed(gI.FREE_CAMERA_KEY)) DisableCursor(); 
        else if (IsMouseButtonReleased(gI.FREE_CAMERA_KEY)) EnableCursor(); 
        
        speedText = "";
        text = "Camera Off";
        if (IsMouseButtonDown(gI.FREE_CAMERA_KEY))
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

        if (IsMouseButtonDown(gI.SELECTION_KEY))
            scene.SelectObject(GetScreenToWorldRay(GetMousePosition(), scene.sceneCamera.Camera()));
        
        Button* button = dynamic_cast<Button*>(scene.ui[0]);
        if (button) if (button->IsClicked()) scene.AddObject(new Box());
        scene.ObjectSpawn();

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

            DrawText(speedText.c_str(), gI.SCREEN_WIDTH/2, gI.SCREEN_HEIGHT/2, 40, DARKGRAY);
            
            scene.DrawSceneUI();

        EndDrawing();

    }
    save.SaveScene(scene);
    gI.UnloadThings();
    CloseWindow();
}