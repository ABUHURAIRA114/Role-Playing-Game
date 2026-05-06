#include "HeaderAndProto.h"
#define gI GlobalInfo::instance

struct DialogueNodes
{
    string dialogue;
    map<int, pair<int, string>> choices;
};

class I_Dialogueable
{
    public:
    int dialogueIdx = 0;
    virtual void DialogueSetup() = 0;
    virtual void Dialogue() = 0;
};

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
    Collider **colliders;

    int objectCount, colliderCount;

    CameraMI sceneCamera;
    TransformMI* selected;
    float selectionSpeed = 2.0f;
    Ray selectionRay;
    RayCollision selectionRayCollision;
    Player* player;

    Box** enemySpawnPositions;
    int enemySpawnCount;
    Box** civilSpawnPositions;
    int civilSpawnCount;
    Box** merchantSpawnPositions;
    int merchantSpawnCount;
    Box** bossSpawnPositions;
    int bossSpawnCount;

    RectTransform **ui;
    int uiCount;
    int dialogueSpeakerTextIdx, dialogueTextIdx,
    choice1Idx;

    PossessedNPC **enemies;
    int enemyCount;

    NPC **npcs;
    int npcCount;

    bool dialogueVisible, endScreenVisible;
    float endScreenTime;
    int gameMode = MENU;

    map<int, AnimationData> billboards;

    Scene() : objects(nullptr), objectCount(0), ui(nullptr), uiCount(0), enemies(nullptr), npcs(nullptr), npcCount(0),
    enemyCount(0), merchantSpawnPositions(nullptr), merchantSpawnCount(0), civilSpawnPositions(nullptr), civilSpawnCount(0), 
    enemySpawnPositions(nullptr), enemySpawnCount(0), bossSpawnPositions(nullptr), bossSpawnCount(0), dialogueVisible(false), \
    sceneCamera(), endScreenTime(0)
    {}

    ~Scene();

    void AddSpawnObject(Box*, Box**&, int&, int);

    void AddObject(Box*, int);
    void AddCollider(Collider*, int);
    void AddUIObject(RectTransform*, int);
    void AddNPC(PossessedNPC*, int);
    void AddNPC(NPC*, int);
    void RemoveObject(int);
    void RemoveCollider(int);
    void RemoveUIObject(int);   
    int FindObjectIndex(string);
    int FindColliderIndex(string);
    int FindUIObjectIndex(string);

    int FindNPCIndex(int);
    int FindEnemyIndex(int);
    bool IsCharacter(int);

    void DuplicateSelected();
    void ObjectSpawn();

    void ShowBossDeathScreen(), ShowPlayerDeathScreen();
    void SelectionMove();
    void SpeedScroll();
    void DrawScene();
    void DrawSceneUI(int);
    void SelectObject(Ray);
    void UpdateNPCs();
};

class Item
{
    protected:
    string name;
    int price;

    public:
    Item(string name="Item", int price=1) : name(name), price(price) {}
    virtual ~Item() {}
    
    virtual Item* Clone() = 0;
    string Name() { return name; }
    int Price() { return price; }
};

class Potion : public Item
{
    int effectType;
    int magnitude;

    public:
    Potion(string name="Potion", int effectType=HEALTH_REGEN, int magnitude=50, int price=2) : Item(name, price), effectType(effectType), magnitude(magnitude) {}

    Item* Clone() { return new Potion(*this); }
    void ApplyEffect(Player& player);
};

struct GlobalInfo
{
    const int SCREEN_WIDTH = 1920;
    const int SCREEN_HEIGHT = 1080;
    const float MAX_SPEED = 20;
    const float WHEEL_SENSITIVITY = 1.0f;
    const float BAR_WIDTH = 300;
    const float INTERACT_RANGE = 1.5f;

