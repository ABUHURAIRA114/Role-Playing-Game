#include "HeaderAndProto.h"
#define gI GlobalInfo::instance

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
    void CameraFreeMove();
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
    Player* player;

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

    void SelectionMove();
    void SpeedScroll();
    void DrawScene();
    void DrawSceneUI();
    void SelectObject(Ray ray);
};

struct GlobalInfo
{
    const int SCREEN_WIDTH = 1920;
    const int SCREEN_HEIGHT = 1080;
    const int MAX_SPEED = 20;
    const int WHEEL_SENSITIVITY = 1.0f;

    const int FREE_CAMERA_KEY = MOUSE_BUTTON_RIGHT;
    const int SELECTION_KEY = MOUSE_BUTTON_LEFT;
    const int FORWARD_KEY = KEY_W;
    const int BACKWARD_KEY = KEY_S;
    const int LEFT_KEY = KEY_A;
    const int RIGHT_KEY = KEY_D;
    const int JUMP_KEY = KEY_SPACE;
    const int SPRINT_KEY = KEY_LEFT_SHIFT;

    const string MODELS_FOLDER_PATH = "./assets/models"; 
    const string TEXTURES_FOLDER_PATH = "./assets/textures"; 
    const string SPRITES_FOLDER_PATH = "./assets/sprites"; 

    const string SAVE_FOLDER_PATH = "./saves"; 
    const string DEFAULT_MODEL_NAME = "DEF_MOD";
    static GlobalInfo instance;
    
    map<string, Model> models;
    map<string, Texture2D> textures;
    Scene scene;
    float dT;

    void Shade();
    void LoadThings();
    void UnloadThings();
    private:
    GlobalInfo() {} 
};

class TransformMI
{
    protected:

    bool isActive;
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
    string assetName;
    BoundingBox boundary;
    void UpdateRotation();
    void UpdateBoundary();

    public:

    Box(string name = "BoxObject", Vector3 position = {0, 0, 0}, Vector3 rotation = {0,0,0}, float size = 1, string assetName = gI.DEFAULT_MODEL_NAME) : TransformMI(name, position, rotation, size),
    assetName(assetName)
    { 
        model = gI.models[assetName];
        try 
        {
            Texture2D tex = gI.textures.at(assetName);
            model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tex;
        }
        catch(out_of_range& e)
        {
            cout<<"Texture not found in map : "<<e.what()<<endl;;
        }
        
        UpdateBoundary(); 
        UpdateRotation();
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
    string AssetName() { return assetName; }
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

class SaveSystem
{
    string saveFilePath;
    public:
    SaveSystem(string path) : saveFilePath(path) {}

    void SaveScene(Scene& scene);
    void LoadScene(Scene& scene);
};

class Item
{

};

class Weapon : public Item
{

};

class Sword : public Weapon
{

};

class Bow : public Weapon
{

};

class Potion : public Item
{

};

class Inventory
{

};

class Character : public TransformMI
{
    protected: 

    bool isGrounded;

    Texture2D anims[4] = {}; // 0: Down, 1: Left, 2: Right, 3: Up
    float frameTimer = 0.0f, frameWidth;
    int currentFrame = 0,currDir = 0;

    float maxHealth, currHealth, 
    speed,
    speedMultiplier = 1.0f;
    Vector3 target;
    float yVelocity;

    public:
    Character(string name = "RJoe", float maxHealth=100, float speed=1, Vector3 position={0,0,0}, Vector3 target={0,0,0})
    : TransformMI(name, position, {0,0,0}, 1), maxHealth(maxHealth), currHealth(maxHealth), speed(speed), target(target)
    {

    }
    virtual ~Character() = 0;

    virtual float MaxHealth() { return maxHealth; }
    virtual float CurrHealth() { return currHealth; }
    virtual float Speed() { return speed; }
    virtual Vector3 Target() { return target; }
    virtual bool IsGrounded() { return isGrounded; }

    virtual void CurrHealth(float value) { currHealth = value; }
    virtual void Speed(float value) { this->speed = value; }
    virtual void Target(Vector3 value) { target= value; }

    virtual void Update();
    virtual void DrawCharacter() {}
};

class Player : public Character
{
    Ray groundRay;
    RayCollision groundInfo;
    bool hasJumped = false;
    Camera3D camera;
    float camDist = 4;

    public:
    Player(string name="Abu Huraira", float maxHealth=200, float speed=2, Vector3 position = {0,10,0}, Vector3 target = {0,0,0}) 
    : Character(name, maxHealth, speed, position, target) {
        camera.fovy = 95.0f;
        camera.position = {position.x, position.y+camDist, position.z+camDist};
        camera.target = position;
        camera.up = {0, 1, 0};
        camera.projection = CAMERA_PERSPECTIVE;
        groundRay.direction = {0, -1, 0};
    }

    float MaxHealth() { return maxHealth; }
    float CurrHealth() { return currHealth; }
    float Speed() { return speed; }
    Vector3 Target() { return target; }
    bool IsGrounded() { return isGrounded; }
    Camera3D Camera() { return camera; }
    void CurrHealth(float value) { currHealth = value; }
    void Speed(float value) { this->speed = value; }
    void Target(Vector3 value) { target= value; }

    void CalculateIsGrounded();
    void Update();
    void DrawCharacter();

    friend void GlobalInfo::LoadThings();
    friend void GlobalInfo::UnloadThings();

};

class NPC : public Character
{

};

class Enemy : public NPC
{

};

class Merchant : public NPC
{

};





























