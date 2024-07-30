#include "raylib.h"
#include <vector>
#include <iostream>
#include <array>
#include <unordered_map>
#include <algorithm>
#include <random>

// Constants
static const int screenWidth = 800;
static const int screenHeight = 450;
static const int cellSize = 32;
static const int gridWidth = screenWidth / cellSize;
static const int gridHeight = screenHeight / cellSize;
static const int FRAME_SPEED = 8;
static const int TOTAL_FRAMES = 4;
static const int CHARACTER_SIZE = cellSize * 2;
static const float MOVE_TIME = 0.2f;  // Time to move from one cell to another
static const int MAX_SCORE = (gridWidth * gridHeight) - 1;

struct TileCoord {
    int x, y;
};

// Tile types
enum class Tile {
    Grass,
    Weed1,
    Weed2,
    SmallWeed1,
    SmallWeed2,
    SmallWeed3,
    SmallWeed4,
    Patch1,
    Patch2,
    Patch3,
    Patch4,
    Patch5,
    Patch6
};

enum class FoodType {
    Apple,
    Melon,
    Pear,
    Pear2,
    Strawberry,
    Grape,
    Blueberry
};

enum class ObjectType {
    Mushroom1, Mushroom2, Mushroom3, Mushroom4, Mushroom5, Mushroom6,
    Weed1, Weed2, Weed3, Weed4,
    Flower1, Flower2, Flower3, Flower4, Flower5, Flower6, Flower7, Flower8, Flower9, Flower10,
    Flower11, Flower12, Flower13, Flower14, Flower15, Flower16, Flower17, Flower18, Flower19, Flower20
};

static const std::unordered_map<Tile, TileCoord> tileCoords = {
        {Tile::Grass, {1, 1}},
        {Tile::Weed1, {0, 5}},
        {Tile::Weed2, {0, 6}},
        {Tile::SmallWeed1, {1, 5}},
        {Tile::SmallWeed2, {1, 6}},
        {Tile::SmallWeed3, {2, 5}},
        {Tile::SmallWeed4, {2, 6}},
        {Tile::Patch1, {3, 5}},
        {Tile::Patch2, {3, 6}},
        {Tile::Patch3, {4, 5}},
        {Tile::Patch4, {4, 6}},
        {Tile::Patch5, {5, 5}},
        {Tile::Patch6, {5, 6}},
};

static const std::unordered_map<FoodType, TileCoord> foodCoords = {
        {FoodType::Apple, {0, 0}},
        {FoodType::Melon, {1, 0}},
        {FoodType::Pear, {2, 0}},
        {FoodType::Pear2, {3, 0}},
        {FoodType::Strawberry, {0, 1}},
        {FoodType::Grape, {1, 1}},
        {FoodType::Blueberry, {2, 1}}
};

static const std::unordered_map<ObjectType, TileCoord> objectCoords = {
        {ObjectType::Mushroom1, {0, 0}},
        {ObjectType::Mushroom2, {1, 0}},
        {ObjectType::Mushroom3, {2, 0}},
        {ObjectType::Mushroom4, {3, 0}},
        {ObjectType::Mushroom5, {4, 0}},
        {ObjectType::Mushroom6, {5, 0}},
        {ObjectType::Weed1, {0, 2}},
        {ObjectType::Weed2, {1, 2}},
        {ObjectType::Weed3, {2, 2}},
        {ObjectType::Weed4, {3, 2}},
        {ObjectType::Flower1, {0, 3}},
        {ObjectType::Flower2, {1, 3}},
        {ObjectType::Flower3, {2, 3}},
        {ObjectType::Flower4, {4, 3}},
        {ObjectType::Flower5, {5, 3}},
        {ObjectType::Flower6, {6, 3}},
        {ObjectType::Flower7, {8, 3}},
        {ObjectType::Flower8, {9, 3}},
        {ObjectType::Flower9, {10, 3}},
        {ObjectType::Flower10, {11, 3}},
        {ObjectType::Flower11, {0, 4}},
        {ObjectType::Flower12, {1, 4}},
        {ObjectType::Flower13, {2, 4}},
        {ObjectType::Flower14, {4, 4}},
        {ObjectType::Flower15, {5, 4}},
        {ObjectType::Flower16, {6, 4}},
        {ObjectType::Flower17, {8, 4}},
        {ObjectType::Flower18, {9, 4}},
        {ObjectType::Flower19, {10, 4}},
        {ObjectType::Flower20, {11, 4}}
};

// 2D array to represent the background
using Background = std::array<std::array<Tile, gridWidth>, gridHeight>;

struct CollectedFruit {
    FoodType type;
    Vector2 position;
};

