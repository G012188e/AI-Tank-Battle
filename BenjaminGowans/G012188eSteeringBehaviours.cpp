#include "G012188eSteeringBehaviours.h"

#include "G012188eTank.h"
#include "../../ObstacleManager.h"
#include "../../ProjectileManager.h"
#include "../../GameObject.h"
#include "../../TankManager.h"

SteeringBehaviours::SteeringBehaviours()
{
	_isSeeking = false;
	_isFleeing = false;
	_isArriving = false;
	_isObsAvoid = false;
	_isPursuing = false;

	_fleeRadius = 100.0f;
	_combinedForce = Vector2D(0.0f, 0.0f);
	_wanderAngle = 0.0f;
}

SteeringBehaviours::~SteeringBehaviours()
{
}

void SteeringBehaviours::ActivateBehavior(Behaviours behaviour)
{
	switch (behaviour)
	{
	case SteeringBehaviours::SEEK:
		_isSeeking = true;
		break;
	
	case SteeringBehaviours::FLEE:
		_isFleeing = true;
		break;

	case SteeringBehaviours::ARRIVE:
		_isArriving = true;
		break;

	case SteeringBehaviours::PURSUIT:
		_isPursuing = true;
		break;

	case SteeringBehaviours::OBSTACLE:
		_isObsAvoid = true;
		break;

	case SteeringBehaviours::WANDER:
		_isWandering = true;
		break;

	default:
		break;
	}
}

void SteeringBehaviours::CancelBehavior(Behaviours behaviour)
{
	switch (behaviour)
	{
	case SteeringBehaviours::SEEK:
		_isSeeking = false;
		break;

	case SteeringBehaviours::FLEE:
		_isFleeing = false;
		break;

	case SteeringBehaviours::ARRIVE:
		_isArriving = false;
		break;

	case SteeringBehaviours::PURSUIT:
		_isPursuing = false;
		break;

	case SteeringBehaviours::OBSTACLE:
		_isObsAvoid = false;
		break;

	case SteeringBehaviours::WANDER:
		_isWandering = false;
		break;

	default:
		break;
	}
}

bool SteeringBehaviours::GetState(Behaviours behaviour)
{
	switch (behaviour)
	{
	case SteeringBehaviours::SEEK:
		return _isSeeking;
		break;

	case SteeringBehaviours::FLEE:
		return _isFleeing;
		break;

	case SteeringBehaviours::ARRIVE:
		return _isArriving;
		break;

	case SteeringBehaviours::PURSUIT:
		return _isPursuing;
		break;

	case SteeringBehaviours::OBSTACLE:
		return _isObsAvoid;
		break;

	case SteeringBehaviours::WANDER:
		return _isWandering;
		break;

	default:
		break;
	}
}

Vector2D SteeringBehaviours::Seek(BaseTank* tank, Vector2D TargetPos)
{
	Vector2D DesiredVelocity = Vec2DNormalize(TargetPos - tank->GetCentralPosition()) * tank->GetMaxSpeed();

	return (DesiredVelocity - tank->GetVelocity());
}

Vector2D SteeringBehaviours::Flee(BaseTank* tank, Vector2D TargetPos)
{
	Vector2D distanceApart = TargetPos - tank->GetPosition();
	
	if (distanceApart.Length() < _fleeRadius)
	{
		Vector2D DesiredVelocity = Vec2DNormalize(tank->GetCentralPosition() - TargetPos) * tank->GetMaxSpeed() * 2.0f;	//Double the result to make it move not just cancel seek

		return (DesiredVelocity - tank->GetVelocity());
	}

	return (Vector2D(0.0f, 0.0f));
}