    const int FREE_CAMERA_KEY = MOUSE_BUTTON_RIGHT;
    const int SELECTION_KEY = MOUSE_BUTTON_LEFT;
    const int FORWARD_KEY = KEY_W;
    const int BACKWARD_KEY = KEY_S;
    const int LEFT_KEY = KEY_A;
    const int RIGHT_KEY = KEY_D;
    const int JUMP_KEY = KEY_SPACE;
    const int SPRINT_KEY = KEY_LEFT_SHIFT;
    const int ATTACK_KEY = KEY_LEFT_CONTROL;
    const int ATTACK_KEY_MOUSE = MOUSE_BUTTON_LEFT;
    const int INV_1 = KEY_ONE;
    const int INV_2 = KEY_TWO;
    const int INTERACT_KEY = KEY_E;
    int mode = GAME;
    
    const string MODELS_FOLDER_PATH = "./assets/models"; 
    const string TEXTURES_FOLDER_PATH = "./assets/textures"; 
    const string SPRITES_FOLDER_PATH = "./assets/sprites"; 
    const string ENEMY_SPAWNER_NAME = "ENEMY_SPAWNER"; 
    const string CIVIL_SPAWNER_NAME = "CIVIL_SPAWNER"; 
    const string MERCHANT_SPAWNER_NAME = "MERCHANT_SPAWNER"; 
    const string BOSS_SPAWNER_NAME     = "BOSS_SPAWNER"; 
    
    const string SAVE_FOLDER_PATH = "./saves"; 
    const string DEFAULT_MODEL_NAME = "DEF_MOD";
    const string COLLIDER_MODEL_NAME = "COLLIDER";
    static GlobalInfo instance;
    
    map<string, Model> models;
    map<string, Texture2D> textures;
    map<string, Texture2D> sprites;
    Scene scene;
    float dT;
    int totalStatPoints = 10;
    
    /// @brief  size-->10
    const string CIVIL_GREETINGS[10] = {
        "I've got work to do, so make it quick.",           "Hmm? Oh, sorry. I was lost in thought.",               "Things have been strange lately. Best to keep your head down.",
        "I don't sleep well anymore. None of us do.",       "Just keep moving. This place isn't safe after dark.",  "Another day, another struggle.",
        "What do you want?",                                "You're blocking my light.",                            "Stay safe out there. These roads aren't what they used to be.",
        "I used to be an adventurer like you, then I took and arrow to the knee"
    };
        
    /// @brief  size-->5
    const string MERCHANT_GREETINGS[5] = {
        "Man welcomes you, and also your coin.", "Man has wares, if you have coin.", "The roads of Waloon can be treacherous. If you must walk them, be wary.", "The roads of Waloon lead to many dangers.", "It is a strange sight, the white flakes that sometimes fall from the sky."
    };

    /// @brief  size-->5
    const string MERCHANT_SELL[5] = {
        "Got a pretty full stock of potions and alchemy reagents.", "So you're interested in my potions and ingredients?", "Looking to protect yourself, or deal some damage?", "Oh, a bit of this and a bit of that.", "Some may call this junk. Me, I call them treasures."  
    };

    /// @brief  size-->5
    const string MERCHANT_BUY[5] = {
        "Coin's coin. What are you selling?", "Look, you need coin, I need merchandise. It's a beautiful relationship.", "I'll take a look, but don't expect me to get excited.", "Let's see what you've got. I'll give you a fair price... well, fair for me.", "I don't ask where things come from. Makes business easier."
    };

    /// @brief  size-->7
    const string POTION_NAMES[7] = {
        "Weak Health Potion", "Mid Health Potion", "Potent Health Potion",
        "Weak Stamina Potion", "Mid Stamina Potion", "Potent Stamina Potion",
        "Strength Potion"
    };

    Potion potions[7];

    void LoadAnim(Character* npc, int stateIdx, const string& stateName,const string& name, int frameCount);

    void Shade(), LoadThings(), 
    PlayerInfo(), Assets(), 
    LoadDialogueBox(), LoadEndScreenUI(), 
    LoadStatsUI(), SetPlayerStats(),
    LoadMenuUI(), LoadPauseUI(),
    LoadNPCs(), UnloadThings();
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

    Box(string name = "BoxObject", string assetName = gI.DEFAULT_MODEL_NAME) : TransformMI(name, {0,0,0}, {0,0,0}, 1),
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

class Collider : public TransformMI
{
    Model model;
    BoundingBox boundary;
    Vector3 scale;  // separate per-axis scale

