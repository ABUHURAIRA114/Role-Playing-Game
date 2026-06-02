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

    // Purpose : Initialises dialogue state when the player opens conversation with this NPC
    // Returns : void
    virtual void DialogueSetup() = 0;

    // Purpose : Runs the active dialogue node each frame — updates UI and processes player input
    // Returns : void
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

    // Purpose : Constructs a perspective camera at the given position, target and up vector
    // Params  : position (Vector3) — initial camera world position
    //           target   (Vector3) — point the camera looks at
    //           up       (Vector3) — up direction (default world-up {0,1,0})
    CameraMI(Vector3 position = {0, 0, 0}, Vector3 target = {0, 0, 0}, Vector3 up = {0, 1, 0}) : camera({position, target, up, 90.0f, CAMERA_PERSPECTIVE}) {}

    // Purpose : Returns the underlying raylib Camera3D
    // Returns : Camera3D — copy of the internal camera
    Camera3D Camera() { return camera; }

    // Purpose : Sets the camera's world position
    // Params  : newPos (Vector3) — new position
    // Returns : void
    void Position(Vector3 newPos) { camera.position = newPos; }

    // Purpose : Sets the camera's look-at target
    // Params  : newTarget (Vector3) — new target point
    // Returns : void
    void Target(Vector3 newTarget) { camera.target = newTarget; }

    // Purpose : Returns the vector from the camera position to its target
    // Returns : Vector3 — direction vector (not normalised)
    Vector3 Direction() { return (Vector3){camera.target.x - camera.position.x, camera.target.y - camera.position.y, camera.target.z - camera.position.z}; }

    // Purpose : Updates camera orientation and position based on mouse delta and WASD input
    //           Used in the editor's free-look mode
    // Returns : void
    void CameraFreeMove();

    // Purpose : Adjusts camera movement speed via the mouse scroll wheel
    // Returns : void
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
    int enemyCount, enemiesRemaining;

    NPC **npcs;
    int npcCount;

    bool dialogueVisible, endScreenVisible, tutorialVisible;
    float endScreenTime;
    int gameMode = MENU;

    map<int, AnimationData> billboards;

    // Purpose : Default constructor — initialises all arrays to nullptr and counts to 0
    Scene() : objects(nullptr), objectCount(0), ui(nullptr), uiCount(0), enemies(nullptr), npcs(nullptr), npcCount(0),
    enemyCount(0), merchantSpawnPositions(nullptr), merchantSpawnCount(0), civilSpawnPositions(nullptr), civilSpawnCount(0), 
    enemySpawnPositions(nullptr), enemySpawnCount(0), bossSpawnPositions(nullptr), bossSpawnCount(0), dialogueVisible(false), 
    sceneCamera(), endScreenTime(0), player(nullptr), enemiesRemaining(0), tutorialVisible(false)
    {}

    // Purpose : Destructor — frees all heap-allocated objects, UI elements, NPCs and spawn arrays
    ~Scene();

    // Purpose : Appends a Box pointer to a given spawn-position array (enemy/civil/merchant/boss)
    // Params  : newObject  (Box*)     — the spawner box to register
    //           spawnArray (Box**&)   — reference to the target spawn array
    //           spawnCount (int&)     — reference to that array's count
    //           k          (int)      — unused legacy parameter (default 0)
    // Returns : void
    void AddSpawnObject(Box*, Box**&, int&, int);

    // Purpose : Adds a Box to the scene's object array; auto-resolves name collisions recursively
    // Params  : Box* — object to add
    //           int  — recursion depth used for name suffix (pass 0 externally)
    // Returns : void
    void AddObject(Box*, int);

    // Purpose : Adds a Collider to the scene's collider array; auto-resolves name collisions
    // Params  : Collider* — collider to add
    //           int       — recursion depth for name suffix (pass 0 externally)
    // Returns : void
    void AddCollider(Collider*, int);

    // Purpose : Adds a UI element to the scene's UI array; auto-resolves name collisions
    // Params  : RectTransform* — UI element to add
    //           int            — recursion depth for name suffix (pass 0 externally)
    // Returns : void
    void AddUIObject(RectTransform*, int);

    // Purpose : Adds a PossessedNPC (enemy) to the enemy array with a unique ID
    // Params  : PossessedNPC* — enemy to add
    //           int           — starting ID offset (incremented on collision)
    // Returns : void
    void AddNPC(PossessedNPC*, int);

    // Purpose : Adds a friendly NPC to the NPC array with a unique ID
    // Params  : NPC* — NPC to add
    //           int  — starting ID offset (incremented on collision)
    // Returns : void
    void AddNPC(NPC*, int);

    // Purpose : Removes the object at the given index, also removes it from any spawn array, then deletes it
    // Params  : index (int) — index into the objects array
    // Returns : void
    void RemoveObject(int);

    // Purpose : Removes and deletes the collider at the given index
    // Params  : index (int) — index into the colliders array
    // Returns : void
    void RemoveCollider(int);

    // Purpose : Removes the UI element at the given index (does not delete it)
    // Params  : index (int) — index into the UI array
    // Returns : void
    void RemoveUIObject(int);

    // Purpose : Finds the index of a scene object by name
    // Params  : name (string) — name to search for
    // Returns : int — index if found, -1 if not found
    int FindObjectIndex(string);

    // Purpose : Finds the index of a collider by name
    // Params  : name (string) — name to search for
    // Returns : int — index if found, -1 if not found
    int FindColliderIndex(string);

    // Purpose : Finds the index of a UI element by name
    // Params  : name (string) — name to search for
    // Returns : int — index if found, -1 if not found
    int FindUIObjectIndex(string);

    // Purpose : Finds the index of a friendly NPC by unique ID
    // Params  : id (int) — NPC ID to search for
    // Returns : int — index if found, -1 if not found
    int FindNPCIndex(int);

    // Purpose : Finds the index of an enemy (PossessedNPC) by unique ID
    // Params  : id (int) — enemy ID to search for
    // Returns : int — index if found, -1 if not found
    int FindEnemyIndex(int);

    // Purpose : Checks whether an ID belongs to any NPC or enemy in the scene
    // Params  : id (int) — ID to check
    // Returns : bool — true if the ID is already in use
    bool IsCharacter(int);

    // Purpose : Duplicates the currently selected object or collider and adds the copy to the scene
    // Returns : void
    void DuplicateSelected();

    // Purpose : Checks all editor UI buttons and spawns the corresponding object or collider into the scene
    // Returns : void
    void ObjectSpawn();

    // Purpose : Populates the end-screen UI with the boss-defeat narrative and makes it visible
    // Returns : void
    void ShowBossDeathScreen();

    // Purpose : Populates the end-screen UI with the player-death narrative and makes it visible
    // Returns : void
    void ShowPlayerDeathScreen();

    // Purpose : Moves/rotates/scales the selected object using keyboard input (editor mode)
    // Returns : void
    void SelectionMove();

    // Purpose : Adjusts the editor selection movement speed via the mouse scroll wheel
    // Returns : void
    void SpeedScroll();

    // Purpose : Draws all scene objects, colliders (editor only), characters and sorted billboards
    // Returns : void
    void DrawScene();

    // Purpose : Draws all UI elements appropriate for the current mode and game state,
    //           then draws merchant/boss interact prompts
    // Params  : mode (int) — current mode (EDITOR or GAME)
    // Returns : void
    void DrawSceneUI(int);

    // Purpose : Casts a ray into the scene and selects the closest hit object or collider
    // Params  : ray (Ray) — the pick ray (typically from screen centre or mouse)
    // Returns : void
    void SelectObject(Ray);

    // Purpose : Calls Update() on all enemies and Boss NPCs each frame
    // Returns : void
    void UpdateNPCs();

    // Purpose : Sets the active state of each object based on certain flags
    void ActiveSet();
};

