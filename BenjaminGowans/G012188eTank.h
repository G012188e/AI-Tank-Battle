#ifndef G012188ETANK_H
#define G012188ETANK_H

#include <SDL.h>

#include "../../Commons.h"

#include "../../BaseTank.h"
#include "../../Waypoint.h"
#include "../../GameObject.h"

#include "G012188eSteeringBehaviours.h"
#include "G012188ePathFinding.h"

using namespace::std;

//---------------------------------------------------------------

enum G012188e_TANK_STATE
{
	STATE_IDLE,
	STATE_WANDER,
	STATE_FLEE,
	STATE_CHASE,
	STATE_FIRE,
	STATE_PATROL,
	STATE_COLLECTABLE
};

class G012188eTank : public BaseTank
{
	//---------------------------------------------------------------
public:
	G012188eTank(SDL_Renderer* renderer, TankSetupDetails details);
	~G012188eTank();

	string OutputStateName();

	void ChangeState(G012188e_TANK_STATE newState);
	void UpdateState(float deltaTime);
	void RunState(float deltaTime);

	vector<BaseTank*> GetSeenTanks()										{ return mTanksICanSee; }

	void Update(float deltaTime, SDL_Event e);

	void FindPath(Vector2D start, Vector2D end);

	void Patrol();
	void Chase();
	void Shoot(float deltaTime);
	void Collectables();

	//---------------------------------------------------------------
protected:
	void	MoveInHeadingDirection(float deltaTime);

private:
	
	vector<BaseTank*>	mTanks;

	Vector2D			mTarget;
	
	Vector2D			mMousePos;
	SteeringBehaviours* mSteering;

	G012188ePathFinding* mPathFinding;
	vector <Vector2D>	 mPath;

	G012188e_TANK_STATE mCurrentState;

	float				mShootRange;
	float				mMineTimer;

	int					 mCurrentPathTarget;
	bool				 mIsPathFinding;
	bool				 mArrived;
};

//-----------
#endif //G012188ETANK_H