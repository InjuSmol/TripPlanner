#pragma once

#include <map>
using namespace std;

class WeightedEdge {
private:
	wstring node1;
	wstring node2;
	float weight;

public:
	WeightedEdge(wstring initNode1, wstring initNode2, float initWeight) {
		this->node1 = initNode1;
		this->node2 = initNode2;
		this->weight = initWeight;
	}
	~WeightedEdge() {
		// NO POINTERS TO CLEAN UP
	}

	wstring getNode1() { return node1; }
	wstring getNode2() { return node2; }
	float getWeight() { return weight; }
};

template <typename T> 
class WeightedGraph {
private:
	std::map<wstring, T*> nodes;
	std::map<wstring, WeightedEdge*> edges;

public:
	WeightedGraph() {}

	void getKeys(vector<wstring>* keys) {
		for (const auto& keyPair : nodes) {
			wstring key = keyPair.first;
			keys->push_back(key);
		}
	}

	bool nodeExists(wstring testNode) {
		bool exists = this->nodes.count(testNode) > 0;
		return exists;
	}
	bool edgeExists(wstring testEdge){
	    bool exists = this -> edges.count(testEdge) > 0; 
	    return exists;
	}

	wstring getEdgeId(wstring node1, wstring node2) {
		return node1 + L"-" + node2;
	}

	void addNode(wstring nodeId, T* nodeData) {
		this->nodes[nodeId] = nodeData;
	}

	T* getNodeData(wstring nodeId) {
		return this->nodes[nodeId];
	}

	void addEdge(wstring node1, wstring node2, float weight) {
		wstring edgeId = this->getEdgeId(node1, node2);
		this->edges[edgeId] = new WeightedEdge(node1, node2, weight);
	}

	void removeEdge(wstring node1, wstring node2) {
		wstring edgeId = this->getEdgeId(node1, node2);
		WeightedEdge* edge = this->edges[edgeId];
	}

	void getNeighbors(vector<wstring>* neighbors, wstring node) {
		for (const auto& keyPair : edges) {
			wstring key = keyPair.first;
			int index = key.find(L"-");
			wstring startNode = key.substr(0, index);
			if (startNode.compare(node) == 0) {
				wstring neighbor = key.substr(index + 1);
				neighbors->push_back(neighbor);
			}
		}
	}

	bool areNeighbors(wstring node1, wstring node2) {
		vector<wstring> neighbors;
		this->getNeighbors(&neighbors, node1);
		return std::find(neighbors.begin(), neighbors.end(), node2) != neighbors.end();
	}

	float getNeighborWeight(wstring node1, wstring node2) {
		if (areNeighbors(node1, node2)) {
			wstring edgeId = this->getEdgeId(node1, node2);
			return this->edges[edgeId]->getWeight();
		}
		return 0.0f;
	}

	void findPath(vector<wstring>* path, wstring node1, wstring node2) {
		wcout << L"Finding path from " << node1 << L" to " << node2 << endl;

		// ADD YOUR CODE HERE

		// IF EITHER AIRPORT DOESN'T EXIST, DO NOTHING
		if ( (!this->nodeExists(node1)) || (!this->nodeExists(node2)) ) {
			return;
		}

		// ADD THE STARTING NODE TO THE PATH
		path->push_back(node1);

		// THESE ARE THE NODES WE'VE VISITED, WE USE THIS
		// TO PREVENT LOOPS
		map<wstring, wstring> visited;
		visited[node1] = node1;

		int i; // USED FOR ITERATION

		// LOOP WILL EXIT IF NO PATH IS POSSIBLE
		while (path->size() > 0)
		{
			// GET LAST NODE IN THE PATH
			wstring last = (*path)[(path->size() - 1)];

			// GET ALL THE NEIGHBORS FOR THE LAST
			// NODE IN THE PATH
			vector<wstring> neighbors;
			getNeighbors(&neighbors, last);

			// GO THROUGH ALL THE NEIGHBORS AND KEEP TRACK OF WHICH IS THE CLOSEST
			// THAT WE HAVE NOT YET BEEN TO
			int closestIndex = -1;
			double closestDistance = 100000.0; // THIS IS DELIBERATELY LARGE
			for (int i = 0; i < neighbors.size(); i++) {
				wstring testNeighbor = neighbors[i];

				if (testNeighbor.compare(node2) == 0) {
					// THE NEIGHBOR IS THE DESTINATION, SO ADD IT TO THE PATH AND WE'RE DONE
					path->push_back(testNeighbor);
					return;
				}

				// WE WILL CONSIDER IT IF WE HAVE NOT YET VISITED IT
				if (visited.count(testNeighbor) == 0) {
					wstring id = this->getEdgeId(last, testNeighbor);
					WeightedEdge* edge = this->edges[id];
					if (edge->getWeight() < closestDistance) {
						closestIndex = i;
						closestDistance = edge->getWeight();
					}
				}
			}

			if (closestIndex >= 0) {
				// LET'S ADD THE CLOSEST NODE
				wstring closestNode = neighbors[closestIndex];
				visited[closestNode] = closestNode;
				path->push_back(closestNode);
			}
			else if (path->size() > 0) {
				// REMOVE THE LAST ELEMENT, IF THERE IS ONE
				path->pop_back();
			}
		}
	}
};