class Item
{
    protected:
    string name;
    int price;

    public:
    // Purpose : Base constructor for any in-game item
    // Params  : name  (string) — display name of the item
    //           price (int)    — base coin value
    Item(string name="Item", int price=1) : name(name), price(price) {}
    virtual ~Item() {}
    
    // Purpose : Creates a heap-allocated deep copy of this item (pure virtual)
    // Returns : Item* — pointer to the cloned item
    virtual Item* Clone() = 0;

    // Purpose : Returns the item's display name
    // Returns : string
    string Name() { return name; }

    // Purpose : Returns the item's base price
    // Returns : int
    int Price() { return price; }
};

class Potion : public Item
{
    int effectType;
    int magnitude;

    public:
    // Purpose : Constructs a potion with a given effect type, magnitude and price
    // Params  : name       (string) — display name
    //           effectType (int)    — PotionEffect enum value (HEALTH_REGEN, STAMINA_REGEN, STRENGTH_BOOST)
    //           magnitude  (int)    — strength of the effect
    //           price      (int)    — coin value
    Potion(string name="Potion", int effectType=HEALTH_REGEN, int magnitude=50, int price=2) : Item(name, price), effectType(effectType), magnitude(magnitude) {}

    // Purpose : Returns a heap-allocated copy of this Potion
    // Returns : Item* — pointer to the cloned potion
    Item* Clone() { return new Potion(*this); }

    // Purpose : Applies the potion's effect to the given player (adds/overwrites the effect entry)
    // Params  : player (Player&) — the player to affect
    // Returns : void
    void ApplyEffect(Player& player);
};

struct GlobalInfo
{
    const int SCREEN_WIDTH          = 1920;
    const int SCREEN_HEIGHT         = 1080;
    const float MAX_SPEED           = 20;
    const float WHEEL_SENSITIVITY   = 1.0f;
    const float BAR_WIDTH           = 300;
    const float INTERACT_RANGE      = 1.5f;
    const float VELOCITY_CONST      = 5;
    const float KNOCKBACK_CONST     = 7;

    const int FREE_CAMERA_KEY           = MOUSE_BUTTON_RIGHT;
    const int SELECTION_KEY             = MOUSE_BUTTON_LEFT;
    const int FORWARD_KEY               = KEY_W;
    const int BACKWARD_KEY              = KEY_S;
    const int LEFT_KEY                  = KEY_A;
    const int RIGHT_KEY                 = KEY_D;
    const int SECONDARY_FORWARD_KEY     = KEY_UP;
    const int SECONDARY_BACKWARD_KEY    = KEY_DOWN;
    const int SECONDARY_LEFT_KEY        = KEY_LEFT;
    const int SECONDARY_RIGHT_KEY       = KEY_RIGHT;
    const int JUMP_KEY                  = KEY_SPACE;
    const int SPRINT_KEY                = KEY_LEFT_SHIFT;
    const int SECONDARY_SPRINT_KEY      = KEY_RIGHT_SHIFT;
    const int ATTACK_KEY                = KEY_LEFT_CONTROL;
    const int SECONDARY_ATTACK_KEY      = KEY_RIGHT_CONTROL;
    const int ATTACK_KEY_MOUSE          = MOUSE_BUTTON_LEFT;
    const int INV_1                     = KEY_ONE;
    const int INV_2                     = KEY_TWO;
    const int INTERACT_KEY              = KEY_E;
    
    int mode = GAME;
    
