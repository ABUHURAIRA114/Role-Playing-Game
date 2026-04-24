#include "HeaderAndProto.h"

class TransformMI
{
    protected:

    string name;
    Vector3 position;
    Vector3 rotation;
    float size;

    public:

    TransformMI(string name = "Transform Object", Vector3 position = {0, 0, 0}, Vector3 rotation = {0, 0, 0}, float size = 1) : name(name), position(position), rotation(rotation), size(size) {}
    virtual ~TransformMI() {}

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

    Box(string name = "Box Object", Vector3 position = {0, 0, 0}, float size = 1, Model model = LoadModelFromMesh(GenMeshCube(1, 1, 1)), Texture albedo = LoadTextureFromImage(GenImageColor(100, 100, DARKPURPLE))) : TransformMI(name, position, {0, 0, 0}, size),
    model(model)
    { 
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = albedo;
        UpdateBoundary(); 
    }

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
    void SpeedScroll();
};

struct Scene
{
    Box **objects;
    int objectCount;
    CameraMI sceneCamera;
    Box* selected;
    float selectionSpeed = 2.0f;
    Ray selectionRay;
    RayCollision selectionRayCollision;
    map<string, Model> models;
    map<string, Texture2D> textures;

    RectTransform **ui;
    int uiCount;

    Scene() : objects(nullptr), objectCount(0), ui(nullptr), uiCount(0), sceneCamera() {}
    ~Scene();
    void AddObject(Box* newObject, int);
    void AddUIObject(RectTransform* newObject, int);
    void RemoveObject(int index);
    void RemoveUIObject(int index);   
    int FindObjectIndex(string name);
    int FindUIObjectIndex(string name);
    void ObjectSpawn();

    void SelectionMove(float dT);
    void SpeedScroll();
    void DrawScene();
    void DrawSceneUI();
    void SelectObject(Ray ray);
    void UnloadThings();
};

class RectTransform
{
    protected:
    
    string name;
    Rectangle rect;

    public: 
    RectTransform (string name = "Rect Object", Vector2 position = {0,0}, Vector2 dimension= {1, 1}) : name(name), rect({position.x, position.y, dimension.x, dimension.y}) {}
    virtual ~RectTransform() {}

    virtual string Name() { return name; }
    virtual Rectangle Rect() { return rect; }
    
    virtual void Name(string name) { this->name = name; }
    virtual void Rect(Rectangle rect) { this->rect = rect; }
};

class Text : public RectTransform
{
    string text;
    Color color;

    public:
    Text(string name = "Text Object", string text = "Text", Vector2 position = {0, 0}, Vector2 dimension = {0, 0}, Color color = BLACK) : RectTransform(name, position, dimension), text(text), color(color) {}

    Color _Color() { return color; }
    void _Color(Color newColor) { color = newColor; }   
    string _Text() { return text; }
    void _Text(string newText) { text = newText; }

    string Name() { return name; }
    Rectangle Rect() { return rect; }
    
    void Name(string name) { this->name = name; }
    void Rect(Rectangle rect) { this->rect = rect; }
};

class Button : public RectTransform
{
    Text text;
    Color backColor;

    public:
    Button(string name = "Button Object", string text = "Text", Vector2 position = {0, 0}, Vector2 dimension = {0, 0}, float textSize = 10, Color color = BLACK) : RectTransform(name, position, dimension), text(name, text, {10, 10}, {textSize, 0}), backColor(color) {}

    Color BackColor() { return backColor; }
    void BackColor(Color newColor) { backColor = newColor; }
    Text _Text() { return text; }
    void _Text(Text newText) { text = newText; }

    bool IsHovering();
    bool IsClicked();

    string Name() { return name; }
    Rectangle Rect() { return rect; }
    
    void Name(string name) { this->name = name; }
    void Rect(Rectangle rect) { this->rect = rect; }
};

enum GridType { HORIZONTAL, VERTICAL };
struct UIGrid
{
    RectTransform **elements;
    Vector2 position;
    float gridGap;
    int elementCount;

    UIGrid(Vector2 position = {0,0}, float gridGap=0) : position(position), gridGap(gridGap), elements(nullptr), elementCount(0) {}
    void AddElement(RectTransform* newObject);
    void RemoveElement(int index);   
    void OrderUI(int gridType);
    ~UIGrid();
};
































