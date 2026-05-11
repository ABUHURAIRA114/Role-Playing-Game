#include "FuncDef.h"

GlobalInfo gI;
SaveSystem save = SaveSystem(gI.SAVE_FOLDER_PATH);
string text;
bool close; // check for when the game should close

// Purpose : Runs at the start of the game, sets things up
// Params  : void
// Returns : void
void Start()
{
    srand(time(0)); 
    gI.scene.sceneCamera.speed = 1.0f;
    gI.scene.sceneCamera.sensitivity = 0.5f;
    gI.scene.AddUIObject(new Button("Button 1"      , "Enemy Spawner"   , (Vector2){(gI.SCREEN_WIDTH - 200.0f), 30}  , (Vector2){200, 40}, 20, MAROON));
    gI.scene.AddUIObject(new Button("Button 2"      , "Civil Spawner"   , (Vector2){(gI.SCREEN_WIDTH - 200.0f), 80}  , (Vector2){200, 40}, 20, MAROON));
    gI.scene.AddUIObject(new Button("Button 3"      , "Merchant Spawner", (Vector2){(gI.SCREEN_WIDTH - 200.0f), 130} , (Vector2){200, 40}, 20, MAROON));
    gI.scene.AddUIObject(new Button("Button Boss"   , "Boss Spawner"    , (Vector2){(gI.SCREEN_WIDTH - 200.0f), 180} , (Vector2){200, 40}, 20, DARKPURPLE));
    gI.scene.AddUIObject(new Button("Button 4"      , "Collider"        , (Vector2){(gI.SCREEN_WIDTH - 410.0f), 30}  , (Vector2){200, 40}, 20, DARKGREEN));

    gI.LoadThings();
    save.LoadScene(gI.scene);
    gI.scene.gameMode = MENU;

    PlayMusicStream(gI.bgMusics[gI.currentMusic]);
}

// Purpose : Runs everyframe for gameloop
// Params  : void
// Returns : void
void Update()
{
    gI.MusicLoop();
    gI.dT = GetFrameTime();
    if (gI.mode == GAME)
    {
        if (IsKeyDown(KEY_E) && IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_LEFT_ALT)) gI.mode = EDITOR;

        switch (gI.scene.gameMode)
        {
            case MENU:
            {
                Button* play     = dynamic_cast<Button*>(gI.scene.ui[gI.scene.FindUIObjectIndex("MEN_BTN_PLAY")]);
                Button* quit     = dynamic_cast<Button*>(gI.scene.ui[gI.scene.FindUIObjectIndex("MEN_BTN_QUIT")]);
                Button* tutorial = dynamic_cast<Button*>(gI.scene.ui[gI.scene.FindUIObjectIndex("MEN_BTN_TUTORIAL")]);
                Button* tutClose = dynamic_cast<Button*>(gI.scene.ui[gI.scene.FindUIObjectIndex("MEN_TUT_BTN_CLOSE")]);
 
                if      (play     && play->IsClicked() && !gI.scene.tutorialVisible)    { gI.scene.gameMode = PLAY; ShowCursor(); save.LoadPlayer();  gI.LoadNPCs();}
                else if (quit     && quit->IsClicked() && !gI.scene.tutorialVisible)    { close = true; }
                else if (tutorial && tutorial->IsClicked())                             { gI.scene.tutorialVisible = true;  }
                else if (tutClose && tutClose->IsClicked())                             { gI.scene.tutorialVisible = false; }

            }
            break;

            case STATS:
            {
                if (!gI.scene.player) return;

                gI.SetPlayerStats();
            }
            break;

            case PAUSE:
            {
                if (!gI.scene.player) return;

                if (IsKeyPressed(KEY_ESCAPE)) { gI.scene.gameMode = PLAY; DisableCursor(); }

                Button* resume = dynamic_cast<Button*>(gI.scene.ui[gI.scene.FindUIObjectIndex("PAU_BTN_RESUME")]);
                Button* menu   = dynamic_cast<Button*>(gI.scene.ui[gI.scene.FindUIObjectIndex("PAU_BTN_MENU")]);
                Button* quit   = dynamic_cast<Button*>(gI.scene.ui[gI.scene.FindUIObjectIndex("PAU_BTN_QUIT")]);

                if  (resume && resume->IsClicked())     { gI.scene.gameMode = PLAY; DisableCursor();  }
                else if (menu   && menu->IsClicked())   { gI.scene.gameMode = MENU; ShowCursor(); save.SavePlayer(*gI.scene.player); save.SaveEnemies(); gI.UnloadNPCs();}
                else if (quit   && quit->IsClicked())   { close = true; save.SavePlayer(*gI.scene.player); save.SaveEnemies(); gI.scene.dialogueVisible = false; }
            }
            break;

            case PLAY:
            {
                if (!gI.scene.player) return;

                if (IsKeyPressed(KEY_ESCAPE) &&  !gI.scene.endScreenVisible) { gI.scene.gameMode = PAUSE; ShowCursor(); }

                gI.scene.player->   Update();
                gI.scene.player->   UpdateEffects();
                gI.scene.player->   StateUpdate();
                gI.scene.player->   Attack();
                gI.scene.player->   Dialogue();
                gI.scene.           UpdateNPCs();

                if (gI.scene.endScreenVisible)
                {
                    gI.scene.endScreenTime+=gI.dT;
                    if (GetKeyPressed() && gI.scene.endScreenTime>=5)
                    {
                        gI.scene.endScreenTime = 0;
                        gI.UnloadNPCs();
                        save.SavePlayer(*gI.scene.player); 
                        gI.scene.endScreenVisible = false;
                        ShowCursor(); 
                        gI.scene.gameMode = MENU;
                    }
                }
            }
            break;
        }
    }
    else if (gI.mode == EDITOR)
    {
        if (IsKeyPressed(KEY_ESCAPE)) { gI.mode = GAME; gI.scene.gameMode = MENU; ShowCursor(); }

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

        gI.scene.ObjectSpawn();
        
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C)) gI.scene.DuplicateSelected();
    }
}