    const string MODELS_FOLDER_PATH     = "./assets/models"; 
    const string TEXTURES_FOLDER_PATH   = "./assets/textures"; 
    const string SPRITES_FOLDER_PATH    = "./assets/sprites"; 
    const string MUSIC_FOLDER_PATH      = "./assets/audio"; 
    const string ENEMY_SPAWNER_NAME     = "ENEMY_SPAWNER"; 
    const string CIVIL_SPAWNER_NAME     = "CIVIL_SPAWNER"; 
    const string MERCHANT_SPAWNER_NAME  = "MERCHANT_SPAWNER"; 
    const string BOSS_SPAWNER_NAME      = "BOSS_SPAWNER"; 
    
    const string SAVE_FOLDER_PATH       = "./saves"; 
    const string DEFAULT_MODEL_NAME     = "DEF_MOD";
    const string COLLIDER_MODEL_NAME    = "COLLIDER";
    static GlobalInfo instance;

    bool isStartingFromScratch = true;
    
    map<string, Model> models;
    map<string, Texture2D> textures;
    map<string, Texture2D> sprites;
    vector<Music> bgMusics;
    Scene scene;
    float dT;
    int totalStatPoints = 10;
    int currentMusic    = 0;
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

    // Purpose : Loads all four directional sprite frames for one animation state into a character
    // Params  : c          (Character*) — target character to populate
    //           stateIdx   (int)        — AnimState enum index (IDLE, MOVING, etc.)
    //           stateName  (string&)    — name suffix of the animation (e.g. "Idle", "Walk")
    //           name       (string&)    — sprite sheet name prefix (e.g. "Warrior", "Possesed")
    //           frameCount (int)        — number of frames in the animation strip
    // Returns : void
    void LoadAnim(Character* npc, int stateIdx, const string& stateName,const string& name, int frameCount);

    // Purpose : Placeholder for future post-process shading
    // Returns : void
    void Shade();

    // Purpose : Calls all sub-loaders to initialise assets, UI and audio at startup
    // Returns : void
    void LoadThings();

    // Purpose : Creates and places the player object, loads its animation sprites
    // Returns : void
    void PlayerInfo();

    // Purpose : Builds the in-game HUD: health/stamina bars, inventory slots, coin display
    // Returns : void
    void PlayerUI();

    // Purpose : Loads all models, textures, sprites and music from the assets folders;
    //           also initialises all potion definitions
    // Returns : void
    void Assets();

    // Purpose : Builds the dialogue box and choice panel UI elements
    // Returns : void
    void LoadDialogueBox();

    // Purpose : Builds the end-screen overlay UI (title, narrative text, hint)
    // Returns : void
    void LoadEndScreenUI();

    // Purpose : Builds the character-stats / attribute-selection UI shown at game start
    // Returns : void
    void LoadStatsUI();

    // Purpose : Handles plus/minus button input for charisma/strength/armour and the start button
    // Returns : void
    void SetPlayerStats();

    // Purpose : Builds the main-menu UI (background, title, play/tutorial/quit buttons, tutorial panel)
    // Returns : void
    void LoadMenuUI();

    // Purpose : Builds the pause-menu UI (overlay, panel, resume/menu/quit buttons)
    // Returns : void
    void LoadPauseUI();

    // Purpose : Spawns all NPCs (enemies, merchants, civilians, boss) from their spawn-point boxes;
    //           loads from save file if not starting from scratch
    // Returns : void
    void LoadNPCs();

    // Purpose : Destroys all NPC and enemy objects and clears the billboard map
    // Returns : void
    void UnloadNPCs();

    // Purpose : Unloads all models, textures, sprites and music streams from GPU/memory
    // Returns : void
    void UnloadThings();

    // Purpose : Advances to the next background music track when the current one reaches ~95%,
    //           or when Ctrl+P is pressed; calls UpdateMusicStream every frame
    // Returns : void
    void MusicLoop();

    private:
    // Purpose : Private constructor — enforces singleton usage via GlobalInfo::instance
    GlobalInfo() {} 
};

class TransformMI
{
    protected:

    string name;
    Vector3 position;
    Vector3 rotation;
    float size;
    bool isActive;

    public:

    // Purpose : Constructs a transform with name, position, rotation and uniform scale
    // Params  : name     (string)  — identifier
    //           position (Vector3) — world position
    //           rotation (Vector3) — Euler rotation in radians
    //           size     (float)   — uniform scale factor
    TransformMI(string name = "Transform Object", Vector3 position = {0, 0, 0}, Vector3 rotation = {0, 0, 0}, float size = 1) : name(name), position(position), rotation(rotation), size(size) {}
    virtual ~TransformMI() {}

    // Purpose : Returns the object's name
    // Returns : string
    string Name() { return name; }

    // Purpose : Returns the object's world position
    // Returns : Vector3
    virtual Vector3 Position() { return position; }

    // Purpose : Returns the object's rotation (Euler, radians)
    // Returns : Vector3
    virtual Vector3 Rotation() { return rotation; }

    // Purpose : Returns the object's uniform scale
    // Returns : float
    virtual float Size() { return size; }

    // Purpose : Returns whether the object is active
    bool IsActive() { return isActive; }

    // Purpose : Sets the object's world position
    // Params  : newPos (Vector3)
    // Returns : void
    virtual void Position(Vector3 newPos) { position = newPos; }

    // Purpose : Sets the object's name
    // Params  : name (string)
    // Returns : void
    void Name(string name) { this->name = name; }

    // Purpose : Sets the object's rotation
    // Params  : newRotation (Vector3) — Euler angles in radians
    // Returns : void
    virtual void Rotation(Vector3 newRotation) { rotation = newRotation; }

