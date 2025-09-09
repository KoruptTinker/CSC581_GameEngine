#include "GameEngine.h"
#include "main.h"

float Platform::lastSpawnTime = 0.0f;
int Platform::platformCount = 0;

bool Collectible::randomInitialized = false;

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  GameEngine engine;
  if (!engine.Initialize("Game Engine", 1200, 800)) {
    return 1;
  }
  engine.GetRenderSystem()->SetScalingMode(ScalingMode::PROPORTIONAL);

  SDL_Texture *entityIdleTexture =
      LoadTexture(engine.GetRenderer(), "media/Idle_KG_1.bmp");
  SDL_Texture *entityJumpRightTexture =
      LoadTexture(engine.GetRenderer(), "media/Jump_Right.bmp");
  SDL_Texture *entityWalkRightTexture =
      LoadTexture(engine.GetRenderer(), "media/Walking_Right.bmp");
  SDL_Texture *entityJumpLeftTexture =
      LoadTexture(engine.GetRenderer(), "media/Jump_Left.bmp");
  SDL_Texture *entityWalkLeftTexture =
      LoadTexture(engine.GetRenderer(), "media/Walking_Left.bmp");
  SDL_Texture *coinsTexture =
      LoadTexture(engine.GetRenderer(), "media/coins.bmp");

  // Create entities
  Player *player = new Player(
      100, 100, entityIdleTexture, entityWalkLeftTexture,
      entityWalkRightTexture, entityJumpLeftTexture, entityJumpRightTexture);
  player->hasPhysics = true; // Enable physics for Player

  // Create platforms with random spawning
  Platform *platform1 = new Platform(0, 725, 400, 75, true); // Ground platform
  platform1->hasPhysics = false;                             // no integration
  platform1->affectedByGravity = false;                      // no gravity
  platform1->isStatic = true;

  Platform *platform2 = new Platform(600, 500, 200, 75);
  platform2->hasPhysics = false; 
  platform2->affectedByGravity = false; 
  platform2->isStatic = true; 

  Platform *platform3 = new Platform(1000, 600, 300, 75);
  platform3->hasPhysics = false; 
  platform3->affectedByGravity = false; 
  platform3->isStatic = true; 

  Platform *platform4 = new Platform(1500, 390, 200, 75);
  platform4->hasPhysics = false; 
  platform4->affectedByGravity = false; 
  platform4->isStatic = true; 

  Platform *platform5 = new Platform(1900, 550, 100, 75);
  platform5->hasPhysics = false; 
  platform5->affectedByGravity = false; 
  platform5->isStatic = true; 

  Collectible *coin1 = new Collectible(300, 650, coinsTexture, 0); 
  Collectible *coin2 = new Collectible(450, 650, coinsTexture, 1); 
  Collectible *coin3 = new Collectible(600, 650, coinsTexture, 2); 
  Collectible *coin4 = new Collectible(750, 600, coinsTexture, 0); 
  Collectible *coin5 = new Collectible(900, 600, coinsTexture, 1); 
  Collectible *coin6 = new Collectible(1100, 600, coinsTexture, 2); 
  Collectible *coin7 = new Collectible(1300, 600, coinsTexture, 0); 
  Collectible *coin8 = new Collectible(1600, 550, coinsTexture, 1);
  Collectible *coin9 = new Collectible(1800, 500, coinsTexture, 2); 

  engine.AddEntity(player);
  engine.AddEntity(platform1);
  engine.AddEntity(platform2);
  engine.AddEntity(platform3);
  engine.AddEntity(platform4);
  engine.AddEntity(platform5);
  
  engine.AddEntity(coin1);
  engine.AddEntity(coin2);
  engine.AddEntity(coin3);
  engine.AddEntity(coin4);
  engine.AddEntity(coin5);
  engine.AddEntity(coin6);
  engine.AddEntity(coin7);
  engine.AddEntity(coin8);
  engine.AddEntity(coin9);

  if (entityIdleTexture) {
    player->SetTexture(entityIdleTexture);
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