    void UpdateBoundary();

    public:
    Collider(string name = "Collider", Vector3 position = {0,0,0}, float size = 1)
    : TransformMI(name, position, {0,0,0}, size), scale({size, size, size})
    {
        model = gI.models[gI.COLLIDER_MODEL_NAME];
        UpdateBoundary();
    }

    Model _Model() { return model; }
    BoundingBox Boundary() { return boundary; }

    void Position(Vector3 newPos) { position = newPos; UpdateBoundary(); }
    void Size(float newSize)      { size = newSize; UpdateBoundary(); }
    void Name(string name)        { this->name = name; }

    Vector3 Position() { return position; }
    Vector3 Rotation() { return rotation; }
    float   Size()     { return size; }
    string  Name()     { return name; }
    Vector3 Scale()    { return scale; }

    void ScaleX(float v) { scale.x = fmax(0.05f, v); UpdateBoundary(); }
    void ScaleY(float v) { scale.y = fmax(0.05f, v); UpdateBoundary(); }
    void ScaleZ(float v) { scale.z = fmax(0.05f, v); UpdateBoundary(); }
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
    string playerSaveFilePath;  // separate file for player data
    public:
    SaveSystem(string path) : saveFilePath(path), playerSaveFilePath(path + "_player.txt") {}

    void SaveScene(Scene& scene);
    void LoadScene(Scene& scene);

    void SavePlayer(Player& player);
    void LoadPlayer(Player& player);
};

class Inventory
{
    static const int MAX_ITEMS = 67;
    static const int MAX_SLOTS = 2;

    Item **items[MAX_SLOTS];
    int itemsCount[MAX_SLOTS] = {};
    
    public:

    Inventory() : items({nullptr, nullptr}), itemsCount({0,0}) {}
    Inventory(const Inventory& inv) 
    {
        for (int i = 0; i<MAX_SLOTS; i++)
        {
            itemsCount[i] = inv.itemsCount[i];

            items[i] = new Item*[itemsCount[i]];
            
            for (int j = 0; j<itemsCount[i]; j++)
                items[i][j] = inv.items[i][j]->Clone();
        }
    }

    ~Inventory();

    void AddItem(Item* item);
    void RemoveItem(int idx);
    int FindItem(string name);

    friend class Player;
    friend class Merchant;
    friend void SaveSystem::SavePlayer(Player& player);
    friend void SaveSystem::LoadPlayer(Player& player);

};

class Character : public TransformMI
{
    protected: 
    int id;

    Ray directionRay;
    RayCollision rayInfo;
    bool isGrounded;

    Texture2D anims[6][4] = {}; // 0: Down, 1: Left, 2: Right, 3: Up
    float frameTimer = 0.0f, frameWidth[7] = {};  
    int currentFrame = 0, currDir = 0;
    float hurtTimer;       // countdown between attacks

    float maxHealth, currHealth, speed, speedMultiplier;
    Vector3 target;
    float yVelocity;
    int state, damage, currDamage;
    
    public:
    Character(string name = "RJoe", float maxHealth=100, float speed=1, Vector3 position={0,0,0}, Vector3 target={0,0,0}, int damage = 10, int id = 0)
    : TransformMI(name, position, {0,0,0}, 2), maxHealth(maxHealth), currHealth(maxHealth), speed(speed), target(target), hurtTimer(0), id(id),
    yVelocity(0), state(0), damage(damage), currDamage(damage), speedMultiplier(1.0f) {}
    virtual ~Character() = 0;

    float MaxHealth() { return maxHealth; }
    float CurrHealth() { return currHealth; }
    float Speed() { return speed; }
    Vector3 Target() { return target; }
    bool IsGrounded() { return isGrounded; }
    int ID() { return id; }
    
    void CurrHealth(float value) { currHealth = value; }
    void Speed(float value) { this->speed = value; }
    void Target(Vector3 value) { target= value; }
    void ID(int value) { id = value; }

    virtual void TakeDamage(float amount) {}
    virtual void Update();
    virtual void DrawCharacter() {}
    virtual void Attack() {}

