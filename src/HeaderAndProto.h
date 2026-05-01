#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <map>
#include <fstream>
#include <algorithm>
#include "CustomExceptions.h"
#include <vector>

using namespace std;

enum Direction { DOWN=0, LEFT=1, RIGHT=2, UP=3 };
enum AnimState { IDLE=0, MOVING=1, JUMPING=2, ATTACKING=3, DIE=4, HURT=5 };
enum PotionEffect { HEALTH_REGEN, STRENGTH_BOOST, STAMINA_REGEN };
enum NPCRelation { ENEMY, FRIENDLY };
enum GridType { HORIZONTAL, VERTICAL };
enum Mode { EDITOR, GAME };

class TransformMI;
class Box;
class Camera_;
struct Scene;
class Text;
class RectTransform;
class Banner;
class Button;
struct UIGrid;
struct GlobalInfo;
class SaveSystem;

class Item;
class Potion;
class Inventory;
class Character;
class Player;
class NPC;
class PossessedNPC;
class Merchant; 
