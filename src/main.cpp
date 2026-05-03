#include "FuncDef.h"

GlobalInfo gI;
SaveSystem save(gI.SAVE_FOLDER_PATH+R"(\scene.txt)");
string text;


void Start()
{
    srand(time(0)); 
    gI.scene.sceneCamera.speed = 1.0f;
    gI.scene.sceneCamera.sensitivity = 0.5f;
    gI.scene.AddUIObject(new Button("Button 1", "Enemy Spawner", (Vector2){gI.SCREEN_WIDTH - 200, 30}, (Vector2){200, 40}, 20, MAROON));
    gI.scene.AddUIObject(new Button("Button 2", "Civil Spawner", (Vector2){gI.SCREEN_WIDTH - 200, 80}, (Vector2){200, 40}, 20, MAROON));
    gI.scene.AddUIObject(new Button("Button 3", "Merchant Spawner", (Vector2){gI.SCREEN_WIDTH - 200, 130}, (Vector2){200, 40}, 20, MAROON));
    gI.scene.AddUIObject(new Button("Button Boss", "Boss Spawner", (Vector2){gI.SCREEN_WIDTH - 200, 180}, (Vector2){200, 40}, 20, DARKPURPLE));
    gI.scene.AddUIObject(new Button("Button 4", "Collider", (Vector2){gI.SCREEN_WIDTH - 410, 30}, (Vector2){200, 40}, 20, DARKGREEN));

    gI.scene.AddUIObject(new Button("Editor", "Editor", (Vector2){gI.SCREEN_WIDTH/2 + 10, 30}, (Vector2){200, 60}, 20, BLUE));
    gI.scene.AddUIObject(new Button("Game", "Game", (Vector2){gI.SCREEN_WIDTH/2 - 210, 30}, (Vector2){200, 60}, 20, MAROON));

    gI.LoadThings();
    save.LoadScene(gI.scene);
    gI.LoadNPCs();
    save.LoadPlayer(*gI.scene.player);
}

void Update()
{
    gI.dT = GetFrameTime();
    gI.Shade(); 
    if (IsMouseButtonPressed(gI.FREE_CAMERA_KEY)) DisableCursor(); 
    else if (IsMouseButtonReleased(gI.FREE_CAMERA_KEY)) EnableCursor(); 

    text = "Camera Off";
    if (IsMouseButtonDown(gI.FREE_CAMERA_KEY))
    {   
        text = "Camera Mode";
        
        gI.scene.sceneCamera.CameraFreeMove();
        gI.scene.sceneCamera.SpeedScroll();
    }
    else 
    {
        gI.scene.SpeedScroll();
        gI.scene.SelectionMove();
    }

    if (IsMouseButtonDown(gI.SELECTION_KEY))
        gI.scene.SelectObject(GetScreenToWorldRay(GetMousePosition(), gI.scene.sceneCamera.Camera()));
    
    Button* button = dynamic_cast<Button*>(gI.scene.ui[0]);
    if (button) if (button->IsClicked()) gI.scene.AddObject(new Box(gI.ENEMY_SPAWNER_NAME, gI.ENEMY_SPAWNER_NAME));
    
    button = dynamic_cast<Button*>(gI.scene.ui[1]);
    if (button) if (button->IsClicked()) gI.scene.AddObject(new Box(gI.CIVIL_SPAWNER_NAME, gI.CIVIL_SPAWNER_NAME));

    button = dynamic_cast<Button*>(gI.scene.ui[2]);
    if (button) if (button->IsClicked()) gI.scene.AddObject(new Box(gI.MERCHANT_SPAWNER_NAME, gI.MERCHANT_SPAWNER_NAME));

    button = dynamic_cast<Button*>(gI.scene.ui[3]);
    if (button) if (button->IsClicked()) gI.scene.AddObject(new Box(gI.BOSS_SPAWNER_NAME, gI.BOSS_SPAWNER_NAME));

    button = dynamic_cast<Button*>(gI.scene.ui[5]);
    if (button) if (button->IsClicked()) gI.mode = EDITOR;

    button = dynamic_cast<Button*>(gI.scene.ui[6]);
    if (button) if (button->IsClicked()) gI.mode = GAME;

    gI.scene.ObjectSpawn();
    
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_D))
        gI.scene.DuplicateSelected();

    if (gI.mode == GAME)
    {
        gI.scene.player->Update();
        gI.scene.player->UpdateEffects();
        gI.scene.player->StateUpdate();

        // Merchant interaction — only allow attack when dialogue is closed
        if (!gI.scene.dialogueVisible)
            gI.scene.player->Attack();

        gI.scene.UpdateNPCs();
        gI.scene.player->Dialogue();

    }
}