    friend void GlobalInfo::LoadAnim(Character* npc, int stateIdx, const string& stateName,const string& name, int frameCount);
};

class Player : public Character
{
    int charisma, strength, armour;
    float maxStamina, currStamina, staminaRegenRate, healthRegenRate;
    int coins;

    Ray groundRay;
    RayCollision groundInfo;

    Camera3D camera;
    float camDist;
    map<int, float> effects;
    Inventory inventory;

    int healthBarIdx, staminaBarIdx, coinsIdx;

    I_Dialogueable* interactNPC;
    
    int lastState;
    bool hasJumped;
    bool attackHitDealt, animEnd, isSprinting;
    float jT=0;

    public:
    Player(string name="Abu Huraira", float maxHealth=100, float maxStamina=100, float staminaRegenRate = 1, float healthRegenRate = 0.5f, float speed=4, int damage = 10, int strength = 0, int charisma = 0, int armour = 0, Vector3 position = {0,10,0}, Vector3 target = {0,0,0}) 
    : Character(name, maxHealth, speed, position, target, damage), hasJumped(false), camDist(2.5f), maxStamina(maxStamina), 
    currStamina(maxStamina), staminaRegenRate(staminaRegenRate), healthRegenRate(healthRegenRate), interactNPC(nullptr), attackHitDealt(true),
    charisma(charisma), strength(strength), armour(armour), coins(50), lastState(-1), animEnd(false)
    {
        camera.fovy = 95.0f;
        camera.position = {position.x, position.y+camDist, position.z+camDist};
        camera.target = position;
        camera.up = {0, 1, 0};
        camera.projection = CAMERA_PERSPECTIVE;
        groundRay.direction = {0, -1, 0};
    }

    float CurrStamina() { return currStamina; }
    Camera3D Camera() { return camera; }
    Inventory _Inventory() { return inventory; }
    int Coins() { return coins; }
    int CurrDamage() { return currDamage; }
    int Charisma() { return charisma; }
    
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
    void BuyItem(Item* value, int coins)
    {
        try
        { inventory.AddItem(value); }
        catch(const failed_execution& e) { throw; }
        catch(const out_of_space& e) { throw; }
        catch(const empty_collection& e) { throw; }
        catch(const out_of_range& e) { throw; }
        catch(...) { throw; }
        InvUI_Update();        
        this->coins -= coins;
    }
    void SellItem(string value, int coins)
    {
        int idx = inventory.FindItem(value);
        if (idx<0) return;

        try { this->coins += coins;  inventory.RemoveItem(idx); InvUI_Update(); }
        catch(const out_of_range& e) { throw; }
        catch(const empty_collection& e) { throw; }
    }

    void TakeDamage(float amount);

    void CalculateIsGrounded();
    void Update(), UpdateEffects(), Dialogue();
    int i1, i2; // inventory ui indices

    void InvUI_Update();
    void StateUpdate();
    void DrawCharacter();
    void Attack();

    friend void GlobalInfo::PlayerInfo();
    friend void GlobalInfo::SetPlayerStats();
    friend void GlobalInfo::UnloadThings();
    friend void Potion::ApplyEffect(Player&);
    friend void SaveSystem::SavePlayer(Player& player);
    friend void SaveSystem::LoadPlayer(Player& player);
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

class Civilian : public NPC, public I_Dialogueable
{
    DialogueNodes dialogues[1];


    public:
    Civilian(string name = "NPC", Vector3 position = {0, 0, 0}, float size = 2, int direction = DOWN)
    : NPC(name, 0, 0, position, FRIENDLY, 0) { 
        this->size = size; currDir = direction;
        dialogues[0].choices = {
            {0, {-1, "Is that right?"}},
            {1, {-1, "Hmmmmm..."}},
            {2, {-1, "Farewell"}}
        };
    }
    ~Civilian() {}

    void DrawCharacter();
    void DialogueSetup();
    void Dialogue();
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
                 float maxHealth = 30, float speed = 2, int damage = 8)
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

class Merchant : public NPC, public I_Dialogueable
{
    DialogueNodes dialogues[3];
    int buyPrices[2];
    int sellPrices[2];
    // animation (mirrors PossessedNPC)
    int  merchantLastState;
    bool merchantAnimEnd;
    float t;