Vector2D SteeringBehaviours::Arrive(BaseTank* tank, Vector2D TargetPos, Deceleration deceleration)
{
	Vector2D ToTarget = TargetPos - tank->GetCentralPosition();

	//calculate the distance to the target
	double dist = ToTarget.Length();

	if (dist >= 0.2f)
	{
		//because Deceleration is enumerated as an int, this value is required
		//to provide fine tweaking of the deceleration..
		const double DecelerationTweaker = 0.8f;

		//calculate the speed required to reach the target given the desired
		//deceleration
		double speed = dist / ((double)deceleration * DecelerationTweaker);

		//make sure the velocity does not exceed the max
		speed = min(speed, tank->GetMaxSpeed());

		//from here proceed just like Seek except we don't need to normalize 
		//the ToTarget vector because we have already gone to the trouble
		//of calculating its length: dist. 
		Vector2D DesiredVelocity = ToTarget * speed / dist;

		return (DesiredVelocity - tank->GetVelocity());
	}

	CancelBehavior(Behaviours::ARRIVE);
	return Vector2D(0.0, 0.0);
}

Vector2D SteeringBehaviours::Pursuit(BaseTank* tank, BaseTank* target)
{
	//if the evader is ahead and facing the agent then we can just seek for the evader's current position.
	Vector2D ToEvader = target->GetCentralPosition() - tank->GetCentralPosition();

	float distance = tank->GetCentralPosition().Distance(target->GetCentralPosition());

	if (distance > 30.0f)
	{
		double RelativeHeading = tank->GetHeading().Dot(target->GetHeading());

		if ((ToEvader.Dot(tank->GetHeading()) > 0) &&
			(RelativeHeading < -0.95))  //acos(0.95)=18 degs
		{
			return Seek(tank, target->GetCentralPosition());
		}

		double LookAheadTime = ToEvader.Length() / (tank->GetMaxSpeed() + 75); // target->GetCurrentSpeed());

		//now seek to the predicted future position of the evader
		return Seek(tank, target->GetCentralPosition() + target->GetVelocity() * LookAheadTime);
	}
	else
	{
		return Vector2D(0.0f, 0.0f);
	}
}

Vector2D SteeringBehaviours::ObstacleAvoidance(BaseTank* tank)
{
	vector<GameObject*> obstacles = ObstacleManager::Instance()->GetObstacles();
	vector<GameObject*> mines = ProjectileManager::Instance()->GetVisibleMines(tank);


	for (int i = 0; i < mines.size(); i++)
	{
		obstacles.push_back(mines[i]);
	}

	GameObject* mostThreatening = nullptr;
	Vector2D avoidance = Vector2D(0.0f, 0.0f);

	//find most Threaten
	//double dynamic_length = tank->GetVelocity().Length() / tank->GetMaxSpeed();
	//Vector2D ahead = ((tank->GetCentralPosition() + Vec2DNormalize(tank->GetVelocity())) * dynamic_length);
	//Vector2D ahead2 = (((tank->GetCentralPosition() + Vec2DNormalize(tank->GetVelocity())) * dynamic_length) * 0.5);

	double dynamic_length = tank->GetVelocity().Length() / tank->GetMaxSpeed();
	Vector2D ahead = tank->GetCentralPosition() +  (Vec2DNormalize(tank->GetVelocity())* dynamic_length);
	

	Vector2D fannedHead;
	fannedHead.x = ahead.x * cos(DegsToRads(30)) - ahead.y * sin(DegsToRads(30));
	fannedHead.y = ahead.x * sin(DegsToRads(30)) + ahead.y * cos(DegsToRads(30));

	for (int i = 0; i < obstacles.size(); i++)
	{
		Rect2D rect = obstacles[i]->GetAdjustedBoundingBox();

		bool collision = (Box(tank, obstacles[i])); // PointInBox(ahead, obstacles[i]->GetAdjustedBoundingBox());
		

		// "position" is the character's current position
		if (collision && (mostThreatening == nullptr || Distance(tank->GetCentralPosition(), obstacles[i]->GetCentralPosition()) < Distance(tank->GetCentralPosition(), mostThreatening->GetCentralPosition())))
		{
			mostThreatening = obstacles[i];
		}
	}

	//collision avoidance
	if (mostThreatening != nullptr)
	{
			avoidance = ahead - mostThreatening->GetCentralPosition();
			avoidance = ((Vec2DNormalize(avoidance)) * _maxAvoidForce);
	}

	else
	{
		avoidance = Vec2DNormalize(avoidance) * 0.0f; // nullify the avoidance force
	}
	
	return avoidance;
}

