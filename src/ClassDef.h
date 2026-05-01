#include "HeaderAndProto.h"
#define gI GlobalInfo::instance

struct AnimationData
{
    Camera3D camera;
    Texture2D frame;
    Rectangle source;
    Vector3 position;
    Vector2 size;
    Color color;
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

    PossessedNPC **npcs;
    int npcCount;

    map<string, AnimationData> billboards;

    Scene() : objects(nullptr), objectCount(0), ui(nullptr), uiCount(0), npcs(nullptr), npcCount(0), sceneCamera() {}
    ~Scene();
    void AddObject(Box* newObject, int);
    void AddUIObject(RectTransform* newObject, int);
    void AddNPC(PossessedNPC* npc);
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
    void UpdateNPCs();
};

class Item
{
    protected:
    string name;
    bool essential;

    public:
    Item(string name="Item", bool essential=false) : name(name), essential(essential) {}
    virtual ~Item() {}

    string Name() { return name; }
};

class Potion : public Item
{
    int effectType;
    int magnitude;

    public:
    Potion(string name="Potion", bool essesntial=false, int effectType=HEALTH_REGEN, int magnitude=50) : Item(name, essential), effectType(effectType), magnitude(magnitude) {}

    void ApplyEffect(Player& player);
};

struct GlobalInfo
{
    const int SCREEN_WIDTH = 1920;
    const int SCREEN_HEIGHT = 1080;
    const float MAX_SPEED = 20;
    const float WHEEL_SENSITIVITY = 1.0f;
    const float BAR_WIDTH = 300;

    const int FREE_CAMERA_KEY = MOUSE_BUTTON_RIGHT;
    const int SELECTION_KEY = MOUSE_BUTTON_LEFT;
    const int FORWARD_KEY = KEY_W;
    const int BACKWARD_KEY = KEY_S;
    const int LEFT_KEY = KEY_A;
    const int RIGHT_KEY = KEY_D;
    const int JUMP_KEY = KEY_SPACE;
    const int SPRINT_KEY = KEY_LEFT_SHIFT;
    const int ATTACK_KEY = KEY_LEFT_CONTROL;
    const int INV_1 = KEY_ONE;
    const int INV_2 = KEY_TWO;

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

    Potion HealthWeak, HealthMid, HealthPotent;
    Potion StaminaWeak, StaminaMid, StaminaPotent;
    Potion Strength;

    void LoadAnim(Character* npc, int stateIdx, const string& stateName,const string& name, int frameCount, const string& spritesPath);

    void Shade(), LoadThings(), PlayerInfo(), Assets(), LoadNPCs(), UnloadThings();
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

    Model _Model() { return model; }
 
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
};

class Banner : virtual public RectTransform
{
    protected:

    Text text;
    Color backColor;

    public:
    Banner(string name = "Banner Object", string text = "Text", Vector2 position = {0, 0}, Vector2 dimension = {0, 0}, float textSize = 10, Color color = BLACK) 
    : RectTransform(name, position, dimension), text(name, text, {10, 10}, {textSize, 0}), backColor(color) {}

    Color BackColor() { return backColor; }
    void BackColor(Color newColor) { backColor = newColor; }
    Text& _Text() { return text; }
    void _Text(Text newText) { text = newText; }
};

class Button : public Banner, virtual public RectTransform
{
    public:
    Button(string name = "Button Object", string text = "Text", Vector2 position = {0, 0}, Vector2 dimension = {0, 0}, float textSize = 10, Color color = BLACK) 
    : RectTransform(name, position, dimension), Banner(name, text, position, dimension, textSize, color) {}

    bool IsHovering();
    bool IsClicked();
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

class Inventory
{
    static const int MAX_ITEMS = 67;
    static const int MAX_SLOTS = 2;

    Item **items[MAX_SLOTS];
    int itemsCount[MAX_SLOTS] = {};
    
    public:

    Inventory() : items({nullptr, nullptr}), itemsCount({0,0}) {}
    ~Inventory();

    void AddItem(Item* item);
    void RemoveItem(int idx);
    int FindItem(string name);

    friend class Player;
};

class Character : public TransformMI
{
    protected: 

    Ray directionRay;
    RayCollision rayInfo;
    bool isGrounded;

    Texture2D anims[7][4] = {}; // 0: Down, 1: Left, 2: Right, 3: Up
    float frameTimer = 0.0f, frameWidth[7] = {};  
    int currentFrame = 0, currDir = 0;
    float hurtTimer;       // countdown between attacks

    float maxHealth, currHealth, speed, speedMultiplier;
    Vector3 target;
    float yVelocity;
    int state, damage, currDamage;
    
