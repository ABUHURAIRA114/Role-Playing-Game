#include "ClassDef.h"

// Misc ==================================================================================================================================================================================

Vector2 GetDirectionalInputV(int up = gI.FORWARD_KEY, int down = gI.BACKWARD_KEY, int left = gI.LEFT_KEY, int right = gI.RIGHT_KEY)
{
    Vector2 input = {0.0f, 0.0f};

    if (IsKeyDown(up)) {
        input.y -= 1.0f; // Move up
    }
    if (IsKeyDown(down)) {
        input.y += 1.0f; // Move down
    }
    if (IsKeyDown(left)) {
        input.x -= 1.0f; // Move left
    }
    if (IsKeyDown(right)) {
        input.x += 1.0f; // Move right
    }

    return input;
}

float GetInputODFrom(int a, int b) 
{
    
    float input = 0.0f;

    if (IsKeyDown(a)) {
        input -= 1.0f; // Move up
    }
    if (IsKeyDown(b)) {
        input += 1.0f; // Move down
    }

    return input;
}

string ReplaceWhiteSpaces(std::string str) 
{
    string newString = str;

    int startPos = 0;
    while((startPos = newString.find(" ", startPos)) != std::string::npos) {
        newString.replace(startPos, 1, "_");
        startPos ++;
    }

    return newString;
}

bool IsBetween(float n, float l, float b)
{
    return n>=l && n<=b;
}

Vector3 Normalize(Vector3 v)
{
    float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length == 0) return {0, 0, 0};
    return {v.x / length, v.y / length, v.z / length};
}