    // Purpose : Sets the object's uniform scale
    // Params  : newSize (float)
    // Returns : void
    virtual void Size(float newSize) { size = newSize; }

    // Purpose : Set the object's active state
    void IsActive(bool value) { isActive = value; }
};

class Box : public TransformMI
{
    Model model;
    string assetName;
    BoundingBox boundary;

    // Purpose : Applies the current Euler rotation to the model transform matrix using quaternions
    // Returns : void
    void UpdateRotation();

    // Purpose : Recalculates the axis-aligned bounding box from the mesh, scaled and offset by position
    // Returns : void
    void UpdateBoundary();

    public:

    // Purpose : Full constructor — loads the named model and texture, applies rotation and boundary
    // Params  : name      (string)  — object identifier
    //           position  (Vector3) — world position
    //           rotation  (Vector3) — Euler rotation in radians
    //           size      (float)   — uniform scale
    //           assetName (string)  — key into GlobalInfo::models and ::textures maps
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

    // Purpose : Short constructor — places the box at the origin with default rotation and scale
    // Params  : name      (string) — object identifier
    //           assetName (string) — key into GlobalInfo::models and ::textures maps
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

    // Purpose : Returns the internal raylib Model
    // Returns : Model
    Model _Model() { return model; }
 
    // Purpose : Returns the current axis-aligned bounding box
    // Returns : BoundingBox
    BoundingBox Boundary() { return boundary; }

    // Purpose : Sets world position and updates the bounding box
    // Params  : newPos (Vector3)
    // Returns : void
    void Position(Vector3 newPos) { position = newPos; UpdateBoundary(); }

    // Purpose : Sets rotation and rebuilds the model transform matrix
    // Params  : newRotation (Vector3) — Euler angles in radians
    // Returns : void
    void Rotation(Vector3 newRotation) { rotation = newRotation; UpdateRotation(); }

    // Purpose : Sets uniform scale and updates the bounding box
    // Params  : newSize (float)
    // Returns : void
    void Size(float newSize) { size = newSize; UpdateBoundary(); }

    // Purpose : Returns the object's world position
    // Returns : Vector3
    Vector3 Position() { return position; }

    // Purpose : Returns the object's rotation (Euler, radians)
    // Returns : Vector3
    Vector3 Rotation() { return rotation; }

    // Purpose : Returns the object's uniform scale
    // Returns : float
    float Size() { return size; }

    // Purpose : Returns the asset name key used to load this box's model and texture
    // Returns : string
    string AssetName() { return assetName; }
};

class Collider : public TransformMI
{
    Model model;
    BoundingBox boundary;
    Vector3 scale;  // separate per-axis scale

    // Purpose : Recalculates the bounding box using per-axis scale values and current position
    // Returns : void
    void UpdateBoundary();

    public:
    // Purpose : Constructs a collider using the COLLIDER model asset with uniform initial scale
    // Params  : name     (string)  — identifier
    //           position (Vector3) — world position
    //           size     (float)   — initial uniform scale applied to all axes
    Collider(string name = "Collider", Vector3 position = {0,0,0}, float size = 1)
    : TransformMI(name, position, {0,0,0}, size), scale({size, size, size})
    {
        model = gI.models[gI.COLLIDER_MODEL_NAME];
        UpdateBoundary();
    }

    // Purpose : Returns the internal raylib Model
    // Returns : Model
    Model _Model() { return model; }

    // Purpose : Returns the current axis-aligned bounding box
    // Returns : BoundingBox
    BoundingBox Boundary() { return boundary; }

    // Purpose : Sets world position and updates the bounding box
    // Params  : newPos (Vector3)
    // Returns : void
    void Position(Vector3 newPos) { position = newPos; UpdateBoundary(); }

    // Purpose : Returns the per-axis scale vector
    // Returns : Vector3
    Vector3 Scale()    { return scale; }

       // Purpose : Returns the object's world position
    // Returns : Vector3
    Vector3 Position() { return position; }

    // Purpose : Sets the X scale component, clamped to a minimum of 0.05, then updates the boundary
    // Params  : v (float) — new X scale
    // Returns : void
    void ScaleX(float v) { scale.x = fmax(0.05f, v); UpdateBoundary(); }

    // Purpose : Sets the Y scale component, clamped to a minimum of 0.05, then updates the boundary
    // Params  : v (float) — new Y scale
    // Returns : void
    void ScaleY(float v) { scale.y = fmax(0.05f, v); UpdateBoundary(); }

    // Purpose : Sets the Z scale component, clamped to a minimum of 0.05, then updates the boundary
    // Params  : v (float) — new Z scale
    // Returns : void
    void ScaleZ(float v) { scale.z = fmax(0.05f, v); UpdateBoundary(); }
};

class RectTransform
{
    protected:
    
    string name;
    Rectangle rect;

    public: 
    // Purpose : Constructs a 2D rect transform with a name, screen position and dimensions
    // Params  : name      (string)  — identifier
    //           position  (Vector2) — top-left screen position
    //           dimension (Vector2) — width and height
    RectTransform (string name = "Rect Object", Vector2 position = {0,0}, Vector2 dimension= {1, 1}) : name(name), rect({position.x, position.y, dimension.x, dimension.y}) {}
    virtual ~RectTransform() {}

    // Purpose : Returns the element's name
    // Returns : string
    virtual string Name() { return name; }

    // Purpose : Returns the element's screen rectangle
    // Returns : Rectangle
    virtual Rectangle Rect() { return rect; }
    
    // Purpose : Sets the element's name
    // Params  : name (string)
    // Returns : void
    virtual void Name(string name) { this->name = name; }

