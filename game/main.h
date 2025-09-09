#pragma once
#include "GameEngine.h"
#include <algorithm>
#include <ctime>
#include <iostream>
#include <vector>
// #include <memory>

// Forward declarations

class Platform : public Entity {
  private:
    static float lastSpawnTime;
    static int platformCount;
    float spawnDelay;
    bool isGroundPlatform;
    bool collidedWithPlayer = false;

  public:
    Platform(float x, float y, float w = 200, float h = 20, bool isGround = false)
        : Entity(x, y, w, h) {
      isStatic = true;
      hasPhysics = false;
      affectedByGravity = false;
      velocity.x = isGround ? 0.0f : -100.0f;
      velocity.y = 0.0f;
      isGroundPlatform = isGround;
  
      // Random spawn delay for variety (1-4 seconds)
      spawnDelay = 1.0f + (rand() % 300) / 100.0f;
  
      // Initialize static variables on first platform creation
      if (platformCount == 0) {
        lastSpawnTime = 0.0f;
        srand(static_cast<unsigned int>(time(nullptr)));
      }
      platformCount++;
    }
  
    void Update(float dt, InputManager *input) override {
      (void)input;
  
      // Update static spawn timer
      lastSpawnTime += dt;
  
      // Only move if not a ground platform
      if (!isGroundPlatform) {
        // Horizontal-only motion for the moving platform
        position = add(position, mul(dt, velocity));
  
        // When platform goes off-screen, respawn with random properties
        if (position.x + dimensions.x < 0) {
          RespawnWithRandomProperties();
        }
      }
    }
  
  private:
    void RespawnWithRandomProperties() {
      // Random Y position (200-800 range)
      position.y = 100.0f + (rand() % 500);
  
      // Random width (100-400 range)
      dimensions.x = 100.0f + (rand() % 300);
  
      // Random speed (-80 to -150)
      velocity.x = -80.0f - (rand() % 70);
  
      // Spawn off-screen to the right
      position.x = 1200.0f;
  
      // Add some randomness to spawn timing
      spawnDelay = 1.0f + (rand() % 300) / 100.0f;
    }
};


class Player : public Entity {
private:
  int currentFrame;
  Uint32 lastFrameTime;
  int animationDelay;

  bool wasGrounded = false;
  bool wasMoving = false;

  Entity *groundRef = nullptr; // platform we're standing on (if any)
  float groundVX = 0.0f;

  SDL_Texture *idleTex, *runLeftTex, *runRightTex, *jumpLeftTex,
      *jumpRightTex; // platform's current x velocity
  bool loopAnimation = true;
  bool flipAnimation = false;

public:
  Player(float x, float y, SDL_Texture *idle, SDL_Texture *runLeft,
             SDL_Texture *runRight, SDL_Texture *jumpLeft,
             SDL_Texture *jumpRight)
      : Entity(x, y, 176, 128) {
    velocity.x = 0.0f; // Move right at 150 pixels per second
    currentFrame = 0;
    lastFrameTime = 0;
    animationDelay = 200;

    tex.num_frames_x = 4;
    tex.num_frames_y = 0;
    tex.frame_width = 100;
    tex.frame_height = 64;
    idleTex = idle;
    runLeftTex = runLeft, runRightTex = runRight, jumpLeftTex = jumpLeft,
    jumpRightTex = jumpRight;
  }

  void Update(float deltaTime, InputManager *input) override {
    // Update animation
    lastFrameTime += (Uint32)(deltaTime * 1000); // Convert to milliseconds
    if (lastFrameTime >= (Uint32)animationDelay) {
      if (flipAnimation) {
        currentFrame = loopAnimation ? (currentFrame - 1 + tex.num_frames_x) %
                                           tex.num_frames_x
                                     : std::max(currentFrame - 1, 0);
      } else {
        currentFrame = loopAnimation ? (currentFrame + 1) % tex.num_frames_x
                                     : std::min(currentFrame + 1,
                                                (int)tex.num_frames_x - 1);
      }
      lastFrameTime = 0;
    }

    // speeds
    constexpr float runSpeed = 200.0f;

    // input
    const bool left = input->IsKeyPressed(SDL_SCANCODE_A) ||
                      input->IsKeyPressed(SDL_SCANCODE_LEFT);
    const bool right = input->IsKeyPressed(SDL_SCANCODE_D) ||
                       input->IsKeyPressed(SDL_SCANCODE_RIGHT);

    // carrier velocity (only meaningful when grounded on a platform)
    const float carrierVX =
        (grounded && groundRef) ? groundRef->velocity.x : 0.0f;

    // base desired velocity from input (world-space)
    float desiredVX = 0.0f;
    if (left ^ right) { // exactly one is held
      if (grounded && !wasMoving) {
        if (left) {
          flipAnimation = true;
          tex = {runLeftTex, 6, 0, 100, 64};
        } else {
          flipAnimation = false;
          tex = {runRightTex, 6, 0, 100, 64};
        }
        wasMoving = true;
      }
      desiredVX = left ? -runSpeed : runSpeed;
    } else {
      wasMoving = false;
    }

    // rule:
    // - if player is giving input -> move at constant runSpeed in world space
    // - if no input              -> ride the platform
    if (desiredVX != 0.0f) {
      velocity.x = desiredVX; // ignore platform motion while moving
    } else {
      velocity.x = carrierVX; // inherit when idle
    }

    if (input->IsKeyPressed(SDL_SCANCODE_SPACE) && grounded) {
      velocity.y = -1500.0f;
      grounded = false;
      wasGrounded = false;
      if (left) {
        flipAnimation = true;
        tex = {jumpLeftTex, 6, 0, 100, 64};
      } else {
        flipAnimation = false;
        tex = {jumpRightTex, 6, 0, 100, 64};
      }
    }

    // Bounce off screen edges (demonstrates entity system working) using window
    // bounds push opposite direction
    if (position.x <= 0) {
      position.x = 0;
    } else if (position.x + dimensions.x >= 1920) {
      position.x = 1920 - dimensions.x;
    }

    // Reset if falls off bottom (demonstrates physics working)
    if (!grounded) { // however you detect “no ground this frame”
      groundRef = nullptr;
      groundVX = 0.0f;
    }
    if (position.y > 1080) { // fell off bottom of screen
      position.x = 100;
      position.y = 100;
      velocity.y = 0.0f;
      grounded = false;
      groundRef = nullptr;
      groundVX = 0.0f;
    }
  }

