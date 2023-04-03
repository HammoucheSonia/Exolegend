#include "Warrior.hpp"
#include "vector2.h"
#include "utils.h"

static inline float mod2PI(float theta)
{
    if (theta > PI)
        return (theta - 2 * PI);
    if (theta < -PI)
        return (theta + 2 * PI);
    return (theta);
}

static inline float norm2(float x, float y)
{
    return (x * x + y * y);
}


void Warrior::setNearestSquare(void)
{
    Position pos = this->robot->getData().position;
	MazeSquare current = this->maze->getSquare(setIndexFromPosition(pos.x), setIndexFromPosition(pos.y));
   
//    return (this->maze->getNearestSquare())
    nearest = current;
}


MazeSquare Warrior::getNearestSquare(void)
{
    return (this->nearest);
}

Warrior::Warrior(): Gladiator() {
    this->reset();
}

void Warrior::reset(void)
{
    this->stop();
    this->direction = 1;
}

bool Warrior::aim(float x, float y)
{
    Position current = this->robot->getData().position;
    if (direction < 0)
        current.a = mod2PI(current.a + PI);
    float delta = 0.05;

    if (norm2(x - current.x, y - current.y) < Warrior::THRESH2)
    {
        this->stop();
        return true;
    }

    float angle0 = std::atan2(y - current.y, x - current.x);
    float angle = angle0;
    angle -= current.a;
    angle = mod2PI(angle);

    if (ghostX == -1)
    {
        ghostX = current.x;
        ghostY = current.y;
        this->theta = angle0;
    }

    if (abs(angle) > PI * 0.51)
    {
        this->direction *= -1;
        this->log("new direction: %.0f angle: %f", this->direction, angle);
        this->speed *= -0.9;
        return (false);
    }
    if (abs(angle) > 0.1)
    {
        delta = 0.05;
        if (abs(angle) > 0.4)
            delta = 0.2;
        this->speed = 0;
        if (angle < 0)
            delta *= -1;
        this->setSpeed(-delta, delta);
        return false;
    }
    if (this->speed * this->speed < Warrior::MAX_SPEED2)
    {
        this->speed += 0.1 * Warrior::MAX_SPEED * this->direction;
    }
    float s = this->speed + Warrior::AMORTIZE * sqrtf(norm2(this->ghostX - current.x, this->ghostY - current.y));
    this->updateGhost(x, y);
    delta = 0.1;
    if (angle < 0)
        delta *= -1;
    this->setSpeed((s - delta), (s + delta));
    return false;
}

void Warrior::updateGhost(float x, float y)
{
    if (norm2(ghostX - x, ghostY - y) < Warrior::THRESH2)
        return ;
    ghostX += 0.001 * Warrior::DELAY * abs(speed) * cos(this->theta);
    ghostY += 0.001 * Warrior::DELAY * abs(speed) * sin(this->theta);
}

void Warrior::setSpeed(float left, float right)
{
    this->control->setWheelSpeed(WheelAxis::LEFT, left, true);
    this->control->setWheelSpeed(WheelAxis::RIGHT, right, true);
}

void Warrior::stop()
{
    this->speed = 0;
    this->theta = 0;
    this->ghostX = -1;
    this->ghostY = -1;
    this->setSpeed(0, 0);
}

int	Warrior::findDirection(void)
{
	float	radius = this->robot->getData().position.a;

	if (radius < 3.f*PI/4.f && radius > PI/4.f) //North
		return (Direction::NORTH);
	else if (radius < PI/4.f && radius > -PI/4.f) //East
		return (Direction::EAST);
	else if (radius < -PI/4.f && radius > -3*PI/4.f) //South
		return (Direction::SOUTH);
	else //West
		return (Direction::WEST);
}

void Warrior::get_square_rotater(MazeSquare current, MazeSquare *allSquare[4])
{
    int facing = this->findDirection();
    switch (facing)
    {
        case Direction::NORTH:
            allSquare[Direction::NORTH] = current.northSquare;
            allSquare[Direction::EAST] = current.eastSquare;
            allSquare[Direction::SOUTH] = current.southSquare;
            allSquare[Direction::WEST] = current.westSquare;
            break;
        case Direction::EAST:
            allSquare[Direction::NORTH] = current.eastSquare;
            allSquare[Direction::EAST] = current.southSquare;
            allSquare[Direction::SOUTH] = current.westSquare;
            allSquare[Direction::WEST] = current.northSquare;
            break;
        case Direction::SOUTH:
            allSquare[Direction::NORTH] = current.southSquare;
            allSquare[Direction::EAST] = current.westSquare;
            allSquare[Direction::SOUTH] = current.northSquare;
            allSquare[Direction::WEST] = current.eastSquare;
            break;
        case Direction::WEST:
            allSquare[Direction::NORTH] = current.westSquare;
            allSquare[Direction::EAST] = current.northSquare;
            allSquare[Direction::SOUTH] = current.eastSquare;
            allSquare[Direction::WEST] = current.southSquare;
            break;
        default:
            break;
    }
}


MazeSquare* Warrior::getJewelNext(MazeSquare *allSquare[4])
{
    if (allSquare[Direction::NORTH] != NULL && allSquare[Direction::NORTH]->coin.value != 0)
        return (allSquare[Direction::NORTH]);
    if (allSquare[Direction::SOUTH] != NULL && allSquare[Direction::SOUTH]->coin.value != 0)
        return (allSquare[Direction::SOUTH]);
    int max = 0;
    MazeSquare* next = NULL;
    
    for (int i = 1; i < 4; i+=2)
    {
        if (allSquare[i] != NULL && allSquare[i]->coin.value > max)
        {
            max = allSquare[i]->coin.value;
            next = allSquare[i];
        }
    }
    return (next);
}