int main () {

    InitWindow(gI.SCREEN_WIDTH, gI.SCREEN_HEIGHT, "Role-Playing Game");
    SetTargetFPS(60);
    Start();

    while (WindowShouldClose() == false){
   
        Update(); 

        BeginDrawing();

            BeginMode3D((gI.mode==EDITOR?gI.scene.sceneCamera.Camera():gI.scene.player->Camera()));

                ClearBackground(SKYBLUE);
                // DrawGrid(10000, 1.0f);
                gI.scene.DrawScene();
                if (gI.scene.selected)
                {
                    Box* box = dynamic_cast<Box*>(gI.scene.selected);
                    Collider* col = dynamic_cast<Collider*>(gI.scene.selected);

                    if (box) DrawBoundingBox(box->Boundary(), GREEN);
                    else if (col) DrawBoundingBox(col->Boundary(), LIME);
                }

            EndMode3D();

            // DrawText("Camera", 10, 20, 20, DARKGREEN);
            // DrawText(TextFormat("X: %.2f Y: %.2f Z: %.2f", gI.scene.sceneCamera.Camera().position.x, gI.scene.sceneCamera.Camera().position.y, gI.scene.sceneCamera.Camera().position.z), 10, 50, 20, DARKBROWN);
            // DrawText(TextFormat("X: %.2f Y: %.2f Z: %.2f", gI.scene.player->Position().x, gI.scene.player->Position().y,  gI.scene.player->Position().z), 10, 200, 20, DARKBROWN);
            // DrawText(TextFormat("X: %.2f Y: %.2f", gI.scene.player->Target().x, gI.scene.player->Target().y), 10, 230, 20, DARKBROWN);
            
            // DrawText(text.c_str(), 10, 80, 20, DARKGRAY);
            // DrawText((gI.scene.player->IsGrounded()? "Grounded" : "Not Grounded"), 10, 260, 20, DARKGRAY);

            // if (gI.scene.selected != nullptr)
            // {
            //     DrawText("Selected", 10, 110, 20, DARKGREEN);
            //     DrawText(TextFormat("Name: %s", gI.scene.selected->Name().c_str()), 10, 140, 20, DARKGREEN);
            //     DrawText(TextFormat("X: %.2f Y: %.2f Z: %.2f", gI.scene.selected->Position().x, gI.scene.selected->Position().y, gI.scene.selected->Position().z), 10, 170, 20, DARKGREEN);
            // }
            
            // DrawText(to_string(gI.scene.player->CurrStamina()).c_str(), gI.SCREEN_WIDTH/2, gI.SCREEN_HEIGHT/2, 40, DARKGRAY);
            
            gI.scene.DrawSceneUI();

            // Merchant interact prompt (drawn in 2D after 3D mode)
            // if (mode == GAME && gI.scene.npcs[0])
            //     gI.scene.npcs[0]->DrawInteractPrompt();
            
            // Dialogue close hint
            if (gI.mode == GAME && gI.scene.dialogueVisible)
                DrawText("[E] Close", 160, gI.SCREEN_HEIGHT - 280, 18, DARKBROWN);

        EndDrawing();

    }

    save.SaveScene(gI.scene);
    save.SavePlayer(*gI.scene.player);
    gI.UnloadThings();

    cout<<"ENDING";
    CloseWindow();
}