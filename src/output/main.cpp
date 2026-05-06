#include "FuncDef.h"

GlobalInfo gI;
SaveSystem save(gI.SAVE_FOLDER_PATH+R"(\scene.txt)");
string text;
bool close;

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
    
}

void Update()
{
    gI.dT = GetFrameTime();
    if (gI.mode == GAME)
    {
        if (IsKeyDown(KEY_E) && IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_LEFT_ALT)) gI.mode = EDITOR;

        switch (gI.scene.gameMode)
        {
            case MENU:
            {
                // Handle menu buttons
                Button* play   = dynamic_cast<Button*>(gI.scene.ui[gI.scene.FindUIObjectIndex("MEN_BTN_PLAY")]);
                // Button* editor = dynamic_cast<Button*>(gI.scene.ui[gI.scene.FindUIObjectIndex("MEN_BTN_EDITOR")]);
                Button* quit   = dynamic_cast<Button*>(gI.scene.ui[gI.scene.FindUIObjectIndex("MEN_BTN_QUIT")]);

                if (play   && play->IsClicked())   { gI.scene.gameMode = PLAY; ShowCursor(); save.LoadPlayer(*gI.scene.player); }
                // if (editor && editor->IsClicked())  { gI.mode = EDITOR; ShowCursor(); }
                if (quit   && quit->IsClicked())    close = true;
                break;
            }

            // case STATS:
            // {
            //     gI.SetPlayerStats();

            //     break;
            // }

            case PAUSE:
            {
                if (IsKeyPressed(KEY_ESCAPE)) gI.scene.gameMode = PLAY;

                Button* resume = dynamic_cast<Button*>(gI.scene.ui[gI.scene.FindUIObjectIndex("PAU_BTN_RESUME")]);
                Button* menu   = dynamic_cast<Button*>(gI.scene.ui[gI.scene.FindUIObjectIndex("PAU_BTN_MENU")]);
                Button* quit   = dynamic_cast<Button*>(gI.scene.ui[gI.scene.FindUIObjectIndex("PAU_BTN_QUIT")]);

                if (resume && resume->IsClicked()) { gI.scene.gameMode = PLAY; DisableCursor(); }
                if (menu   && menu->IsClicked())   { gI.scene.gameMode = MENU; ShowCursor(); save.SavePlayer(*gI.scene.player); }
                if (quit   && quit->IsClicked())   { close = true; save.SavePlayer(*gI.scene.player); }
                break;
            }

            case PLAY:
            {
                if (IsKeyPressed(KEY_ESCAPE)) { gI.scene.gameMode = PAUSE; ShowCursor(); }

                gI.scene.player->Update();
                gI.scene.player->UpdateEffects();
                gI.scene.player->StateUpdate();
                gI.scene.player->Attack();
                gI.scene.player->Dialogue();
                gI.scene.UpdateNPCs();

                if (gI.scene.endScreenVisible && GetKeyPressed() && gI.scene.endScreenTime>=5)
                {
                    save.SavePlayer(*gI.scene.player); 
                    gI.scene.endScreenVisible = false;
                    ShowCursor(); 
                    gI.scene.gameMode = MENU;
                }
                break;
            }
        }
    }
    else if (gI.mode == EDITOR)
    {
        // ESC from editor back to menu
        if (IsKeyPressed(KEY_ESCAPE)) { gI.mode = GAME; gI.scene.gameMode = MENU; ShowCursor(); }

        gI.dT = GetFrameTime();
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
        
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C))
            gI.scene.DuplicateSelected();
    }
}

int main () {

    InitWindow(gI.SCREEN_WIDTH, gI.SCREEN_HEIGHT, "Role-Playing Game");
    SetTargetFPS(60);
    Start();

    while (!WindowShouldClose()){
   
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
            
            gI.scene.DrawSceneUI(gI.mode);

            // Merchant interact prompt (drawn in 2D after 3D mode)
            // if (mode == GAME && gI.scene.npcs[0])
            //     gI.scene.npcs[0]->DrawInteractPrompt();
            
            // Dialogue close hint
            if (gI.mode == GAME && gI.scene.dialogueVisible)
                DrawText("[E] Close", 160, gI.SCREEN_HEIGHT - 280, 18, DARKBROWN);

        EndDrawing();

    }

    save.SaveScene(gI.scene);
    
    gI.UnloadThings();

    cout<<"ENDING";
    CloseWindow();
}