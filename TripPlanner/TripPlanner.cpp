// C++ LIBRARIES FOR I/O AND TEXT
#include <sstream>
#include <string>
#include <iostream>

// OUR TYPES
#include "cppTPS.h"
#include "cppTPS_Transaction.h"
#include "WeightedGraph.h"
#include "Airport.h"

class AppendStop_Transaction : public cppTPS_Transaction {
private:
	wstring code;
	vector<wstring>* tripStops;

public:
    AppendStop_Transaction(vector<wstring>* initStops, wstring initCode) {
		this->code = initCode;
		this->tripStops = initStops;
    }

    /**
     * This method is called by jTPS when a transaction is executed.
     */
    virtual void doTransaction() {
		this->tripStops->push_back(this->code);
    }

    /**
     * This method is called by jTPS when a transaction is undone.
     */
    virtual void undoTransaction() {
		this->tripStops->pop_back();
    }

    virtual wstring toString() {
		return L"Appending Stop";
    }
};

vector<wstring> stops;
WeightedGraph<Airport> graph;
cppTPS tps;

void initAllAirports();
void initEdge(wstring node1, wstring node2);
void displayAirports();
void displayCurrentTrip();
void displayMenu();
bool processUserInput();

int main() {
	initAllAirports();

	// LOOP FLAG VARIABLE
	bool keepGoing = true;
	while (keepGoing) {
		displayAirports();
		displayCurrentTrip();
		displayMenu();
		keepGoing = processUserInput();
	}
	wcout << L"GOODBYE";
    return 0;
}

void displayAirports() {
	// LIST OF AIRPORTS
	wstring lastStop =L"";
	if (!stops.empty()){lastStop = stops[stops.size()-1];}
	wcout << L"\n\nAIRPORTS YOU CAN TRAVEL TO AND FROM:" << endl;
	vector<wstring> codes;
	graph.getKeys(&codes);
	for (int i = 0; i < codes.size(); i++) {
		if ((i % 10) == 0) {
			wcout << "\t";
		}
		
		
		if (!stops.empty()){
		    if (graph.edgeExists(graph.getEdgeId(lastStop, codes[i]))){
		        wcout << codes[i];
		        if (i < (codes.size() - 1))
			    wcout << ", ";
		    if ((i % 10) == 9) {
			    wcout << endl;
		    }
		        
		    }
		}
		else{
		    wcout << codes[i];
		    if (i < (codes.size() - 1))
			    wcout << ", ";
		    if ((i % 10) == 9) {
			    wcout << endl;
		    }
	}}
	wcout << endl << endl;
}

void displayCurrentTrip() {
	// DISPLAY YOUR CURRENT TRIP STOPS:
	wstringstream wss;
	wss << L"Current Trip Stops: " << endl;
	for (int i = 0; i < stops.size(); i++) {
		wss << L"\t" << (i + 1) << L". " << stops[i] << endl;
	}
	wss << endl;
	wss << L"Current Trip Legs: " << endl;
	int legNum = 1;
	float tripDistance = 0.0f;
	float legDistance = 0.0f;
	for (int i = 0; i < stops.size(); i++) {
		wstring lastStop, nextStop;
		legDistance = 0.0f;

		// IS THERE ANOTHER LEG?
		if (legNum < stops.size()) {
			wss << L"\t" << (i + 1) << L". ";

			// GET THE LAST STOP
			lastStop = stops[legNum - 1];

			// AND GET THE NEXT STOP
			nextStop = stops[legNum];

			// FIND THE ROUTE FOR THIS LEG
			vector<wstring> route;
			graph.findPath(&route, lastStop, nextStop);

			if (route.size() < 2) {
				wcout << L"No Route Found from " << lastStop << L" to " << nextStop << endl;
			}
			else {
				for (int i = 0; i < route.size() - 1; i++) {
					Airport* a1 = graph.getNodeData(route[i]);
					Airport* a2 = graph.getNodeData(route[i + 1]);
					float distance = Airport::calculateDistance(a1, a2);
					legDistance += distance;
					if (i == 0)
						wss << a1->getCode();
					wss << L"-" << a2->getCode();
				}
				wss << L" (Leg Distance: " << legDistance << L" miles)" << endl;
			}

			// MOVE TO THE NEXT LEG
			legNum++;
			tripDistance += legDistance;
		}
	}
	wss << L"Total Trip Distance: " << tripDistance << L" miles" << endl << endl;
	wstring output = wss.str();
	wcout << output;
}

void displayMenu() {
	// DISPLAY THE MENU
	wcout << "ENTER A SELECTION" << endl;
	wcout << "A) Add an Airport to the Graph" << endl;
	wcout << "B) Add an Edge to the Graph" << endl;
	wcout << "S) Add a Stop to your Trip" << endl;
	wcout << "U) Undo" << endl;
	wcout << "R) Redo" << endl;
	wcout << "E) Empty Trip" << endl;
	wcout << "Q) Quit" << endl;
	wcout << "-";
}