float Magnitude(Vector3 v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector2 Normalize(Vector2 v)
{
    float length = sqrt(v.x * v.x + v.y * v.y);
    if (length == 0) return {0, 0};
    return {v.x / length, v.y / length};
}

Vector3 operator+(Vector3& a, Vector3& b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vector3 operator-(Vector3& a, Vector3& b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vector3 operator-(Vector3& a, Vector2& b)
{
    return {a.x - b.x, a.y, a.z - b.y};
}

Vector3 operator-(Vector2& a, Vector3& b)
{
    return {a.x - b.x, b.y, a.y - b.z};
}

float DotProduct(Vector3 a, Vector3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 CrossProduct(Vector3 a, Vector3 b)
{
    Vector3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

// Box ==================================================================================================================================================================================

void Box::UpdateRotation() {
    
    Quaternion qPitch = QuaternionFromAxisAngle((Vector3){1, 0, 0}, rotation.x);
    Quaternion qYaw   = QuaternionFromAxisAngle((Vector3){0, 1, 0}, rotation.y);
    
    // 3. Combine them (Order matters! Usually Yaw * Pitch * Roll)
    Quaternion qCombined = QuaternionMultiply(qYaw, qPitch);
    
    // 4. Convert the Quaternion to a Transform Matrix
    Matrix rotationMatrix = QuaternionToMatrix(qCombined);
    
    // 5. Apply it to your model
    model.transform = rotationMatrix;
}

void Box::UpdateBoundary() {
    boundary = GetMeshBoundingBox(model.meshes[0]);
    boundary.min = Vector3Scale(boundary.min, size);
    boundary.max = Vector3Scale(boundary.max, size);
    boundary.min = Vector3Add(boundary.min, position);
    boundary.max = Vector3Add(boundary.max, position);
}

// CameraMI ==================================================================================================================================================================================

void CameraMI::CameraFreeMove()
{
    pitch = Clamp(pitch + GetMouseDelta().y * sensitivity * gI.dT, -90.0f, 90.0f);
    yaw = Clamp(yaw + GetMouseDelta().x * sensitivity * gI.dT, -90.0f, 90.0f);
    target.x = camera.position.x + 10*sin(pitch)*cos(yaw);
    target.z = camera.position.z + 10*sin(pitch)*sin(yaw);
    target.y = camera.position.y + 10*cos(pitch);
    camera.target = target;

    direction = target-camera.position;  
    Vector2 input = GetDirectionalInputV(gI.FORWARD_KEY, gI.BACKWARD_KEY, gI.LEFT_KEY, gI.RIGHT_KEY);

    camera.position = (Vector3){
        camera.position.x + speed * gI.dT * (input.y* -direction.x  + input.x * CrossProduct(direction, camera.up).x),
        camera.position.y + speed * gI.dT * input.y * -direction.y,
        camera.position.z + speed * gI.dT * (input.y * -direction.z + input.x * CrossProduct(direction, camera.up).z)
    };
}

void CameraMI::SpeedScroll()
{
    if (GetMouseWheelMoveV().y != 0)
    {
        speed += GetMouseWheelMoveV().y * gI.WHEEL_SENSITIVITY;
        speed = Clamp(speed, 0.1f, gI.MAX_SPEED);
    }
}

// Scene ==================================================================================================================================================================================

void Scene::AddObject(Box* newObject, int i=0) 
{
    string name = ReplaceWhiteSpaces(newObject->Name());

    if (i>0) name = newObject->Name()+to_string(i);
    if (FindObjectIndex(name) != -1) 
    {
        AddObject(newObject, i+1); // try with new name
        return;
    } 
    newObject->Name(name);

    Box** newObjects = new Box*[objectCount + 1];
    for (int i = 0; i < objectCount; i++) {
        newObjects[i] = objects[i];
    }
    newObjects[objectCount] = newObject;
    delete[] objects;
    objects = newObjects;
    objectCount++;
}

void Scene::AddUIObject(RectTransform* newObject, int i=0)
{
    string name = ReplaceWhiteSpaces(newObject->Name());
    if (i>0) name = newObject->Name()+to_string(i);
    if (FindUIObjectIndex(name) != -1) 
    {
        AddUIObject(newObject, i+1); // try with new name
        return;
    } 
    newObject->Name(name);

    RectTransform** newObjects = new RectTransform*[uiCount + 1];
    for (int i = 0; i < uiCount; i++) {
        newObjects[i] = ui[i];
    }
    newObjects[uiCount] = newObject;
    delete[] ui;
    ui = newObjects;
    uiCount++;    
}

void Scene::RemoveObject(int index) {
    if (index < 0 || index >= objectCount) return;
    Box** newObjects = new Box*[objectCount - 1];
    for (int i = 0, j = 0; i < objectCount; i++) {
        if (i != index) {
            newObjects[j++] = objects[i];
        }
    }
    delete[] objects;
    objects = newObjects;
    objectCount--;
}

void Scene::RemoveUIObject(int index) {
    if (index < 0 || index >= uiCount) return;
    RectTransform** newObjects = new RectTransform*[uiCount - 1];
    for (int i = 0, j = 0; i < uiCount; i++) {
        if (i != index) {
            newObjects[j++] = ui[i];
        }
    }
    delete[] ui;
    ui = newObjects;
    uiCount--;
}

int Scene::FindObjectIndex(string name)
{
    for (int i = 0; i < objectCount; i++) {
        if (objects[i]->Name() == name) {
            return i;
        }
    }
    return -1; // Not found
}

int Scene::FindUIObjectIndex(string name)
{
    for (int i = 0; i < uiCount; i++) {
        if (ui[i]->Name() == name) {
            return i;
        }
    }
    return -1; // Not found
}

void Scene::SelectionMove()
{
    if (selected != nullptr)
    {
        Vector2 moveInput = GetDirectionalInputV(gI.FORWARD_KEY, gI.BACKWARD_KEY, gI.LEFT_KEY, gI.RIGHT_KEY);
        Vector2 rotationInput = GetDirectionalInputV(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT);
        float sizeInput = GetInputODFrom(KEY_MINUS, KEY_EQUAL);
        float heightInput = GetInputODFrom(KEY_Q, KEY_E);
        
        selected->Size(selected->Size() + sizeInput * gI.dT * selectionSpeed);

        selected->Position({
            selected->Position().x + moveInput.x * gI.dT * selectionSpeed,
            selected->Position().y  + heightInput * gI.dT * selectionSpeed,
            selected->Position().z + moveInput.y * selectionSpeed * gI.dT 
        });

        selected->Rotation({
            selected->Rotation().x + rotationInput.y * gI.dT * selectionSpeed, 
            selected->Rotation().y + rotationInput.x * gI.dT * selectionSpeed, 
            selected->Rotation().z
        });

        if (IsKeyPressed(KEY_DELETE))
        {
            RemoveObject(FindObjectIndex(selected->Name()));
            selected = nullptr;
        }
    }
}

void Scene::SpeedScroll()
{
    if (GetMouseWheelMoveV().y != 0)
    {
        selectionSpeed += GetMouseWheelMoveV().y * gI.WHEEL_SENSITIVITY;
        selectionSpeed = Clamp(selectionSpeed, 0.1f, gI.MAX_SPEED);
    }
}   

void Scene::DrawScene()
{
    for (int i = 0; i < objectCount; i++)
    {
        Box* box = objects[i];
        DrawModel(box->_Model(), box->Position(), box->Size(), WHITE);
        DrawModelWires(box->_Model(), box->Position(), box->Size(), BLACK);
    }

    for (int i = 0; i < npcCount; i++) npcs[i]->DrawCharacter();
    player->DrawCharacter();
    
    std::vector<std::pair<float, const AnimationData*>> sortedBillboards;
    sortedBillboards.reserve(billboards.size()); // Pre-allocate memory for speed

    // 2. Calculate distances and populate the list
    for (const auto& bb : billboards) 
    {
        // Calculate vector from the camera to the billboard
        float dx = bb.second.camera.position.x - bb.second.position.x;
        float dy = bb.second.camera.position.y - bb.second.position.y;
        float dz = bb.second.camera.position.z - bb.second.position.z;

        // Calculate Squared Distance (Omit the expensive sqrtf call!)
        float distSq = (dx * dx) + (dy * dy) + (dz * dz);

        sortedBillboards.push_back({ distSq, &bb.second });
    }

    // 3. Sort the vector from Furthest to Closest (Descending Order)
    std::sort(sortedBillboards.begin(), sortedBillboards.end(), 
        [](const std::pair<float, const AnimationData*>& a, const std::pair<float, const AnimationData*>& b) {
            return a.first > b.first; // '>' ensures furthest objects are drawn first
        });

    // 4. Render them in the correct sorted order
    for (const auto& item : sortedBillboards) 
    {
        const AnimationData* b = item.second;
        DrawBillboardRec(b->camera, b->frame, b->source, b->position, b->size, b->color);
    }
}

void Scene::DrawSceneUI()
{
    for (int i = 0; i< uiCount; i++)
    {
        RectTransform* uiElement = ui[i];
        Button* button = dynamic_cast<Button*>(uiElement);
        Text* text = dynamic_cast<Text*>(uiElement);
        Banner* banner = dynamic_cast<Banner*>(uiElement);

        if (button != nullptr)
        {
            DrawRectangleRec(button->Rect(), button->BackColor());
            DrawText(button->_Text()._Text().c_str(), button->Rect().x + button->_Text().Rect().x, button->Rect().y + button->_Text().Rect().y, button->_Text().Rect().width, WHITE);
        }
        else if (text != nullptr)
        {
            DrawText(text->_Text().c_str(), text->Rect().x, text->Rect().y, 20, text->_Color());
        }
        else if (banner != nullptr)
        {
            DrawRectangleRec(banner->Rect(), banner->BackColor());
            DrawText(banner->_Text()._Text().c_str(), banner->Rect().x + banner->_Text().Rect().x, banner->Rect().y + banner->_Text().Rect().y, banner->_Text().Rect().width, WHITE);    
        }
    }
}

void Scene::AddNPC(PossessedNPC* npc)
{
    PossessedNPC** newNPCs = new PossessedNPC*[npcCount + 1];
    for (int i = 0; i < npcCount; i++) newNPCs[i] = npcs[i];
    newNPCs[npcCount] = npc;
    delete[] npcs;
    npcs = newNPCs;
    npcCount++;
}

void Scene::UpdateNPCs()
{
    for (int i = 0; i < npcCount; i++)
    {
        npcs[i]->Update();
    }
}

void Scene::SelectObject(Ray ray)
{
    float dist;
    selectionRay = ray;
    selected = nullptr;
    for (int i = 0; i < objectCount; i++)
    {
        Box* box = objects[i];
        selectionRayCollision = GetRayCollisionBox(ray, box->Boundary());
        if (i==0) dist = selectionRayCollision.distance;
        if (selectionRayCollision.hit && selectionRayCollision.distance<=dist) {
            selected = box;
        }
    } 
}

void Scene::ObjectSpawn()
{
    for (int i = 0; i<uiCount; i++)
    {
        if (ui[i]->Name().find("Spawner") != std::string::npos)
        {
            Button* button = dynamic_cast<Button*>(ui[i]);

            if (!button) continue;
            if (!button->IsClicked()) continue;
            cout<<"Spawning "<<button->_Text()._Text()<<endl;
            AddObject(new Box("Gameobject", {0,0,0}, {0,0,0}, 1, button->_Text()._Text()));
        }
    }
}

Scene::~Scene()
{
    for (int i = 0; i<objectCount; i++)
        delete objects[i];
    delete[] objects;

    for (int i = 0; i<uiCount; i++)
        delete ui[i];
    delete[] ui;

    for (int i = 0; i < npcCount; i++)
        delete npcs[i];
    delete[] npcs;

    delete player;
}

// Button ==================================================================================================================================================================================

bool Button::IsHovering()
{
    Vector2 mousePosition = GetMousePosition();
    return IsBetween(mousePosition.x, rect.x, rect.x+rect.width) && IsBetween(mousePosition.y, rect.y, rect.y+rect.height); 
}

bool Button::IsClicked()
{
    if (!IsHovering()) return false;

    if (!IsMouseButtonPressed(gI.SELECTION_KEY)) return false;

    return true;
}

// UIGRID ==================================================================================================================================================================================

void UIGrid::AddElement(RectTransform* newObject)
{
    cout<<newObject->Name()<<" "<<elementCount <<endl;
    RectTransform** newObjects = new RectTransform*[elementCount + 1];
    for (int i = 0; i < elementCount; i++) {
        newObjects[i] = elements[i];
    }
    newObjects[elementCount] = newObject;
    delete[] elements;
    elements = newObjects;
    elementCount++;    
}

void UIGrid::RemoveElement(int index) {
    if (index < 0 || index >= elementCount) return;
    RectTransform** newObjects = new RectTransform*[elementCount - 1];
    for (int i = 0, j = 0; i < elementCount; i++) {
        if (i != index) {
            newObjects[j++] = elements[i];
        }
    }
    delete[] elements;
    elements = newObjects;
    elementCount--;
}

void UIGrid::OrderUI(int gridType)
{
    switch (gridType)
    {
        case VERTICAL:
        {
            for (int i = 0; i<elementCount; i++)
            {
                RectTransform* rect = elements[i];
                cout<<rect->Name()<<endl;

                rect->Rect({position.x, position.y + gridGap*i, rect->Rect().width, rect->Rect().height});
            }

            break;
        }

        case HORIZONTAL:
        {
            for (int i = 0; i<elementCount; i++)
            {
                RectTransform* rect = elements[i];
                rect->Rect({position.x + gridGap*i, position.y, rect->Rect().width, rect->Rect().height});
            }

            break;
        }
    }
}

UIGrid::~UIGrid()
{
    delete[] elements;
}

// Potion ==================================================================================================================================================================================

void Potion::ApplyEffect(Player& player)
{
    player.effects[effectType] = magnitude;
}

// SaveSystem ==================================================================================================================================================================================
// <NAME> <ASSET_NAME> <POS_X> <POS_Y> <POS_Z> <ROT_X> <ROT_Y> <ROT_Z> <SCALE>

void SaveSystem::SaveScene(Scene& scene)
{   
    ofstream file(saveFilePath, ios::out | ios::binary);

    if (file.is_open())
    {
        for (int i = 0; i<scene.objectCount; i++)
        {
            Box obj = *scene.objects[i];
            file<<obj.Name()<<" "<<(obj.AssetName()==""?gI.DEFAULT_MODEL_NAME:obj.AssetName())<<" "
            <<obj.Position().x<<" "<<obj.Position().y<<" "<<obj.Position().z<<" "
            <<obj.Rotation().x<<" "<<obj.Rotation().y<<" "<<obj.Rotation().z<<" "
            <<obj.Size()<<" EL : --> ";
            if (i<scene.objectCount-1) file<<endl;
        }
    }
}

void SaveSystem::LoadScene(Scene& scene)
{   
    ifstream file(saveFilePath, ios::in | ios::binary);

    if (file.is_open())
    {
        while(!file.eof())
        {
            string name, assetName;
            Vector3 position;
            Vector3 rotation;
            float scale;
            file>>name;
            // cout<<name;
            if (name == "") return;
            
            file>>assetName>>position.x>>position.y>>position.z>>rotation.x>>rotation.y>>rotation.z>>scale;

            cout<<name<<" "<<assetName<<" "
            <<position.x<<" "<<position.y<<" "<<position.z<<" "
            <<rotation.x<<" "<<rotation.y<<" "<<rotation.z<<" "<<scale<<endl;

            scene.AddObject(new Box(name, position, rotation, scale, assetName));

            string line;
            getline(file, line);
        }
    }
}

// GlobalInfo ==================================================================================================================================================================================

void GlobalInfo::Shade()
{
    
}

void GlobalInfo::LoadAnim(Character* c, int stateIdx, const string& stateName, const string& name, int frameCount, const string& spritesPath)
{
    const char* dirs[4] = {"Down", "Left", "Right", "Up"};
    for (int d = 0; d < 4; d++)
    {
        string path = spritesPath + "/" + name + dirs[d] + stateName + ".png";
        c->anims[stateIdx][d] = LoadTexture(path.c_str());
        SetTextureFilter(c->anims[stateIdx][d], TEXTURE_FILTER_POINT);
    }

    // frameWidth derived from texture width / frameCount
    if (c->anims[stateIdx][0].id > 0 && c->anims[stateIdx][0].width > 0) c->frameWidth[stateIdx] = c->anims[stateIdx][0].width / (float)frameCount;
    else c->frameWidth[stateIdx] = 1;
}

void GlobalInfo::PlayerInfo()
{
    gI.scene.player = new Player();

    LoadAnim(scene.player, IDLE,     "Idle",    "Warrior",  5, SPRITES_FOLDER_PATH);
    LoadAnim(scene.player, MOVING,   "Walk",    "Warrior",  8, SPRITES_FOLDER_PATH);
    LoadAnim(scene.player, JUMPING,  "Jump",    "Warrior",  5, SPRITES_FOLDER_PATH);
    LoadAnim(scene.player, ATTACKING,"Attack01","Warrior",  6, SPRITES_FOLDER_PATH);
    LoadAnim(scene.player, DIE,      "Death",   "Warrior",  5, SPRITES_FOLDER_PATH);
    LoadAnim(scene.player, HURT,     "Hurt",    "Warrior",  4, SPRITES_FOLDER_PATH);
    
    int barWidth = BAR_WIDTH,
    barHeight = 25,
    margin = 20,
    padding = 2;

    float posX = margin,
    posY = margin + 15.0f;

    scene.AddUIObject(new Banner("HEALTH_BG", "", (Vector2){posX, posY}, (Vector2){barWidth, barHeight}, 0, BLACK));
    scene.AddUIObject(new Banner("STAMINA_BG", "", (Vector2){posX, posY + barHeight + 15}, (Vector2){barWidth, barHeight}, 0, BLACK));

    scene.player->healthBarIdx = scene.uiCount;
    scene.AddUIObject(new Banner("HEALTH_BAR", "", (Vector2){posX + padding, posY + padding}, (Vector2){barWidth - 2*padding, barHeight - 2*padding}, 0, RED));
    scene.player->staminaBarIdx = scene.uiCount;
    scene.AddUIObject(new Banner("STAMINA_BAR", "", (Vector2){posX + padding, posY + barHeight + 15 + padding}, (Vector2){barWidth - 2*padding, barHeight - 2*padding}, 0, GREEN));
    
    int buttonSize = 75;
    UIGrid grid({margin, (float)buttonSize/10.0f + 1.5f*buttonSize}, 11.0f * (float)buttonSize/10.0f);

    scene.AddUIObject(new Banner("PL_INV", "", Vector2Zero(), (Vector2){buttonSize, buttonSize}, 50, DARKBROWN));
    scene.AddUIObject(new Banner("PL_INV_1", "", Vector2Zero(), (Vector2){buttonSize, buttonSize}, 50, DARKBROWN));

    grid.AddElement(scene.ui[scene.FindUIObjectIndex("PL_INV")]);
    grid.AddElement(scene.ui[scene.FindUIObjectIndex("PL_INV_1")]);

    grid.OrderUI(VERTICAL);

    scene.player->i1 = scene.uiCount;
    scene.AddUIObject(new Text("PL_INV_T", to_string(scene.player->i1), (Vector2){(float)buttonSize/10.0f + buttonSize + margin, scene.ui[scene.uiCount-2]->Rect().y}, {60, 50}, BLACK));
    scene.player->i2 = scene.uiCount;
    scene.AddUIObject(new Text("PL_INV_T_1", to_string(scene.player->i2), (Vector2){(float)buttonSize/10.0f + buttonSize + margin, scene.ui[scene.uiCount-2]->Rect().y}, {60, 50}, BLACK));


    try
    {
        scene.player->AddItem(new Potion(HealthMid));

        scene.player->AddItem(new Potion(StaminaPotent));
    }
    catch(const failed_execution& e)
    {
        cout << e.what() << '\n';
    }
    catch(const out_of_space& e)
    {
        cout << e.what() << '\n';
    }
    catch(const empty_collection& e)
    {
        cout << e.what() << '\n';
    }
    catch(const out_of_range& e)
    {
        cout << e.what() << '\n';
    }
    catch(...)
    {
        cout <<"Unknown Exception\n";
    }
}

void GlobalInfo::Assets()
{
    UIGrid grid({gI.SCREEN_WIDTH-300, 100}, 45);
    
    models[DEFAULT_MODEL_NAME] = LoadModelFromMesh(GenMeshCube(1, 1, 1));
    textures[DEFAULT_MODEL_NAME] = LoadTextureFromImage(GenImageChecked(10, 10, 10, 10, DARKPURPLE, WHITE));
    
    FilePathList files = LoadDirectoryFiles(MODELS_FOLDER_PATH.c_str());
    for (int i = 0; i<(int)files.count; i++)
    {
        if (IsFileExtension(files.paths[i], ".obj") || IsFileExtension(files.paths[i], ".gltf"))
        {
            string name = GetFileNameWithoutExt(files.paths[i]);
            models[name] = LoadModel(files.paths[i]);
            if (FileExists((TEXTURES_FOLDER_PATH+"\\"+name+".png").c_str()))
                textures[name] = LoadTexture((TEXTURES_FOLDER_PATH+"\\"+name+".png").c_str());
            scene.AddUIObject(new Button("Spawner"+to_string(i), name, {0,0}, {300, 40}, 20, DARKGRAY));
            grid.AddElement(scene.ui[scene.uiCount-1]);
        
        }
    }
    grid.OrderUI(VERTICAL);
    UnloadDirectoryFiles(files);

    HealthWeak = Potion("Weak Health Potion", false, HEALTH_REGEN, 10);
    HealthMid = Potion("Mid Health Potion", false, HEALTH_REGEN, 30);
    HealthPotent = Potion("Potent Health Potion", false, HEALTH_REGEN, 60);

    StaminaWeak = Potion("Weak Stamina Potion", false, STAMINA_REGEN, 10);
    StaminaMid = Potion("Mid Stamina Potion", false, STAMINA_REGEN, 30);
    StaminaPotent = Potion("Potent Stamina Potion", false, STAMINA_REGEN, 60);
}

// Spawn positions for possessed NPCs scattered around the scene
static const Vector3 NPC_SPAWN_POSITIONS[] = {
    {  5, 0.5f,  5 },
    { -5, 0.5f,  8 },
    {  8, 0.5f, -4 },
    { -8, 0.5f, -6 },
    {  3, 0.5f, -9 },
};
static const int NPC_COUNT = 5;

void GlobalInfo::LoadNPCs()
{
    for (int n = 0; n < NPC_COUNT; n++)
    {
        PossessedNPC* npc = new PossessedNPC(
            "Possessed_" + to_string(n),
            NPC_SPAWN_POSITIONS[n],
            60,   // maxHealth
            2.5f, // speed
            10    // damage
        );

        // Load animations — frame counts match Player's Warrior sprite sheets
        // IDLE      (5 frames), MOVING/Walk (8 frames), JUMPING (5 frames),
        // ATTACKING (6 frames), DIE/Death   (5 frames), HURT     (3 frames)
        LoadAnim(npc, IDLE,     "Idle", "Possesed",    5, SPRITES_FOLDER_PATH);
        LoadAnim(npc, MOVING,   "Walk", "Possesed",    6, SPRITES_FOLDER_PATH);
        LoadAnim(npc, JUMPING,  "Jump", "Possesed",    5, SPRITES_FOLDER_PATH);
        LoadAnim(npc, ATTACKING,"Attack01", "Possesed",11, SPRITES_FOLDER_PATH);
        LoadAnim(npc, DIE,      "Death", "Possesed",   10, SPRITES_FOLDER_PATH);
        LoadAnim(npc, HURT,     "Hurt", "Possesed",    4, SPRITES_FOLDER_PATH);

        scene.AddNPC(npc);
    }
}

void GlobalInfo::LoadThings()
{
    Assets();
    PlayerInfo();
    LoadNPCs();
}

void GlobalInfo::UnloadThings()
{
    for (auto model : models) 
    {
        UnloadModel(model.second);
    }
    for (auto tex : textures) 
    {
        UnloadTexture(tex.second);
    }

    for (int i = 0; i <= 4; i++) 
    {
        for (int j = 0; j < 4; j++) 
        {
            UnloadTexture(scene.player->anims[i][j]);
        }
    }

    // Unload NPC textures
    for (int n = 0; n < scene.npcCount; n++)
    {
        for (int i = 0; i < 7; i++)
            for (int j = 0; j < 4; j++)
                UnloadTexture(scene.npcs[n]->anims[i][j]);
    }
}

// Character ==================================================================================================================================================================================

void Character::Update()
{
    target.y = position.y;
    Vector3 direction = (target-position);
    if (Magnitude(direction) > 0)
    {
        position = position + Normalize(direction)*speed*gI.dT*speedMultiplier;
    }

    float ax = fabs(direction.x);
    float az = fabs(direction.z);
    const float hysteresis = 1.3f; // current axis must be beaten by this factor to switch

    if (currDir == LEFT || currDir == RIGHT)
    {
        // Currently horizontal — only switch to vertical if z clearly dominates
        if (az > ax * hysteresis)
            currDir = (direction.z > 0) ? DOWN : UP;
        else if (ax > 0.05f)
            currDir = (direction.x > 0) ? RIGHT : LEFT;
    }
    else
    {
        // Currently vertical — only switch to horizontal if x clearly dominates
        if (ax > az * hysteresis)
            currDir = (direction.x > 0) ? RIGHT : LEFT;
        else if (az > 0.05f)
            currDir = (direction.z > 0) ? DOWN : UP;
    }

}

Character::~Character() {}

// Inventory ==================================================================================================================================================================================

int Inventory::FindItem(string name)
{
    for (int i=0; i<MAX_SLOTS; i++)
    {
        if (itemsCount[i]<=0) continue;
        if (items[i][0]->Name() != name) continue;

        return i;
    }

    return -1;
}

void Inventory::AddItem(Item* item)
{
    int idx = FindItem(item->Name());

    if (idx != -1)
    {
        if (itemsCount[idx]>=MAX_ITEMS) throw out_of_space("Inventory Slot Full");

        Item** newItems = new Item*[itemsCount[idx] + 1];

        for (int i = 0; i < itemsCount[idx]; i++) {
            newItems[i] = items[idx][i];
        }

        newItems[itemsCount[idx]] = item;
        delete[] items[idx];
        items[idx] = newItems;
        itemsCount[idx]++;
        return;
    }

    for (int i = 0; i<MAX_SLOTS; i++)
    {
        if (itemsCount[i]<=0) 
        {
            items[i] = new Item*[1];
            items[i][0] = item;
            itemsCount[i]=1;

            return;
        }
    }
    
    throw failed_execution("Failed to Add Item " + item->Name());
}

void Inventory::RemoveItem(int idx)
{
    if (idx>=MAX_SLOTS || idx<0) throw out_of_range("Invalid Slot Index");

    if (itemsCount[idx]<=0) throw empty_collection("Slot Empty");

    Item** newItems = new Item*[itemsCount[idx] - 1];

    for (int i = 0; i < itemsCount[idx]; i++) 
    {
        if (i == idx) continue;

        newItems[i] = items[idx][i];
    }

    delete[] items[idx];
    items[idx] = newItems;
    itemsCount[idx]--;
}

Inventory::~Inventory()
{
    for (int i = 0; i<itemsCount[0]; i++)
        delete items[i];
    
    for (int i = 0; i<itemsCount[1]; i++)
        delete items[i];
}

// NPC ==================================================================================================================================================================================

NPC::~NPC() {}

// PossessedNPC ==================================================================================================================================================================================

void PossessedNPC::TakeDamage(float amount)
{
    if (state == DIE) return;
    currHealth = Clamp(currHealth - amount, 0, maxHealth);
    if (currHealth <= 0)
    {
        state = DIE;
    }
    else
    {
        state = HURT;
        hurtTimer = 0.4f;
    }
}

void PossessedNPC::Update()
{
    if (state == DIE) return;

    // Hurt flash — stay in HURT briefly, no movement
    if (state == HURT)
    {
        hurtTimer -= gI.dT;
        if (hurtTimer <= 0) state = IDLE;
        return;
    }

    Player* player = gI.scene.player;
    if (!player) return;

    Vector3 toPlayer = {
        player->Position().x - position.x,
        0,
        player->Position().z - position.z
    };
    float dist = Magnitude(toPlayer);

    attackTimer -= gI.dT;
    if (attackTimer < 0) attackTimer = 0;

    if (dist <= ATTACK_RANGE)
    {
        // In melee range — attack
        target = position; // stop moving
        state = ATTACKING;

        if (attackTimer <= 0)
        {
            attackTimer = ATTACK_COOLDOWN;
            // Deal damage to player
            float newHp = Clamp(player->CurrHealth() - (float)damage, 0, player->MaxHealth());
            player->CurrHealth(newHp);
        }
    }
    else if (dist <= AGGRO_RANGE)
    {
        // Chase player
        target = {player->Position().x, position.y, player->Position().z};
        state = MOVING;
    }
    else
    {
        // Return to spawn or idle
        float distToSpawn = Magnitude({spawnPos.x - position.x, 0, spawnPos.z - position.z});
        if (distToSpawn > 0.1f)
        {
            target = spawnPos;
            state = MOVING;
        }
        else
        {
            target = position;
            state = IDLE;
        }
    }

    Character::Update();
}

void PossessedNPC::DrawCharacter()
{
    if (anims[IDLE][0].id == 0) return; // textures not loaded

    int i = state;
    if (i >= 7) i = IDLE; // safety clamp

    if (state != npcLastState)
    {
        currentFrame = 0;
        frameTimer = 0.0f;
        npcLastState = state;
    }

    Texture2D anim[4] = {anims[i][0], anims[i][1], anims[i][2], anims[i][3]};

    frameTimer += gI.dT;
    if (frameTimer >= 0.12f)
    {
        frameTimer = 0.0f;
        if (anim[currDir].id > 0 && anim[currDir].width > 0)
        {
            int maxFrames = anim[currDir].width / (int)frameWidth[i];
            if (currentFrame < maxFrames - 1)
                currentFrame++;
            else
            {
                if (state == HURT)
                    state = IDLE; // return to idle after attack/hurt anim
                else if (state != DIE)
                    currentFrame = 0;
                // DIE stays on last frame
            }
        }
        else currentFrame = 0;
    }

    Rectangle sourceRec = {
        (float)currentFrame * frameWidth[i],
        0,
        (float)frameWidth[i],
        (float)anim[currDir].height
    };

    AnimationData animData =
    {
        gI.scene.player->Camera(),
        anim[currDir],
        sourceRec,
        position,
        (Vector2){ size * 2, size * 2 },
        WHITE
    };

    gI.scene.billboards[name] = animData;
}

// Player ==================================================================================================================================================================================

float jT=0, yPos=0;
bool isSprinting;
void Player::Update() 
{
    if (currHealth == 0)
    {
        state = DIE;
        return;
    }

    if (IsKeyPressed(gI.ATTACK_KEY)) state = ATTACKING;

    if (state == HURT)
    {
        hurtTimer -= gI.dT;
        if (hurtTimer <= 0) state = IDLE;
        return;
    }

    groundRay.position = position-(Vector3){0, size/2-0.1f, 0};
    Vector2 moveInput = GetDirectionalInputV();
    Vector3 inputDir = {moveInput.x * gI.dT * speed * speedMultiplier, 0, moveInput.y * gI.dT * speed * speedMultiplier};

    directionRay.position = position;
    Vector3 currentTarget = target;
    target = (position + inputDir);
    directionRay.direction = Normalize(target-position);
    
    for (int i = 0; i<gI.scene.objectCount; i++) 
    {
        rayInfo = GetRayCollisionBox(directionRay, gI.scene.objects[i]->Boundary());
        
        if (rayInfo.hit)
        {
            if (rayInfo.distance<=0.5f)
            {
                target = position;
            }
            
        }
    }
    
    isSprinting = (IsKeyDown(gI.SPRINT_KEY) && currStamina>0 ? true : false);

    speedMultiplier = (isSprinting?2:1);
    Character::Update();

    camera.position = {position.x, position.y+camDist, position.z+camDist};
    camera.target = position;
    
    CalculateIsGrounded();

    if (hasJumped)
    {
        jT+=gI.dT;
        if (jT>3.0f || (jT>1.0f && isGrounded)) hasJumped = false;
    }
    
    if (isGrounded && !hasJumped)
    {
        if (state != ATTACKING) state = (Magnitude(inputDir) > 0) ? state = MOVING: state = IDLE;
        
        if (IsKeyPressed(gI.JUMP_KEY))
        {
            hasJumped = true;
            isGrounded = false;
            yVelocity = 5.0f;
            yPos = position.y;
        }
        else yVelocity = 0;
    }
    else 
    {
        if (state != ATTACKING) state = JUMPING;
        yVelocity -= 9.8f * gI.dT;
    }
    
    yVelocity = Clamp(yVelocity, -15, 10);
    position.y +=  yVelocity * gI.dT;

}

bool attackHitDealt = false;
void Player::Attack()
{
    if (currHealth < 0) return;
    
    if (gI.scene.player->state != ATTACKING) 
    {
        attackHitDealt = false;
        return;
    }
    
    if (!attackHitDealt)
    {
        attackHitDealt = true;
        const float PLAYER_ATTACK_RANGE = 1.5f;
        for (int i = 0; i < gI.scene.npcCount; i++)
        {
            PossessedNPC* npc = gI.scene.npcs[i];

            if (npc->State() == DIE) continue;

            Vector3 diff = { npc->Position().x - position.x, 0, npc->Position().z - position.z };
            if (Magnitude(diff) <= PLAYER_ATTACK_RANGE) npc->TakeDamage((float)gI.scene.player->CurrDamage());
        }
    }
}

bool animEnd = false;
int lastState = -1;
void Player::DrawCharacter()
{
    // Ensure frameWidth is 
    int i = state;
    if (state != lastState) {
        currentFrame = 0;
        frameTimer = 0.0f;
        lastState = state;
    }
   
    Texture2D anim[4] = {anims[i][0], anims[i][1], anims[i][2], anims[i][3]};

    // If not moving, reset to first frame
    frameTimer += gI.dT;
    if (frameTimer >= 0.1f/(isSprinting?1.5f:1))
    { 
        frameTimer = 0.0f;
        if (anim[currDir].id > 0 && anim[currDir].width > 0)
        {
            int maxFrames = anim[currDir].width / (int)frameWidth[i];
            if (currentFrame < maxFrames ) 
                currentFrame++;
            if (currentFrame >= maxFrames ) 
            {
                if (state != JUMPING && state != DIE) 
                {
                    if (state == ATTACKING) state = IDLE;
                    else currentFrame = 0;
                }
                else 
                    currentFrame = maxFrames-1;
            }
        }
        else currentFrame = 0;
    }
    
    Rectangle sourceRec = {
        (float)currentFrame * frameWidth[i],
        0,
        (float)frameWidth[i],
        (float)anim[currDir].height
    };
    // DrawSphere(position, 0.1f, BLACK);
    AnimationData animData = 
    {
        camera,
        anim[currDir],
        sourceRec,
        position,
        (Vector2){ size*2, size*2 },
        WHITE
    };

    gI.scene.billboards[name] = animData;
}

void Player::CalculateIsGrounded()
{
    isGrounded = false;
    if (position.y<=size/2+0.1f)
    {
         isGrounded = true;
         position.y = size/2+0.1f;
         return;

    }
    for (int i = 0; i<gI.scene.objectCount; i++) 
    {
        groundInfo = GetRayCollisionBox(groundRay, gI.scene.objects[i]->Boundary());

        if (groundInfo.hit)
        {
            if (groundInfo.distance<=0.2f)
            {
                isGrounded = true;
                break;
            }
            
        }
    }
}

void Player::StateUpdate()
{
    Banner* bar = dynamic_cast<Banner*>(gI.scene.ui[healthBarIdx]);
    
    if (bar)
    {
        Rectangle rect = bar->Rect();
        rect.width = (currHealth/maxHealth)*(gI.BAR_WIDTH-3);
        bar->Rect(rect);
    }
    
    bar = dynamic_cast<Banner*>(gI.scene.ui[staminaBarIdx]);
    
    if (bar)
    {
        Rectangle rect = bar->Rect();
        rect.width = (currStamina/maxStamina)*(gI.BAR_WIDTH-3);
        bar->Rect(rect);
    }

    currStamina -= (isSprinting&&state!=IDLE? gI.dT*12.0f:0);

    if (!(isSprinting&&state!=IDLE))
    {
        currStamina = Clamp(currStamina+ staminaRegenRate*gI.dT, 0, maxStamina);
    }
}

void Player::InvUI_Update()
{
    Text* tx = dynamic_cast<Text*>(gI.scene.ui[i1]);
    if (tx) tx->_Text((inventory.itemsCount[0]==0?"No Item In Slot": ( inventory.items[0][0]->Name()+" x"+to_string(inventory.itemsCount[0]) ) ) );
    
    tx = dynamic_cast<Text*>(gI.scene.ui[i2]);
    if (tx) tx->_Text((inventory.itemsCount[1]==0?"No Item In Slot": ( inventory.items[1][0]->Name()+" x"+to_string( inventory.itemsCount[1]) ) ) );
}

void Player::UpdateEffects()
{
    if (IsKeyPressed(gI.INV_1))
    {
        if (inventory.itemsCount[0]>0)
        {
            Potion* potion = dynamic_cast<Potion*>(inventory.items[0][0]);
            if (potion)
            {
                potion->ApplyEffect(*this);
                inventory.RemoveItem(inventory.FindItem(potion->Name()));
                InvUI_Update();
                delete potion;
            }
        }
    }

    if (IsKeyPressed(gI.INV_2))
    {
        if (inventory.itemsCount[1]>0)
        {
            Potion* potion = dynamic_cast<Potion*>(inventory.items[1][0]);
            if (potion)
            {
                potion->ApplyEffect(*this);
                inventory.RemoveItem(inventory.FindItem(potion->Name()));
                InvUI_Update();
                delete potion;
            }
        }
    }

    map<int, float> temp = effects;
    vector<int> damageBoosts;
    
    for (auto effect : temp)
    {
        switch(effect.first)
        {
            case HEALTH_REGEN:
            {
                currHealth = Clamp(currHealth+effect.second, 0, maxHealth);
                effects.erase(effect.first);
                break;
            }

            case STAMINA_REGEN:
            {
                currStamina = Clamp(currStamina+effect.second, 0, maxStamina);
                effects.erase(effect.first);
                break;
            }

            case STRENGTH_BOOST:
            {
                
                if (effect.second<=0)
                {
                    effects.erase(effect.first);
                    currDamage = damage;
                    continue;
                }

                damageBoosts.push_back(effect.second);
                effects[effect.first] -= gI.dT;
                break;
            }
        }
    }

    for (int i = 0; i < damageBoosts.size(); i++)
    {
        currDamage = damage + 10;
    }
}


































