#include "G012188eTank.h"

#include "../../TankManager.h"
#include "../../WaypointManager.h"
#include "../../PickUpManager.h"
#include "../../Commons.h"
#include "../../C2DMatrix.h"

//--------------------------------------------------------------------------------------------------

G012188eTank::G012188eTank(SDL_Renderer* renderer, TankSetupDetails details)
	: BaseTank(renderer, details)
{
	mSteering = new SteeringBehaviours();
	mPathFinding = new G012188ePathFinding();

	mMousePos = Vector2D(0.0f, 0.0f);

	mShootRange = 200.0f;
	mMineTimer = 0.0f;

	mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(0)->GetPosition());
	mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(1)->GetPosition());
	mCurrentPathTarget = 0;
	mIsPathFinding = false;
	mCurrentState = STATE_IDLE;
}

//--------------------------------------------------------------------------------------------------

G012188eTank::~G012188eTank()
{
}

//--------------------------------------------------------------------------------------------------

void G012188eTank::ChangeState(G012188e_TANK_STATE newState)
{
	if (newState != mCurrentState)
	{
		mIsPathFinding = false;
		mCurrentPathTarget = 0;
		mSteering->CancelBehavior(SteeringBehaviours::Behaviours::SEEK);
		mSteering->CancelBehavior(SteeringBehaviours::Behaviours::FLEE);
		mSteering->CancelBehavior(SteeringBehaviours::Behaviours::AVOIDANCE);
		mSteering->CancelBehavior(SteeringBehaviours::Behaviours::ARRIVE);
		mSteering->CancelBehavior(SteeringBehaviours::Behaviours::PURSUIT);
		mSteering->CancelBehavior(SteeringBehaviours::Behaviours::OBSTACLE);

		mCurrentState = newState;

		//Start new state
		switch (mCurrentState)
		{
			case STATE_IDLE:
			{
				//Stands still
				break;
			}
			case STATE_WANDER:
			{
				mSteering->ActivateBehavior(SteeringBehaviours::Behaviours::OBSTACLE);

				break;
			}
			case STATE_CHASE:
			{
				mSteering->ActivateBehavior(SteeringBehaviours::Behaviours::OBSTACLE);
				mSteering->ActivateBehavior(SteeringBehaviours::Behaviours::PURSUIT);

				break;
			}
			case STATE_FLEE:
			{
				mSteering->ActivateBehavior(SteeringBehaviours::Behaviours::OBSTACLE);
				mSteering->ActivateBehavior(SteeringBehaviours::Behaviours::FLEE);

				break;
			}
			case STATE_FIRE:
			{
				mSteering->ActivateBehavior(SteeringBehaviours::Behaviours::OBSTACLE);
				mSteering->ActivateBehavior(SteeringBehaviours::Behaviours::PURSUIT);

				break;
			}
			case STATE_PATROL:
			{
				mSteering->ActivateBehavior(SteeringBehaviours::Behaviours::OBSTACLE);

				mPath.clear();
				mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(0)->GetPosition());
				mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(1)->GetPosition());
				mPath.push_back(Vector2D(230.0f, 500.0f));
				mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(3)->GetPosition());
				mPath.push_back(Vector2D(500.0f, 500.0f));
				mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(5)->GetPosition());
				mPath.push_back(Vector2D(750.0f, 500.0f));
				mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(7)->GetPosition());
				mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(6)->GetPosition());
				mPath.push_back(Vector2D(750.0f, 140.0f));
				mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(4)->GetPosition());
				mPath.push_back(Vector2D(500.0f, 140.0f));
				mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(2)->GetPosition());
				mPath.push_back(Vector2D(230.0f, 140.0f));

				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(0)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(1)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(3)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(9)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(8)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(11)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(15)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(23)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(24)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(25)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(26)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(22)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(21)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(20)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(19)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(18)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(17)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(16)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(12)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(13)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(10)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(5)->GetPosition());
				//mPath.push_back(WaypointManager::Instance()->GetWaypointWithID(4)->GetPosition());

				//Find closest waypoint
				double distanceTank = 100;
				Waypoint* tankWaypoint = WaypointManager::Instance()->GetWaypointWithID('0');

				for each (Waypoint* node in WaypointManager::Instance()->GetAllWaypoints())
				{
					double x = abs(node->GetPosition().x - this->GetCentralPosition().x);
					double y = abs(node->GetPosition().y - this->GetCentralPosition().y);
					double distance = sqrt(x + y);

					if (distance < distanceTank)
					{
						distanceTank = distance;
						tankWaypoint = node;
					}
				}

				for (int i = 0; i < (int)mPath.size(); i++)
				{
					if (tankWaypoint->GetPosition() == mPath[i])
					{
						mCurrentPathTarget = i;
					}
				}

				mSteering->ActivateBehavior(SteeringBehaviours::Behaviours::SEEK);
				mIsPathFinding = true;
				break;
			}

			case STATE_COLLECTABLE:
				mSteering->ActivateBehavior(SteeringBehaviours::Behaviours::OBSTACLE);
				mSteering->ActivateBehavior(SteeringBehaviours::Behaviours::SEEK);

				this->FindPath(this->GetCentralPosition(), PickUpManager::Instance()->GetAllPickUps()[0]->GetCentralPosition());
				mIsPathFinding = true;
				break;
		}

		//cout << "Changed state to: " << OutputStateName() << endl;
	}
}

