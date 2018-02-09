  #ifndef G012188ESTEERINGBEHAVIOURS_H
#define G012188ESTEERINGBEHAVIOURS_H

#include "../../Commons.h"
#include "../../Collisions.h"

#include <math.h>

using namespace::std;

class G012188eTank;
class BaseTank;
class GameObject;
class ObstacleManager;
class ProjectileManager;
class TankManager;

class SteeringBehaviours
{
public:

	enum Behaviours
	{
		SEEK,
		FLEE,
		AVOIDANCE,
		ARRIVE,
		PURSUIT,
		OBSTACLE,
		WANDER
	};

	enum Deceleration
	{
		SLOW = 3,
		NORMAL = 2,
		FAST = 1
	};

public:

	SteeringBehaviours();
	~SteeringBehaviours();

	Vector2D Seek(BaseTank* tank, Vector2D TargetPos);
	Vector2D Flee(BaseTank* tank, Vector2D TargetPos);
	Vector2D Arrive(BaseTank* tank, Vector2D TargetPos, Deceleration deceleration);
	Vector2D Pursuit(BaseTank* tank, BaseTank* target);
	Vector2D ObstacleAvoidance(BaseTank* tank);
	Vector2D Wander(BaseTank* tank);
	 
	Vector2D CombineForces(BaseTank* tank, Vector2D targetPos);

	void ActivateBehavior(Behaviours behaviour);
	void CancelBehavior(Behaviours behaviour);

	bool GetState(Behaviours behaviour);

	//Obstacle Avoidnce extra functions
	static double Distance(Vector2D a, Vector2D b);

	bool PointInBox(Vector2D ahead, Rect2D obstacle);
	bool Box(BaseTank* tank, GameObject* object);

	//Wander Extra Functions
	Vector2D SetAngle(Vector2D vector, int value);

private:

	bool				_isSeeking;
	bool				_isFleeing;
	bool				_isArriving;
	bool				_isPursuing;
	bool				_isObsAvoid;
	bool				_isWandering;

	float				_fleeRadius;
	Vector2D			_combinedForce;

	ObstacleManager*	_obstacles;
	TankManager*		_manager;

	Vector2D			_ahead;
	Vector2D			_ahead2;

	float				_maxAvoidForce = 500.0f;
	float				_circleDistance = 2.0f;
	float				_wanderAngle;
};

#endif //G012188ESTEERINGBEHAVIOURS_H

