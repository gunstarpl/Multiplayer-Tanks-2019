#pragma once

#include "Precompiled.hpp"
#include "Common/StateMachine.hpp"

class GameStateBase : public State<GameStateBase>
{
protected:
    // Protected constructor.
    GameStateBase();
    virtual ~GameStateBase();

    // State machine transitions.
    virtual bool onStateEnter(State<GameStateBase>* newState) override;
    virtual bool onStateExit(State<GameStateBase>* previousState) override;

public:
    // Base state interface.
    virtual void handleEvent(const sf::Event& event);
    virtual void update(float timeDelta);
    virtual void tick(float timeDelta);
    virtual void draw(float timeAlpha);
};
