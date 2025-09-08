#pragma once
#include <iostream>
#include "GameEngine.h"
#include <vector>
#include <ctime>
#include <algorithm>
// #include <memory>

class TestEntity : public Entity {
private:
  int currentFrame;
  Uint32 lastFrameTime;
  int animationDelay;
  
  Entity* groundRef = nullptr;   // platform we're standing on (if any)
  float   groundVX  = 0.0f;  

  SDL_Texture *idleTex, *runTex, *jumpTex;    // platform's current x velocity

public:
  
  TestEntity(float x, float y, SDL_Texture *idle, SDL_Texture *run, SDL_Texture *jump) : Entity(x, y, 176, 128) {
    velocity.x = 0.0f; // Move right at 150 pixels per second
    currentFrame = 0;
    lastFrameTime = 0;
    animationDelay = 200;

    tex.num_frames_x = 4;
    tex.num_frames_y = 0;
    tex.frame_width = 100;
    tex.frame_height = 64;
    idleTex = idle; runTex = run, jumpTex = jump;
  }

  void Update(float deltaTime, InputManager *input) override {
    // Update animation
    lastFrameTime += (Uint32)(deltaTime * 1000); // Convert to milliseconds
    if (lastFrameTime >= (Uint32)animationDelay) {
      currentFrame = (currentFrame + 1) % tex.num_frames_x;
      lastFrameTime = 0;
    }

    // speeds
    constexpr float runSpeed = 200.0f;

    // input
    const bool left  = input->IsKeyPressed(SDL_SCANCODE_A) ||
                      input->IsKeyPressed(SDL_SCANCODE_LEFT);
    const bool right = input->IsKeyPressed(SDL_SCANCODE_D) ||
                      input->IsKeyPressed(SDL_SCANCODE_RIGHT);

    // carrier velocity (only meaningful when grounded on a platform)
    const float carrierVX = (grounded && groundRef) ? groundRef->velocity.x : 0.0f;

    // base desired velocity from input (world-space)
    float desiredVX = 0.0f;
    if (left ^ right) {                    // exactly one is held
      // if(grounded ) {
      //   tex = {runTex, 7, 0, 100, 64};
      // }
      desiredVX = left ? -runSpeed : runSpeed;
    }

    // rule:
    // - if player is giving input -> move at constant runSpeed in world space
    // - if no input              -> ride the platform
    if (desiredVX != 0.0f) {
      velocity.x = desiredVX;              // ignore platform motion while moving
    } else {
      velocity.x = carrierVX;              // inherit when idle
    }

    if (input->IsKeyJustPressed(SDL_SCANCODE_SPACE) && grounded) {
      velocity.y = -1500.0f;
      grounded = false;
      tex = {jumpTex, 6, 0, 100, 64};
    }

    // Bounce off screen edges (demonstrates entity system working) using window bounds push opposite direction
    if (position.x <= 0) {
      position.x = 0;
    } else if (position.x + dimensions.x >= 1920) {
      position.x = 1920 - dimensions.x;
    }

    // Reset if falls off bottom (demonstrates physics working)
    if (!grounded) {           // however you detect “no ground this frame”
      groundRef = nullptr;
      groundVX  = 0.0f;
    }
    if (position.y > 1080) { // fell off bottom of screen
      position.x = 100;
      position.y = 100;
      velocity.y = 0.0f;
      grounded = false;
      groundRef = nullptr;
      groundVX  = 0.0f;
    }
  }

  void OnCollision(Entity* other, CollisionData* collData) override {
    if (collData->normal.y == -1.0f && collData->normal.x == 0.0f) {
      if(!grounded) {
        tex = {idleTex, 4, 0, 100, 64};
      }
      grounded = true;
      velocity.y = 0.0f;
      groundRef = other;
    }  else if (collData->normal.x != 0.0f) {
      velocity.x = 0.0f;   // or keep desiredVX if you resolve penetration separately
    }
  }

  // Get current frame for rendering
  bool GetSourceRect(SDL_FRect &out) const override {
    out = SampleTextureAt(currentFrame, 0);
    return true;
  }
};

class Platform : public Entity {
private:
  static float lastSpawnTime;
  static int platformCount;
  float spawnDelay;
  bool isGroundPlatform;
  
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
    (void) input;
    
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
    position.y = 200.0f + (rand() % 600);
    
    // Random width (100-400 range)
    dimensions.x = 100.0f + (rand() % 300);
    
    // Random speed (-80 to -150)
    velocity.x = -80.0f - (rand() % 70);
    
    // Spawn off-screen to the right
    position.x = 1900.0f;
    
    // Add some randomness to spawn timing
    spawnDelay = 1.0f + (rand() % 300) / 100.0f;
  }
};