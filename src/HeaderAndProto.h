#include <raylib.h>
#include <raymath.h>
#include <iostream>

using namespace std;

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int MAX_SPEED = 20;

const int FREE_CAMERA_KEY = KEY_LEFT_CONTROL;
const int SELECTION_KEY = KEY_LEFT_ALT;

class TransformMI;
class Box;
class Camera_;
class Scene;