//--------------------------------------------------------------------------------------------------

void G012188eTank::Update(float deltaTime, SDL_Event e)
{
	switch(e.type)
	{
	case SDL_MOUSEBUTTONDOWN:

		mMousePos.x = e.button.x;
		mMousePos.y = e.button.y;

		//cout << "Clicks Happened at x:" << mMousePos.x << " y:" << mMousePos.y << "\n";

		break;
	}

	UpdateState(deltaTime);
	RunState(deltaTime);

	//Call parent update.
	BaseTank::Update(deltaTime, e);
}

//--------------------------------------------------------------------------------------------------

void G012188eTank::UpdateState(float deltaTime)
{
	mMineTimer += deltaTime;

	if (mMineTimer > 5.00f)
	{
		this->DropAMine();
		mMineTimer = 0.0f;
	}

	switch (mCurrentState)
	{
		case STATE_IDLE:
		{
			ChangeState(STATE_PATROL);
			break;
		}
		case STATE_CHASE:
		{
			if (mTanksICanSee.empty() && mTanksICanHear.empty())
			{
				ChangeState(STATE_PATROL);
			}
			else
			{
				//Rotate man to face enemy tank.
				Vector2D ToEvader;
					
				if (mTanksICanSee.size() != 0)
				{
					ToEvader = Vec2DNormalize((mTanksICanSee[0]->GetCentralPosition() + mTanksICanSee[0]->GetHeading()) - this->GetCentralPosition());
				}

				if (mTanksICanHear.size() != 0)
				{
					ToEvader = Vec2DNormalize((mTanksICanHear[0]->GetCentralPosition() + mTanksICanHear[0]->GetHeading()) - this->GetCentralPosition());
				}

				double dot = ToEvader.Dot(mManFireDirection);

				if (dot < 0.99)
				{
					RotateManByRadian(kManTurnRate, mManFireDirection.Sign(ToEvader), deltaTime);
				}
				if (mTanksICanSee.size() != 0)
				{
					if (mTanksICanSee[0]->GetCentralPosition().Distance(this->GetCentralPosition()) < mShootRange)
					{
						ChangeState(STATE_FIRE);
					}
				}

				if (mTanksICanHear.size() != 0)
				{
					if (mTanksICanHear[0]->GetCentralPosition().Distance(this->GetCentralPosition()) < mShootRange)
					{
						ChangeState(STATE_FIRE);
					}
				}
			}

			break;
		}
		case STATE_FLEE:
		{
			break;
		}
		case STATE_FIRE:
		{		
			if (mTanksICanSee.empty() && mTanksICanHear.empty())
			{
				ChangeState(STATE_PATROL);
			}
			if (!mTanksICanSee.empty())
			{
				if (mTanksICanSee[0]->GetCentralPosition().Distance(this->GetCentralPosition()) > mShootRange)
				{
					ChangeState(STATE_CHASE);
				}
			}
			if (!mTanksICanHear.empty())
			{
				if (mTanksICanHear[0]->GetCentralPosition().Distance(this->GetCentralPosition()) > mShootRange)
				{
					ChangeState(STATE_CHASE);
				}
			}
			break;
		}
		case STATE_PATROL:
		{
			if (!mTanksICanSee.empty())
			{
				ChangeState(STATE_CHASE);
			}

			if (!mTanksICanHear.empty())
			{
				ChangeState(STATE_CHASE);
			}

			if (!PickUpManager::Instance()->GetAllPickUps().empty() && mTanksICanHear.empty() && mTanksICanSee.empty())
			{
				ChangeState(STATE_COLLECTABLE);
			}

			break;
		}
		case STATE_COLLECTABLE:
		{
			if (mTanksICanSee.size() > 0 || mTanksICanHear.size() > 0)
			{
				ChangeState(STATE_CHASE);
			}
			else if (mVelocity.isZero())
			{
				ChangeState(STATE_PATROL);
			}
			break;
		}
		default:
			break;
	}
}

//--------------------------------------------------------------------------------------------------