    string greeting, sell, buy;
    vector<string> items = {};

    public:
    Merchant(string name = "Merchant-Man", Vector3 position = {0,0,0}, vector<string> items = {}, float size = 2, int direction = DOWN)
    : NPC(name, 0, 0, position, FRIENDLY, 0),
      merchantLastState(-1), merchantAnimEnd(false)
    {
        buyPrices[0]=0;
        buyPrices[1]=0;
        sellPrices[0]=0;
        sellPrices[1]=0;

        currDir = direction;
        this->size = size;
        dialogues[0] = 
        {   
            "I'd even buy one of your relatives, if you're looking to sell! Ha ha ha... That's a little joke.",
            {
                {0, {1 , "Buy" }},
                {1, {2 , "Sell"}},
                {2, {-1 , "Farewell"}}
            }
        };

        this->items.push_back(items.size()>0 ? items[0] : "Weak Health Potion");
        this->items.push_back(items.size()>1 ? items[1] : "Potent Stamina Potion");

        dialogues[1] = 
        {   
            "Some people call these junk, me? I call them treasure.",
            {
                {0, {1, "Item"}},
                {1, {1, "Number 1"}},
                {2, {0, "Back"}}
            }
        };

        dialogues[2] = 
        {   
            "Look, you need coin, I need merchandise. It's a beautiful relationship.",
            {
                {0, {2, "Slot 1 Item"}},
                {1, {2, "Slot 2 Item"}},
                {2, {0, "Back"}}
            }
        };
    }
    
    void DialogueSetup();
    void Dialogue();
    void Update() {}   // stays in place
    void DrawCharacter();
    void DrawInteractPrompt();

    friend void GlobalInfo::UnloadThings();
};

// ==================== Boss ====================
// Dialogueable boss that uses Warrior (player) sprites.
// Dialogue choices:
//   1. Fight him   -> turns hostile, acts like PossessedNPC
//   2. Join him    -> player gains a damage buff and dialogue closes
//   3. Leave the village -> closes dialogue, teleports player away

enum BossDialogueOutcome { BOSS_NONE = 0, BOSS_FIGHT, BOSS_JOIN, BOSS_LEAVE };

class Boss : public NPC, public I_Dialogueable
{
    // --- Dialogue ---
    DialogueNodes dialogues[1];  // single node with 3 choices

    // --- Combat (mirrors PossessedNPC) ---
    const float AGGRO_RANGE   = 10.0f;
    const float ATTACK_RANGE  = 1.4f;
    const float ATTACK_COOLDOWN = 1.2f;
    float attackTimer;
    Vector3 spawnPos;

    // --- Animation ---
    int  bossLastState;
    bool bossAnimEnd;

    // --- State ---
    BossDialogueOutcome outcome;   // result of the dialogue choice
    bool isHostile;                // true after "fight" chosen

    public:
    Boss(string name = "The Warlord", Vector3 position = {0, 0, 0},
         float maxHealth = 250, float speed = 3.0f, int damage = 20)
        : NPC(name, maxHealth, speed, position, FRIENDLY, damage),
          attackTimer(0), spawnPos(position),
          bossLastState(-1), bossAnimEnd(false),
          outcome(BOSS_NONE), isHostile(false)
    {
        // Build the single dialogue node
        dialogues[0].dialogue =
            "So... another wanderer stumbles into MY village. "
            "State your business, or I'll make it short for you.";
        dialogues[0].choices = {
            {0, {-1, "Fight you!"}},
            {1, {-1, "I'll join you."}},
            {2, {-1, "I'm leaving the village."}}
        };
    }

    // ---- I_Dialogueable ----
    void DialogueSetup();
    void Dialogue();

    // ---- Character overrides ----
    void TakeDamage(float amount);
    void Update();
    void DrawCharacter();
    void DrawInteractPrompt();

    BossDialogueOutcome Outcome() { return outcome; }
    bool IsHostile() { return isHostile; }
};