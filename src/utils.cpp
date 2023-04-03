#include "vector2.h"
#include "Warrior.hpp"
#include "utils.h"

#include <sys/time.h>

bool	detectOutside(Warrior *gladiator, unsigned long start_time)
{
    unsigned long  end_time;
    MazeSquare      near = gladiator->getNearestSquare();
	int				i = near.i;
	int				j = near.j;

	end_time = millis();
    float 			time_elapsed = (end_time - start_time) / 1000;
	int             restricted = (int)((int)(time_elapsed + 1) / 20);
    // gladiator->log("time el + 1: %f", time_elapsed + 1);
    // gladiator->log("int timeelapsed + 1: %d", (int)(time_elapsed + 1));
    // gladiator->log("int tt/20: %d", (int)(time_elapsed + 1) / 20);
    // gladiator->log("total: %d", (int)((int)(time_elapsed + 1) / 20));
	// gladiator->log("restricted : %d, i : %d, j : %d", restricted, i, j);
	if ((i < restricted ) || (j < restricted) || (i > 13 - restricted) || (j > 13 - restricted))
		return (true);
	return (false);
}

/* Helper */
float	setPositionFromIndex(int index)
{
	return ((index + 0.5f) * 0.214f);
}

int	setIndexFromPosition(float position)
{
	return (position / 0.214f);
}

/* Target */
void	init_target(Vect2 &target, Warrior *gladiator)
{
	Position pos = gladiator->robot->getData().position;

	if (pos.x > 9*0.214f || pos.y > 9*0.214f)
	{
		target.set_x(setPositionFromIndex(7));
		target.set_y((setPositionFromIndex(6)));
	}
	else
	{
		target.set_x(setPositionFromIndex(6));
		target.set_y((setPositionFromIndex(7)));
	}
}

void	targetMiddle(Vect2 &target)
{
	target.set_x(setPositionFromIndex(6.5));
	target.set_y((setPositionFromIndex(6.5)));
}

void	update_target(Vect2 &target, Warrior *gladiator)
{
	static int i = 0;

	Vect2 pos = gladiator->getNextSquare();
	target.set_x(pos.x());
	target.set_y(pos.y());
	i++;
}

void targetCenterNearest(Vect2& target, Warrior *gladiator)
{
	target.set_x(setPositionFromIndex(gladiator->getNearestSquare().i));
	target.set_y(setPositionFromIndex(gladiator->getNearestSquare().j));
}

void 	setTarget(Vect2& target, float x, float y)
{
	target.set_x(setPositionFromIndex(x));
	target.set_y((setPositionFromIndex(y)));
}
