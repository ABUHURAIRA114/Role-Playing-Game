#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <map>
#include <fstream>
#include <algorithm>
#include "CustomExceptions.h"
#include <vector>
#include <cfloat>
using namespace std;

enum Direction { DOWN=0, LEFT=1, RIGHT=2, UP=3 };
enum AnimState { IDLE=0, MOVING=1, JUMPING=2, ATTACKING=3, DIE=4, HURT=5 };
enum PotionType {
    WEAK_HEALTH_POTION  , MID_HEALTH_POTION , POTENT_HEALTH_POTION,
    WEAK_STAMINA_POTION , MID_STAMINA_POTION, POTENT_STAMINA_POTION,
    STRENGTH_POTION
};
enum PotionEffect { HEALTH_REGEN, STRENGTH_BOOST, STAMINA_REGEN };
enum NPCRelation { ENEMY, FRIENDLY };
enum GridType { HORIZONTAL, VERTICAL };
enum Mode { EDITOR, GAME };
enum GameMode { MENU, STATS, PAUSE, PLAY };
enum BossDialogueOutcome { BOSS_NONE = 0, BOSS_FIGHT, BOSS_JOIN, BOSS_LEAVE };

// Stores Dialogue Info including choices and where each choice leads
struct DialogueNodes; 
// Stores Animation Data of the character billboards including frame, camera etc that would be rendered in scene
struct AnimationData;
// Transform object, used as parent, handles positioning
class TransformMI;
// Standard gameobject, has a model to be rendered and collider(only for selection in editor)
class Box;
// Standard collider object, used for player collision checks
class Collider;
// Free camera for editor 
class Camera_;
// class to handle the player, map, characters, npcs and rendering 
struct Scene;
// simple ui for plain text
class Text;
// handles ui positioning and sizing
class RectTransform;
// uiobject with simple panel and text
class Banner;
// extension of banner that detects camera hover and clicks
class Button;
// grid to align any given UI objects automatically
struct UIGrid;
// encapsulated all the info regarding the game
struct GlobalInfo;
// savesystem to handle player, map and enemies saving and loading
class SaveSystem;

// an interface (abstract class) for dialogueable NPCs
class I_Dialogueable;

// parent class for things that can be stored in inventory
class Item;
// potions extending items, contatins effects and effectiveness that player can use
class Potion;
// stores all the items player has
class Inventory;
// parent class for all the npcs, and player
class Character;
// the player class, contains player attributes and functions
class Player;
// parent class of npcs, contains the generic stuff all npcs would have and allows to store them in one array
class NPC;
// a normal regular civilian class
class Civilian;
// hostile enemies 
class PossessedNPC;
// merchant npcs that player can trade items with
class Merchant;
// the final boss
class Boss;
