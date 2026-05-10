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

struct DialogueNodes;
struct AnimationData;
class TransformMI;
class Box;
class Collider;
class Camera_;
struct Scene;
class Text;
class RectTransform;
class Banner;
class Button;
struct UIGrid;
struct GlobalInfo;
class SaveSystem;

class I_Dialogueable;

class Item;
class Potion;
class Inventory;
class Character;
class Player;
class NPC;
class Civilian;
class PossessedNPC;
class Merchant;
class MerchantNPC;
class Boss;
