#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <utility>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

pair<bool,int> realEqualPaths(Node* root, int pathLengthCounter)
{

	// Check for null root
    if (root == nullptr) {
        return pair<bool, int>(true, 0);
    }

    // Base case
	if (root->left == nullptr && root->right == nullptr) {
		return pair<bool, int>(true, pathLengthCounter);
	}

	//initialization of right & left subtree
	pair<bool,int> leftSubtree = {false, 0};
	pair<bool,int> rightSubtree = {false, 0};

	if (root->left) {
		leftSubtree = realEqualPaths(root->left, pathLengthCounter + 1);
	}
	if (root->right) {
		rightSubtree = realEqualPaths(root->right, pathLengthCounter + 1);
	}

	if (leftSubtree.second == 0) {
		return pair<bool, int>(rightSubtree.first, rightSubtree.second);
	} 
	if (rightSubtree.second == 0) {
		return pair<bool, int>(leftSubtree.first, leftSubtree.second);
	}
	//previous two if blocks check if there is EITHER no left or no right subtree

	if (leftSubtree.first == true && rightSubtree.first == true) {
		if (leftSubtree.second == rightSubtree.second) {
			return pair<bool, int>(true, leftSubtree.second); //being handed off to parent to check against next subtree
		}
		else {
			return pair<bool, int>(false, -1);
		}
	}
	else {
		return pair<bool, int>(false, -1); //-1 path length indicates unequal lengths
	}
}


bool equalPaths(Node* root)
{
	if (root == nullptr) {
		return true;
	}
    pair<bool, int> answerPair = realEqualPaths(root, 0);
	return answerPair.first;
}