    // Purpose : Sets the element's screen rectangle
    // Params  : rect (Rectangle)
    // Returns : void
    virtual void Rect(Rectangle rect) { this->rect = rect; }
};

class Text : public RectTransform
{
    string text;
    Color color;

    public:
    // Purpose : Constructs a text UI element with a string and colour
    // Params  : name      (string)  — identifier
    //           text      (string)  — display text
    //           position  (Vector2) — screen position
    //           dimension (Vector2) — width (used as font size) and height
    //           color     (Color)   — text colour
    Text(string name = "Text Object", string text = "Text", Vector2 position = {0, 0}, Vector2 dimension = {0, 0}, Color color = BLACK) : RectTransform(name, position, dimension), text(text), color(color) {}

    // Purpose : Returns the text colour
    // Returns : Color
    Color _Color() { return color; }

    // Purpose : Sets the text colour
    // Params  : newColor (Color)
    // Returns : void
    void _Color(Color newColor) { color = newColor; }

    // Purpose : Returns the display string
    // Returns : string
    string _Text() { return text; }

    // Purpose : Sets the display string
    // Params  : newText (string)
    // Returns : void
    void _Text(string newText) { text = newText; }
};

class Banner : virtual public RectTransform
{
    protected:

    Text text;
    Color backColor;

    public:
    // Purpose : Constructs a banner (coloured rectangle with an embedded Text label)
    // Params  : name      (string)  — identifier
    //           text      (string)  — label string
    //           position  (Vector2) — screen position
    //           dimension (Vector2) — banner size
    //           textSize  (float)   — font size of the label
    //           color     (Color)   — background fill colour
    Banner(string name = "Banner Object", string text = "Text", Vector2 position = {0, 0}, Vector2 dimension = {0, 0}, float textSize = 10, Color color = BLACK) 
    : RectTransform(name, position, dimension), text(name, text, {10, 10}, {textSize, 0}), backColor(color) {}

    // Purpose : Returns the background fill colour
    // Returns : Color
    Color BackColor() { return backColor; }

    // Purpose : Sets the background fill colour
    // Params  : newColor (Color)
    // Returns : void
    void BackColor(Color newColor) { backColor = newColor; }

    // Purpose : Returns a reference to the embedded Text object
    // Returns : Text&
    Text& _Text() { return text; }

    // Purpose : Replaces the embedded Text object
    // Params  : newText (Text)
    // Returns : void
    void _Text(Text newText) { text = newText; }
};

class Button : public Banner, virtual public RectTransform
{
    public:
    // Purpose : Constructs a clickable button (Banner with hover/click detection)
    // Params  : name      (string)  — identifier
    //           text      (string)  — label string
    //           position  (Vector2) — screen position
    //           dimension (Vector2) — button size
    //           textSize  (float)   — font size of the label
    //           color     (Color)   — background fill colour
    Button(string name = "Button Object", string text = "Text", Vector2 position = {0, 0}, Vector2 dimension = {0, 0}, float textSize = 10, Color color = BLACK) 
    : RectTransform(name, position, dimension), Banner(name, text, position, dimension, textSize, color) {}

    // Purpose : Returns true if the mouse cursor is currently within the button's rectangle
    // Returns : bool
    bool IsHovering();

    // Purpose : Returns true if the button is hovered and the selection mouse button was just pressed
    // Returns : bool
    bool IsClicked();
};

struct UIGrid
{
    RectTransform **elements;
    Vector2 position;
    float gridGap;
    int elementCount;

    // Purpose : Constructs an empty UI grid at the given screen position with the given gap between elements
    // Params  : position (Vector2) — anchor position
    //           gridGap  (float)   — pixel spacing between elements
    UIGrid(Vector2 position = {0,0}, float gridGap=0) : position(position), gridGap(gridGap), elements(nullptr), elementCount(0) {}

    // Purpose : Appends a UI element pointer to the grid's element array
    // Params  : newObject (RectTransform*) — element to add
    // Returns : void
    void AddElement(RectTransform* newObject);

    // Purpose : Removes the element at the given index from the grid (does not delete the element)
    // Params  : index (int) — index into the elements array
    // Returns : void
    void RemoveElement(int index);

    // Purpose : Repositions all elements either vertically or horizontally based on gridGap
    // Params  : gridType (int) — GridType enum: VERTICAL or HORIZONTAL
    // Returns : void
    void OrderUI(int gridType);

    // Purpose : Destructor — frees the elements pointer array (does not delete the elements themselves)
    ~UIGrid();
};

class SaveSystem
{
    string saveFilePath;

    public:
    // Purpose : Constructs a SaveSystem that reads and writes to the given folder path
    // Params  : path (string) — path to the save folder
    SaveSystem(string path) : saveFilePath(path){}

    // Purpose : Writes all scene objects and colliders to scene.txt
    // Params  : scene (Scene&) — the scene to serialise
    // Returns : void
    void SaveScene(Scene& scene);

    // Purpose : Reads scene.txt and reconstructs all objects, colliders and spawn markers
    // Params  : scene (Scene&) — the scene to populate
    // Returns : void
    void LoadScene(Scene& scene);

    // Purpose : Writes the player's full state (stats, inventory, effects, position) to player.txt
    // Params  : player (Player&) — the player to serialise
    // Returns : void
    void SavePlayer(Player& player);

    // Purpose : Reads player.txt and restores the player's state; redirects to STATS screen if save is "END"
    // Returns : void
    void LoadPlayer();

    // Purpose : Writes all living and dead enemy states to enemies.txt, then unloads them
    // Returns : void
    void SaveEnemies();

    // Purpose : Reads enemies.txt and spawns PossessedNPCs with saved positions, states and health
    // Returns : void
    void LoadEnemies();

