#pragma once

#include <list>
#include <queue>
#include <Stack>

#include "../../Commons.h"

using namespace::std;

class Waypoint;
class WaypointManager;
class G012188eTank;
class TankManager;

struct Node
{
	int fScore;
	int gScore;
	int hScore;

	Node* parent;
	Waypoint* waypoint;
};

class G012188ePathFinding
{
public:
	G012188ePathFinding();
	~G012188ePathFinding();

	void SearchBFS(Waypoint* node);
	void SearchDFS(Waypoint* node);

	vector <Vector2D> A_Star(Waypoint* startNode, Waypoint* endNode, Vector2D destination);

	int Heuristic(Vector2D a, Vector2D b);

private:

	//list <Waypoint*> _totalPath;
};