// Structures
struct Character {
    Vector2 position;
    Vector2 targetPosition;
    int direction;  // 0: down, 1: up, 2: left, 3: right
    Rectangle frameRec;
    int currentFrame;
    int framesCounter;
    std::vector<CollectedFruit> collectedFruits;
    float moveTimer;
    float interpolationTime;
    std::vector<Vector2> fruitTargetPositions;
    std::vector<Vector2> previousPositions;
    int nextDirection;
};

struct Object {
    Vector2 position;
    bool isFood;
    union {
        FoodType foodType;
        ObjectType objectType;
    };
};

struct Food {
    Vector2 position;
    FoodType type;
};

// Static Global Variables
static Character character;
static Food food;
static bool gameOver = false;
static int score = 0;
static int maxScore = 0;
static Texture2D tilesetTexture;
static Texture2D foodTexture;
static Texture2D objectTexture;
static Texture2D characterTexture;
static Background background;
static std::vector<Object> objects;
static RenderTexture2D backgroundAndObjectsTexture;

// Function Declarations
static void InitGame();
static void UpdateGame();
static void DrawGame();
static void UpdateDrawFrame();
static void GenerateBackground();
static void GenerateObjects();
static void GenerateFood();
static void DrawTile(Tile tile, int x, int y);
static void DrawObject(const Object& obj);
static bool CheckSelfCollision(const Character& character);
static float Lerp(float a, float b, float t);

int main() {
    InitWindow(screenWidth, screenHeight, "Snake gameeeeee");

    // Load textures
    tilesetTexture = LoadTexture("assets/textures/grass_tileset.png");
    foodTexture = LoadTexture("assets/textures/food.png");
    objectTexture = LoadTexture("assets/textures/mush_flow_rocks.png");
    characterTexture = LoadTexture("assets/textures/character_spritesheet.png");

    if (tilesetTexture.id == 0 || foodTexture.id == 0 || objectTexture.id == 0 || characterTexture.id == 0) {
        std::cout << "Failed to load one or more textures!" << std::endl;
        return -1;
    }

    InitGame();

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        UpdateDrawFrame();
    }

    UnloadTexture(tilesetTexture);
    UnloadTexture(foodTexture);
    UnloadTexture(objectTexture);
    UnloadTexture(characterTexture);
    UnloadRenderTexture(backgroundAndObjectsTexture);
    CloseWindow();
    return 0;
}

static void InitGame() {
    // Character initialization
    character.position = {static_cast<float>(gridWidth / 2), static_cast<float>(gridHeight / 2)};
    character.targetPosition = character.position;
    character.direction = 0;  // Start facing down
    character.nextDirection = 0;  // Initialize nextDirection
    character.frameRec = {0.0f, 0.0f, static_cast<float>(characterTexture.width / TOTAL_FRAMES), static_cast<float>(characterTexture.height / 4)};
    character.currentFrame = 0;
    character.framesCounter = 0;
    character.collectedFruits.clear();
    character.fruitTargetPositions.clear();
    character.moveTimer = 0.0f;
    character.interpolationTime = 0.0f;

    // Generate game elements
    GenerateBackground();
    GenerateObjects();
    GenerateFood();

    // Create and render background and objects texture
    if (backgroundAndObjectsTexture.id != 0) {
        UnloadRenderTexture(backgroundAndObjectsTexture);
    }
    backgroundAndObjectsTexture = LoadRenderTexture(screenWidth, screenHeight);

    BeginTextureMode(backgroundAndObjectsTexture);
        ClearBackground(RAYWHITE);
        // Draw background
        for (int y = 0; y < gridHeight; y++) {
            for (int x = 0; x < gridWidth; x++) {
                DrawTile(background[y][x], x, y);
            }
        }
        // Draw objects
        for (const auto& obj : objects) {
            DrawObject(obj);
        }
    EndTextureMode();

    // Reset game state
    gameOver = false;
    score = 0;
}


static void GenerateBackground() {
    for (int y = 0; y < gridHeight; y++) {
        for (int x = 0; x < gridWidth; x++) {
            int random = GetRandomValue(0, 100);
            if (random < 80) {
                background[y][x] = Tile::Grass;  // Main background
            } else if (random < 85) {
                background[y][x] = static_cast<Tile>(GetRandomValue(static_cast<int>(Tile::Weed1), static_cast<int>(Tile::Weed2)));
            } else if (random < 90) {
                background[y][x] = static_cast<Tile>(GetRandomValue(static_cast<int>(Tile::SmallWeed1), static_cast<int>(Tile::SmallWeed4)));
            } else {
                background[y][x] = static_cast<Tile>(GetRandomValue(static_cast<int>(Tile::Patch1), static_cast<int>(Tile::Patch6)));
            }
        }
    }
}