bool processUserInput() {
	// GET THE USER SELECTION
	wstring entry;
	wcin >> entry;

    if (entry.compare(L"A") == 0){
        wstring initCode; 
        int initLatitudeDegrees;
	    int initLatitudeMinutes;
	    int initLongitudeDegrees;
	    int initLongitudeMinutes;    
        
        wcout << "\nEnter new Airport Code: ";  
        wcin >> initCode;
        wcout << "\nEnter new Airport Latitude Degree: ";  
        wcin >> initLatitudeDegrees;
        wcout << "\nEnter new Airport LatitudeMinutes: ";  
        wcin >> initLatitudeMinutes;
        wcout << "\nEnter new Airport Longitude Degrees: ";  
        wcin >> initLongitudeDegrees;
        wcout << "\nEnter new Airport Longitude Minutes: ";  
        wcin >> initLongitudeMinutes;
        Airport* newAirport = new Airport(initCode, initLatitudeDegrees, initLatitudeMinutes, initLongitudeDegrees, initLongitudeMinutes);
        graph.addNode(newAirport -> getCode(), newAirport);
        
    }
    else if (entry.compare(L"B") == 0) {
        wstring node1; 
        wstring node2; 
        wcout << "\nEnter new Edge first Airport: ";  
        wcin >> node1; 
        wcout << "\nEnter new Edge Second Airport: "; 
        wcin >> node2; 
        if (graph.nodeExists(node1) && graph.nodeExists(node2)){
            if (!graph.edgeExists(graph.getEdgeId(node1, node2))){
                wcout <<"\nNew Edge has been added"; 
                initEdge(node1, node2); 
            }
            else {wcout <<"\nDUPLICATE EDGE ERROR - NO EDGE ADDED";}
        }
        else {wcout <<"\nINVALID AIRPORT CODEs ERROR - NO EDGE ADDED";}
    }
	else if (entry.compare(L"S") == 0) {
		wcout << "\nEnter the Airport Code: ";
		wcin >> entry;
		if (graph.nodeExists(entry)) {
			vector<wstring> neighbors;
			graph.getNeighbors(&neighbors, entry);

			// MAKE SURE IT IS NOT THE SAME AIRPORT CODE AS THE PREVIOUS STOP
			if (stops.size() > 0) {
				wstring lastStop = stops[stops.size() - 1];
				if (lastStop.compare(entry) == 0) {
					wcout <<"\nDUPLICATE STOP ERROR - NO STOP ADDED";
				}
				else {
					AppendStop_Transaction* transaction = new AppendStop_Transaction(&stops, entry);
					tps.addTransaction(transaction);
				}
			}
			else {
				AppendStop_Transaction* transaction = new AppendStop_Transaction(&stops, entry);
				tps.addTransaction(transaction);
			}
		}
		else {
			wcout << "\nINVALID AIRPORT CODE ERROR - NO STOP ADDED";
		}
	}
	// UNDO A TRANSACTION
	else if (entry.compare(L"U") == 0) {
		tps.undoTransaction();
	}
	// REDO A TRANSACTION
	else if (entry.compare(L"R") == 0) {
		tps.doTransaction();
	}
	// CLEAR ALL TRANSACTIONS
	else if (entry.compare(L"E") == 0) {
		tps.clearAllTransactions();
	}
	// QUIT
	else if (entry.compare(L"Q") == 0) {
		return false;
	}
	return true;
}

