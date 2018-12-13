//
//			Tyler Hobbs
//			CSC 412
//			Slider Puzzle Program #1
//			31 October 2018
//

// cleanup output to console and put depth nodes and time in the front int the csv

#include<cstdlib>
#include<cstring>
#include<ctime>
#include<fstream>
#include<iomanip>
#include<iostream>
#include<map>
#include<queue>
#include<stack>
#include<string>
#include<vector>

using namespace std;

// puzzle node for searching that holds relevant data

struct puzzleNode {

	int puzzleAry[9];
	int depth;
	int heuristic;
	string puzzleString;
	vector<string> path;
};

// same as puzzleNode with the comparison function for a priority queue

struct aStarNode {

	int puzzleAry[9];
	int depth;
	int heuristic;
	string puzzleString;
	vector<string> path;

	// cheating the max heap sorting used in a priority queue by inverting greater / less than
	bool operator < (const aStarNode& compareNode) const
	{
		return heuristic > compareNode.heuristic;
	}

};


// Declaring global variables
// I use a single dimension 9 element array to represent the puzzle

int goalAry[9];
int goal2d[3][3];
int initAry[9];
string goalString;
string initString;
queue<puzzleNode> BFSqueue;
stack<puzzleNode> DFSstack;
map<string, bool> visitMap;
ofstream outFile;
priority_queue<aStarNode> MisplacedQ;
priority_queue<aStarNode> ManhattanQ;

// my integer swap function, as assigned in the slides

void mySwap(int &numA, int &numB) {
	int temp;

	temp = numA;
	numA = numB;
	numB = temp;

}

void randomPuzzle() {

	// easiest way I could think to randomize a sequence of 9 numbers 0-8 without repeats
	// might not be the most efficient, especially for the last few elements

	int tile = 0;
	bool repeat;

	srand(time(NULL));

	// iterates the initAry for assignment
	// initAry already set to 0s in loadPuzzle()
	for (int i = 0; i < 9; i++) {

		do {
			
			repeat = false;
			
			// random number 0-8
			tile = rand() % 9;
			
			//iterate through previous elements
			for (int j = 0; j < i; j++) {
				
				// checks previously assigned elements for repeats
				if (tile == initAry[j]) {
					repeat = true;
				}

			}

			// no repeat? put the tile in the puzzle
			if (!repeat) {
				initAry[i] = tile;
			}

			// repeats? try another rand
		} while (repeat);

	}

}

void loadPuzzle() {

	// initializes global variables that might be reused and declares local variables

	string choice = "";
	string input = "";
	initString = "";
	
	// clear possibly non-empty data structures
	
	while (!BFSqueue.empty()) {
		BFSqueue.pop();
	}

	while (!MisplacedQ.empty()) {
		MisplacedQ.pop();
	}

	while (!ManhattanQ.empty()) {
		ManhattanQ.pop();
	}

	while (!DFSstack.empty()) {
		DFSstack.pop();
	}
	
	// luckily maps have .clear()

	visitMap.clear();

	for (int i = 0; i < 9; i++) {
		initAry[i] = 0;
	}

	// sets the goal of a solved puzzle

	goalAry[0] = 1;
	goalAry[1] = 2;
	goalAry[2] = 3;
	goalAry[3] = 4;
	goalAry[4] = 5;
	goalAry[5] = 6;
	goalAry[6] = 7;
	goalAry[7] = 8;
	goalAry[8] = 0;

	// I created the 2D goal array to make calculating Manhattan distance easier
	// goal2d[row][column]

	goal2d[0][0] = 1;
	goal2d[0][1] = 2;
	goal2d[0][2] = 3;
	goal2d[1][0] = 4;
	goal2d[1][1] = 5;
	goal2d[1][2] = 6;
	goal2d[2][0] = 7;
	goal2d[2][1] = 8;
	goal2d[2][2] = 0;

	goalString = "123456780";

	do{

		cout << endl << endl << "\t1 -\t\tManually load in a Puzzle?";
		cout << endl << "\t2 -\t\tRandomly generate a Puzzle?";
		cout << endl << "\t3 -\t\tUse the initialized Puzzle?";
		cout << endl << endl << "\t\t\tEnter your selection: ";

		cin >> choice;

		if (choice == "1") {
			// manually load in a puzzle

			cout << endl << endl << "\tEnter in your puzzle tiles from left to right starting at the top, 0 for the space: ";
			
			for (int i = 0; i < 9; i++) {
				
				do {
					cout << endl << "\t\t\tPosition #" << i + 1 << ": ";
					cin >> input;
					
					// input validation

					if (stoi(input) >= 0 && stoi(input) < 10) {
						initAry[i] = stoi(input);
						initString += to_string(initAry[i]);
					}
					else {
						cout << endl << endl << "\t\tPlease use a valid input...........";
					}

				} while (!(stoi(input) >= 0 && stoi(input) < 10));

			}
			
			return;

		}
		else if (choice == "2") {
			// randomly generate a puzzle
			
			randomPuzzle();
			
			// random puzzle made ^ put in the init string

			for (int i = 0; i < 9; i++) {
				initString += to_string(initAry[i]);
			}
			
			return;

		}
		else if (choice == "3") {
			// the known good puzzle given in the slides
			initAry[0] = 7;
			initAry[1] = 8;
			initAry[2] = 3;
			initAry[3] = 4;
			initAry[4] = 1;
			initAry[5] = 5;
			initAry[6] = 6;
			initAry[7] = 0;
			initAry[8] = 2;
			initString = "783415602";
			
			return;
		}
		else {

			system("CLS");
			cout << endl << "\tYou entered an invalid selection!";
			cout << endl << endl << "\tPlease choose a valid selection.";

		}

	} while (choice != "1" || choice != "2" || choice != "3");

}

