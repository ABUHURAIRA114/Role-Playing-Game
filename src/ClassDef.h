#include "HeaderAndProto.h"

class TransformMI
{
    protected:

    string name;
    Vector3 position;
    Vector3 rotation;
    float size;

    public:

    TransformMI(Vector3 position = {0, 0, 0}, Vector3 rotation = {0, 0, 0}, float size = 1) : position(position), rotation(rotation), size(size) {}

    virtual string Name() { return name; }
    virtual Vector3 Position() { return position; }
    virtual Vector3 Rotation() { return rotation; }
    virtual float Size() { return size; }

    virtual void Position(Vector3 newPos) { position = newPos; }
    virtual void Name(string name) { this->name = name; }
    virtual void Rotation(Vector3 newRotation) { rotation = newRotation; }
    virtual void Size(float newSize) { size = newSize; }
};

class Box : public TransformMI
{
    Model model;
    BoundingBox boundary;

    void UpdateRotation();
    void UpdateBoundary();

    public:

    Box(Vector3 position = {0, 0, 0}, float size = 1) : TransformMI(position, {0, 0, 0}, size)
    { model = LoadModelFromMesh(GenMeshCube(size, size, size)); UpdateBoundary(); }

    Model Model() { return model; }

    BoundingBox Boundary() { return boundary; }
    void Position(Vector3 newPos) { position = newPos; UpdateBoundary(); }
    void Rotation(Vector3 newRotation) { rotation = newRotation; UpdateRotation(); }
    void Size(float newSize) { size = newSize; UpdateBoundary(); }
    void Name(string name) { this->name = name; }

    Vector3 Position() { return position; }
    Vector3 Rotation() { return rotation; }
    float Size() { return size; }
    string Name() { return name; }
};

class CameraMI
{
    Camera3D camera;
    float pitch, yaw;
    Vector3 target;
    Vector3 direction;

    public:

    float speed = 2.0f;
    float sensitivity = 0.5f;

    CameraMI(Vector3 position = {0, 0, 0}, Vector3 target = {0, 0, 0}, Vector3 up = {0, 1, 0}) : camera({position, target, up, 90.0f, CAMERA_PERSPECTIVE}) {}

    Camera3D Camera() { return camera; }
    void Position(Vector3 newPos) { camera.position = newPos; }
    void Target(Vector3 newTarget) { camera.target = newTarget; }
    Vector3 Direction() { return (Vector3){camera.target.x - camera.position.x, camera.target.y - camera.position.y, camera.target.z - camera.position.z}; }
    void CameraFreeMove(float dT);
};

struct Scene
{
    Box **objects;
    int objectCount;
    CameraMI sceneCamera;

    Scene() : objects(nullptr), objectCount(0), sceneCamera() {}

    void AddObject(Box* newObject);
    void RemoveObject(int index);
    Box** Objects() { return objects; }
    int ObjectCount() { return objectCount; }   
    int FindIndex(string name);
};