void initAllAirports() {
	// FIRST ADD ALL THE AIRPORTS
	graph.addNode(L"AUS", new Airport(L"AUS", 30, 18, 97, 42));
	graph.addNode(L"AVL", new Airport(L"AVL", 36, 00, 82, 32));
	graph.addNode(L"BGR", new Airport(L"BGR", 44, 48, 68, 40));
	graph.addNode(L"BOS", new Airport(L"BOS", 42, 22, 71, 2));
	graph.addNode(L"BUF", new Airport(L"BUF", 42, 56, 78, 44));
	graph.addNode(L"BVT", new Airport(L"BVT", 44, 28, 73, 9));
	graph.addNode(L"CLE", new Airport(L"CLE", 41, 24, 81, 81));
	graph.addNode(L"COD", new Airport(L"COD", 44, 33, 109, 4));
	graph.addNode(L"DCA", new Airport(L"DCA", 38, 51, 77, 2));
	graph.addNode(L"DEN", new Airport(L"DEN", 39, 45, 104, 52));
	graph.addNode(L"DFW", new Airport(L"DFW", 32, 51, 96, 51));
	graph.addNode(L"ELP", new Airport(L"ELP", 31, 48, 106, 24));
	graph.addNode(L"HOU", new Airport(L"HOU", 29, 59, 95, 22));
	graph.addNode(L"IND", new Airport(L"IND", 39, 44, 86, 17));
	graph.addNode(L"JAX", new Airport(L"JAX", 30, 30, 81, 42));
	graph.addNode(L"LAX", new Airport(L"LAX", 33, 56, 118, 24));
	graph.addNode(L"LGA", new Airport(L"LGA", 40, 47, 73, 58));
	graph.addNode(L"LIT", new Airport(L"LIT", 34, 44, 92, 14));
	graph.addNode(L"MCI", new Airport(L"MCI", 39, 7, 94, 35));
	graph.addNode(L"MIA", new Airport(L"MIA", 25, 48, 80, 16));
	graph.addNode(L"MKE", new Airport(L"MKE", 42, 87, 87, 74));
	graph.addNode(L"MOT", new Airport(L"MOT", 48, 25, 101, 21));
	graph.addNode(L"MSY", new Airport(L"MSY", 29, 59, 90, 15));
	graph.addNode(L"OMA", new Airport(L"OMA", 41, 30, 95, 80));
	graph.addNode(L"PHX", new Airport(L"PHX", 33, 26, 112, 1));
	graph.addNode(L"PWM", new Airport(L"PWM", 43, 39, 70, 19));
	graph.addNode(L"SFO", new Airport(L"SFO", 37, 46, 122, 26));
	graph.addNode(L"SEA", new Airport(L"SEA", 47, 39, 122, 18));
	graph.addNode(L"SLC", new Airport(L"SLC", 40, 46, 111, 58));
	graph.addNode(L"VGT", new Airport(L"VGT", 36, 5, 115, 10));

	// NOW CONNECT THE AIRPORTS
	initEdge(L"AUS", L"DFW");
	initEdge(L"AVL", L"IND");
	initEdge(L"BOS", L"BUF");
	initEdge(L"BOS", L"CLE");
	initEdge(L"BOS", L"PWM");
	initEdge(L"BUF", L"CLE");
	initEdge(L"BVT", L"BGR");
	initEdge(L"CLE", L"IND");
	initEdge(L"CLE", L"MKE");
	initEdge(L"COD", L"MOT");
	initEdge(L"COD", L"SEA");
	initEdge(L"COD", L"DEN");
	initEdge(L"DCA", L"AVL");
	initEdge(L"DCA", L"JAX");
	initEdge(L"DEN", L"DFW");
	initEdge(L"DEN", L"ELP");
	initEdge(L"DFW", L"HOU");
	initEdge(L"ELP", L"PHX");
	initEdge(L"HOU", L"LIT");
	initEdge(L"IND", L"MKE");
	initEdge(L"JAX", L"AVL");
	initEdge(L"LAX", L"MIA");
	initEdge(L"LAX", L"SFO");
	initEdge(L"LAX", L"VGT");
	initEdge(L"LGA", L"IND");
	initEdge(L"LGA", L"DCA");
	initEdge(L"LGA", L"BOS");
	initEdge(L"LGA", L"MIA");
	initEdge(L"MCI", L"DEN");
	initEdge(L"MCI", L"DFW");
	initEdge(L"MCI", L"IND");
	initEdge(L"MCI", L"OMA");
	initEdge(L"MIA", L"BOS");
	initEdge(L"MIA", L"JAX");
	initEdge(L"MIA", L"MSY");
	initEdge(L"MKE", L"MOT");
	initEdge(L"MOT", L"SEA");
	initEdge(L"MOT", L"DEN");
	initEdge(L"MOT", L"IND");
	initEdge(L"MSY", L"JAX");
	initEdge(L"MSY", L"IND");
	initEdge(L"OMA", L"DEN");
	initEdge(L"OMA", L"MOT");
	initEdge(L"OMA", L"IND");
	initEdge(L"PHX", L"LAX");
	initEdge(L"PWM", L"BVT");
	initEdge(L"PWM", L"BGR");
	initEdge(L"SFO", L"VGT");
	initEdge(L"SFO", L"DEN");
	initEdge(L"SFO", L"SEA");
	initEdge(L"SLC", L"SFO");
	initEdge(L"SLC", L"DEN");
	initEdge(L"SLC", L"SEA");
	initEdge(L"VGT", L"PHX");
}

void initEdge(wstring node1, wstring node2) {
	Airport* a1 = graph.getNodeData(node1);
	Airport* a2 = graph.getNodeData(node2);
	float distance = Airport::calculateDistance(a1, a2);
	graph.addEdge(node1, node2, distance);
	graph.addEdge(node2, node1, distance);
}