#include "GameEngine.h"
// #include <memory>
#include "main.h"

// Static member definitions for Platform class
float Platform::lastSpawnTime = 0.0f;
int Platform::platformCount = 0;

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  GameEngine engine;
  if (!engine.Initialize("Game Engine", 1800, 1000)) {
    return 1;
  }
  engine.GetRenderSystem()->SetScalingMode(ScalingMode::PROPORTIONAL);

  SDL_Texture *entityIdleTexture = 
    LoadTexture(engine.GetRenderer(),
                "media/Idle_KG_1.bmp");
  SDL_Texture *entityJumpRightTexture = 
    LoadTexture(engine.GetRenderer(),
                "media/Jump_Right.bmp");
  SDL_Texture *entityWalkRightTexture = 
    LoadTexture(engine.GetRenderer(),
                "media/Walking_Right.bmp");
  SDL_Texture *entityJumpLeftTexture = 
    LoadTexture(engine.GetRenderer(),
                "media/Jump_Left.bmp");
  SDL_Texture *entityWalkLeftTexture = 
    LoadTexture(engine.GetRenderer(),
                "media/Walking_Left.bmp");

  // Create entities
  TestEntity *testEntity = new TestEntity(100, 100, entityIdleTexture, entityWalkLeftTexture, entityWalkRightTexture, entityJumpLeftTexture, entityJumpRightTexture);
  testEntity->hasPhysics = true; // Enable physics for TestEntity

  // Create platforms with random spawning
  Platform *platform1 = new Platform(0, 925, 400, 75, true);  // Ground platform
  platform1->hasPhysics = false;        // no integration
  platform1->affectedByGravity = false; // no gravity
  platform1->isStatic = true;

  Platform *platform2 = new Platform(600, 900, 200, 75);
  platform2->hasPhysics = false; // we want horizontal motion we code ourselves
  platform2->affectedByGravity = false; // but no falling
  platform2->isStatic = true; // treat as static for collisions if you have special handling

  Platform *platform3 = new Platform(1000, 850, 300, 75);
  platform3->hasPhysics = false; // we want horizontal motion we code ourselves
  platform3->affectedByGravity = false; // but no falling
  platform3->isStatic = true; // treat as static for collisions if you have special handling

  Platform *platform4 = new Platform(1500, 890, 200, 75);
  platform4->hasPhysics = false; // we want horizontal motion we code ourselves
  platform4->affectedByGravity = false; // but no falling
  platform4->isStatic = true; // treat as static for collisions if you have special handling

  Platform *platform5 = new Platform(1900, 750, 100, 75);
  platform5->hasPhysics = false; // we want horizontal motion we code ourselves
  platform5->affectedByGravity = false; // but no falling
  platform5->isStatic = true; // treat as static for collisions if you have special handling

  // Add entities to the engine
  engine.AddEntity(testEntity);
  engine.AddEntity(platform1);
  engine.AddEntity(platform2);
  engine.AddEntity(platform3);
  engine.AddEntity(platform4);
  engine.AddEntity(platform5);

  if (entityIdleTexture) {
    testEntity->SetTexture(entityIdleTexture);
  }

  SDL_Texture *platformTexture =
    LoadTexture(engine.GetRenderer(),
                "media/cartooncrypteque_platform_basicground_idle.bmp");
  if (platformTexture) {
    platform1->SetTexture(platformTexture);
    platform2->SetTexture(platformTexture);
    platform3->SetTexture(platformTexture);
    platform4->SetTexture(platformTexture);
    platform5->SetTexture(platformTexture);
  }

  engine.Run();

  SDL_Log("Cleaning up resources...");
  engine.Shutdown();
  SDL_Log("Shutdown complete. Exiting.");

  return 0;
}