#include "Inputs.h"

// Misc ==================================================================================================================================================================================

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

Vector3 operator+(Vector3& a, Vector3& b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vector3 operator-(Vector3& a, Vector3& b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
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
    boundary = GetModelBoundingBox(model);
    boundary.min = Vector3Scale(boundary.min, size);
    boundary.max = Vector3Scale(boundary.max, size);
    boundary.min = Vector3Add(boundary.min, position);
    boundary.max = Vector3Add(boundary.max, position);
}

// CameraMI ==================================================================================================================================================================================

void CameraMI::CameraFreeMove(float dT)
{
    pitch = Clamp(pitch + GetMouseDelta().y * sensitivity * dT, -90.0f, 90.0f);
    yaw = Clamp(yaw + GetMouseDelta().x * sensitivity * dT, -90.0f, 90.0f);
    target.x = camera.position.x + 10*sin(pitch)*cos(yaw);
    target.z = camera.position.z + 10*sin(pitch)*sin(yaw);
    target.y = camera.position.y + 10*cos(pitch);
    camera.target = target;

    direction = target-camera.position;  
    Vector2 input = GetDirectionalInputV(FORWARD_KEY, BACKWARD_KEY, LEFT_KEY, RIGHT_KEY);

    camera.position = (Vector3){
        camera.position.x + speed * dT * (input.y* -direction.x  + input.x * CrossProduct(direction, camera.up).x),
        camera.position.y + speed * dT * input.y * -direction.y,
        camera.position.z + speed * dT * (input.y * -direction.z + input.x * CrossProduct(direction, camera.up).z)
    };
}

void CameraMI::SpeedScroll()
{
    if (GetMouseWheelMoveV().y != 0)
    {
        speed += GetMouseWheelMoveV().y * WHEEL_SENSITIVITY;
        speed = Clamp(speed, 0.1f, MAX_SPEED);
    }
}

// Scene ==================================================================================================================================================================================

void Scene::AddObject(Box* newObject, int i=0) 
{
    string name = newObject->Name();
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
    string name = newObject->Name();
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

void Scene::SelectionMove(float dT)
{
    if (selected != nullptr)
    {
        Vector2 moveInput = GetDirectionalInputV(FORWARD_KEY, BACKWARD_KEY, LEFT_KEY, RIGHT_KEY);
        Vector2 rotationInput = GetDirectionalInputV(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT);
        float sizeInput = GetInputODFrom(KEY_MINUS, KEY_EQUAL);
        float heightInput = GetInputODFrom(KEY_Q, KEY_E);
        
        selected->Size(selected->Size() + sizeInput * dT * selectionSpeed);

        selected->Position({
            selected->Position().x + moveInput.x * dT * selectionSpeed,
            selected->Position().y  + heightInput * dT * selectionSpeed,
            selected->Position().z + moveInput.y * selectionSpeed * dT 
        });

        selected->Rotation({
            selected->Rotation().x + rotationInput.y * dT * selectionSpeed, 
            selected->Rotation().y + rotationInput.x * dT * selectionSpeed, 
            selected->Rotation().z
        });
    }
}

void Scene::SpeedScroll()
{
    if (GetMouseWheelMoveV().y != 0)
    {
        selectionSpeed += GetMouseWheelMoveV().y * WHEEL_SENSITIVITY;
        selectionSpeed = Clamp(selectionSpeed, 0.1f, MAX_SPEED);
    }
}   

void Scene::DrawScene()
{
    for (int i = 0; i < objectCount; i++)
    {
        Box* box = objects[i];
        DrawModel(box->Model(), box->Position(), box->Size(), WHITE);
    }
}

void Scene::DrawSceneUI()
{
    for (int i = 0; i< uiCount; i++)
    {
        RectTransform* uiElement = ui[i];
        Button* button = dynamic_cast<Button*>(uiElement);
        Text* text = dynamic_cast<Text*>(uiElement);

        if (button != nullptr)
        {
            DrawRectangleRec(button->Rect(), button->BackColor());
            DrawText(button->_Text()._Text().c_str(), button->Rect().x + button->_Text().Rect().x, button->Rect().y + button->_Text().Rect().y, button->_Text().Rect().width, WHITE);
        }
        else if (text != nullptr)
        {
            DrawText(text->_Text().c_str(), text->Rect().x, text->Rect().y, 20, text->_Color());
        }
    }
}

void Scene::SelectObject(Ray ray)
{
    selectionRay = ray;
    selected = nullptr;
    for (int i = 0; i < objectCount; i++)
    {
        Box* box = objects[i];
        selectionRayCollision = GetRayCollisionBox(ray, box->Boundary());
        if (selectionRayCollision.hit) {
            selected = box;
            break;
        }
    } 
}

void Scene::UnloadThings()
{
    for (auto model : models) 
    {
        UnloadModel(model.second);
    }
    for (auto tex : textures) 
    {
        UnloadTexture(tex.second);
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
            AddObject(new Box("Gameobject", {0,0,0}, 1, models[button->_Text()._Text()], textures[button->_Text()._Text()]));
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

    if (!IsMouseButtonPressed(SELECTION_KEY)) return false;

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




















































