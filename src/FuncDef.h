#include "Inputs.h"

// Misc ==================================================================================================================================================================================

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

void Scene::AddObject(Box* newObject) 
{
    Box** newObjects = new Box*[objectCount + 1];
    for (int i = 0; i < objectCount; i++) {
        newObjects[i] = objects[i];
    }
    newObjects[objectCount] = newObject;
    delete[] objects;
    objects = newObjects;
    objectCount++;
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

int Scene::FindIndex(string name)
{
    for (int i = 0; i < objectCount; i++) {
        if (objects[i]->Name() == name) {
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
        DrawModel(box->Model(), box->Position(), box->Size(), box->_Color());
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





















