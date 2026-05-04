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

// Collider ==================================================================================================================================================================================

void Collider::UpdateBoundary() {
        boundary = GetMeshBoundingBox(model.meshes[0]);
        boundary.min = {boundary.min.x * scale.x + position.x,
                        boundary.min.y * scale.y + position.y,
                        boundary.min.z * scale.z + position.z};
        boundary.max = {boundary.max.x * scale.x + position.x,
                        boundary.max.y * scale.y + position.y,
                        boundary.max.z * scale.z + position.z};
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

void Scene::AddSpawnObject(Box* newObject, Box**& spawnArray, int& spawnCount, int k=0) 
{
    Box** newSpawners = new Box*[spawnCount + 1];
    for (int i = 0; i < spawnCount; i++) {
        newSpawners[i] = spawnArray[i];
    }
    newSpawners[spawnCount] = newObject;
    delete[] spawnArray;
    spawnArray = newSpawners;
    spawnCount++;
}

void Scene::AddObject(Box* newObject, int i=0) 
{
    string name = ReplaceWhiteSpaces(newObject->Name());

    if (i>0) name = newObject->Name()+to_string(i);
    if (FindObjectIndex(name) != -1 && newObject->Name()!=gI.ENEMY_SPAWNER_NAME  && newObject->Name()!=gI.CIVIL_SPAWNER_NAME  && newObject->Name()!=gI.MERCHANT_SPAWNER_NAME && newObject->Name()!=gI.BOSS_SPAWNER_NAME) 
    {
        AddObject(newObject, i+1); // try with new name
        return;
    } 
    newObject->Name(name);

    Box** newObjects = new Box*[objectCount + 1];
    for (int j = 0; j < objectCount; j++) {
        newObjects[j] = objects[j];
    }
    newObjects[objectCount] = newObject;
    delete[] objects;
    objects = newObjects;
    objectCount++;
}

void Scene::AddCollider(Collider* newObject, int i=0) 
{
    string name = ReplaceWhiteSpaces(newObject->Name());

    if (i>0) name = newObject->Name()+to_string(i);
    if (FindObjectIndex(name) != -1 && newObject->Name()!=gI.ENEMY_SPAWNER_NAME  && newObject->Name()!=gI.CIVIL_SPAWNER_NAME  && newObject->Name()!=gI.MERCHANT_SPAWNER_NAME && newObject->Name()!=gI.BOSS_SPAWNER_NAME) 
    {
        AddCollider(newObject, i+1); // try with new name
        return;
    } 
    newObject->Name(name);

    Collider** newObjects = new Collider*[colliderCount + 1];
    for (int j = 0; j < colliderCount; j++) {
        newObjects[j] = colliders[j];
    }
    newObjects[colliderCount] = newObject;
    delete[] colliders;
    colliders = newObjects;
    colliderCount++;
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
    for (int j = 0; j < uiCount; j++) {
        newObjects[j] = ui[j];
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

int Scene::FindColliderIndex(string name)
{
    for (int i = 0; i < colliderCount; i++)
        if (colliders[i]->Name() == name) return i;
    return -1;
}

void Scene::RemoveCollider(int index)
{
    if (index < 0 || index >= colliderCount) return;

    delete colliders[index];

    Collider** newColliders = new Collider*[colliderCount - 1];
    for (int i = 0, j = 0; i < colliderCount; i++)
        if (i != index) newColliders[j++] = colliders[i];

    delete[] colliders;
    colliders = newColliders;
    colliderCount--;
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

int Scene::FindNPCIndex(int id)
{
    for (int i = 0; i < npcCount; i++) {
        if (npcs[i]->ID() == id) {
            return i;
        }
    }
    return -1; // Not found   
}

int Scene::FindEnemyIndex(int id)
{
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i]->ID() == id) {
            return i;
        }
    }
    return -1; // Not found   
}

bool Scene::IsCharacter(int id)
{
    return player->ID() == id || FindEnemyIndex(id) != -1 || FindNPCIndex(id) != -1;
}

void Scene::SelectionMove()
{
    if (selected != nullptr)
    {
        Collider* collider = dynamic_cast<Collider*>(selected);
        bool isCollider = (collider != nullptr);

        Vector2 moveInput = GetDirectionalInputV(gI.FORWARD_KEY, gI.BACKWARD_KEY, gI.LEFT_KEY, gI.RIGHT_KEY);
        float heightInput = GetInputODFrom(KEY_Q, KEY_E);

        selected->Position({
            selected->Position().x + moveInput.x * gI.dT * selectionSpeed,
            selected->Position().y + heightInput * gI.dT * selectionSpeed,
            selected->Position().z + moveInput.y * selectionSpeed * gI.dT
        });

        if (isCollider)
        {
            Vector2 scaleInput = GetDirectionalInputV(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT);
            float scaleY = GetInputODFrom(KEY_MINUS, KEY_EQUAL);

            collider->ScaleX(collider->Scale().x + scaleInput.y * gI.dT * selectionSpeed);
            collider->ScaleZ(collider->Scale().z + scaleInput.x * gI.dT * selectionSpeed);
            collider->ScaleY(collider->Scale().y + scaleY        * gI.dT * selectionSpeed);
        }
        else
        {
            Vector2 rotationInput = GetDirectionalInputV(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT);
            float sizeInput = GetInputODFrom(KEY_MINUS, KEY_EQUAL);

            selected->Size(selected->Size() + sizeInput * gI.dT * selectionSpeed);
            selected->Rotation({
                selected->Rotation().x + rotationInput.y * gI.dT * selectionSpeed,
                selected->Rotation().y + rotationInput.x * gI.dT * selectionSpeed,
                selected->Rotation().z
            });
        }

        if (IsKeyPressed(KEY_DELETE))
        {
            Collider* col = dynamic_cast<Collider*>(selected);
            if (col) RemoveCollider(FindColliderIndex(col->Name()));
            else     RemoveObject(FindObjectIndex(selected->Name()));
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
    }

    if (gI.mode == EDITOR)
    for (int i = 0; i < colliderCount; i++)
    {
        Collider* col = colliders[i];
        BeginBlendMode(BLEND_ALPHA);
        DrawModelEx(col->_Model(), col->Position(), {0,1,0}, 0, col->Scale(), (Color){0,255,0,150});
        EndBlendMode();
    }
    
    for (int i = 0; i < enemyCount; i++) enemies[i]->DrawCharacter();
    for (int i = 0; i < npcCount; i++) npcs[i]->DrawCharacter();

    player->DrawCharacter();
    
    std::vector<std::pair<double, const AnimationData*>> sortedBillboards;
    sortedBillboards.reserve(billboards.size()); // Pre-allocate memory for speed

    // 2. Calculate distances and populate the list
    for (const auto& bb : billboards) 
    {
        // Calculate vector from the camera to the billboard
        double dx = bb.second.camera.position.x - bb.second.position.x;
        double dy = bb.second.camera.position.y - bb.second.position.y;
        double dz = bb.second.camera.position.z - bb.second.position.z;

        // Calculate Squared Distance (Omit the expensive sqrtf call!)
        double distSq = (dx * dx) + (dy * dy) + (dz * dz);

        sortedBillboards.push_back({ distSq, &bb.second });
    }

    // 3. Sort the vector from Furthest to Closest (Descending Order)
    std::sort(sortedBillboards.begin(), sortedBillboards.end(), 
        [](const std::pair<double, const AnimationData*>& a, const std::pair<double, const AnimationData*>& b) {
            return a.first > b.first; // '>' ensures furthest objects are drawn first
        });

    // 4. Render them in the correct sorted order
    for (const auto& item : sortedBillboards) 
    {
        const AnimationData* b = item.second;
        DrawBillboardRec(b->camera, b->frame, b->source, b->position, b->size, b->color);
    }
}

void Scene::DrawSceneUI(int mode)
{
    for (int i = 0; i < uiCount; i++)
    {
        RectTransform* uiElement = ui[i];
        string uiName = uiElement->Name();

        // Editor-only elements
        if (uiName.find("Spawner") != string::npos)
        {
            if (mode != EDITOR) continue;
        }
        // Game UI — filter by gameMode prefix
        else if (uiName.find("MEN_") != string::npos)
        {
            if (mode != GAME || gameMode != MENU)  continue;
        }
        else if (uiName.find("PAU_") != string::npos)
        {
            if (mode != GAME || gameMode != PAUSE) continue;
        }
        else if (uiName.find("PLA_") != string::npos)
        {
            if (mode != GAME || gameMode != PLAY)  continue;
        }
        else if (uiName.find("DIALOGUE_") != string::npos ||
                 uiName.find("CHOICE_")   != string::npos)
        {
            if (mode != GAME || !dialogueVisible)  continue;
        }
        else if (uiName.find("END_") != string::npos)
        {
            if (mode != GAME || !endScreenVisible) continue;
        }
        else if (uiName.find("HEALTH") != string::npos ||
                 uiName.find("STAMINA") != string::npos ||
                 uiName.find("PL_")    != string::npos)
        {
            if (mode != GAME || gameMode != PLAY)  continue;
        }

        Button* button = dynamic_cast<Button*>(uiElement);
        Text*   text   = dynamic_cast<Text*>(uiElement);
        Banner* banner = dynamic_cast<Banner*>(uiElement);

        if (button)
        {
            DrawRectangleRec(button->Rect(), button->BackColor());
            DrawText(button->_Text()._Text().c_str(),
                button->Rect().x + button->_Text().Rect().x,
                button->Rect().y + button->_Text().Rect().y,
                button->_Text().Rect().width, WHITE);
        }
        else if (text)
        {
            DrawText(text->_Text().c_str(), text->Rect().x, text->Rect().y, 20, text->_Color());
        }
        else if (banner)
        {
            DrawRectangleRec(banner->Rect(), banner->BackColor());
            DrawText(banner->_Text()._Text().c_str(),
                banner->Rect().x + banner->_Text().Rect().x,
                banner->Rect().y + banner->_Text().Rect().y,
                banner->_Text().Rect().width, WHITE);
        }
    }
}

void Scene::AddNPC(PossessedNPC* npc, int k = 0)
{
    int id = 0;
    if (k>0) id = npc->ID()+k;
    if (IsCharacter(id)) 
    {
        AddNPC(npc, k+rand()%100);
        return;
    } 
    npc->ID(id);

    PossessedNPC** newNPCs = new PossessedNPC*[enemyCount + 1];
    for (int i = 0; i < enemyCount; i++) newNPCs[i] = enemies[i];
    newNPCs[enemyCount] = npc;
    delete[] enemies;
    enemies = newNPCs;
    enemyCount++;
}

void Scene::AddNPC(NPC* npc, int k=0)
{
    int id = 0;
    if (k>0) id = npc->ID()+k;
    if (IsCharacter(id)) 
    {
        AddNPC(npc, k+rand()%100); // try with new name
        return;
    } 
    npc->ID(id);

    NPC** newNPCs = new NPC*[npcCount + 1];
    for (int i = 0; i < npcCount; i++) newNPCs[i] = npcs[i];
    newNPCs[npcCount] = npc;
    delete[] npcs;
    npcs = newNPCs;
    npcCount++;
}
 
void Scene::UpdateNPCs()
{
    for (int i = 0; i < enemyCount; i++)
    {
        enemies[i]->Update();
    }

    // Update dialogueable NPCs that also move (Boss)
    for (int i = 0; i < npcCount; i++)
    {
        Boss* boss = dynamic_cast<Boss*>(npcs[i]);
        if (boss) boss->Update();
    }
}

void Scene::SelectObject(Ray ray)
{
    float dist = FLT_MAX;
    selectionRay = ray;
    selected = nullptr;

    for (int i = 0; i < objectCount; i++)
    {
        selectionRayCollision = GetRayCollisionBox(ray, objects[i]->Boundary());
        if (selectionRayCollision.hit && selectionRayCollision.distance <= dist)
        {
            dist = selectionRayCollision.distance;
            selected = objects[i];
        }
    }

    for (int i = 0; i < colliderCount; i++)
    {
        selectionRayCollision = GetRayCollisionBox(ray, colliders[i]->Boundary());
        if (selectionRayCollision.hit && selectionRayCollision.distance <= dist)
        {
            dist = selectionRayCollision.distance;
            selected = colliders[i];
        }
    }
}

void Scene::DuplicateSelected()
{
    if (!selected) return;

    Collider* collider = dynamic_cast<Collider*>(selected);
    Box* box = dynamic_cast<Box*>(selected);

    if (collider)
    {
        Collider* dupe = new Collider(
            collider->Name(),
            collider->Position(),
            collider->Size()
        );
        dupe->ScaleX(collider->Scale().x);
        dupe->ScaleY(collider->Scale().y);
        dupe->ScaleZ(collider->Scale().z);
        AddCollider(dupe);
        selected = colliders[colliderCount - 1];
    }
    else if (box)
    {
        Box* dupe = new Box(
            box->Name(),
            box->Position(),
            box->Rotation(),
            box->Size(),
            box->AssetName()
        );
        AddObject(dupe);
        selected = objects[objectCount - 1];
    }

}

void Scene::ObjectSpawn()
{
    for (int i = 0; i < uiCount; i++)
    {
        Button* button = dynamic_cast<Button*>(ui[i]);
        if (!button || !button->IsClicked()) continue;

        string text = button->_Text()._Text();

        // --- Collider button ---
        if (text == "Collider")
        {
            Ray ray = GetScreenToWorldRay(
                {(float)gI.SCREEN_WIDTH / 2, (float)gI.SCREEN_HEIGHT / 2},
                sceneCamera.Camera()
            );
            Vector3 spawnPos = {
                ray.position.x + ray.direction.x * 5,
                ray.position.y + ray.direction.y * 5,
                ray.position.z + ray.direction.z * 5
            };
            spawnPos.y = fmax(spawnPos.y, 0);

            Collider* col = new Collider("Collider", spawnPos, 1);
            AddCollider(col);
            selected = colliders[colliderCount - 1];
            continue;
        }

        // --- Scene object spawners only ---
        if (ui[i]->Name().find("Spawner") == std::string::npos) continue;

        cout << "Spawning " << text << endl;

        Ray ray = GetScreenToWorldRay(
            {(float)gI.SCREEN_WIDTH / 2, (float)gI.SCREEN_HEIGHT / 2},
            sceneCamera.Camera()
        );

        Vector3 spawnPos = {0, 0, 0};
        float closestDist = FLT_MAX;
        bool hitSomething = false;

        for (int j = 0; j < objectCount; j++)
        {
            RayCollision hit = GetRayCollisionBox(ray, objects[j]->Boundary());
            if (hit.hit && hit.distance < closestDist)
            {
                closestDist = hit.distance;
                spawnPos = { hit.point.x, hit.point.y + 0.5f, hit.point.z };
                hitSomething = true;
            }
        }

        if (!hitSomething)
        {
            if (fabs(ray.direction.y) > 0.0001f)
            {
                float t = -ray.position.y / ray.direction.y;
                if (t > 0)
                    spawnPos = { ray.position.x + ray.direction.x * t, 0, ray.position.z + ray.direction.z * t };
                else
                    spawnPos = { ray.position.x, 0, ray.position.z };
            }
            else
                spawnPos = { ray.position.x + ray.direction.x * 10, 0, ray.position.z + ray.direction.z * 10 };
        }

        spawnPos.y = fmax(spawnPos.y, 0);
        AddObject(new Box("Gameobject", spawnPos, {0,0,0}, 1, text));
    }
}

void Scene::ShowBossDeathScreen()
{
    Banner* title = dynamic_cast<Banner*>(ui[FindUIObjectIndex("END_TITLE")]);
    Banner* text  = dynamic_cast<Banner*>(ui[FindUIObjectIndex("END_TEXT")]);
    if (title) title->_Text()._Text("The Tyrant Falls");
    if (text)  text->_Text()._Text(
        "The Boss hit the ground and did not rise.\n\n"
        "For a moment, no one moved. The wind itself seemed to pause, "
        "\nas if the world needed a second to understand what had just happened.\n\n"
        "Then, slowly, a door opened. Then another. Then another.\n\n"
        "The people of Waloon stepped out into the light, blinking, "
        "uncertain, \nlike men emerging from a cave they had lived in "
        "so long they had forgotten the sun.\n\n"
        "The warrior stood in the square, bloodied and breathing hard, "
        "\nand said nothing. There was nothing left to say.\n\n"
        "That night they lit fires for the first time in years. "
        "\nNot to ward off the dark, but because they could.\n\n"
        "Waloon was free."
    );
    endScreenVisible = true;
}

void Scene::ShowPlayerDeathScreen()
{
    Banner* title = dynamic_cast<Banner*>(ui[FindUIObjectIndex("END_TITLE")]);
    Banner* text  = dynamic_cast<Banner*>(ui[FindUIObjectIndex("END_TEXT")]);
    if (title) title->_Text()._Text("Darkness Falls");
    if (text)  text->_Text()._Text(
        "The warrior fell.\n\n"
        "Not with a cry, with a silence that spread outward like ripples "
        "\non still water, until the whole village held its breath.\n\n"
        "The Boss stood over him for a long moment, saying nothing. "
        "\nThere was no triumph in his eyes. Only the cold satisfaction "
        "\nof a man who had never once doubted the outcome.\n\n"
        "Waloon remained his. The gates stayed shut. "
        "The people stayed afraid.\n\n"
        "Somewhere, a child asked his mother if anyone would ever come.\n\n"
        "She did not answer. For she had no hope to give."
    );
    endScreenVisible = true;
}

Scene::~Scene()
{
    for (int i = 0; i<objectCount; i++) delete objects[i];
    delete[] objects;

    for (int i = 0; i<uiCount; i++) delete ui[i];
    delete[] ui;

    for (int i = 0; i < enemyCount; i++) delete enemies[i];
    delete[] enemies;

    for (int i = 0; i < npcCount; i++) delete npcs[i]; 
    delete[] npcs;

    for (int i = 0; i < enemySpawnCount; i++) delete enemySpawnPositions[i];
    delete[] enemySpawnPositions;

    for (int i = 0; i < civilSpawnCount; i++) delete civilSpawnPositions[i];
    delete[] civilSpawnPositions;

    for (int i = 0; i < merchantSpawnCount; i++) delete merchantSpawnPositions[i];
    delete[] merchantSpawnPositions;

    for (int i = 0; i < bossSpawnCount; i++) delete bossSpawnPositions[i];
    delete[] bossSpawnPositions;

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
        for (int i = 0; i < scene.objectCount; i++)
        {
            Box obj = *scene.objects[i];
            file << obj.Name() << " " << (obj.AssetName()==""?gI.DEFAULT_MODEL_NAME:obj.AssetName()) << " "
                 << obj.Position().x << " " << obj.Position().y << " " << obj.Position().z << " "
                 << obj.Rotation().x << " " << obj.Rotation().y << " " << obj.Rotation().z << " "
                 << obj.Size() << " EL : --> " << endl;
        }

        for (int i = 0; i < scene.colliderCount; i++)
        {
            Collider* col = scene.colliders[i];
            file << col->Name() << " COLLIDER "
                 << col->Position().x << " " << col->Position().y << " " << col->Position().z << " "
                 << col->Scale().x   << " "  << col->Scale().y   << " " << col->Scale().z    << " "
                 << col->Size() << " EL : --> " << endl;
        }
    }
}

void SaveSystem::LoadScene(Scene& scene)
{   
    ifstream file(saveFilePath, ios::in | ios::binary);
    if (file.is_open())
    {
        while (!file.eof())
        {
            string name, assetName;
            Vector3 position;
            Vector3 rotationOrScale;
            float scale;

            file >> name;
            if (name == "") return;

            file >> assetName
                 >> position.x      >> position.y      >> position.z
                 >> rotationOrScale.x >> rotationOrScale.y >> rotationOrScale.z
                 >> scale;

            string line;
            getline(file, line);

            if (assetName == "COLLIDER")
            {
                Collider* col = new Collider(name, position, scale);
                col->ScaleX(rotationOrScale.x);
                col->ScaleY(rotationOrScale.y);
                col->ScaleZ(rotationOrScale.z);
                scene.AddCollider(col);
                continue;
            }

            if      (name == gI.ENEMY_SPAWNER_NAME)    scene.AddSpawnObject(new Box(name, position, rotationOrScale, scale, assetName), gI.scene.enemySpawnPositions,    gI.scene.enemySpawnCount);
            else if (name == gI.CIVIL_SPAWNER_NAME)    scene.AddSpawnObject(new Box(name, position, rotationOrScale, scale, assetName), gI.scene.civilSpawnPositions,    gI.scene.civilSpawnCount);
            else if (name == gI.MERCHANT_SPAWNER_NAME) scene.AddSpawnObject(new Box(name, position, rotationOrScale, scale, assetName), gI.scene.merchantSpawnPositions, gI.scene.merchantSpawnCount);
            else if (name == gI.BOSS_SPAWNER_NAME)     scene.AddSpawnObject(new Box(name, position, rotationOrScale, scale, assetName), gI.scene.bossSpawnPositions,     gI.scene.bossSpawnCount);

            scene.AddObject(new Box(name, position, rotationOrScale, scale, assetName));
        }
    }
}

void SaveSystem::SavePlayer(Player& player)
{
    // Format (human-readable, one value per line with a label):
    // NAME <name>
    // POS <x> <y> <z>
    // HEALTH <currHealth> <maxHealth>
    // STAMINA <currStamina> <maxStamina> <regenRate>
    // HEALTH_REGEN <healthRegenRate>
    // SPEED <speed>
    // DAMAGE <damage> <currDamage>
    // STRENGTH <strength>
    // CHARISMA <charisma>
    // ARMOUR <armour>
    // COINS <coins>
    // EFFECTS <count>
    // EFFECT <type> <timeRemaining>  (one per active effect)
    // INV_SLOT <slotIdx> <itemName> <count>  (one per occupied slot)

    ofstream file(playerSaveFilePath, ios::out);
    if (!file.is_open())
    {
        cout << "SavePlayer: could not open " << playerSaveFilePath << endl;
        return;
    }

    // --- Identity & transform ---
    file << "NAME "        << player.name                                                  << "\n";
    file << "POS "         << player.position.x  << " " << player.position.y  << " " << player.position.z  << "\n";

    // --- Vitals ---
    file << "HEALTH "      << player.currHealth   << " " << player.maxHealth               << "\n";
    file << "STAMINA "     << player.currStamina  << " " << player.maxStamina << " " << player.staminaRegenRate << "\n";
    file << "HEALTH_REGEN "<< player.healthRegenRate                                       << "\n";

    // --- Combat stats ---
    file << "SPEED "       << player.speed                                                 << "\n";
    file << "DAMAGE "      << player.damage       << " " << player.currDamage              << "\n";
    file << "STRENGTH "    << player.strength                                              << "\n";
    file << "CHARISMA "    << player.charisma                                              << "\n";
    file << "ARMOUR "      << player.armour                                                << "\n";

    // --- Economy ---
    file << "COINS "       << player.coins                                                 << "\n";

    // --- Active effects (e.g. strength boost timer) ---
    file << "EFFECTS "     << (int)player.effects.size()                                   << "\n";
    for (const auto& e : player.effects)
        file << "EFFECT "  << e.first << " " << e.second                                  << "\n";

    // --- Inventory (up to MAX_SLOTS slots) ---
    for (int s = 0; s < Inventory::MAX_SLOTS; s++)
    {
        if (player.inventory.itemsCount[s] <= 0) continue;
        // Format: INV_SLOT <slotIdx> <count> <itemName with spaces>
        file << "INV_SLOT " << s << " "
             << player.inventory.itemsCount[s] << " "
             << player.inventory.items[s][0]->Name() << "\n";
    }

    file.close();
    cout << "Player saved to " << playerSaveFilePath << endl;
}

void SaveSystem::LoadPlayer(Player& player)
{
    ifstream file(playerSaveFilePath, ios::in);
    if (!file.is_open())
    {
        cout << "LoadPlayer: no save found at " << playerSaveFilePath << endl;
        return;
    }

    string token;
    while (file >> token)
    {
        if (token == "NAME")
        {
            string name;
            getline(file, name);  // reads " Weak Health Potion"
            // trim the leading space
            if (!name.empty() && name[0] == ' ') name = name.substr(1);

            player.name = name;
        }
        else if (token == "POS")
        {
            file >> player.position.x >> player.position.y >> player.position.z;
        }
        else if (token == "HEALTH")
        {
            file >> player.currHealth >> player.maxHealth;
        }
        else if (token == "STAMINA")
        {
            file >> player.currStamina >> player.maxStamina >> player.staminaRegenRate;
        }
        else if (token == "HEALTH_REGEN")
        {
            file >> player.healthRegenRate;
        }
        else if (token == "SPEED")
        {
            file >> player.speed;
        }
        else if (token == "DAMAGE")
        {
            file >> player.damage >> player.currDamage;
        }
        else if (token == "STRENGTH")
        {
            file >> player.strength;
        }
        else if (token == "CHARISMA")
        {
            file >> player.charisma;
        }
        else if (token == "ARMOUR")
        {
            file >> player.armour;
        }
        else if (token == "COINS")
        {
            file >> player.coins;
        }
        else if (token == "EFFECTS")
        {
            int count; file >> count;
            player.effects.clear();
            for (int i = 0; i < count; i++)
            {
                string effectToken; int type; float value;
                file >> effectToken >> type >> value;   // "EFFECT <type> <value>"
                player.effects[type] = value;
            }
        }
        else if (token == "INV_SLOT")
        {
            int slotIdx, count;
            string itemName;
            file >> slotIdx >> count;
            getline(file, itemName);  // reads " Weak Health Potion"
            // trim the leading space
            if (!itemName.empty() && itemName[0] == ' ') itemName = itemName.substr(1);

            for (int i = 0; i < 7; i++)
            {
                if (gI.potions[i].Name() == itemName)
                {
                    for (int q = 0; q < count; q++)
                    {
                        try { player.inventory.AddItem(new Potion(gI.potions[i])); }
                        catch(...) { break; }
                    }
                    break;
                }
            }
        }
    }

    file.close();

    // Sync the UI bars and inventory display after loading
    player.InvUI_Update();
    player.StateUpdate();

    cout << "Player loaded from " << playerSaveFilePath << endl;
}

// GlobalInfo ==================================================================================================================================================================================

void GlobalInfo::LoadMenuUI()
{
    float w = SCREEN_WIDTH, h = SCREEN_HEIGHT;
    Color darkBrown = (Color){60, 35, 20, 255};
    Color cream     = (Color){245, 235, 210, 255};
    Color btnColor  = (Color){80, 48, 28, 255};

    // Full background
    scene.AddUIObject(new Banner("MEN_BG", "",
        (Vector2){0, 0}, (Vector2){w, h}, 0, darkBrown));

    // Title panel
    scene.AddUIObject(new Banner("MEN_TITLE_BG", "",
        (Vector2){w*0.25f, h*0.15f}, (Vector2){w*0.5f, 80}, 0, cream));
    scene.AddUIObject(new Banner("MEN_TITLE", "WALOON",
        (Vector2){w*0.25f + 10, h*0.15f + 10}, (Vector2){w*0.5f - 20, 60}, 42, darkBrown));

    scene.AddUIObject(new Banner("MEN_SUBTITLE", "A land under shadow",
        (Vector2){w*0.25f, h*0.15f + 90}, (Vector2){w*0.5f, 30}, 18,
        (Color){180, 150, 110, 255}));

    float btnW = 260, btnH = 52;
    float btnX = (w - btnW) / 2.0f;
    float startY = h * 0.42f;
    float gap = 70;

    scene.AddUIObject(new Button("MEN_BTN_PLAY", "Begin",
        (Vector2){btnX, startY}, (Vector2){btnW, btnH}, 24, btnColor));

    scene.AddUIObject(new Button("MEN_BTN_EDITOR", "Editor",
        (Vector2){btnX, startY + gap}, (Vector2){btnW, btnH}, 24, btnColor));

    scene.AddUIObject(new Button("MEN_BTN_QUIT", "Quit",
        (Vector2){btnX, startY + gap*2}, (Vector2){btnW, btnH}, 24,
        (Color){100, 30, 20, 255}));

    // Version tag
    scene.AddUIObject(new Banner("MEN_VERSION", "v0.1",
        (Vector2){w - 70, h - 30}, (Vector2){60, 22}, 14,
        (Color){100, 70, 45, 255}));
}

void GlobalInfo::LoadPauseUI()
{
    float w = SCREEN_WIDTH, h = SCREEN_HEIGHT;

    Color darkBrown = (Color){60, 35, 20, 255};
    Color cream     = (Color){245, 235, 210, 255};
    Color btnColor  = (Color){80, 48, 28, 255};

    // Dim overlay
    scene.AddUIObject(new Banner("PAU_OVERLAY", "",
        (Vector2){0, 0}, (Vector2){w, h}, 0, (Color){20, 12, 8, 180}));

    // Panel border
    float panelW = 320, panelH = 380;
    float panelX = (w - panelW) / 2.0f;
    float panelY = (h - panelH) / 2.0f;

    scene.AddUIObject(new Banner("PAU_BORDER", "",
        (Vector2){panelX - 4, panelY - 4},
        (Vector2){panelW + 8, panelH + 8},
        0, darkBrown));

    // Main panel
    scene.AddUIObject(new Banner("PAU_PANEL", "",
        (Vector2){panelX, panelY},
        (Vector2){panelW, panelH},
        0, cream));

    // Title
    scene.AddUIObject(new Banner("PAU_TITLE", "PAUSED",
        (Vector2){panelX + 10, panelY + 25},
        (Vector2){panelW - 20, 50},
        28, darkBrown));

    // Divider
    scene.AddUIObject(new Banner("PAU_DIVIDER", "",
        (Vector2){panelX + 20, panelY + 88},
        (Vector2){panelW - 40, 3},
        0, darkBrown));

    float btnW = panelW - 60;
    float btnH = 50;
    float btnX = panelX + 30;
    float gap  = 65;
    float startY = panelY + 110;

    scene.AddUIObject(new Button("PAU_BTN_RESUME", "Resume",
        (Vector2){btnX, startY},
        (Vector2){btnW, btnH},
        22, btnColor));

    scene.AddUIObject(new Button("PAU_BTN_MENU", "Main Menu",
        (Vector2){btnX, startY + gap},
        (Vector2){btnW, btnH},
        22, btnColor));

    scene.AddUIObject(new Button("PAU_BTN_QUIT", "Quit Game",
        (Vector2){btnX, startY + gap * 2},
        (Vector2){btnW, btnH},
        22, (Color){100, 30, 20, 255}));
}

void GlobalInfo::Shade()
{
    
}

void GlobalInfo::LoadEndScreenUI()
{
    float w = SCREEN_WIDTH, h = SCREEN_HEIGHT;
    Color cream     = (Color){245, 235, 210, 255};
    Color darkBrown = (Color){60, 35, 20, 255};

    // --- Shared overlay + panel ---
    scene.AddUIObject(new Banner("END_OVERLAY", "",
        (Vector2){0, 0}, (Vector2){w, h}, 0, cream));

    scene.AddUIObject(new Banner("END_TITLE", "",
        (Vector2){w*0.1f, h*0.08f}, (Vector2){w*0.8f, 60}, 38, darkBrown));

    scene.AddUIObject(new Banner("END_TEXT", "",
        (Vector2){w*0.1f, h*0.22f}, (Vector2){w*0.8f, h*0.55f}, 22, darkBrown));

    scene.AddUIObject(new Banner("END_HINT", "[Press any key to continue]",
        (Vector2){w*0.1f, h*0.85f}, (Vector2){w*0.8f, 30}, 18, (Color){100, 70, 45, 255}));
}

void GlobalInfo::LoadAnim(Character* c, int stateIdx, const string& stateName, const string& name, int frameCount)
{
    const char* dirs[4] = {"Down", "Left", "Right", "Up"};
    for (int d = 0; d < 4; d++)
    {
        string sprite = name + dirs[d] + stateName;
    
        try
        { c->anims[stateIdx][d] = sprites[sprite]; }
        catch(...) { cout<<"Sprite "<<sprite<<" not found"<<endl; }
    }

    // frameWidth derived from texture width / frameCount
    if (c->anims[stateIdx][0].id > 0 && c->anims[stateIdx][0].width > 0) c->frameWidth[stateIdx] = c->anims[stateIdx][0].width / (float)frameCount;
    else c->frameWidth[stateIdx] = 1;
}

void GlobalInfo::PlayerInfo()
{
    gI.scene.player = new Player();

    LoadAnim(scene.player, IDLE,     "Idle",    "Warrior",  5);
    LoadAnim(scene.player, MOVING,   "Walk",    "Warrior",  8);
    LoadAnim(scene.player, JUMPING,  "Jump",    "Warrior",  5);
    LoadAnim(scene.player, ATTACKING,"Attack01","Warrior",  6);
    LoadAnim(scene.player, DIE,      "Death",   "Warrior",  5);
    LoadAnim(scene.player, HURT,     "Hurt",    "Warrior",  4);
    
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
    scene.player->coinsIdx = scene.uiCount;
    scene.AddUIObject(new Text("PL_COINS", "0 Coins", (Vector2){(float)buttonSize/10.0f + margin, scene.ui[scene.uiCount-3]->Rect().y+buttonSize+10}, {60, 50}, YELLOW));


    try
    {
        scene.player->AddItem(new Potion(potions[POTENT_HEALTH_POTION]));
        scene.player->AddItem(new Potion(potions[POTENT_HEALTH_POTION]));

        scene.player->AddItem(new Potion(potions[POTENT_STAMINA_POTION]));
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

void GlobalInfo::LoadDialogueBox()
{
    int boxWidth = SCREEN_WIDTH - 110;
    int boxHeight = 200;
    int margin = 50;
    int padding = 6;

    float posX = margin;
    float posY = SCREEN_HEIGHT - boxHeight - margin;

    // --- COLORS (Cream/Brown theme) ---
    Color borderColor = (Color){92, 64, 51, 255};     // dark brown
    Color panelColor  = (Color){245, 240, 220, 255};  // cream/white

    // --- BORDER (BACKGROUND) ---
    scene.AddUIObject(new Banner("DIALOGUE_BG", "", (Vector2){posX, posY}, (Vector2){boxWidth, boxHeight}, 0, borderColor));
    // --- INNER PANEL ---
    scene.AddUIObject(new Banner("DIALOGUE_PANEL", "", (Vector2){posX + padding, posY + padding}, (Vector2){boxWidth - 2*padding, boxHeight - 2*padding}, 0, panelColor));
    // --- SPEAKER NAME ---
    scene.dialogueSpeakerTextIdx = scene.uiCount;
    scene.AddUIObject(new Banner("DIALOGUE_NAME", "Speaker", (Vector2){posX + padding + 10, posY + padding + 5}, (Vector2){300, 40}, 23, borderColor));
    // --- DIALOGUE TEXT ---
    scene.dialogueTextIdx = scene.uiCount;
    scene.AddUIObject(new Banner("DIALOGUE_TEXT", "Dialogue goes here...", (Vector2){posX + padding + 10, posY + padding + 50}, (Vector2){boxWidth - 40, boxHeight-70}, 25, DARKBROWN));
    
    boxWidth = 400, boxHeight = 150, margin = 50, padding = 6;

    // Position relative to dialogue box
    float dialogueTop = SCREEN_HEIGHT - 200 - margin; // same boxHeight as your dialogue
    posX = SCREEN_WIDTH - margin - boxWidth - 10;
    posY = dialogueTop - boxHeight - 10; // 10px gap above dialogue

    // --- COLORS ---
    borderColor = (Color){92, 64, 51, 255};     // dark brown
    panelColor  = (Color){245, 240, 220, 255};  // cream

    // --- BORDER ---
    scene.AddUIObject(new Banner("CHOICE_BG", "", (Vector2){posX, posY}, (Vector2){boxWidth, boxHeight}, 0, borderColor));
    // --- INNER PANEL ---
    scene.AddUIObject(new Banner("CHOICE_PANEL", "", (Vector2){posX + padding, posY + padding}, (Vector2){boxWidth - 2*padding, boxHeight - 2*padding}, 0, panelColor));

    // --- CHOICES TEXT (3 OPTIONS) ---
    scene.choice1Idx = scene.uiCount;
    scene.AddUIObject(new Banner("CHOICE_1", "Choice 1", (Vector2){posX + padding + 10, posY + padding + 10}, (Vector2){boxWidth - 30, 35}, 20, DARKBROWN));
    scene.AddUIObject(new Banner("CHOICE_2", "Choice 2", (Vector2){posX + padding + 10, posY + padding + 50}, (Vector2){boxWidth - 30, 35}, 20, DARKBROWN));
    scene.AddUIObject(new Banner("CHOICE_3", "Choice 3", (Vector2){posX + padding + 10, posY + padding + 90}, (Vector2){boxWidth - 30, 35}, 20, DARKBROWN));
}

void GlobalInfo::Assets()
{
    UIGrid grid({gI.SCREEN_WIDTH-300, 180}, 32);

    models[COLLIDER_MODEL_NAME] = LoadModelFromMesh(GenMeshCube(1, 1, 1));
    textures[COLLIDER_MODEL_NAME] = LoadTextureFromImage(GenImageChecked(10, 10, 10, 10, GREEN, WHITE));

    models[COLLIDER_MODEL_NAME].materials[0].maps[MATERIAL_MAP_DIFFUSE].color = (Color){0, 255, 0, 80}; 
    
    models[DEFAULT_MODEL_NAME] = LoadModelFromMesh(GenMeshCube(1, 1, 1));
    textures[DEFAULT_MODEL_NAME] = LoadTextureFromImage(GenImageChecked(10, 10, 10, 10, DARKPURPLE, WHITE));

    models[ENEMY_SPAWNER_NAME] = LoadModelFromMesh(GenMeshCube(1, .1, 1));
    textures[ENEMY_SPAWNER_NAME] = LoadTextureFromImage(GenImageChecked(10, 10, 10, 10, RED, WHITE));   

    models[MERCHANT_SPAWNER_NAME] = LoadModelFromMesh(GenMeshCube(1, .1, 1));
    textures[MERCHANT_SPAWNER_NAME] = LoadTextureFromImage(GenImageChecked(10, 10, 10, 10, BROWN, WHITE));

    models[CIVIL_SPAWNER_NAME] = LoadModelFromMesh(GenMeshCube(1, .1, 1));
    textures[CIVIL_SPAWNER_NAME] = LoadTextureFromImage(GenImageChecked(10, 10, 10, 10, GREEN, WHITE));

    models[BOSS_SPAWNER_NAME] = LoadModelFromMesh(GenMeshCube(1, .1, 1));
    textures[BOSS_SPAWNER_NAME] = LoadTextureFromImage(GenImageChecked(10, 10, 10, 10, PURPLE, WHITE));

    FilePathList files = LoadDirectoryFiles(MODELS_FOLDER_PATH.c_str());
    for (int i = 0; i<(int)files.count; i++)
    {
        if (IsFileExtension(files.paths[i], ".obj") || IsFileExtension(files.paths[i], ".gltf"))
        {
            string name = GetFileNameWithoutExt(files.paths[i]);
            models[name] = LoadModel(files.paths[i]);
            if (FileExists((TEXTURES_FOLDER_PATH+"\\"+name+".png").c_str()))
                textures[name] = LoadTexture((TEXTURES_FOLDER_PATH+"\\"+name+".png").c_str());
            scene.AddUIObject(new Button("Spawner"+to_string(i), name, {0,0}, {300, 30}, 17, DARKGRAY));
            grid.AddElement(scene.ui[scene.uiCount-1]);
        
        }
    }
    grid.OrderUI(VERTICAL);
    UnloadDirectoryFiles(files);
    
    files = LoadDirectoryFiles(SPRITES_FOLDER_PATH.c_str());

    for (int i = 0; i<(int)files.count; i++)
    {
        if (IsFileExtension(files.paths[i], ".png"))
        {
            string name = GetFileNameWithoutExt(files.paths[i]);
            sprites[name] = LoadTexture(files.paths[i]);
            SetTextureFilter(sprites[name], TEXTURE_FILTER_POINT);
        }
    }

    UnloadDirectoryFiles(files);

    potions[WEAK_HEALTH_POTION]     = Potion("Weak Health Potion", HEALTH_REGEN, 10, 2);
    potions[MID_HEALTH_POTION]      = Potion("Mid Health Potion", HEALTH_REGEN, 30, 6);
    potions[POTENT_HEALTH_POTION]   = Potion("Potent Health Potion", HEALTH_REGEN, 60, 14);

    potions[WEAK_STAMINA_POTION]    = Potion("Weak Stamina Potion", STAMINA_REGEN, 10, 1);
    potions[MID_STAMINA_POTION]     = Potion("Mid Stamina Potion", STAMINA_REGEN, 30, 3);
    potions[POTENT_STAMINA_POTION]  = Potion("Potent Stamina Potion", STAMINA_REGEN, 60, 6);

    potions[STRENGTH_POTION]        = Potion("Strength Potion", STRENGTH_BOOST, 60, 20);
}

string UnRepeatedPotion(string type, int tries = 10)
{
    string name = gI.potions[rand()%7].Name();
    if (name.find(type) != string::npos && tries>0)
        return UnRepeatedPotion(type, tries-1);
    return name;
}

string Type(string name)
{
    if (name.find("Health") != string::npos)
        return "Health";

    if (name.find("Stamina") != string::npos)
        return "Stamina";

    if (name.find("Strength") != string::npos)
        return "Strength";
    
    return "No Type";
}

void GlobalInfo::LoadNPCs()
{
    cout<<"Enemy Spawn Count "<<scene.enemySpawnCount<<endl;
    for (int n = 0; n < scene.enemySpawnCount; n++)
    {
        PossessedNPC* npc = new PossessedNPC(
            "Possessed_" + to_string(n),
            scene.enemySpawnPositions[n]->Position() + (Vector3){0, 0.75f, 0},
            30,   // maxHealth
            2.5f, // speed
            10    // damage
        );

        LoadAnim(npc, IDLE,     "Idle", "Possesed",    5);
        LoadAnim(npc, MOVING,   "Walk", "Possesed",    6);
        LoadAnim(npc, JUMPING,  "Jump", "Possesed",    5);
        LoadAnim(npc, ATTACKING,"Attack01", "Possesed",11);
        LoadAnim(npc, DIE,      "Death", "Possesed",   10);
        LoadAnim(npc, HURT,     "Hurt", "Possesed",    4);

        scene.AddNPC(npc);
    }

    string names[15] = {
        "Merchant-Man",             "Little Lamb",              "Belathor",
        "Man Of Many Skills",       "Grub",                     "Phillip Overcharge",
        "Alchemove",                "Edmund Fleece",            "Qasim of Questionable Repute",
        "He Who Can Be Named",      "He Who Doesn't Remain",    "The 8th Name",
        "Barry the Barely-Honest",  "Khaas Khubaib",            "Aam Ali"
    };

    
    for (int n = 0; n < scene.merchantSpawnCount; n++)
    {
        string potion = potions[rand()%7].Name();
        Merchant* m = new Merchant(
            names[rand()%15],
            scene.merchantSpawnPositions[n]->Position() + (Vector3){0, 0.5f, 0},
            {potion, UnRepeatedPotion(Type(potion))}, 3.0f
        );
        
        LoadAnim(m, IDLE, "Idle", "Man", 12);

        scene.AddNPC(m);
    }

    for (int n = 0; n < scene.civilSpawnCount; n++)
    {
        Civilian* m = new Civilian(
            names[rand()%15],
            scene.civilSpawnPositions[n]->Position() + (Vector3){0, 0.65f, 0}, 1.35f
        );
        
        LoadAnim(m, IDLE, "Idle", "Old", 1);

        scene.AddNPC(m);
    }

    // --- Boss ---
    for (int n = 0; n < scene.bossSpawnCount; n++)
    {
        Boss* boss = new Boss(
            "The Warlord",
            scene.bossSpawnPositions[n]->Position() + (Vector3){0, 0.75f, 0},
            250,  // maxHealth — tanky
            3.0f, // speed
            20    // damage
        );

        // Reuse Warrior (player) sprites — same as player but tinted red when hostile
        LoadAnim(boss, IDLE,      "Idle",     "Warrior", 5);
        LoadAnim(boss, MOVING,    "Walk",     "Warrior", 8);
        LoadAnim(boss, JUMPING,   "Jump",     "Warrior", 5);
        LoadAnim(boss, ATTACKING, "Attack01", "Warrior", 6);
        LoadAnim(boss, DIE,       "Death",    "Warrior", 5);
        LoadAnim(boss, HURT,      "Hurt",     "Warrior", 4);

        scene.AddNPC(boss);
    }
}

void GlobalInfo::LoadThings()
{
    Assets();
    PlayerInfo();
    LoadDialogueBox();
    LoadEndScreenUI();
    LoadPauseUI();
    LoadMenuUI();
}

void GlobalInfo::UnloadThings()
{
    for (const auto& model : models) 
    {
        UnloadModel(model.second);
    }
    for (const auto& tex : textures) 
    {
        UnloadTexture(tex.second);
    }

    for (const auto& sprite : sprites)
    {
        UnloadTexture(sprite.second);
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

    delete items[idx][itemsCount[idx]-1];
    items[idx][itemsCount[idx]-1] = nullptr;
    itemsCount[idx]--;
}

Inventory::~Inventory()
{
    for (int i = 0; i<MAX_SLOTS; i++)
    {
        for (int j = 0; j<itemsCount[i]; j++)
            delete items[i][j];
        
        delete[] items[i];
    }
}

// NPC ==================================================================================================================================================================================

NPC::~NPC() {}

// Civilian ==================================================================================================================================================================================

void Civilian::DialogueSetup()
{
    target = (gI.scene.player!=nullptr?gI.scene.player->Position():(Vector3){0,0,0});
    Character::Update();

    dialogueIdx = 0;
    dialogues[0].dialogue = gI.CIVIL_GREETINGS[rand()%10]; ;  
}

void Civilian::Dialogue()
{
    if (dialogueIdx == -1) return;

    Banner* banner = dynamic_cast<Banner*>(gI.scene.ui[gI.scene.dialogueTextIdx]);
    if (banner) banner->_Text()._Text(dialogues[dialogueIdx].dialogue);

    banner = dynamic_cast<Banner*>(gI.scene.ui[gI.scene.dialogueSpeakerTextIdx]);
    if (banner) banner->_Text()._Text(name);
 
    for (const auto& choice : dialogues[dialogueIdx].choices)
    {
        banner = dynamic_cast<Banner*>(gI.scene.ui[gI.scene.choice1Idx+choice.first]);
        if (banner) banner->_Text()._Text(to_string(choice.first+1)+". "+choice.second.second);
    }
    
    int inp = GetKeyPressed();
     
    if (inp >= '1' && inp <= '3')
    {
        int choiceIdx = inp - '1';
        dialogueIdx = dialogues[dialogueIdx].choices[choiceIdx].first;
    }
}

void Civilian::DrawCharacter()
{
    if (anims[IDLE][0].id == 0) return;

    int i = IDLE;

    Texture2D anim[4] = {anims[i][0], anims[i][1], anims[i][2], anims[i][3]};

    Rectangle sourceRec = {
        (float)currentFrame * frameWidth[i],
        0,
        (float)frameWidth[i],
        (float)anim[currDir].height
    };

    if (!gI.scene.player) return;

    AnimationData animData =
    {
        gI.scene.player->Camera(),
        anim[currDir],
        sourceRec,
        position,
        (Vector2){ size, size },
        WHITE
    };

    gI.scene.billboards[id] = animData;
}

// Merchant ==================================================================================================================================================================================

void Merchant::DialogueSetup()
{
    dialogueIdx = 0;

    dialogues[0].dialogue = gI.MERCHANT_GREETINGS[rand()%5];
    dialogues[1].dialogue = gI.MERCHANT_SELL[rand()%5];
    dialogues[2].dialogue = gI.MERCHANT_BUY[rand()%5];

    Player* pl = gI.scene.player;

    if (!pl) return;

    target = pl->Position();
    Character::Update();

    t = (float)pl->Charisma() / 10.0f; // normalize 0-1
    float multiplier = 1.0f - (t * t * 0.5f); // max 50% discount at charisma 10

    for (int i = 0; i<2; i++)
    {
        for (int p = 0; p < 7; p++)
            if (gI.potions[p].Name() == items[i])
            {
                buyPrices[i] = (int)ceil((float)gI.potions[p].Price()*multiplier);
                break;
            }
    }

    auto& buyChoices = dialogues[1].choices;
    buyChoices[0].second = items[0] + " at "+to_string(buyPrices[0]);
    buyChoices[1].second = items[1] + " at "+to_string(buyPrices[1]);
}

void Merchant::Dialogue()
{
    if (dialogueIdx == -1) return;

    Player* pl = gI.scene.player;

    if (!pl) return;

    if (dialogueIdx == 2)
    {
        auto& sellChoices = dialogues[2].choices;

        for (int i = 0; i<2; i++)
        {
            if (pl->_Inventory().itemsCount[i] <= 0) continue;

            for (int p = 0; p < 7; p++)
                if (gI.potions[p].Name() == pl->_Inventory().items[i][0]->Name())
                {
                    sellPrices[i] = (int)ceil((gI.potions[p].Price()/1.5f) * (1.0f + t*t*0.5f));
                    break;
                }
        }

        sellChoices[0].second = (pl->_Inventory().itemsCount[0] > 0) ? pl->_Inventory().items[0][0]->Name() + " for " + to_string(sellPrices[0]): "Nothing";
        sellChoices[1].second = (pl->_Inventory().itemsCount[1] > 0) ? pl->_Inventory().items[1][0]->Name() + " for " + to_string(sellPrices[1]) : "Nothing";
    }

    Banner* banner = dynamic_cast<Banner*>(gI.scene.ui[gI.scene.dialogueTextIdx]);
    if (banner) banner->_Text()._Text(dialogues[dialogueIdx].dialogue);

    banner = dynamic_cast<Banner*>(gI.scene.ui[gI.scene.dialogueSpeakerTextIdx]);
    if (banner) banner->_Text()._Text(name);
 
    for (const auto& choice : dialogues[dialogueIdx].choices)
    {
        banner = dynamic_cast<Banner*>(gI.scene.ui[gI.scene.choice1Idx+choice.first]);
        if (banner) banner->_Text()._Text(to_string(choice.first+1)+". "+choice.second.second);
    }
    
    int inp = GetKeyPressed();
     
    if (inp >= '1' && inp <= '3')
    {
        int choiceIdx = inp - '1';
        auto it = dialogues[dialogueIdx].choices.find(choiceIdx);
        if (it == dialogues[dialogueIdx].choices.end()) return;

        string choiceText = it->second.second;

        // --- Buy logic (node 1 -> buying an item) ---
        if (dialogueIdx == 1 && choiceIdx < 2)
        {
            for (int p = 0; p < 7; p++)
            {
                if (gI.potions[p].Name() != items[choiceIdx]) continue;
                
                if (pl->Coins() < buyPrices[choiceIdx]) break;

                try { pl->BuyItem(new Potion(gI.potions[p]), buyPrices[choiceIdx]); }
                catch(...) {}

                break;
            } 
        }

        if (dialogueIdx == 2 && choiceIdx < 2)
        {  
            if (pl->_Inventory().itemsCount[choiceIdx] > 0) 
            
            for (int p = 0; p < 7; p++)
            {
                if (gI.potions[p].Name() != pl->_Inventory().items[choiceIdx][0]->Name()) continue;
                
                try { pl->SellItem(pl->_Inventory().items[choiceIdx][0]->Name(),  sellPrices[choiceIdx]);}
                catch(...) {}
                break;
            }
        }

        dialogueIdx = dialogues[dialogueIdx].choices[choiceIdx].first;
    }
}

void Merchant::DrawCharacter()
{
    if (anims[IDLE][0].id == 0) return;

    int i = IDLE; // merchant always idle

    Texture2D anim[4] = {anims[i][0], anims[i][1], anims[i][2], anims[i][3]};

    frameTimer += gI.dT;
    if (frameTimer >= 0.12f)
    {
        frameTimer = 0.0f;
        if (anim[currDir].id > 0 && anim[currDir].width > 0 && (int)frameWidth[i] > 0)
        {
            int maxFrames = anim[currDir].width / (int)frameWidth[i];
            currentFrame = (currentFrame + 1) % maxFrames;
        }
        else currentFrame = 0;
    }

    Rectangle sourceRec = {
        (float)currentFrame * frameWidth[i],
        0,
        (float)frameWidth[i],
        (float)anim[currDir].height
    };

    if (!gI.scene.player) return;

    AnimationData animData =
    {
        gI.scene.player->Camera(),
        anim[currDir],
        sourceRec,
        position,
        (Vector2){ size, size},
        WHITE
    };

    gI.scene.billboards[id] = animData;
}

void Merchant::DrawInteractPrompt()
{
    if (!gI.scene.player || gI.scene.dialogueVisible) return;

    Vector3 toPlayer = {
        gI.scene.player->Position().x - position.x,
        0,
        gI.scene.player->Position().z - position.z
    };
    float dist = Magnitude(toPlayer);
    if (dist <= gI.INTERACT_RANGE)
    {
        Vector2 screenPos = GetWorldToScreen(
            {position.x, position.y + 1.5f, position.z},
            gI.scene.player->Camera()
        );
        DrawText("[E] Talk", (int)screenPos.x - 30, (int)screenPos.y - 10, 20, YELLOW);
    }
}

// PossessedNPC ==================================================================================================================================================================================

void PossessedNPC::TakeDamage(float amount)
{
    if (currHealth<=0) return;
    if (state == DIE) return;
    
    currHealth = Clamp(currHealth - amount, 0, maxHealth);
    state = HURT;
    hurtTimer = 0.4f;
}

void PossessedNPC::Update()
{
    if (currHealth<=0) return;

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
            player->TakeDamage((float)damage);
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
    if (i >= 6) i = IDLE; // safety clamp

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
        if (anim[currDir].id > 0 && anim[currDir].width > 0 && (int)frameWidth[i] > 0)
        {
            int maxFrames = anim[currDir].width / (int)frameWidth[i];
            if (currentFrame < maxFrames - 1)
                currentFrame++;
            else
            {
                if (state == HURT)
                    state = (currHealth>0)?IDLE:DIE; // return to idle after attack/hurt anim
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
        (Vector2){ size, size },
        WHITE
    };

    gI.scene.billboards[id] = animData;
}

// Player ==================================================================================================================================================================================

void Player::TakeDamage(float amount)
{
    if (currHealth<=0) return;
    
    state = HURT;
    hurtTimer = 0.3f;

    currHealth -= (amount/armour);

    if (currHealth<=0) gI.scene.ShowPlayerDeathScreen();
}

void Player::Dialogue()
{
    bool interacted = IsKeyPressed(gI.INTERACT_KEY);

    if (interacted)
    {
        // If already in dialogue, E always closes — never re-opens same frame
        if (gI.scene.dialogueVisible || interactNPC)
        {
            gI.scene.dialogueVisible = false;
            interactNPC = nullptr;
            return;
        }

        // Find closest dialogueable NPC in range
        float closestDist = gI.INTERACT_RANGE;
        interactNPC = nullptr;

        for (int i = 0; i < gI.scene.npcCount; i++)
        {
            float dist = Vector3Distance(gI.scene.npcs[i]->Position(), position);
            if (dist > gI.INTERACT_RANGE) continue;

            I_Dialogueable* candidate = dynamic_cast<I_Dialogueable*>(gI.scene.npcs[i]);
            if (!candidate) continue;

            if (dist < closestDist)
            {
                closestDist = dist;
                interactNPC = candidate;
            }
        }

        if (interactNPC)
        {
            gI.scene.dialogueVisible = true;
            interactNPC->DialogueSetup();
        }
    }

    // Every frame: run active dialogue
    if (gI.scene.dialogueVisible && interactNPC)
    {
        interactNPC->Dialogue();
        if (interactNPC->dialogueIdx == -1)
        {
            gI.scene.dialogueVisible = false;
            interactNPC = nullptr;
        }
    }
}

void Player::Update() 
{
    if (gI.scene.dialogueVisible || gI.scene.endScreenVisible) return;

    if (currHealth == 0)
    {
        state = DIE;
        position = {0,0,0};
        currHealth = maxHealth;
        return;
    }

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
    
    for (int i = 0; i<gI.scene.colliderCount; i++) 
    {
        rayInfo = GetRayCollisionBox(directionRay, gI.scene.colliders[i]->Boundary());
        
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

void Player::Attack()
{
    if (currHealth < 0 || gI.scene.dialogueVisible) return;
    
    if (IsKeyPressed(gI.ATTACK_KEY) || IsMouseButtonPressed(gI.ATTACK_KEY_MOUSE)) state = ATTACKING;

    if (gI.scene.player->state != ATTACKING) 
    {
        attackHitDealt = false;
        return;
    }
    
    if (!attackHitDealt)
    {
        attackHitDealt = true;
        const float PLAYER_ATTACK_RANGE = 1.5f;
        for (int i = 0; i < gI.scene.enemyCount; i++)
        {
            PossessedNPC* npc = gI.scene.enemies[i];

            if (npc->CurrHealth() <= 0) continue;

            Vector3 diff = { npc->Position().x - position.x, 0, npc->Position().z - position.z };
            if (Magnitude(diff) <= PLAYER_ATTACK_RANGE) npc->TakeDamage((float)(gI.scene.player->CurrDamage()+strength)/1.5f);
            if (npc->CurrHealth() <= 0) 
            {
                int randomCoins = rand()%11+5;
                coins+=randomCoins;
                InvUI_Update();
            }
        }

        // Also hit the Boss if he's hostile
        for (int i = 0; i < gI.scene.npcCount; i++)
        {
            Boss* boss = dynamic_cast<Boss*>(gI.scene.npcs[i]);
            if (!boss || !boss->IsHostile() || boss->CurrHealth() <= 0) continue;

            Vector3 diff = { boss->Position().x - position.x, 0, boss->Position().z - position.z };
            if (Magnitude(diff) <= PLAYER_ATTACK_RANGE)
            {
                boss->TakeDamage((float)(gI.scene.player->CurrDamage() + strength) / 1.5f);
                if (boss->CurrHealth() <= 0)
                {
                    coins += rand() % 51 + 50; // big coin reward for killing the boss
                    InvUI_Update();
                }
            }
        }
    }
}

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
        (Vector2){ size, size},
        WHITE
    };

    gI.scene.billboards[id] = animData;
}

void Player::CalculateIsGrounded()
{
    isGrounded = false;
    if (position.y<=0.65f)
    {
         isGrounded = true;
         position.y = 0.65f;
         return;

    }
    // for (int i = 0; i<gI.scene.objectCount; i++) 
    // {
    //     groundInfo = GetRayCollisionBox(groundRay, gI.scene.objects[i]->Boundary());

    //     if (groundInfo.hit)
    //     {
    //         if (groundInfo.distance<=0.2f)
    //         {
    //             isGrounded = true;
    //             break;
    //         }
            
    //     }
    // }
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

        tx = dynamic_cast<Text*>(gI.scene.ui[coinsIdx]);
    if (tx) tx->_Text( to_string(coins) + " Coins" );
}

void Player::UpdateEffects()
{
    if (gI.scene.dialogueVisible) return;

    int inp = (IsKeyPressed(gI.INV_1))?0:(IsKeyPressed(gI.INV_2)?1:-1);

    if (inp>=0 && inventory.itemsCount[inp]>0)
    {
        Potion* potion = dynamic_cast<Potion*>(inventory.items[inp][0]);
        if (potion)
        {
            potion->ApplyEffect(*this);
            try
            { inventory.RemoveItem(inp); }
            catch(const out_of_range& e) { cout<<e.what()<<endl; }
            catch(const empty_collection& e) { cout<<e.what()<<endl; }

            InvUI_Update();
        }
    }
    
    map<int, float> temp = effects;
    vector<int> damageBoosts;
    
    for (const auto& effect : temp)
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

// Boss ==================================================================================================================================================================================

void Boss::DialogueSetup()
{
    // Face the player
    if (gI.scene.player)
    {
        target = gI.scene.player->Position();
        Character::Update();
    }
    dialogueIdx = 0;
    outcome = BOSS_NONE;
}

void Boss::Dialogue()
{
    if (dialogueIdx == -1) return;

    // Update UI text
    Banner* banner = dynamic_cast<Banner*>(gI.scene.ui[gI.scene.dialogueTextIdx]);
    if (banner) banner->_Text()._Text(dialogues[dialogueIdx].dialogue);

    banner = dynamic_cast<Banner*>(gI.scene.ui[gI.scene.dialogueSpeakerTextIdx]);
    if (banner) banner->_Text()._Text(name);

    for (const auto& choice : dialogues[dialogueIdx].choices)
    {
        banner = dynamic_cast<Banner*>(gI.scene.ui[gI.scene.choice1Idx + choice.first]);
        if (banner) banner->_Text()._Text(to_string(choice.first + 1) + ". " + choice.second.second);
    }

    int inp = GetKeyPressed();
    if (inp >= '1' && inp <= '3')
    {
        int choiceIdx = inp - '1';

        switch (choiceIdx)
        {
            case 0: // Fight
                outcome   = BOSS_FIGHT;
                isHostile = true;
                relation  = ENEMY;
                break;

            case 1: // Join — buff the player with a permanent +10 damage boost
                outcome = BOSS_JOIN;
                {
                    Banner* title = dynamic_cast<Banner*>(gI.scene.ui[gI.scene.FindUIObjectIndex("END_TITLE")]);
                    Banner* text  = dynamic_cast<Banner*>(gI.scene.ui[gI.scene.FindUIObjectIndex("END_TEXT")]);
                    if (title) title->_Text()._Text("An Unlikely Alliance");
                    if (text)  text->_Text()._Text(
                        "And so the warrior knelt before the Dread Lord of Waloon, "
                        "not in defeat, but in dark accord.\n\n"
                        "Their united force was unmatched. Together they swept through "
                        "the valleys east of Waloon, crushing every \nbanner that dared "
                        "rise against them. Villages fell.\nKingdoms bent the knee.\n\n"
                        "But power is a hungry thing.\n\n"
                        "In the dead of winter, when the spoils had been divided \nand "
                        "the fires of conquest had cooled, the Boss turned on his "
                        "\nally without warning. A blade in the dark. A throne for one.\n\n"
                        "The warrior was never seen again.\n\n"
                        "Waloon endured. Colder than before."
                    );
                    gI.scene.endScreenVisible = true;
                }
                break;

            case 2: // Leave the village — teleport player far away
                outcome = BOSS_LEAVE;
                if (gI.scene.player)
                    gI.scene.player->Position({50.0f, 0.65f, 50.0f});
                {
                    Banner* title = dynamic_cast<Banner*>(gI.scene.ui[gI.scene.FindUIObjectIndex("END_TITLE")]);
                    Banner* text  = dynamic_cast<Banner*>(gI.scene.ui[gI.scene.FindUIObjectIndex("END_TEXT")]);
                    if (title) title->_Text()._Text("The Road Away");
                    if (text)  text->_Text()._Text(
                        "And so he walked.\n\n"
                        "No words. No final stand. Just the sound of boots on stone "
                        "\nand the village growing smaller behind him.\n\n"
                        "The villagers watched from their windows as the one warrior\n"
                        "who could have saved them disappeared into the grey hills.\n\n"
                        "The Boss laughed, a long, slow laugh \nthat rolled through "
                        "the streets like smoke.\n\n"
                        "That winter was the harshest Waloon had ever known. "
                        "\nNot because of the cold, but because of what the cold "
                        "carried with it, the Boss's will, unchallenged and absolute.\n\n"
                        "They still tell the story. They say a warrior came once.\n\n"
                        "He left, they say."
                    );
                    gI.scene.endScreenVisible = true;
                }
                break;
        }

        dialogueIdx = -1; // close dialogue
    }
}

void Boss::TakeDamage(float amount)
{
    if (currHealth <= 0) return;
    if (state == DIE) return;

    currHealth = Clamp(currHealth - amount, 0, maxHealth);
    state = HURT;
    hurtTimer = 0.4f;

    if (currHealth<=0) gI.scene.ShowBossDeathScreen();
}

void Boss::Update()
{
    if (currHealth <= 0) return;
    if (state == DIE) return;

    if (state == HURT)
    {
        hurtTimer -= gI.dT;
        if (hurtTimer <= 0) state = IDLE;
        return;
    }

    Player* player = gI.scene.player;
    if (!player) return;

    // Only chase/attack if hostile
    if (!isHostile)
    {
        state = IDLE;
        target = position;
        return;
    }

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
        target = position;
        state  = ATTACKING;
        if (attackTimer <= 0)
        {
            attackTimer = ATTACK_COOLDOWN;
            player->TakeDamage((float)damage);
        }
    }
    else if (dist <= AGGRO_RANGE)
    {
        target = {player->Position().x, position.y, player->Position().z};
        state  = MOVING;
    }
    else
    {
        float distToSpawn = Magnitude({spawnPos.x - position.x, 0, spawnPos.z - position.z});
        if (distToSpawn > 0.1f) { target = spawnPos; state = MOVING; }
        else                    { target = position;  state = IDLE; }
    }

    Character::Update();
}

void Boss::DrawCharacter()
{
    if (anims[IDLE][0].id == 0) return;

    int i = state;
    if (i >= 6) i = IDLE;

    if (state != bossLastState)
    {
        currentFrame   = 0;
        frameTimer     = 0.0f;
        bossLastState  = state;
    }

    Texture2D anim[4] = {anims[i][0], anims[i][1], anims[i][2], anims[i][3]};

    frameTimer += gI.dT;
    if (frameTimer >= 0.12f)
    {
        frameTimer = 0.0f;
        if (anim[currDir].id > 0 && anim[currDir].width > 0 && (int)frameWidth[i] > 0)
        {
            int maxFrames = anim[currDir].width / (int)frameWidth[i];
            if (currentFrame < maxFrames - 1)
                currentFrame++;
            else
            {
                if (state == HURT)
                    state = (currHealth > 0) ? IDLE : DIE;
                else if (state != DIE)
                    currentFrame = 0;
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

    if (!gI.scene.player) return;

    // Tint red when hostile
    Color tint = isHostile ? (Color){255, 100, 100, 255} : WHITE;

    AnimationData animData = {
        gI.scene.player->Camera(),
        anim[currDir],
        sourceRec,
        position,
        (Vector2){ size, size },
        tint
    };

    gI.scene.billboards[id] = animData;
}

void Boss::DrawInteractPrompt()
{
    if (!gI.scene.player || gI.scene.dialogueVisible || isHostile) return;

    Vector3 toPlayer = {
        gI.scene.player->Position().x - position.x,
        0,
        gI.scene.player->Position().z - position.z
    };
    float dist = Magnitude(toPlayer);
    if (dist <= gI.INTERACT_RANGE)
    {
        Vector2 screenPos = GetWorldToScreen(
            {position.x, position.y + 1.5f, position.z},
            gI.scene.player->Camera()
        );
        DrawText("[E] Speak", (int)screenPos.x - 35, (int)screenPos.y - 10, 20, RED);
    }
}