static void GenerateObjects() {
    objects.clear();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> objDist(15, 25);  // Number of objects
    std::uniform_int_distribution<> typeDist(0, static_cast<int>(ObjectType::Flower20));
    std::uniform_int_distribution<> xDist(0, gridWidth - 1);
    std::uniform_int_distribution<> yDist(0, gridHeight - 1);

    int objectCount = objDist(gen);
    for (int i = 0; i < objectCount; i++) {
        Object obj;
        obj.position = {static_cast<float>(xDist(gen)), static_cast<float>(yDist(gen))};
        obj.isFood = false;
        obj.objectType = static_cast<ObjectType>(typeDist(gen));
        objects.push_back(obj);
    }
}

static void GenerateFood() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> posDist(0, std::min(gridWidth, gridHeight) - 1);
    std::uniform_int_distribution<> typeDist(0, static_cast<int>(FoodType::Blueberry));

    bool validPosition;
    do {
        validPosition = true;
        food.position = {static_cast<float>(posDist(gen)), static_cast<float>(posDist(gen))};

        // Check if food spawns on the character
        if (food.position.x == character.position.x && food.position.y == character.position.y) {
            validPosition = false;
            continue;
        }

        // Check if food spawns on any part of the snake
        for (const auto& fruit : character.collectedFruits) {
            if (food.position.x == fruit.position.x && food.position.y == fruit.position.y) {
                validPosition = false;
                break;
            }
        }
    } while (!validPosition);

    food.type = static_cast<FoodType>(typeDist(gen));
}

static void DrawTile(Tile tile, int x, int y) {
    TileCoord coord = tileCoords.at(tile);
    Rectangle sourceRec = {
            static_cast<float>(coord.x * 16),
            static_cast<float>(coord.y * 16),
            16,
            16
    };
    Rectangle destRec = {
            static_cast<float>(x * cellSize),
            static_cast<float>(y * cellSize),
            static_cast<float>(cellSize),
            static_cast<float>(cellSize)
    };
    DrawTexturePro(tilesetTexture, sourceRec, destRec, {0, 0}, 0, WHITE);
}

static void DrawObject(const Object& obj) {
    TileCoord coord;
    Texture2D texture;
    if (obj.isFood) {
        coord = foodCoords.at(obj.foodType);
        texture = foodTexture;
    } else {
        coord = objectCoords.at(obj.objectType);
        texture = objectTexture;
    }
    Rectangle sourceRec = {
            static_cast<float>(coord.x * 16),
            static_cast<float>(coord.y * 16),
            16,
            16
    };
    Rectangle destRec = {
            obj.position.x * cellSize,
            obj.position.y * cellSize,
            static_cast<float>(cellSize),
            static_cast<float>(cellSize)
    };
    DrawTexturePro(texture, sourceRec, destRec, {0, 0}, 0, WHITE);
}

static bool CheckSelfCollision(const Character& character) {
    Vector2 head = character.position;
    for (size_t i = 1; i < character.collectedFruits.size(); ++i) {
        if (head.x == character.collectedFruits[i].position.x &&
            head.y == character.collectedFruits[i].position.y) {
            return true;
        }
    }
    return false;
}

static float Lerp(float a, float b, float t) {
    return a + t * (b - a);
}