void G012188eTank::RunState(float deltaTime)
{
	switch (mCurrentState)
	{
	case STATE_IDLE:
	{
		mVelocity = Vector2D(0.0f, 0.0f);
		break;
	}
	case STATE_CHASE:
	{
		Chase();
		break;
	}
	case STATE_FLEE:
	{
		break;
	}
	case STATE_FIRE:
	{
		Chase();
		Shoot(deltaTime);
		break;
	}
	case STATE_PATROL:
	{
		//Rotate man to face front of tank.	
		Vector2D ToEvader = Vec2DNormalize(mHeading * -1.0f);

		ToEvader.Normalize();
		double dot = ToEvader.Dot(mManFireDirection);


		float toTargetAng = atan2(ToEvader.x, ToEvader.y);
		float turretAng = atan2(mManFireDirection.x, mManFireDirection.y);
		float deltaAng = toTargetAng - turretAng;

		// when delta ang is 0 sin is also 0, which means dont move
		// as delta ang increases, the sin is postive until it gets to 180 deg then 
		if (sin(deltaAng) > 0)
		{
			RotateManByRadian(kManTurnRate, -1, deltaTime);
		}
		else
		{
			RotateManByRadian(kManTurnRate, 1, deltaTime);
		}

		if (mIsPathFinding == true)
		{
			this->Patrol();
		}
		break;
	}
	case STATE_COLLECTABLE:
	{
		if (mIsPathFinding == true)
		{
			this->Collectables();
		}
		break;
	}
	default:
		break;
	}
}

//--------------------------------------------------------------------------------------------------

void G012188eTank::MoveInHeadingDirection(float deltaTime)
{
	//Get the force that propels in current heading.
	Vector2D force = mSteering->CombineForces(this, mTarget);

	if (force != force)
	{
		force = Vector2D(0.0f, 0.0f);
	}

	//Acceleration = Force/Mass
	Vector2D acceleration = force/GetMass();

	//cout << mVelocity;

	if (force == Vector2D( 0.0f, 0.0f ))
	{
		mVelocity = { 0.0f, 0.0f };
	}

	//Update velocity.
	mVelocity += acceleration * deltaTime;
	
	//Don't allow the tank does not go faster than max speed.
	mVelocity.Truncate(GetMaxSpeed()); //TODO: Add Penalty for going faster than MAX Speed.

	//Finally, update the position.
	Vector2D newPosition = GetPosition();
		newPosition.x += mVelocity.x*deltaTime;
		newPosition.y += (mVelocity.y/**-1.0f*/)*deltaTime; //Y flipped as adding to Y moves down screen.
	SetPosition(newPosition);

	//update heading
	Vector2D newHeading = mVelocity;
	if (newHeading.x == 0.0f && newHeading.y == 0.0f)
		newHeading = mHeading;
	Vec2DNormalize(newHeading);

	if (force != Vector2D(0.0f, 0.0f))
	{
		RotateHeadingToFacePosition(GetCentralPosition() + newHeading * 100.0f, deltaTime);
	}

	if (mVelocity != mVelocity)
	{
		mVelocity = { 0.0f, 0.0f };
	}
}

//--------------------------------------------------------------------------------------------------

void G012188eTank::Patrol()
{

	Vector2D ToTarget = mPath[mCurrentPathTarget] - this->GetCentralPosition();
	bool finishedPath = false;

	// If not made it to next next
	if (abs(ToTarget.Length()) >= 10.0f)
	{
		mArrived = false;
	}

	// If made it to next node
	if (ToTarget.Length() <= 10.0f && mArrived == false)
	{
		mArrived = true;
	}

	//Last node decision
	if ((mArrived == true) && (mCurrentPathTarget == (mPath.size() - 1)))
	{
		if (mCurrentState == STATE_PATROL)
		{
			mCurrentPathTarget = 0;
		}
		else
		{
			mIsPathFinding = false;
			mSteering->CancelBehavior(SteeringBehaviours::Behaviours::SEEK);
			mSteering->CancelBehavior(SteeringBehaviours::Behaviours::OBSTACLE);
		}

		finishedPath = true;
	}

	// Incrment to next node if not fiNshed route
	if ((mArrived == true) && (finishedPath == false))
	{
		mCurrentPathTarget++;
	}

	// decide to skip last node if further away
	if (mCurrentPathTarget == (mPath.size() - 2))
	{
		Vector2D ToTargetNext = mPath[mCurrentPathTarget + 1] - this->GetCentralPosition();

		if (ToTarget.Length() > ToTargetNext.Length())
		{
			mCurrentPathTarget++;
		}
	}

	mTarget = mPath[mCurrentPathTarget];
}

//--------------------------------------------------------------------------------------------------

void G012188eTank::Chase()
{
	if(mTanksICanSee.size() > 0)
	mTarget = mTanksICanSee[0]->GetCentralPosition();

	else if (mTanksICanHear.size() > 0)
		mTarget = mTanksICanHear[0]->GetCentralPosition();
}