  void OnCollision(Entity *other, CollisionData *collData) override {
    if (dynamic_cast<Platform*>(other) && collData->normal.y == -1.0f && collData->normal.x == 0.0f) {
      if (!wasGrounded || !wasMoving) {
        tex = {idleTex, 4, 0, 100, 64};
        wasGrounded = true;
        wasMoving = false;
      }
      grounded = true;
      velocity.y = 0.0f;
      groundRef = other;
    } else if (dynamic_cast<Platform*>(other) && collData->normal.x != 0.0f) {
      velocity.x =
          0.0f;
    }
  }

  // Get current frame for rendering
  bool GetSourceRect(SDL_FRect &out) const override {
    out = SampleTextureAt(currentFrame, 0);
    return true;
  }
};

class Collectible : public Entity {
private:
  int currentFrame;
  Uint32 lastFrameTime;
  int animationDelay;
  int coinType; // 0, 1, or 2 for different coin types (rows)
  bool isCollected;
  float respawnTimer;
  float respawnDelay;
  Entity* groundRef; // platform we're standing on (if any)
  static bool randomInitialized;
  bool collidedWithPlayer = true;

public:
  Collectible(float x, float y, SDL_Texture* coinTexture, int type = 0)
      : Entity(x, y, 50, 50) {
    currentFrame = 0;
    lastFrameTime = 0;
    animationDelay = 100; // Faster animation for coins
    coinType = type;
    isCollected = false;
    respawnTimer = 0.0f;
    respawnDelay = 2.0f; // Respawn after 2 seconds
    groundRef = nullptr;
    
    // Initialize random seed on first collectible creation
    if (!randomInitialized) {
      srand(static_cast<unsigned int>(time(nullptr)));
      randomInitialized = true;
    }
    
    // Set up texture properties for coins.bmp
    tex.sheet = coinTexture;
    tex.num_frames_x = 7; // 7 frames horizontally
    tex.num_frames_y = 3; // 3 different coin types (rows)
    tex.frame_width = 18;
    tex.frame_height = 18;
    
    // Enable physics and gravity for collectibles
    hasPhysics = true;
    affectedByGravity = true;
    isStatic = false;
  }

  void Update(float deltaTime, InputManager* input) override {
    (void)input; // Collectibles don't respond to input
    
    // Handle respawn timer
    if (isCollected) {
      respawnTimer += deltaTime;
      if (respawnTimer >= respawnDelay) {
        RespawnAtRandomPosition();
        respawnTimer = 0.0f;
      }
      return;
    }
    
    // Check if collectible fell below screen (assuming 800px height)
    if (position.y > 800) {
      RespawnAtRandomPosition();
      return;
    }
    
    // Handle platform movement - inherit platform velocity when grounded
    if (grounded && groundRef) {
      velocity.x = groundRef->velocity.x; // Move with the platform
    }
    
    // Reset grounded state each frame (will be set by collision if on platform)
    grounded = false;
    groundRef = nullptr;
    
    // Update animation
    lastFrameTime += (Uint32)(deltaTime * 1000); // Convert to milliseconds
    if (lastFrameTime >= (Uint32)animationDelay) {
      currentFrame = (currentFrame + 1) % tex.num_frames_x;
      lastFrameTime = 0;
    }
  }

  void OnCollision(Entity* other, CollisionData* collData) override {
    // Check if colliding with Player (player)
    if (dynamic_cast<Player*>(other) && !isCollected) {
      isCollected = true;
      isVisible = false; 
      respawnTimer = 0.0f; 
      dimensions.x = 0.0f;
      dimensions.y = 0.0f;
      return;
    }
    
    // Check if colliding with Platform from above (landing on top)
    if (dynamic_cast<Platform*>(other) && collData && collData->normal.y == -1.0f) {
      // Landing on top of platform - stop falling
      grounded = true;
      velocity.y = 0.0f;
      groundRef = other; // Set reference to the platform we're standing on
      // Position the collectible on top of the platform
      position.y = other->position.y - dimensions.y;
    }
  }

  // Get current frame for rendering
  bool GetSourceRect(SDL_FRect& out) const override {
    if (isCollected) return false;
    
    out = SampleTextureAt(currentFrame, coinType);
    return true;
  }
  
  bool IsCollected() const { return isCollected; }
  int GetCoinType() const { return coinType; }

private:
  void RespawnAtRandomPosition() {
    // Random X position off-screen to the right (1200-1400 range)
    position.x = 1200.0f + (rand() % 200);
    
    // Random Y position (100-400 range)
    position.y = 100.0f + (rand() % 300);
    
    // Reset velocity
    velocity.x = -50.0f - (rand() % 100); // Move left at random speed
    velocity.y = 0.0f;
    
    // Reset platform reference
    groundRef = nullptr;
    grounded = false;
    
    // Make visible again
    isCollected = false;
    isVisible = true;
    dimensions.x = 50.0f;
    dimensions.y = 50.0f;
    
    // Randomize coin type for variety
    coinType = rand() % 3;
  }
};
