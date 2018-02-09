#include "G012188ePathFinding.h"

#include "G012188eTank.h"
#include "../../TankManager.h"
#include "../../WaypointManager.h"
#include "../../Waypoint.h"

G012188ePathFinding::G012188ePathFinding()
{
}


G012188ePathFinding::~G012188ePathFinding()
{
}

//------------------------------------------------------------------------------------

void G012188ePathFinding::SearchBFS(Waypoint* node)
{
	queue < Waypoint* > nodeQueue;
	vector <int> visitedNodes;

	nodeQueue.push(node);

	//std::cout << "BFS started from " <<  node->GetID() << " : " << endl;

	while (!nodeQueue.empty())
	{
		node = nodeQueue.front();
		nodeQueue.pop();

		//std::cout << node->GetID() << " ";

		vector<int> children = node->GetConnectedWaypointIDs();

		for each (int node in children)
		{
			if (find(visitedNodes.begin(), visitedNodes.end(), node) == visitedNodes.end()) // Not found
			{
				visitedNodes.push_back(node);

				nodeQueue.push(WaypointManager::Instance()->GetWaypointWithID(node));
			}
		}
	}

	//std::cout << endl;
}

//------------------------------------------------------------------------------------

void G012188ePathFinding::SearchDFS(Waypoint* node)
{
	stack < Waypoint* > nodeStack;
	vector <int> visitedNodes;

	nodeStack.push(node);

	//std::cout << "DFS started from " << node->GetID() << " : " << endl;

	while (!nodeStack.empty())
	{
		node = nodeStack.top();
		nodeStack.pop();

		//std::cout << node->GetID() << " ";

		if (find(visitedNodes.begin(), visitedNodes.end(), node->GetID()) == visitedNodes.end()) // Not found
		{
			visitedNodes.push_back(node->GetID());

			vector<int> children = node->GetConnectedWaypointIDs();

			for each (int node in children)
			{
				nodeStack.push(WaypointManager::Instance()->GetWaypointWithID(node));
			}

		}
	}

	//std::cout << endl;
}

//------------------------------------------------------------------------------------

vector <Vector2D> G012188ePathFinding::A_Star(Waypoint* startNode, Waypoint* endNode, Vector2D destination)
{
	vector <Node*> openList;
	vector <Node*> closedList;

	Node* child;
	Node* currentNode;
	Node* start;

	//Create the nodes from the waypoints
	int noOfNodes = WaypointManager::Instance()->GetAllWaypoints().size();
	vector <Node*> availableNodes;

	for (int i = 0; i < noOfNodes; i++)
	{
		Node* node = new Node();
		node->fScore = 0;
		node->gScore = 0;
		node->hScore = 0;
		node->parent = nullptr;
		node->waypoint = WaypointManager::Instance()->GetWaypointWithID(i);

		availableNodes.push_back(node);
	}

	//Get first node
	for (int i = 0; i < availableNodes.size(); i++)
	{
		if (availableNodes[i]->waypoint == startNode)
		{
			start = availableNodes[i];
		}
	}

	//Push start node on to open list
	openList.push_back(start);
	currentNode = start;

	//std::cout << "Start at : " << startNode->GetID() << "; End at : " << endNode->GetID() << endl;

	//Run until no nodes left
	while (!openList.empty())
	{
		//Find Lowest fScore in openList
		int lowestIndex = 0;

		for (int i = 0; i < openList.size(); i++)
		{
			if (openList[i]->fScore < openList[lowestIndex]->fScore)
			{
				lowestIndex = i;
			}
		}

		currentNode = openList[lowestIndex];

		//Ending final path sorting / return
		if (currentNode->waypoint == endNode)
		{
			Node* curr = currentNode;
			vector <Vector2D> path;

			path.push_back(destination);

			while (curr->parent)
			{
				//std::cout << curr->waypoint->GetID();
				path.push_back(curr->waypoint->GetPosition());
				curr = curr->parent;
			}
			//Need to add in start node
			//std::cout << curr->waypoint->GetID();
			path.push_back(curr->waypoint->GetPosition());

			//std::cout << endl; // << "Done!!!" << endl;
			std::reverse(path.begin(), path.end());
			return path;
		}

		openList.erase(remove(openList.begin(), openList.end(), currentNode), openList.end());
		closedList.push_back(currentNode);

		//Get the children of the node
		vector <int> children = currentNode->waypoint->GetConnectedWaypointIDs();

		for each (int kid in children)
		{
			for (int i = 0; i < availableNodes.size(); i++)
			{
				if (availableNodes[i]->waypoint->GetID() == kid)
				{
					child = availableNodes[i];
				}
			}

			if (find(closedList.begin(), closedList.end(), child) != closedList.end()) //Is it in closedList
			{
				//Not a valid route to take
				continue;
			}

			int gScore = currentNode->gScore + 1; // +1 becasue its a direct  child
			bool bestGscore = false;

			if (find(openList.begin(), openList.end(), child) == openList.end()) //Is it not in openList
			{
				bestGscore = true;
				child->hScore = Heuristic(child->waypoint->GetPosition(), endNode->GetPosition());
				openList.push_back(child);
			}
			else if (gScore < child->gScore)
			{
				//already seen it but it was worst last time we found it
				bestGscore = true;
			}

			if (bestGscore == true)
			{
				//Found optimal node for path
				child->parent = currentNode;
				child->gScore = gScore;
				child->fScore = child->gScore + child->hScore;

			}
		}
	}
}

//------------------------------------------------------------------------------------

int G012188ePathFinding::Heuristic(Vector2D a, Vector2D b)
{
	int d1 = abs(b.x - a.x);
	int d2 = abs(b.y - a.y);

	return d1 + d2;
}