void displayPuzzle() {

	// formatted output of the initialized puzzle
	
	system("CLS");

	cout << "\t\t\t" << setfill('-') << setw(13) << "";
	cout << endl << "\t\t\t| " << initAry[0] << " | " << initAry[1] << " | " << initAry[2] << " |";
	cout << endl << "\t\t\t" << setfill('-') << setw(13) << "";
	cout << endl << "\t\t\t| " << initAry[3] << " | " << initAry[4] << " | " << initAry[5] << " |";
	cout << endl << "\t\t\t" << setfill('-') << setw(13) << "";
	cout << endl << "\t\t\t| " << initAry[6] << " | " << initAry[7] << " | " << initAry[8] << " |";
	cout << endl << "\t\t\t" << setfill('-') << setw(13) << "";

	return;
}

int findGoalRow(int value) {

	for (int row = 0; row < 3; row++) {

		for (int col = 0; col < 3; col++) {

			if (goal2d[row][col] == value) {
				return row;
			}

		}

	}

}

int findGoalCol(int value) {

	for (int row = 0; row < 3; row++) {

		for (int col = 0; col < 3; col++) {

			if (goal2d[row][col] == value) {
				return col;
			}

		}

	}

}

bool checkVisit(string state) {

	// created an auto-typed iterator

	auto it = visitMap.begin();

	// search for the given state

	it = visitMap.find(state);

	if (it != visitMap.end()) {
		// doesn't get to the end, found in map
		return true;
	}
	else {
		// gets to the end, not found in map
		return false;
	}

}

int calculateMisplaced(aStarNode current) {

	int misplacedTiles = 0;

	// calculates misplaced tiles
	// doesn't include the space (0) since its not technically a tile
	// and in the book and slides it doesn't count the space

	for (int i = 0; i < 9; i++) {
		if (current.puzzleAry[i] != goalAry[i]) {
			if (current.puzzleAry[i] > 0) {
				misplacedTiles++;
			}
		}
	}

	return misplacedTiles;
}

int calculateManhattan(aStarNode current) {

	int manhattanSum = 0;
	int tempAry[3][3];
	int tempTile = 0;

	// load a temporary 2d array from the current node's single dimension puzzle array
	// for use in calculating the manhattan distance

	for (int row = 0; row < 3; row++) {

		for (int col = 0; col < 3; col++) {
			
			tempAry[row][col] = current.puzzleAry[tempTile];
			tempTile++;
		}

	}

	// if current (a,b) and goal (c,d), then manhattan distance is calculated by
	// |a-c| + |b-d| and since its the absolute value of the difference, i believe 
	// goal and current can be interchanged without effect on the result

	for (int row = 0; row < 3; row++) {

		for (int col = 0; col < 3; col++) {

			if (tempAry[row][col] > 0 && tempAry[row][col] != goal2d[row][col]) {
				manhattanSum += abs(row - findGoalRow(tempAry[row][col])) + abs(col - findGoalCol(tempAry[row][col]));
			}
		}
	}


	return manhattanSum;
}