//--------------------------------------------------------------------------------------------------

void G012188eTank::Shoot(float deltaTime)
{
	Vector2D ToEvader;
	float turretAng;
	float toTargetAng;

	//Rotate man to face enemy tank.
	if (!mTanksICanSee.empty())
	{
		ToEvader = (mTanksICanSee[0]->GetCentralPosition() - this->GetCentralPosition());
	}

	else if (!mTanksICanHear.empty())
	{
		ToEvader = (mTanksICanHear[0]->GetCentralPosition() - this->GetCentralPosition());
	}

	ToEvader.Normalize();
	double dot = ToEvader.Dot(mManFireDirection);


	toTargetAng = atan2(ToEvader.x, ToEvader.y);
	turretAng = atan2(mManFireDirection.x, mManFireDirection.y);
	float deltaAng = toTargetAng - turretAng;

	// when delta ang is 0 sin is also 0, which means dont move
	// as delta ang increases, the sin is postive until it gets to 180 deg then 
	if (sin(deltaAng) > 0)
	{
		RotateManByRadian(kManTurnRate, -1, deltaTime);
	}
	else
	{
		RotateManByRadian(kManTurnRate, 1, deltaTime);
	}

	if (dot > 0.99)
	{
		this->FireABullet();
	}
}

//--------------------------------------------------------------------------------------------------

void G012188eTank::Collectables()
{
	Vector2D ToTarget = mPath[mCurrentPathTarget] - this->GetCentralPosition();
	//ToTarget.Normalize();
	double dist = ToTarget.Length();


	// If not made it to next next
	if (dist >= 5.0f)
	{
		mArrived = false;
	}

	// If made it to next node
	if (dist <= 5.0f && mArrived == false)
	{
		mArrived = true;
	}

	//Last node decision
	if ((mArrived == true) && (mCurrentPathTarget == mPath.size() -1))
	{
			mIsPathFinding = false;
			mSteering->CancelBehavior(SteeringBehaviours::Behaviours::SEEK);
			mSteering->CancelBehavior(SteeringBehaviours::Behaviours::OBSTACLE);
	}

	// Incrment to next node if not finshed route
	if (mArrived == true && (mCurrentPathTarget != mPath.size() - 1))
	{
		mCurrentPathTarget++;
	}

	// decide to skip last node if further away
	if (mCurrentPathTarget == (mPath.size() - 2))
	{
		Vector2D ToTargetNext = mPath[mCurrentPathTarget + 1] - this->GetCentralPosition();

		if (ToTarget.Length() > ToTargetNext.Length())
		{
			mCurrentPathTarget++;
		}
	}

	mTarget = mPath[mCurrentPathTarget];
}

//--------------------------------------------------------------------------------------------------

string G012188eTank::OutputStateName()
{
	switch (mCurrentState)
	{
	case STATE_IDLE:
		return "Idle";
		break;
	case STATE_WANDER:
		return "Wander";
		break;
	case STATE_FLEE:
		return "Flee";
		break;
	case STATE_CHASE:
		return "Chase";
		break;
	case STATE_FIRE:
		return "Fire";
		break;
	case STATE_PATROL:
		return "Patrol";
		break;
	case STATE_COLLECTABLE:
		return "Collectable Hunting";
		break;

	default:
		return "You are an idiot";
		break;
	}
}

//--------------------------------------------------------------------------------------------------

void G012188eTank::FindPath(Vector2D start, Vector2D end)
{
	double distanceTank = 100;
	Waypoint* tankWaypoint = WaypointManager::Instance()->GetWaypointWithID('0');

	double distanceTarget = 100;
	Waypoint* targetWaypoint = WaypointManager::Instance()->GetWaypointWithID('0');

	//Find closest node to tank
	for each (Waypoint* node in WaypointManager::Instance()->GetAllWaypoints())
	{
		double x = abs(node->GetPosition().x - this->GetCentralPosition().x);
		double y = abs(node->GetPosition().y - this->GetCentralPosition().y);
		double distance = sqrt(x + y);

		if (distance < distanceTank)
		{
			distanceTank = distance;
			tankWaypoint = node;
		}

		//Find closest node to destination
		x = abs(node->GetPosition().x - end.x);
		y = abs(node->GetPosition().y - end.y);
		distance = sqrt(x + y);

		if (distance < distanceTarget)
		{
			distanceTarget = distance;
			targetWaypoint = node;
		}
	}

	mPath.clear();
	mCurrentPathTarget = 0;
	mPath = mPathFinding->A_Star(tankWaypoint, targetWaypoint, end);

	mIsPathFinding = false;
}

//--------------------------------------------------------------------------------------------------