int main () {

    InitWindow(gI.SCREEN_WIDTH, gI.SCREEN_HEIGHT, "The Kalled City Of Waloon");
    InitAudioDevice();
    SetTargetFPS(60);
    Start(); // initial setup

    while (!close){
   
        Update(); // run the update function 

        BeginMode3D(((gI.mode==EDITOR || !gI.scene.player)?gI.scene.sceneCamera.Camera():gI.scene.player->Camera()));

            ClearBackground(SKYBLUE);
            
            gI.scene.DrawScene();
            
            if (gI.mode == EDITOR) DrawGrid(10000, 1.0f);
            if (gI.scene.selected)
            {
                Box* box = dynamic_cast<Box*>(gI.scene.selected);
                Collider* col = dynamic_cast<Collider*>(gI.scene.selected);

                if (box) DrawBoundingBox(box->Boundary(), GREEN);
                else if (col) DrawBoundingBox(col->Boundary(), LIME);
            }

        EndMode3D();

        BeginDrawing();

            if (gI.mode == EDITOR)
            {
                DrawText("Camera", 10, 20, 20, DARKGREEN);
                DrawText(TextFormat("X: %.2f Y: %.2f Z: %.2f", gI.scene.sceneCamera.Camera().position.x, gI.scene.sceneCamera.Camera().position.y, gI.scene.sceneCamera.Camera().position.z), 10, 50, 20, DARKBROWN);
                
                DrawText(text.c_str(), 10, 80, 20, DARKGRAY);

                if (gI.scene.selected != nullptr)
                {
                    DrawText("Selected", 10, 110, 20, DARKGREEN);
                    DrawText(TextFormat("Name: %s", gI.scene.selected->Name().c_str()), 10, 140, 20, DARKGREEN);
                    DrawText(TextFormat("X: %.2f Y: %.2f Z: %.2f", gI.scene.selected->Position().x, gI.scene.selected->Position().y, gI.scene.selected->Position().z), 10, 170, 20, DARKGREEN);
                }
            }

            gI.scene.DrawSceneUI(gI.mode);

            if (gI.mode == GAME && gI.scene.dialogueVisible)
                DrawText("[E] Close", 160, gI.SCREEN_HEIGHT - 280, 18, DARKBROWN);

        EndDrawing();
    }

    save.SaveScene(gI.scene);
    
    gI.UnloadThings();
    cout<<"ENDING";
    CloseWindow();
}