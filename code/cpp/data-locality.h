#include <time.h>

#ifndef cpp_data_locality_h
#define cpp_data_locality_h

// TODO(bob):
//
// cache effects are magnified by:
// - turning up number of actors
// - adding padding to the actor class
//   (both because it spaces the actors out more in memory)
//   in examples below, creating much bigger shuffled array just to spread
//   them out more.
// - adding padding to component magnifies it too, but also punishes best case

namespace DataLocality
{
  void sleepFor500Cycles() {}

  struct Thing
  {
    void doStuff() {}
  };

  static const int NUM_THINGS = 3;

  void callDoNothing()
  {
    Thing things[NUM_THINGS];

    //^do-nothing
    for (int i = 0; i < NUM_THINGS; i++)
    {
      sleepFor500Cycles();
      things[i].doStuff();
    }
    //^do-nothing
  }

  //^components
  class AIComponent
  {
  public:
    void update() { /* Работа с данными и изменение состояний... */ }

  private:
    // Goals, mood, etc. ...
  };

  class PhysicsComponent
  {
  public:
    void update() { /* Работа с данными и изменение состояний... */ }

  private:
    // Модель, скорость, масса и т.д. ...
  };

  class RenderComponent
  {
  public:
    void render() { /* Работа с данными и изменение состояний... */ }

  private:
    // Графическая модель, текстуры, шейдеры и т.д. ...
  };
  //^components

  //^game-entity
  class GameEntity
  {
  public:
    GameEntity(AIComponent* ai,
               PhysicsComponent* physics,
               RenderComponent* render)
    : ai_(ai), physics_(physics), render_(render)
    {}

    AIComponent* ai() { return ai_; }
    PhysicsComponent* physics() { return physics_; }
    RenderComponent* render() { return render_; }

  private:
    AIComponent* ai_;
    PhysicsComponent* physics_;
    RenderComponent* render_;
  };
  //^game-entity

  void gameLoop()
  {
    int numEntities = 123;
    GameEntity* entities[123];
    bool gameOver = false;

    //^game-loop
    while (!gameOver)
    {
      // AI
      for (int i = 0; i < numEntities; i++)
      {
        entities[i]->ai()->update();
      }

      // Физика
      for (int i = 0; i < numEntities; i++)
      {
        entities[i]->physics()->update();
      }

      // Отрисовка
      for (int i = 0; i < numEntities; i++)
      {
        entities[i]->render()->render();
      }

      // Другие действия в игровом цикле...
    }
    //^game-loop
  }

  static const int MAX_ENTITIES = 100;

  void componentArrays()
  {
    //^component-arrays
    AIComponent* aiComponents =
        new AIComponent[MAX_ENTITIES];
    PhysicsComponent* physicsComponents =
        new PhysicsComponent[MAX_ENTITIES];
    RenderComponent* renderComponents =
        new RenderComponent[MAX_ENTITIES];
    //^component-arrays

    int numEntities = 123;
    bool gameOver = false;

    //^game-loop-arrays
    while (!gameOver)
    {
      // AI
      for (int i = 0; i < numEntities; i++)
      {
        aiComponents[i].update();
      }

      // Физика
      for (int i = 0; i < numEntities; i++)
      {
        physicsComponents[i].update();
      }

      // Отрисовка
      for (int i = 0; i < numEntities; i++)
      {
        renderComponents[i].render();
      }

      // Другие действия в игровом цикле...
    }
    //^game-loop-arrays

    delete [] aiComponents;
    delete [] physicsComponents;
    delete [] renderComponents;
  }

  //^particle-system
  class Particle
  {
  public:
    //^omit particle-system
    bool isActive() { return false; }
    //^omit particle-system
    void update() { /* Gravity, etc. ... */ }
    // Положение, скорость и т.д. ...
  };

  class ParticleSystem
  {
  public:
    ParticleSystem()
    : numParticles_(0)
    {}

    void update();
    //^omit particle-system
    void activateParticle(int index);
    void deactivateParticle(int index);
    //^omit particle-system
  private:
    static const int MAX_PARTICLES = 100000;

    int numParticles_;
    Particle particles_[MAX_PARTICLES];
  };
  //^particle-system

  //^update-particle-system
  void ParticleSystem::update()
  {
    for (int i = 0; i < numParticles_; i++)
    {
      particles_[i].update();
    }
  }
  //^update-particle-system

  void updateParticlesSlow()
  {
    Particle particles_[100];
    int numParticles_ = 0;
    //^particles-is-active
    for (int i = 0; i < numParticles_; i++)
    {
      if (particles_[i].isActive())
      {
        particles_[i].update();
      }
    }
    //^particles-is-active
  }

  Particle particles[100];
  int numActive_ = 0;
  void updateParticles()
  {
    //^update-particles
    for (int i = 0; i < numActive_; i++)
    {
      particles[i].update();
    }
    //^update-particles
  }

  //^activate-particle
  void ParticleSystem::activateParticle(int index)
  {
    // Не должно быть уже активным!
    assert(index >= numActive_);

    // Поменяем его с первой неактивной частицей
    // сразу после активной
    Particle temp = particles_[numActive_];
    particles_[numActive_] = particles_[index];
    particles_[index] = temp;

    // Теперь их стало на одну больше
    numActive_++;
  }
  //^activate-particle

  //^deactivate-particle
  void ParticleSystem::deactivateParticle(int index)
  {
    // Не должно быть уже активным!
    assert(index < numActive_);

    // Теперь их стало на одну меньше
    numActive_--;

    // Поменяем его с первой активной частицей
    // сразу после неактивной
    Particle temp = particles_[numActive_];
    particles_[numActive_] = particles_[index];
    particles_[index] = temp;
  }
  //^deactivate-particle

  enum Mood {
    MOOD_WISTFUL
  };

  class Animation {};
  class Vector {};
  class LootType {};

  namespace HotColdMixed
  {
    //^ai-component
    class AIComponent
    {
    public:
      void update() { /* ... */ }

    private:
      Animation* animation_;
      double energy_;
      Vector goalPos_;
    };
    //^ai-component
  }

  namespace HotColdMixedLoot
  {
    //^loot-drop
    class AIComponent
    {
    public:
      void update() { /* ... */ }

    private:
      // Какие-то поля...
      LootType drop_;
      int minDrops_;
      int maxDrops_;
      double chanceOfDrop_;
    };
    //^loot-drop
  }

  namespace HotCold
  {
    class LootDrop;

    //^hot-cold
    class AIComponent
    {
    public:
      // Методы...
    private:
      Animation* animation_;
      double energy_;
      Vector goalPos_;

      LootDrop* loot_;
    };

    class LootDrop
    {
      friend class AIComponent;
      LootType drop_;
      int minDrops_;
      int maxDrops_;
      double chanceOfDrop_;
    };
    //^hot-cold
  }
}

#endif