    public:
    Character(string name = "RJoe", float maxHealth=100, float speed=1, Vector3 position={0,0,0}, Vector3 target={0,0,0}, int damage = 10)
    : TransformMI(name, position, {0,0,0}, 1), maxHealth(maxHealth), currHealth(maxHealth), speed(speed), target(target), hurtTimer(0),
    yVelocity(0), state(0), damage(damage), currDamage(damage), speedMultiplier(1.0f) {}
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
    virtual void Attack() {}

    friend void GlobalInfo::LoadAnim(Character* npc, int stateIdx, const string& stateName,const string& name, int frameCount, const string& spritesPath);
};

class Player : public Character
{
    float maxStamina, currStamina, staminaRegenRate, healthRegenRate;
    Ray groundRay;
    RayCollision groundInfo;
    bool hasJumped;
    Camera3D camera;
    float camDist;
    map<int, float> effects;
    Inventory inventory;
    int healthBarIdx, staminaBarIdx;

    public:
    Player(string name="Abu Huraira", float maxHealth=100, float maxStamina=100, float staminaRegenRate = 1, float healthRegenRate = 0.5f, float speed=4, int damage = 10, Vector3 position = {0,10,0}, Vector3 target = {0,0,0}) 
    : Character(name, maxHealth, speed, position, target, damage), hasJumped(false), camDist(2.5f), maxStamina(maxStamina), currStamina(maxStamina), staminaRegenRate(staminaRegenRate), healthRegenRate(healthRegenRate)
    {
        camera.fovy = 95.0f;
        camera.position = {position.x, position.y+camDist, position.z+camDist};
        camera.target = position;
        camera.up = {0, 1, 0};
        camera.projection = CAMERA_PERSPECTIVE;
        groundRay.direction = {0, -1, 0};
    }

    float MaxHealth() { return maxHealth; }
    float CurrHealth() { return currHealth; }
    float CurrStamina() { return currStamina; }
    float Speed() { return speed; }
    Vector3 Target() { return target; }
    bool IsGrounded() { return isGrounded; }
    Camera3D Camera() { return camera; }
    
    void AddItem(Item* value) 
    {
        try
        { inventory.AddItem(value); }
        catch(const failed_execution& e) { throw; }
        catch(const out_of_space& e) { throw; }
        catch(const empty_collection& e) { throw; }
        catch(const out_of_range& e) { throw; }
        catch(...) { throw; }
        InvUI_Update();
    }

    int CurrDamage() { return currDamage; }

    void CurrHealth(float value) 
    {
        if (currHealth>value)
        {
            state = HURT;
            hurtTimer = 0.3f;
        }
        currHealth = value; 
    }
    void Speed(float value) { this->speed = value; }
    void Target(Vector3 value) { target= value; }
    void _Inventory(Inventory value) { inventory = value; }

    void CalculateIsGrounded();
    void Update(), UpdateEffects();
    int i1, i2; // inventory ui indices

    void InvUI_Update();
    void StateUpdate();
    void DrawCharacter();
    void Attack();

    friend void GlobalInfo::PlayerInfo();
    friend void GlobalInfo::UnloadThings();
    friend void Potion::ApplyEffect(Player&);
};

class NPC : public Character
{
    protected:
    int relation; // NPCRelation enum: ENEMY or FRIENDLY

    public:
    NPC(string name = "NPC", float maxHealth = 60, float speed = 2, Vector3 position = {0, 0, 0},
        int relation = FRIENDLY, int damage = 8)
        : Character(name, maxHealth, speed, position, position, damage), relation(relation) {}

    virtual ~NPC() = 0;

    int Relation() { return relation; }
    void Relation(int r) { relation = r; }

    virtual int State() { return state; }
};

class PossessedNPC : public NPC
{
    const float AGGRO_RANGE = 6;   // detect player
    const float ATTACK_RANGE = 1.2f;  // melee range
    const float ATTACK_COOLDOWN = 1.5f;

    float attackTimer;       // countdown between attacks
    Vector3 spawnPos;        // original idle position

    // per-NPC animation state helpers (mirrors Player)
    int  npcLastState;
    bool npcAnimEnd;

    public:
    PossessedNPC(string name = "Possessed", Vector3 position = {0, 0, 0},
                 float maxHealth = 60, float speed = 2, int damage = 8)
        : NPC(name, maxHealth, speed, position, ENEMY, damage),
          attackTimer(0), spawnPos(position),
          npcLastState(-1), npcAnimEnd(false) {}

    // Called externally when player's attack lands on this NPC
    void TakeDamage(float amount);

    void Update();
    void DrawCharacter();
    int State() { return state; }

    friend void GlobalInfo::UnloadThings();
};

class Merchant : public NPC
{
    public:
    Merchant(string name = "Merchant", Vector3 position = {0,0,0})
        : NPC(name, 100, 0, position, FRIENDLY, 0) {}
    virtual ~Merchant() {}
};





