static void UpdateGame() {
    if (!gameOver) {
        float deltaTime = GetFrameTime();
        character.moveTimer += deltaTime;
        character.interpolationTime = character.moveTimer / MOVE_TIME;

        // Handle input
        if (IsKeyPressed(KEY_RIGHT) && character.direction != 2) character.nextDirection = 3;
        else if (IsKeyPressed(KEY_LEFT) && character.direction != 3) character.nextDirection = 2;
        else if (IsKeyPressed(KEY_UP) && character.direction != 0) character.nextDirection = 1;
        else if (IsKeyPressed(KEY_DOWN) && character.direction != 1) character.nextDirection = 0;

        // Move character (can only move when it is time to move)
        if (character.moveTimer >= MOVE_TIME) {
            character.moveTimer -= MOVE_TIME;
            character.interpolationTime = 0;

            // Apply the next direction
            character.direction = character.nextDirection;

            // Update collected fruits positions before moving the character
            for (int i = character.collectedFruits.size() - 1; i > 0; i--) {
                character.collectedFruits[i].position = character.collectedFruits[i-1].position;
                character.fruitTargetPositions[i] = character.fruitTargetPositions[i-1];
            }
            if (!character.collectedFruits.empty()) {
                character.collectedFruits[0].position = character.position;
                character.fruitTargetPositions[0] = character.targetPosition;
            }

            // Move character to target position
            character.position = character.targetPosition;

            // Set new target position based on current direction
            Vector2 newPosition = character.position;
            switch (character.direction) {
                case 0: newPosition.y += 1; break; // Down
                case 1: newPosition.y -= 1; break; // Up
                case 2: newPosition.x -= 1; break; // Left
                case 3: newPosition.x += 1; break; // Right
            }

            // Check for boundary collision
            if (newPosition.x < 0 || newPosition.x >= gridWidth ||
                newPosition.y < 0 || newPosition.y >= gridHeight) {
                gameOver = true;
                return;
            }

            character.targetPosition = newPosition;

            // Check for food collision
            if (character.position.x == food.position.x && character.position.y == food.position.y) {
                score++;
                if (score > maxScore) maxScore = score;

                // Add new fruit at the end of the line plus one
                Vector2 newFruitPosition, newFruitTargetPosition;
                if (character.collectedFruits.empty()) {
                    // If it's the first fruit, place it at the character's previous position
                    newFruitPosition = character.position;
                    newFruitTargetPosition = character.position; // It will move to this position in the next update
                } else {
                    // Place the new fruit one step behind the last fruit
                    Vector2 lastFruitDirection = {
                            character.collectedFruits.back().position.x - character.fruitTargetPositions.back().x,
                            character.collectedFruits.back().position.y - character.fruitTargetPositions.back().y
                    };
                    newFruitPosition = {
                            character.collectedFruits.back().position.x + lastFruitDirection.x,
                            character.collectedFruits.back().position.y + lastFruitDirection.y
                    };
                    newFruitTargetPosition = character.collectedFruits.back().position;
                }

                character.collectedFruits.push_back({food.type, newFruitPosition});
                character.fruitTargetPositions.push_back(newFruitTargetPosition);

                if (score >= MAX_SCORE) {
                    gameOver = true;
                    return;
                }

                GenerateFood();
            }

            // Check for self-collision
            if (CheckSelfCollision(character)) {
                gameOver = true;
                return;
            }
        }

        // Update animation
        character.framesCounter++;
        if (character.framesCounter >= (60 / FRAME_SPEED)) {
            character.framesCounter = 0;
            character.currentFrame++;
            if (character.currentFrame >= TOTAL_FRAMES) character.currentFrame = 0;
            character.frameRec.x = static_cast<float>(character.currentFrame * characterTexture.width / TOTAL_FRAMES);
        }

        // Update frame for animation
        character.frameRec.y = static_cast<float>(character.direction * characterTexture.height / 4);
    } else {
        if (IsKeyPressed(KEY_ENTER)) {
            InitGame();
        }
    }
}

static void DrawGame() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // draw pre-rendered
    DrawTextureRec(backgroundAndObjectsTexture.texture,
                   (Rectangle){ 0, 0, (float)backgroundAndObjectsTexture.texture.width, (float)-backgroundAndObjectsTexture.texture.height },
                   (Vector2){ 0, 0 }, WHITE);

    // Calculate interpolated position for character
    float t = character.interpolationTime;
    Vector2 interpolatedPos = {
            Lerp(character.position.x, character.targetPosition.x, t),
            Lerp(character.position.y, character.targetPosition.y, t)
    };

    // Draw collected fruits with interpolation
    for (size_t i = 0; i < character.collectedFruits.size(); i++) {
        Vector2 fruitPos = character.collectedFruits[i].position;
        Vector2 fruitTargetPos = character.fruitTargetPositions[i];
        Vector2 interpolatedFruitPos = {
                Lerp(fruitPos.x, fruitTargetPos.x, t),
                Lerp(fruitPos.y, fruitTargetPos.y, t)
        };
        DrawObject({interpolatedFruitPos, true, {.foodType = character.collectedFruits[i].type}});
    }

    // Draw character
    DrawTexturePro(characterTexture, character.frameRec,
                   {interpolatedPos.x * cellSize - cellSize / 2,
                    interpolatedPos.y * cellSize - cellSize / 2,
                    static_cast<float>(CHARACTER_SIZE),
                    static_cast<float>(CHARACTER_SIZE)},
                   {0, 0}, 0, WHITE);

    // Draw food
    DrawObject({food.position, true, {.foodType = food.type}});

    // Draw UI elements
    DrawText(TextFormat("Score: %d", score), 10, 10, 20, BLACK);
    DrawText(TextFormat("Max Score: %d", maxScore), 10, 40, 20, BLACK);

    if (gameOver) {
        DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 150});
        if (score >= MAX_SCORE) {
            DrawText("You Win!", screenWidth / 2 - MeasureText("You Win!", 40) / 2, screenHeight / 2 - 40, 40, WHITE);
        } else {
            DrawText("Game Over!", screenWidth / 2 - MeasureText("Game Over!", 40) / 2, screenHeight / 2 - 40, 40, WHITE);
        }
        DrawText("Press ENTER to restart", screenWidth / 2 - MeasureText("Press ENTER to restart", 20) / 2, screenHeight / 2 + 20, 20, LIGHTGRAY);
    }

    EndDrawing();
}

static void UpdateDrawFrame() {
    UpdateGame();
    DrawGame();
}