MazeSquare* Warrior::getJewelBackMe(MazeSquare *allSquare[4])
{
    MazeSquare* next = NULL;
    MazeSquare* allSquareTMP[4] = {allSquare[0], allSquare[1], allSquare[2], allSquare[3]};
    while (allSquare[2] != NULL)
    {
        if (allSquare[2]->coin.value != 0)
        {
            next = allSquareTMP[2];  /* SetFirstActionToJoinJewel */
            break ;
        }
        this->get_square_rotater(*allSquare[2], allSquare);
    }
    /* Reset allSquare*/
    allSquare[0] = allSquareTMP[0];
    allSquare[1] = allSquareTMP[1];
    allSquare[2] = allSquareTMP[2];
    allSquare[3] = allSquareTMP[3];
    return (next);
}

MazeSquare* Warrior::getJewelFrontMe(MazeSquare *allSquare[4])
{
    MazeSquare* next = NULL;
    MazeSquare* allSquareTMP[4] = {allSquare[0], allSquare[1], allSquare[2], allSquare[3]};
    while (allSquare[0] != NULL)
    {
        if (allSquare[0]->coin.value != 0)
        {
            next = allSquareTMP[0];  /* SetFirstActionToJoinJewel */
            break ;
        }
        this->get_square_rotater(*allSquare[0], allSquare);
    }
    /* Reset allSquare*/
    allSquare[0] = allSquareTMP[0];
    allSquare[1] = allSquareTMP[1];
    allSquare[2] = allSquareTMP[2];
    allSquare[3] = allSquareTMP[3];
    return (next);
}

MazeSquare* Warrior::getNearestJewelInDirection(MazeSquare *allSquare[4])
{
    int max = 0;
    MazeSquare* next = NULL;
    MazeSquare* allSquareTMP[4] = {allSquare[0], allSquare[1], allSquare[2], allSquare[3]};
    for (int i = 1; i < 4; i+=2)
    {
        int count = 0;
        while (allSquare[i] != NULL)
        {
            if (allSquare[i]->coin.value != 0)
            {
                if (count < max)
                {
                    max = count;
                    next = allSquareTMP[i];  /* SetFirstActionToJoinJewel */
                }
                break ;
            }
            count++;
            this->get_square_rotater(*allSquare[i], allSquare);
        }
        /* Reset allSquare*/
        allSquare[0] = allSquareTMP[0];
        allSquare[1] = allSquareTMP[1];
        allSquare[2] = allSquareTMP[2];
        allSquare[3] = allSquareTMP[3];
    }
    return (next);
}

Vect2 Warrior::getBestCaseRecenter(t_coord major, t_coord minor1, t_coord minor2)
{
    int majorValue = this->maze->getSquare(major.x, major.y).coin.value;
    int minor1Value = this->maze->getSquare(minor1.x, minor1.y).coin.value;
    int minor2Value = this->maze->getSquare(minor2.x, minor2.y).coin.value;

    if (minor1Value > minor2Value && minor1Value > majorValue)
        return ((Vect2){setPositionFromIndex((float)minor1.x), setPositionFromIndex((float)minor1.y)});
    if (minor2Value > majorValue)
        return ((Vect2){setPositionFromIndex((float)minor2.x), setPositionFromIndex((float)minor2.y)});
    return ((Vect2){setPositionFromIndex((float)major.x), setPositionFromIndex((float)major.y)});
}

Vect2 Warrior::moveToCenter(MazeSquare current)
{
    if (current.i < 7 && current.j < 7)
        return (this->getBestCaseRecenter((t_coord){current.i + 1, current.j + 1}, (t_coord){current.i + 1, current.j}, (t_coord){current.i, current.j + 1}));
    if (current.i < 7 && current.j > 7)
        return (this->getBestCaseRecenter({current.i + 1, current.j - 1}, {current.i + 1, current.j}, {current.i, current.j - 1}));
    if (current.i > 7 && current.j < 7)
        return (this->getBestCaseRecenter({current.i - 1, current.j + 1}, {current.i - 1, current.j}, {current.i, current.j + 1}));
    if (current.i > 7 && current.j > 7)
        return (this->getBestCaseRecenter({current.i - 1, current.j - 1}, {current.i - 1, current.j}, {current.i, current.j - 1}));
    return ((Vect2){setPositionFromIndex(6.5), setPositionFromIndex(6.5)});
}

Vect2 Warrior::getNextSquare()
{
    Position pos = this->robot->getData().position;
	MazeSquare current = this->maze->getSquare(setIndexFromPosition(pos.x), setIndexFromPosition(pos.y));
	MazeSquare* allSquare[4];
	this->get_square_rotater(current, allSquare);

    MazeSquare* next = NULL;

    next = this->getJewelFrontMe(allSquare);
    if (next)
        return ((Vect2){setPositionFromIndex(next->i), setPositionFromIndex(next->j)});
    next = this->getJewelNext(allSquare);
    if (next)
	    return ((Vect2){setPositionFromIndex(next->i), setPositionFromIndex(next->j)});
	next = this->getNearestJewelInDirection(allSquare);
    if (next)
        return ((Vect2){setPositionFromIndex(next->i), setPositionFromIndex(next->j)});

    return (this->moveToCenter(current));
}