void generateStatesBFS(puzzleNode current) {

	// generates non visited states clockwise from the 0's position

	vector<string> tempPathVector;
	string tempPath;
	int upTempState[9];
	int rightTempState[9];
	int downTempState[9];
	int leftTempState[9];
	string upTempString;
	string rightTempString;
	string downTempString;
	string leftTempString;


	if (current.puzzleAry[0] == 0) {
		
		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[0], rightTempState[1]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[0], rightTempString[1]);

		if (!checkVisit(rightTempString)) {

			puzzleNode newNode;
			
			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[0]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[0], downTempState[3]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[0], downTempString[3]);

		if (!checkVisit(downTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[0]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

	}

	if(current.puzzleAry[1]==0){
		
		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[1], rightTempState[2]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[1], rightTempString[2]);

		if (!checkVisit(rightTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[1]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[1], downTempState[4]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[1], downTempString[4]);

		if (!checkVisit(downTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[1]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[1], leftTempState[0]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[1], leftTempString[0]);

		if (!checkVisit(leftTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[1]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

	}

	if (current.puzzleAry[2] == 0) {

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[2], downTempState[5]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[2], downTempString[5]);

		if (!checkVisit(downTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[2]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[2], leftTempState[1]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[2], leftTempString[1]);

		if (!checkVisit(leftTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[2]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

	}

	if (current.puzzleAry[3] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[3], upTempState[0]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[3], upTempString[0]);

		if (!checkVisit(upTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[3]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[3], rightTempState[4]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[3], rightTempString[4]);

		if (!checkVisit(rightTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[3]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[3], downTempState[6]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[3], downTempString[6]);

		if (!checkVisit(downTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[3]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

	}

	if (current.puzzleAry[4] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[4], upTempState[1]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[4], upTempString[1]);

		if (!checkVisit(upTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[4]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[4], rightTempState[5]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[4], rightTempString[5]);

		if (!checkVisit(rightTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[4]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[4], downTempState[7]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[4], downTempString[7]);

		if (!checkVisit(downTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[4]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[4], leftTempState[3]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[4], leftTempString[3]);

		if (!checkVisit(leftTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[4]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

	}

	if (current.puzzleAry[5] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[5], upTempState[2]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[5], upTempString[2]);

		if (!checkVisit(upTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[5]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[5], downTempState[8]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[5], downTempString[8]);

		if (!checkVisit(downTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[5]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[5], leftTempState[4]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[5], leftTempString[4]);

		if (!checkVisit(leftTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[5]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

	}

	if (current.puzzleAry[6] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[6], upTempState[3]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[6], upTempString[3]);

		if (!checkVisit(upTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[6]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[6], rightTempState[7]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[6], rightTempString[7]);

		if (!checkVisit(rightTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[6]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

	}

	if (current.puzzleAry[7] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[7], upTempState[4]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[7], upTempString[4]);

		if (!checkVisit(upTempString)) {
			
			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[7]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[7], rightTempState[8]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[7], rightTempString[8]);

		if (!checkVisit(rightTempString)) {
			
			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[7]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[7], leftTempState[6]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[7], leftTempString[6]);

		if (!checkVisit(leftTempString)) {
			
			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[7]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

	}

	if (current.puzzleAry[8] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[8], upTempState[5]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[8], upTempString[5]);

		if (!checkVisit(upTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[8]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[8], leftTempState[7]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[8], leftTempString[7]);

		if (!checkVisit(leftTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[8]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			visitMap[newNode.puzzleString] = true;

			BFSqueue.push(newNode);
		}

	}

}

void generateStatesDFS(puzzleNode current) {

	// generates non visited states clockwise from the 0's position

	int upTempState[9];
	int rightTempState[9];
	int downTempState[9];
	int leftTempState[9];
	string upTempString;
	string rightTempString;
	string downTempString;
	string leftTempString;



	if (current.puzzleAry[0] == 0) {

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[0], rightTempState[1]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[0], rightTempString[1]);

		if (!checkVisit(rightTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[0], downTempState[3]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[0], downTempString[3]);

		if (!checkVisit(downTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

	}

	if (current.puzzleAry[1] == 0) {

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[1], rightTempState[2]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[1], rightTempString[2]);

		if (!checkVisit(rightTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[1], downTempState[4]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[1], downTempString[4]);

		if (!checkVisit(downTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[1], leftTempState[0]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[1], leftTempString[0]);

		if (!checkVisit(leftTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

	}

	if (current.puzzleAry[2] == 0) {

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[2], downTempState[5]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[2], downTempString[5]);

		if (!checkVisit(downTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[2], leftTempState[1]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[2], leftTempString[1]);

		if (!checkVisit(leftTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

	}

	if (current.puzzleAry[3] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[3], upTempState[0]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[3], upTempString[0]);

		if (!checkVisit(upTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[3], rightTempState[4]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[3], rightTempString[4]);

		if (!checkVisit(rightTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[3], downTempState[6]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[3], downTempString[6]);

		if (!checkVisit(downTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

	}

	if (current.puzzleAry[4] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[4], upTempState[1]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[4], upTempString[1]);

		if (!checkVisit(upTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[4], rightTempState[5]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[4], rightTempString[5]);

		if (!checkVisit(rightTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[4], downTempState[7]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[4], downTempString[7]);

		if (!checkVisit(downTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[4], leftTempState[3]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[4], leftTempString[3]);

		if (!checkVisit(leftTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

	}

	if (current.puzzleAry[5] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[5], upTempState[2]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[5], upTempString[2]);

		if (!checkVisit(upTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[5], downTempState[8]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[5], downTempString[8]);

		if (!checkVisit(downTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[5], leftTempState[4]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[5], leftTempString[4]);

		if (!checkVisit(leftTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);;
		}

	}

	if (current.puzzleAry[6] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[6], upTempState[3]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[6], upTempString[3]);

		if (!checkVisit(upTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[6], rightTempState[7]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[6], rightTempString[7]);

		if (!checkVisit(rightTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

	}

	if (current.puzzleAry[7] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[7], upTempState[4]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[7], upTempString[4]);

		if (!checkVisit(upTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[7], rightTempState[8]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[7], rightTempString[8]);

		if (!checkVisit(rightTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[7], leftTempState[6]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[7], leftTempString[6]);

		if (!checkVisit(leftTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

	}

	if (current.puzzleAry[8] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[8], upTempState[5]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[8], upTempString[5]);

		if (!checkVisit(upTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[8], leftTempState[7]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[8], leftTempString[7]);

		if (!checkVisit(leftTempString)) {

			puzzleNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			// add path to new node

			visitMap[newNode.puzzleString] = true;

			DFSstack.push(newNode);
		}

	}

}

void generateStatesMisplaced(aStarNode current) {

	// generates non visited states clockwise from the 0's position

	vector<string> tempPathVector;
	string tempPath;
	int upTempState[9];
	int rightTempState[9];
	int downTempState[9];
	int leftTempState[9];
	string upTempString;
	string rightTempString;
	string downTempString;
	string leftTempString;


	if (current.puzzleAry[0] == 0) {

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[0], rightTempState[1]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[0], rightTempString[1]);

		if (!checkVisit(rightTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[0]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[0], downTempState[3]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[0], downTempString[3]);

		if (!checkVisit(downTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[0]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

	}

	if (current.puzzleAry[1] == 0) {

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[1], rightTempState[2]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[1], rightTempString[2]);

		if (!checkVisit(rightTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[1]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[1], downTempState[4]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[1], downTempString[4]);

		if (!checkVisit(downTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[1]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[1], leftTempState[0]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[1], leftTempString[0]);

		if (!checkVisit(leftTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[1]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

	}

	if (current.puzzleAry[2] == 0) {

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[2], downTempState[5]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[2], downTempString[5]);

		if (!checkVisit(downTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[2]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[2], leftTempState[1]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[2], leftTempString[1]);

		if (!checkVisit(leftTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[2]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

	}

	if (current.puzzleAry[3] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[3], upTempState[0]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[3], upTempString[0]);

		if (!checkVisit(upTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[3]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[3], rightTempState[4]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[3], rightTempString[4]);

		if (!checkVisit(rightTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[3]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[3], downTempState[6]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[3], downTempString[6]);

		if (!checkVisit(downTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[3]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

	}

	if (current.puzzleAry[4] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[4], upTempState[1]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[4], upTempString[1]);

		if (!checkVisit(upTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[4]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[4], rightTempState[5]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[4], rightTempString[5]);

		if (!checkVisit(rightTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[4]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[4], downTempState[7]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[4], downTempString[7]);

		if (!checkVisit(downTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[4]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[4], leftTempState[3]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[4], leftTempString[3]);

		if (!checkVisit(leftTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[4]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

	}

	if (current.puzzleAry[5] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[5], upTempState[2]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[5], upTempString[2]);

		if (!checkVisit(upTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[5]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[5], downTempState[8]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[5], downTempString[8]);

		if (!checkVisit(downTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[5]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[5], leftTempState[4]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[5], leftTempString[4]);

		if (!checkVisit(leftTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[5]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

	}

	if (current.puzzleAry[6] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[6], upTempState[3]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[6], upTempString[3]);

		if (!checkVisit(upTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[6]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[6], rightTempState[7]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[6], rightTempString[7]);

		if (!checkVisit(rightTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[6]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

	}

	if (current.puzzleAry[7] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[7], upTempState[4]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[7], upTempString[4]);

		if (!checkVisit(upTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[7]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[7], rightTempState[8]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[7], rightTempString[8]);

		if (!checkVisit(rightTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[7]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[7], leftTempState[6]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[7], leftTempString[6]);

		if (!checkVisit(leftTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[7]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

	}

	if (current.puzzleAry[8] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[8], upTempState[5]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[8], upTempString[5]);

		if (!checkVisit(upTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[8]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[8], leftTempState[7]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[8], leftTempString[7]);

		if (!checkVisit(leftTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[8]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateMisplaced(newNode);

			visitMap[newNode.puzzleString] = true;

			MisplacedQ.push(newNode);
		}

	}

}

void generateStatesManhattan(aStarNode current) {

	// generates non visited states clockwise from the 0's position

	vector<string> tempPathVector;
	string tempPath;
	int upTempState[9];
	int rightTempState[9];
	int downTempState[9];
	int leftTempState[9];
	string upTempString;
	string rightTempString;
	string downTempString;
	string leftTempString;


	if (current.puzzleAry[0] == 0) {

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[0], rightTempState[1]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[0], rightTempString[1]);

		if (!checkVisit(rightTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[0]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[0], downTempState[3]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[0], downTempString[3]);

		if (!checkVisit(downTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[0]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

	}

	if (current.puzzleAry[1] == 0) {

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[1], rightTempState[2]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[1], rightTempString[2]);

		if (!checkVisit(rightTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[1]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[1], downTempState[4]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[1], downTempString[4]);

		if (!checkVisit(downTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[1]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[1], leftTempState[0]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[1], leftTempString[0]);

		if (!checkVisit(leftTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[1]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

	}

	if (current.puzzleAry[2] == 0) {

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[2], downTempState[5]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[2], downTempString[5]);

		if (!checkVisit(downTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[2]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[2], leftTempState[1]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[2], leftTempString[1]);

		if (!checkVisit(leftTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[2]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

	}

	if (current.puzzleAry[3] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[3], upTempState[0]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[3], upTempString[0]);

		if (!checkVisit(upTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[3]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[3], rightTempState[4]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[3], rightTempString[4]);

		if (!checkVisit(rightTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[3]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[3], downTempState[6]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[3], downTempString[6]);

		if (!checkVisit(downTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[3]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

	}

	if (current.puzzleAry[4] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[4], upTempState[1]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[4], upTempString[1]);

		if (!checkVisit(upTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[4]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[4], rightTempState[5]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[4], rightTempString[5]);

		if (!checkVisit(rightTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[4]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[4], downTempState[7]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[4], downTempString[7]);

		if (!checkVisit(downTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[4]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[4], leftTempState[3]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[4], leftTempString[3]);

		if (!checkVisit(leftTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[4]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

	}

	if (current.puzzleAry[5] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[5], upTempState[2]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[5], upTempString[2]);

		if (!checkVisit(upTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[5]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

		// Down
		for (int i = 0; i < 9; i++) {
			downTempState[i] = current.puzzleAry[i];
		}

		mySwap(downTempState[5], downTempState[8]);

		downTempString = "";

		downTempString = current.puzzleString;

		swap(downTempString[5], downTempString[8]);

		if (!checkVisit(downTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = downTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = downTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[5]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[5], leftTempState[4]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[5], leftTempString[4]);

		if (!checkVisit(leftTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[5]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

	}

	if (current.puzzleAry[6] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[6], upTempState[3]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[6], upTempString[3]);

		if (!checkVisit(upTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[6]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[6], rightTempState[7]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[6], rightTempString[7]);

		if (!checkVisit(rightTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[6]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

	}

	if (current.puzzleAry[7] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[7], upTempState[4]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[7], upTempString[4]);

		if (!checkVisit(upTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[7]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

		// Right
		for (int i = 0; i < 9; i++) {
			rightTempState[i] = current.puzzleAry[i];
		}

		mySwap(rightTempState[7], rightTempState[8]);

		rightTempString = "";

		rightTempString = current.puzzleString;

		swap(rightTempString[7], rightTempString[8]);

		if (!checkVisit(rightTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = rightTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = rightTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[7]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[7], leftTempState[6]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[7], leftTempString[6]);

		if (!checkVisit(leftTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[7]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

	}

	if (current.puzzleAry[8] == 0) {
		// Up 
		for (int i = 0; i < 9; i++) {
			upTempState[i] = current.puzzleAry[i];
		}

		mySwap(upTempState[8], upTempState[5]);

		upTempString = "";

		upTempString = current.puzzleString;

		swap(upTempString[8], upTempString[5]);

		if (!checkVisit(upTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = upTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = upTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[8]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

		// Left
		for (int i = 0; i < 9; i++) {
			leftTempState[i] = current.puzzleAry[i];
		}

		mySwap(leftTempState[8], leftTempState[7]);

		leftTempString = "";

		leftTempString = current.puzzleString;

		swap(leftTempString[8], leftTempString[7]);

		if (!checkVisit(leftTempString)) {

			aStarNode newNode;

			newNode.depth = current.depth + 1;
			newNode.puzzleString = leftTempString;

			for (int i = 0; i < 9; i++) {
				newNode.puzzleAry[i] = leftTempState[i];
			}

			tempPathVector = current.path;

			tempPath = "";
			tempPath += to_string(newNode.puzzleAry[8]);
			tempPath += ",";

			tempPathVector.push_back(tempPath);

			newNode.path = tempPathVector;

			newNode.heuristic = current.depth + calculateManhattan(newNode);

			visitMap[newNode.puzzleString] = true;

			ManhattanQ.push(newNode);
		}

	}

}


//			getInvCount and isSolvable are functions taken from 
//			https://www.geeksforgeeks.org/check-instance-8-puzzle-solvable/
//			to determine if a given puzzle configuration is valid.
//			As long as there is an even number of tile inversions,
//			then a given puzzle is valid. We were not given direction
//			to the known good puzzle validator, so hopefully these work.

// A utility function to count inversions in given array 'arr[]' 
int getInvCount(int arr[])
{
	int inv_count = 0;
	for (int i = 0; i < 9 - 1; i++)
		for (int j = i + 1; j < 9; j++)
			// Value 0 is used for empty space 
			if (arr[j] && arr[i] && arr[i] > arr[j])
				inv_count++;
	return inv_count;
}

// This function returns true if given 8 puzzle is solvable. 
bool isSolvable()
{
	// Count inversions in given 8 puzzle 
	int invCount = getInvCount(initAry);

	// return true if inversion count is even. 
	return (invCount % 2 == 0);
}

void BFSPuzzle() {

	puzzleNode current;
	vector<string> temp;
	string pathString = "";
	string depthString = "";
	string nodeString = "";
	string searchType = "BFS,";
	string timeString = "";
	bool doubleCheck;
	bool solved = false;
	int nodeCount = 0;

	if (!isSolvable) {
		cout << endl << endl << "\t\t\tGiven puzzle is not valid, searching anyways!";
	}

	visitMap.clear();

	current.depth = 0;
	current.puzzleString = initString;

	for (int i = 0; i < 9; i++) {
		current.puzzleAry[i] = initAry[i];
	}

	pathString += "Initialized - ";
	pathString += current.puzzleString;
	pathString += ",";

	temp.push_back(pathString);

	current.path = temp;

	visitMap[current.puzzleString] = true;

	BFSqueue.push(current);

	clock_t time;
	time = clock();

	while (!BFSqueue.empty()) {

		current = BFSqueue.front();
		BFSqueue.pop();
		doubleCheck = false;
		nodeCount++;

		//for (int i = 0; i < 9; i++) {
		//	if (current.puzzleAry[i] == goalAry[i]) {
		//		doubleCheck = true;
		//	}
		//	else {
		//		doubleCheck = false;
		//	}
		//}

		if (current.puzzleString == goalString) {


			pathString = "";
			pathString = "Solved,";
			current.path.push_back(pathString);

			cout << endl << endl << "\t\t\tPuzzle Solved!";
			int i = 5;
			cout << endl << "\t\t";
			for (auto it = current.path.begin(); it != current.path.end(); it++) {
				cout << *it << " ";
				i++;
				if (i % 10 == 0) {
					cout << endl << "\t\t";
				}
			}

			outFile << searchType;

			depthString += "Depth - ";
			depthString += to_string(current.depth);
			depthString += ",";

			outFile << depthString;

			nodeString += "Nodes Expanded - ";
			nodeString += to_string(nodeCount);
			nodeString += ",";

			outFile << nodeString;

			time = clock() - time;

			timeString = "Time Elapsed - ";
			timeString += to_string(time);
			timeString += " ms,";

			outFile << timeString;

			for (auto it = current.path.begin(); it != current.path.end(); it++) {
				outFile << *it;
			}

			outFile << "\n";

			cout << endl << "\t\t" << nodeString;
			cout << endl << "\t\t" << timeString;
			cout << endl << "\t\t" << depthString;

			solved = true;

		}
		else {

			generateStatesBFS(current);

		}

	}


	if (!solved) {

		system("CLS");

		outFile << searchType;

		depthString = "Depth - ";
		depthString += to_string(current.depth);
		depthString += ",";

		outFile << depthString;

		nodeString += "Nodes Expanded - ";
		nodeString += to_string(nodeCount);
		nodeString += ",";
		outFile << nodeString;
		
		time = clock() - time;

		timeString = "Time Elapsed - ";
		timeString += to_string(time);
		timeString += " ms,";

		outFile << timeString;

		outFile << pathString;

		outFile << "Search Failed,";

		outFile << "\n";
		cout << endl << endl << "\t\t\tBreadth First Search failed to solve the given puzzle.";
		cout << endl << "\t\t" << nodeString ;
		cout << endl << "\t\t" << timeString ;
		cout << endl << endl << "\t\t\tPlease try again using a valid puzzle configuration!";
	}
	
}

void DFSPuzzle() {

	puzzleNode current;
	vector<string> temp;
	string outString = "";
	string depthString = "";
	string nodeString = "";
	string searchType = "DFS,";
	string timeString = "";
	int nodeCount = 0;
	bool doubleCheck;
	bool solved = false;

	if (!isSolvable) {
		cout << endl << endl << "\t\t\tGiven puzzle is not valid, searching anyways!";
	}

	visitMap.clear();

	current.depth = 0;
	current.puzzleString = initString;

	for (int i = 0; i < 9; i++) {
		current.puzzleAry[i] = initAry[i];
	}

	outString += "Initialized - ";
	outString += current.puzzleString;
	outString += ",";

	visitMap[current.puzzleString] = true;

	DFSstack.push(current);

	clock_t time;
	time = clock();

	while (!DFSstack.empty()) {

		current = DFSstack.top();
		DFSstack.pop();
		doubleCheck = false;
		nodeCount++;

		//for (int i = 0; i < 9; i++) {
		//	if (current.puzzleAry[i] == goalAry[i]) {
		//		doubleCheck = true;
		//	}
		//	else {
		//		doubleCheck = false;
		//	}
		//}

		if (current.puzzleString == goalString) {

			cout << endl << endl << "\t\t\t Puzzle Solved!";

			outFile << searchType;

			depthString += "Depth - ";
			depthString += to_string(current.depth);
			depthString += ",";

			outFile << depthString;

			nodeString += "Nodes Expanded - ";
			nodeString += to_string(nodeCount);
			nodeString += ",";

			outFile << nodeString;

			time = clock() - time;

			timeString = "Time Elapsed - ";
			timeString += to_string(time);
			timeString += " ms,";

			outFile << timeString;

			outString += "Puzzle Solved,";

			outFile << outString;

			// no path for DFS

			outFile << "\n";

			cout << endl << endl << "\t\t" << nodeString ;
			cout << endl << "\t\t" << timeString ;
			cout << endl << "\t\t" << depthString ;

			solved = true;

		}
		else {

			generateStatesDFS(current);

		}

	}

	if (!solved) {

		system("CLS");

		outFile << searchType;

		depthString += "Depth - ";
		depthString += to_string(current.depth);
		depthString += ",";

		outFile << depthString;

		nodeString += "Nodes Expanded - ";
		nodeString += to_string(nodeCount);
		nodeString += ",";

		outFile << nodeString;

		time = clock() - time;

		timeString = "Time Elapsed - ";
		timeString += to_string(time);
		timeString += " ms,";

		outFile << timeString;

		outFile << outString;

		outFile << "Search Failed,";

		outFile << "\n";

		cout << endl << endl << "\t\t\tDepth First Search failed to solve the given puzzle.";
		cout << endl << "\t\t" << nodeString;
		cout << endl << "\t\t" << timeString;
		cout << endl << endl << "\t\t\tPlease try again using a valid puzzle configuration!";
	}

}

void manhattanDistance() {

	aStarNode current;
	vector<string> temp;
	string pathString = "";
	string depthString = "";
	string nodeString = "";
	string searchType = "A* Manhattan Distance,";
	string timeString = "";
	bool doubleCheck;
	bool solved = false;
	int nodeCount = 0;

	if (!isSolvable) {
		cout << endl << endl << "\t\t\tGiven puzzle is not valid, searching anyways!";
	}

	visitMap.clear();

	current.depth = 0;
	current.puzzleString = initString;

	for (int i = 0; i < 9; i++) {
		current.puzzleAry[i] = initAry[i];
	}


	pathString += "Initialized - ";
	pathString += current.puzzleString;
	pathString += ",";

	temp.push_back(pathString);

	current.path = temp;

	visitMap[current.puzzleString] = true;

	ManhattanQ.push(current);

	clock_t time;
	time = clock();

	while (!ManhattanQ.empty()) {

		current = ManhattanQ.top();
		ManhattanQ.pop();
		doubleCheck = false;
		nodeCount++;

		//for (int i = 0; i < 9; i++) {
		//	if (current.puzzleAry[i] == goalAry[i]) {
		//		doubleCheck = true;
		//	}
		//	else {
		//		doubleCheck = false;
		//	}
		//}

		if (current.puzzleString == goalString) {


			pathString = "";
			pathString = "Solved,";
			current.path.push_back(pathString);

			cout << endl << endl << "\t\t\tPuzzle Solved!";
			int i = 5;
			cout << endl << "\t\t";
			for (auto it = current.path.begin(); it != current.path.end(); it++) {
				cout << *it << " ";
				i++;
				if (i % 10 == 0) {
					cout << endl << "\t\t";
				}
			}

			outFile << searchType;

			depthString = "Depth - ";
			depthString += to_string(current.depth);
			depthString += ",";

			outFile << depthString;

			nodeString += "Nodes Expanded - ";
			nodeString += to_string(nodeCount);
			nodeString += ",";

			outFile << nodeString;

			time = clock() - time;

			timeString = "Time Elapsed - ";
			timeString += to_string(time);
			timeString += " ms,";

			outFile << timeString;

			for (auto it = current.path.begin(); it != current.path.end(); it++) {
				outFile << *it;
			}

			outFile << "\n";
			
			cout << endl << "\t\t" << nodeString;
			cout << endl << "\t\t" << timeString;
			cout << endl << "\t\t" << depthString;

			solved = true;
			break;

		}
		else {

			generateStatesManhattan(current);

		}

	}


	if (!solved) {

		system("CLS");

		outFile << searchType;

		depthString += "Depth - ";
		depthString += to_string(current.depth);
		depthString += ",";

		outFile << depthString;

		nodeString += "Nodes Expanded - ";
		nodeString += to_string(nodeCount);
		nodeString += ",";
		outFile << nodeString;

		time = clock() - time;

		timeString = "Time Elapsed - ";
		timeString += to_string(time);
		timeString += " ms,";

		outFile << timeString;

		outFile << pathString;

		outFile << "Search Failed,";

		outFile << "\n";
		cout << endl << endl << "\t\t\tA* Search using Manhattan Distance failed to solve the given puzzle.";
		cout << endl << "\t\t" << nodeString;
		cout << endl << "\t\t" << timeString;
		cout << endl << endl << "\t\t\tPlease try again using a valid puzzle configuration!";
	}

}

void misplacedTiles() {
	
	aStarNode current;
	vector<string> temp;
	string pathString = "";
	string depthString = "";
	string nodeString = "";
	string searchType = "A* Misplaced Tiles,";
	string timeString = "";
	bool doubleCheck;
	bool solved = false;
	int nodeCount = 0;

	if (!isSolvable) {
		cout << endl << endl << "\t\t\tGiven puzzle is not valid, searching anyways!";
	}

	visitMap.clear();

	current.depth = 0;
	current.puzzleString = initString;

	for (int i = 0; i < 9; i++) {
		current.puzzleAry[i] = initAry[i];
	}


	pathString += "Initialized - ";
	pathString += current.puzzleString;
	pathString += ",";

	temp.push_back(pathString);

	current.path = temp;

	visitMap[current.puzzleString] = true;

	MisplacedQ.push(current);

	clock_t time;
	time = clock();

	while (!MisplacedQ.empty()) {

		current = MisplacedQ.top();
		MisplacedQ.pop();
		doubleCheck = false;
		nodeCount++;

		//for (int i = 0; i < 9; i++) {
		//	if (current.puzzleAry[i] == goalAry[i]) {
		//		doubleCheck = true;
		//	}
		//	else {
		//		doubleCheck = false;
		//	}
		//}

		if (current.puzzleString == goalString) {


			pathString = "";
			pathString = "Solved,";
			current.path.push_back(pathString);

			cout << endl << endl << "\t\t\tPuzzle Solved!";
			int i = 5;
			cout << endl << "\t\t";
			for (auto it = current.path.begin(); it != current.path.end(); it++) {
				cout << *it << " ";
				i++;
				if (i % 10 == 0) {
					cout << endl << "\t\t";
				}
			}

			outFile << searchType;

			depthString = "Depth - ";
			depthString += to_string(current.depth);
			depthString += ",";

			outFile << depthString;

			nodeString += "Nodes Expanded - ";
			nodeString += to_string(nodeCount);
			nodeString += ",";

			outFile << nodeString;

			time = clock() - time;

			timeString = "Time Elapsed - ";
			timeString += to_string(time);
			timeString += " ms,";

			outFile << timeString;

			for (auto it = current.path.begin(); it != current.path.end(); it++) {
				outFile << *it;
			}

			outFile << "\n";

			cout << endl << "\t\t" << nodeString;
			cout << endl << "\t\t" << timeString;
			cout << endl << "\t\t" << depthString;

			solved = true;
			break;

		}
		else {

			generateStatesMisplaced(current);

		}

	}


	if (!solved) {

		system("CLS");

		outFile << searchType;

		depthString += "Depth - ";
		depthString += to_string(current.depth);
		depthString += ",";

		outFile << depthString;

		nodeString += "Nodes Expanded - ";
		nodeString += to_string(nodeCount);
		nodeString += ",";
		outFile << nodeString;

		time = clock() - time;

		timeString = "Time Elapsed - ";
		timeString += to_string(time);
		timeString += " ms,";

		outFile << timeString;

		outFile << pathString;

		outFile << "Search Failed,";

		outFile << "\n";
		cout << endl << endl << "\t\t\tA* Search using misplaced tiles failed to solve the given puzzle.";
		cout << endl << "\t\t" << nodeString;
		cout << endl << "\t\t" << timeString;
		cout << endl << endl << "\t\t\tPlease try again using a valid puzzle configuration!";
	}

}

int main() {

	string choice = "";

	outFile.open("results.csv", ios::out);

	// main menu

	do {

		cout << endl << endl << "\t1 -\tLoad a new Puzzle";
		cout << endl << "\t2 -\tDisplay the currently loaded Puzzle";
		cout << endl << "\t3 -\tBreadth First Search the current Puzzle";
		cout << endl << "\t4 -\tDepth First Search the current Puzzle";
		cout << endl << "\t5 -\tA* Search the current Puzzle (Misplaced Tiles)";
		cout << endl << "\t6 -\tA* Search the current Puzzle (Manhattan Distance)";
		cout << endl << "\t-999 -\tExit the Application";

		cout << endl << endl << "\tEnter your desired selection: ";
		cin >> choice;

		if (choice == "1") {
			
			system("CLS");
			loadPuzzle();
			system("CLS");
			cout << endl << "\tPuzzle successfully loaded!";
			
		}
		else if (choice == "2") {

			displayPuzzle();

		}
		else if (choice == "3") {

			displayPuzzle();
			cout << endl << endl << "\tBreadth First Search of the Puzzle" << endl << endl;
			BFSPuzzle();

		}
		else if (choice == "4") {

			displayPuzzle();
			cout << endl << endl << "\tDepth First Search of the Puzzle" << endl << endl;
			DFSPuzzle();

		}
		else if (choice == "5") {

			displayPuzzle();
			cout << endl << endl << "\tA* Misplaced Tiles Search of the Puzzle" << endl << endl;
			misplacedTiles();

		}
		else if (choice == "6") {

			displayPuzzle();
			cout << endl << endl << "\tA* Manhattan Distance Search of the Puzzle" << endl << endl;
			manhattanDistance();

		}
		else if (choice == "-999") {

			outFile.close();
			break;

		}
		else {

			system("CLS");
			cout << endl << "\tYou entered an invalid selection!";
			cout << endl << endl << "\tPlease choose a valid selection: ";
		
		}

	} while (choice != "-999");

	cin.get();

	return 0;
}