    // Purpose : Deletes all enemy objects and resets the scene's enemy arrays to nullptr
    // Returns : void
    void UnloadEnemies();
};

class Inventory
{
    static const int MAX_ITEMS = 67;
    static const int MAX_SLOTS = 2;

    Item **items[MAX_SLOTS];
    int itemsCount[MAX_SLOTS] = {};
    
    public:

    // Purpose : Default constructor — initialises both item slot arrays to nullptr with count 0
    Inventory() : items({nullptr, nullptr}), itemsCount({0,0}) {}

    // Purpose : Deep-copy constructor — clones every item in every slot
    // Params  : inv (const Inventory&) — inventory to copy from
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

    // Purpose : Destructor — deletes all items and the slot arrays
    ~Inventory();

    // Purpose : Adds an item to an existing matching slot or the first empty slot
    //           Throws out_of_space if the matching slot is full, or failed_execution if no slot is free
    // Params  : item (Item*) — heap-allocated item to add (ownership transferred)
    // Returns : void
    void AddItem(Item* item);

    // Purpose : Removes and deletes the last item in the given slot
    //           Throws out_of_range if idx is invalid, empty_collection if the slot is empty
    // Params  : idx (int) — slot index (0 or 1)
    // Returns : void
    void RemoveItem(int idx);

    // Purpose : Searches all slots for an item matching the given name
    // Params  : name (string) — item name to search for
    // Returns : int — slot index if found, -1 if not found
    int FindItem(string name);

    friend class Player;
    friend class Merchant;
    friend void SaveSystem::SavePlayer(Player& player);
    friend void SaveSystem::LoadPlayer();

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
    // Purpose : Base character constructor — sets health, speed, damage, position and ID
    // Params  : name      (string)  — display name
    //           maxHealth (float)   — maximum health points
    //           speed     (float)   — base movement speed
    //           position  (Vector3) — initial world position
    //           target    (Vector3) — initial movement target
    //           damage    (int)     — base attack damage
    //           id        (int)     — unique character ID
    Character(string name = "RJoe", float maxHealth=100, float speed=1, Vector3 position={0,0,0}, Vector3 target={0,0,0}, int damage = 10, int id = 0)
    : TransformMI(name, position, {0,0,0}, 2), maxHealth(maxHealth), currHealth(maxHealth), speed(speed), target(target), hurtTimer(0), id(id),
    yVelocity(0), state(0), damage(damage), currDamage(damage), speedMultiplier(1.0f) {}
    virtual ~Character() = 0;

    // Purpose : Returns the character's maximum health
    // Returns : float
    float MaxHealth() { return maxHealth; }

    // Purpose : Returns the character's current health
    // Returns : float
    float CurrHealth() { return currHealth; }

    // Purpose : Returns the character's base movement speed
    // Returns : float
    float Speed() { return speed; }

    // Purpose : Returns the character's current movement target
    // Returns : Vector3
    Vector3 Target() { return target; }

    // Purpose : Returns whether the character is currently on the ground
    // Returns : bool
    bool IsGrounded() { return isGrounded; }

    // Purpose : Returns the character's unique ID
    // Returns : int
    int ID() { return id; }
    
    // Purpose : Sets the character's current health directly
    // Params  : value (float)
    // Returns : void
    void CurrHealth(float value) { currHealth = value; }

    // Purpose : Sets the character's movement speed
    // Params  : value (float)
    // Returns : void
    void Speed(float value) { this->speed = value; }

    // Purpose : Sets the character's movement target
    // Params  : value (Vector3)
    // Returns : void
    void Target(Vector3 value) { target= value; }

    // Purpose : Sets the character's unique ID
    // Params  : value (int)
    // Returns : void
    void ID(int value) { id = value; }

    // Purpose : Base damage handler (no-op); override in subclasses for full behaviour
    // Params  : amount (float) — incoming raw damage
    // Returns : void
    virtual void TakeDamage(float amount) {}

    // Purpose : Moves the character toward its target and updates the facing direction
    // Returns : void
    virtual void Update();

    // Purpose : Submits this character's billboard data for sorted rendering (pure-virtual intent)
    // Returns : void
    virtual void DrawCharacter() {}

    // Purpose : Handles attack input and hit detection (no-op base; overridden in Player)
    // Returns : void
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
    bool hasJumped,
    attackHitDealt,
    animEnd, isSprinting;
    float jT=0;
    int i1, i2; // inventory ui indices

    Vector3 knockbackVelocity;
    float   knockbackVal;

