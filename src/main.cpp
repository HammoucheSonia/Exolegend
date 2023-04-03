#include <cmath>
#undef abs
#include "vector2.h"
#include "utils.h"
#include "Warrior.hpp"
#include <sys/time.h>
#include <Arduino.h>

Warrior* gladiator;

enum class State {
    INIT,
    SEARCH,
    KILL,
    RUN,
    GOINSIDE,
};

State state;

void reset() {
    gladiator->reset();
    state = State::INIT;
}

void setup() {
    //instanciation de l'objet gladiator
    gladiator = new Warrior();
    //enregistrement de la fonction de reset qui s'éxecute à chaque fois avant qu'une partie commence
    gladiator->game->onReset(&reset);
}


unsigned long start_time;

void loop() {
    static Vect2  target;
    int status;

    if (gladiator->game->isStarted())
    {
        gladiator->setNearestSquare();
//        struct timeval start_time;
        if (state == State::INIT)
        {
            setTarget(target, gladiator->getNearestSquare().i, gladiator->getNearestSquare().j);
            // init_target(target, gladiator);
            start_time = millis();
            //          gettimeofday(&start_time, NULL);
            state = State::SEARCH;
        }
        if (detectOutside(gladiator, start_time))
        {
            targetMiddle(target);
            state = State::GOINSIDE;
        }
        if (state == State::GOINSIDE && !detectOutside(gladiator, start_time))
        {
            targetCenterNearest(target, gladiator);
            state = State::SEARCH;
        }

        status = gladiator->aim(target.x(), target.y()) ;
        if (status && state == State::SEARCH)
        {
            update_target(target, gladiator);
            gladiator->log("TargetReach: NewTarget: %f, %f", target.x(), target.y());
        }
        else if (status && state == State::GOINSIDE)
        {
            //do something else
        }
        //display state
        // gladiator->log("State: %d", (int)state);
    }
    delay(4); // boucle à 250Hz
}