Vector2D SteeringBehaviours::Wander(BaseTank* Tank)
{
	Vector2D circleCenter;

	circleCenter = Tank->GetVelocity();
	//circleCenter.Normalize();
	circleCenter *= _circleDistance;

	Vector2D displacement;

	displacement = Vector2D(0, -1);
	displacement *= 5;
	//
	// Randomly change the vector direction
	// by making it change its current angle
	displacement = SetAngle(displacement, _wanderAngle);
	//
	// Change wanderAngle just a bit, so it
	// won't have the same value in the
	// next game frame.
	_wanderAngle += ((rand() % 30) * 5) - (5 * 0.5);

	Vector2D wanderForce;
	wanderForce = circleCenter + displacement;

	return wanderForce;
}

Vector2D SteeringBehaviours::CombineForces(BaseTank* tank, Vector2D targetPos)
{

	_combinedForce = Vector2D(0.0f, 0.0f);

	if (_isSeeking == true)
	{
		_combinedForce += Seek(tank, targetPos) * 3;
	}

	if (_isFleeing == true)
	{
		_combinedForce += Flee(tank, targetPos);
	}

	if (_isArriving == true)
	{
		_combinedForce += Arrive(tank, targetPos, Deceleration::SLOW);
	}

	if (_isPursuing == true)
	{
		vector<BaseTank*> tankList = TankManager::Instance()->GetVisibleTanks(tank);
		vector<BaseTank*> audible = TankManager::Instance()->GetAudibleTanks(tank);

		if (audible.size() > 0)
		{
			tankList.push_back(audible[0]);
		}

		if (tankList.size() > 0)
		{
			_combinedForce += Pursuit(tank, tankList[0]) * 3;
		}

	}

	if (_isObsAvoid == true)
	{
		_combinedForce += ObstacleAvoidance(tank) * 10.0f;
	}

	if (_isWandering == true)
	{
		_combinedForce += Wander(tank);
	}

	return _combinedForce;
}

// Extra Functions for Obsacle Avoidance
double SteeringBehaviours::Distance(Vector2D a, Vector2D b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

bool SteeringBehaviours::Box(BaseTank* tank, GameObject* object)
{
	Rect2D tankRect = tank->GetAdjustedBoundingBox();
	Rect2D obstacleRect = object->GetAdjustedBoundingBox();

	if (tankRect.x > (obstacleRect.x + obstacleRect.width + 1.0f))
	{
		return false;
	}
	else if ((tankRect.x + tankRect.width) < (obstacleRect.x - 1.0f))
	{
		return false;
	}
	else if (tankRect.y > (obstacleRect.y + obstacleRect.height + 1.0f))
	{
		return false;
	}
	else if ((tankRect.y + tankRect.height) < (obstacleRect.y - 1.0f))
	{
		return false;
	}

	return true;
}

bool SteeringBehaviours::PointInBox(Vector2D ahead, Rect2D obstacle)
{
	if (ahead.x > obstacle.x &&
		ahead.x < (obstacle.x + obstacle.width) &&
		ahead.y > obstacle.y &&
		ahead.y < (obstacle.y + obstacle.height))
	{
		return true;
	}

	return false;
}

//Wander Extra Functions
Vector2D SteeringBehaviours::SetAngle(Vector2D vector, int value)
{
	int len = vector.Length();
	vector.x = cos(value) * len;
	vector.y = sin(value) * len;

	return vector;
}