    public:
    // Purpose : Constructs the player with all RPG stats and caches UI element indices
    // Params  : name             (string)  — character name
    //           maxHealth        (float)   — max HP
    //           maxStamina       (float)   — max stamina
    //           staminaRegenRate (float)   — stamina regen per second
    //           healthRegenRate  (float)   — passive health regen per second
    //           speed            (float)   — base move speed
    //           damage           (int)     — base attack damage
    //           strength         (int)     — strength stat (boosts attack)
    //           charisma         (int)     — charisma stat (merchant discounts)
    //           armour           (int)     — armour stat (damage reduction)
    //           position         (Vector3) — initial world position
    //           target           (Vector3) — initial look target
    Player(string name="Abu Huraira", float maxHealth=100, float maxStamina=100, float staminaRegenRate = 4, float healthRegenRate = 0.5f, float speed=4, int damage = 10, int strength = 1, int charisma = 1, int armour = 1, Vector3 position = {-1.1f,0,9.25f}, Vector3 target = {0,0,0}) 
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
        healthBarIdx  = gI.scene.FindUIObjectIndex("PLA_HEALTH_BAR");
        staminaBarIdx = gI.scene.FindUIObjectIndex("PLA_STAMINA_BAR");
        i1            = gI.scene.FindUIObjectIndex("PLA_PL_INV_T");
        i2            = gI.scene.FindUIObjectIndex("PLA_PL_INV_T_1");
        coinsIdx      = gI.scene.FindUIObjectIndex("PLA_PL_COINS");
    }

    // Purpose : Returns the player's current stamina
    // Returns : float
    float CurrStamina() { return currStamina; }

    // Purpose : Returns the player's third-person camera
    // Returns : Camera3D
    Camera3D Camera() { return camera; }

    // Purpose : Returns a copy of the player's inventory (use with caution — copies all item pointers)
    // Returns : Inventory
    Inventory _Inventory() { return inventory; }

    // Purpose : Returns the player's current coin count
    // Returns : int
    int Coins() { return coins; }

    // Purpose : Returns the player's current (possibly boosted) damage value
    // Returns : int
    int CurrDamage() { return currDamage; }

    // Purpose : Returns the player's charisma stat
    // Returns : int
    int Charisma() { return charisma; }
    
    // Purpose : Adds an item to the inventory and refreshes the inventory UI
    //           Re-throws any inventory exceptions to the caller
    // Params  : value (Item*) — heap-allocated item to add
    // Returns : void
    void AddItem(Item* value) 
    {
        try
        { inventory.AddItem(value); }
        catch(...) { throw; }

        InvUI_Update();
    }

    // Purpose : Buys an item from a merchant — adds it to inventory and deducts coins
    //           Re-throws any inventory exceptions to the caller
    // Params  : value (Item*) — item to add
    //           coins (int)   — price to deduct
    // Returns : void
    void BuyItem(Item* value, int coins)
    {
        try
        { inventory.AddItem(value); this->coins -= coins; InvUI_Update(); }
        catch(...) { throw; }
    }

    // Purpose : Sells an item by name — removes it from inventory and adds coins
    //           Re-throws any inventory exceptions to the caller
    // Params  : value (string) — name of the item to sell
    //           coins (int)    — price to add
    // Returns : void
    void SellItem(string value, int coins)
    {
        int idx = inventory.FindItem(value);
        if (idx<0) return;

        try { this->coins += coins;  inventory.RemoveItem(idx); InvUI_Update(); }
        catch(...) { throw; }
    }

    // Purpose : Applies armour-reduced damage, triggers knockback, and shows death screen if health reaches 0
    // Params  : amount    (float)   — incoming raw damage
    //           attackPos (Vector3) — world position of the attacker (used for knockback direction)
    // Returns : void
    void TakeDamage(float, Vector3);

    // Purpose : Sets isGrounded based on whether the player's Y position is at or below ground level
    // Returns : void
    void CalculateIsGrounded();

    // Purpose : Handles player movement, jumping, sprinting, collision and gravity each frame
    // Returns : void
    void Update();

    // Purpose : Applies and ticks active potion effects; handles stamina potion and strength boost logic
    // Returns : void
    void UpdateEffects();

    // Purpose : Handles opening/closing dialogue with nearby NPCs and runs the active dialogue each frame
    // Returns : void
    void Dialogue();

    // Purpose : Refreshes the inventory slot text and coin counter in the HUD
    // Returns : void
    void InvUI_Update();

    // Purpose : Scales the health and stamina bars to reflect current values; drains/regens stamina
    // Returns : void
    void StateUpdate();

    // Purpose : Submits the player's current animation frame as a billboard for sorted rendering
    // Returns : void
    void DrawCharacter();

    // Purpose : Handles attack input, triggers the ATTACKING state, and deals damage to enemies and the Boss
    // Returns : void
    void Attack();

    friend class GlobalInfo;
    friend void Potion::ApplyEffect(Player&);
    friend void SaveSystem::SavePlayer(Player& player);
    friend void SaveSystem::LoadPlayer();
};

class NPC : public Character
{
    protected:
    int relation; // NPCRelation enum: ENEMY or FRIENDLY

    public:
    // Purpose : Base NPC constructor — sets health, speed, position, relation and damage
    // Params  : name     (string)  — display name
    //           maxHealth(float)   — max HP
    //           speed    (float)   — movement speed
    //           position (Vector3) — world position
    //           relation (int)     — NPCRelation enum (ENEMY or FRIENDLY)
    //           damage   (int)     — attack damage
    NPC(string name = "NPC", float maxHealth = 60, float speed = 2, Vector3 position = {0, 0, 0},
        int relation = FRIENDLY, int damage = 8)
        : Character(name, maxHealth, speed, position, position, damage), relation(relation) {}

    virtual ~NPC() = 0;

    // Purpose : Returns the NPC's current relation (ENEMY or FRIENDLY)
    // Returns : int
    int Relation() { return relation; }

    // Purpose : Sets the NPC's relation value
    // Params  : r (int) — NPCRelation enum value
    // Returns : void
    void Relation(int r) { relation = r; }

    // Purpose : Returns the NPC's current animation state
    // Returns : int
    virtual int State() { return state; }
};

class Civilian : public NPC, public I_Dialogueable
{
    DialogueNodes dialogues[1];

    public:
    // Purpose : Constructs a stationary, friendly civilian with preset dialogue choices
    // Params  : name      (string)  — display name
    //           position  (Vector3) — world position
    //           size      (float)   — sprite display size
    //           direction (int)     — initial facing direction (Direction enum)
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

    // Purpose : Submits the civilian's idle billboard frame for sorted rendering
    // Returns : void
    void DrawCharacter();

