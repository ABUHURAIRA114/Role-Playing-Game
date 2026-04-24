#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <map>

using namespace std;

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
const string MODELS_FOLDER_PATH = "./assets/models"; 
const string TEXTURES_FOLDER_PATH = "./assets/textures"; 

class TransformMI;
class Box;
class Camera_;
class Scene;
class Text;
class RectTransform;
class Button;
class UIGrid;