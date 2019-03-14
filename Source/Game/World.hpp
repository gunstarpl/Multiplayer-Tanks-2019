#pragma once

#include "Precompiled.hpp"
#include "Game/Handle.hpp"
#include "Game/Object.hpp"

class World : public Serializable
{
public:
    World();
    ~World();

    // Initializes the world.
    bool initialize();

    // Updates all objects in the world.
    void update(float timeDelta);

    // Draws all objects in the world.
    void draw(float updateAlpha);

    // Object management methods.
    Handle addObject(Object* object);
    Object* getObject(Handle handle);
    void destroyObject(Handle handle);

private:
    // Processes pending objects.
    void processPendingObjects();

    // Serialization methods.
    virtual bool onSerialize(MemoryBuffer& buffer) override;
    virtual bool onDeserialize(MemoryBuffer& buffer) override;

private:
    // List of objects currently in the world.
    struct ObjectEntry
    {
        ObjectEntry(int index = 0) :
            handle(index),
            object(nullptr),
            exists(false),
            destroy(false)
        {
        }

        Handle handle;
        Object* object;
        bool exists;
        bool destroy;
    };

    std::vector<ObjectEntry> m_objects;
};