    // Purpose : Faces the player and picks a random greeting line to start dialogue
    // Returns : void
    void DialogueSetup();

    // Purpose : Updates dialogue UI text and processes player key input to advance choices
    // Returns : void
    void Dialogue();
};

class PossessedNPC : public NPC
{
    const float AGGRO_RANGE = 6;   // detect player
    const float ATTACK_RANGE = 1.2f;  // melee range
    const float ATTACK_COOLDOWN = 1.2f;

    float attackTimer;       // countdown between attacks
    Vector3 spawnPos;        // original idle position

    // per-NPC animation state helpers (mirrors Player)
    int  npcLastState;
    bool npcAnimEnd;

    Vector3 knockbackVelocity;
    float   knockbackVal;

    public:
    // Purpose : Constructs an enemy NPC with optional saved state for loading from file
    // Params  : name           (string)  — identifier
    //           position       (Vector3) — spawn and starting world position
    //           maxHealth      (float)   — maximum HP
    //           speed          (float)   — movement speed
    //           damage         (int)     — attack damage
    //           initState      (int)     — starting AnimState (default IDLE)
    //           initCurrHealth (float)   — starting HP (-1 means use maxHealth)
    PossessedNPC(string name = "Possessed", Vector3 position = {0, 0, 0},
                 float maxHealth = 30, float speed = 2, int damage = 8,
                 int initState = IDLE, float initCurrHealth = -1)
        : NPC(name, maxHealth, speed, position, ENEMY, damage),
          attackTimer(0), spawnPos(position),
          npcLastState(-1), npcAnimEnd(false)
    {
        state = initState;
        currHealth = (initCurrHealth < 0) ? maxHealth : initCurrHealth;
    }

    // Purpose : Applies damage, knockback and transitions to the HURT state
    // Params  : amount    (float)   — raw damage to apply
    //           attackPos (Vector3) — world position of the attacker (knockback direction source)
    // Returns : void
    void TakeDamage(float, Vector3);

    // Purpose : Runs the enemy AI each frame: HURT cooldown, aggro/chase/attack/return-to-spawn logic
    // Returns : void
    void Update();

    // Purpose : Advances the animation frame and submits the enemy billboard for sorted rendering
    // Returns : void
    void DrawCharacter();

    // Purpose : Returns the enemy's current animation state
    // Returns : int
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
    // Purpose : Constructs a stationary merchant with two items to sell, calculated from player charisma
    // Params  : name      (string)         — display name
    //           position  (Vector3)        — world position
    //           items     (vector<string>) — list of up to 2 potion names to sell (defaults if empty)
    //           size      (float)          — sprite display size
    //           direction (int)            — initial facing direction (Direction enum)
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
    
    // Purpose : Faces the player, picks random dialogue lines, and calculates buy prices from player charisma
    // Returns : void
    void DialogueSetup();

    // Purpose : Handles the full buy/sell dialogue tree and executes transactions each frame
    // Returns : void
    void Dialogue();

    // Purpose : No-op — merchants stay in place
    // Returns : void
    void Update() {}   // stays in place

    // Purpose : Submits the merchant's idle billboard frame for sorted rendering
    // Returns : void
    void DrawCharacter();

    // Purpose : Draws a "[E] Talk" world-space prompt above the merchant when the player is in range
    // Returns : void
    void DrawInteractPrompt();

    friend void GlobalInfo::UnloadThings();
};

class Boss : public NPC, public I_Dialogueable
{
    // --- Dialogue ---
    DialogueNodes dialogues[1];  // single node with 3 choices

    // --- Combat (mirrors PossessedNPC) ---
    const float AGGRO_RANGE   = 10.0f;
    const float ATTACK_RANGE  = 1.4f;
    const float ATTACK_COOLDOWN = 0.7f;
    float attackTimer;
    Vector3 spawnPos;

    // --- Animation ---
    int  bossLastState;
    bool bossAnimEnd;

    Vector3 knockbackVelocity;
    float   knockbackVal;

    // --- State ---
    BossDialogueOutcome outcome;   // result of the dialogue choice
    bool isHostile;                // true after "fight" chosen

    public:
    // Purpose : Constructs the Boss with dialogue, combat and animation state initialised
    // Params  : name      (string)  — display name (default "The Warlord")
    //           position  (Vector3) — world position
    //           maxHealth (float)   — max HP
    //           speed     (float)   — movement speed
    //           damage    (int)     — attack damage per hit
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

    // Purpose : Faces the player and resets dialogue state to the opening node
    // Returns : void
    void DialogueSetup();

    // Purpose : Updates dialogue UI and processes the player's choice (fight/join/leave),
    //           triggering the appropriate end-screen outcome
    // Returns : void
    void Dialogue();

    // Purpose : Applies damage and knockback; triggers death screen if health reaches 0
    // Params  : amount    (float)   — raw incoming damage
    //           attackPos (Vector3) — attacker world position (knockback direction source)
    // Returns : void
    void TakeDamage(float, Vector3);

    // Purpose : Runs Boss AI each frame: HURT cooldown, idle when friendly, chase/attack when hostile
    // Returns : void
    void Update();

    // Purpose : Advances the Boss animation frame and submits the billboard (red tint when hostile)
    // Returns : void
    void DrawCharacter();

    // Purpose : Draws a world-space interact prompt above the Boss when the player is in range and not hostile
    // Returns : void
    void DrawInteractPrompt();

    // Purpose : Returns the result of the Boss dialogue choice
    // Returns : BossDialogueOutcome
    BossDialogueOutcome Outcome() { return outcome; }

    // Purpose : Returns whether the Boss is currently in combat mode
    // Returns : bool
    bool IsHostile() { return isHostile; }
};