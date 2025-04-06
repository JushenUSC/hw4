#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include <cmath> 
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
	int getHeight (AVLNode<Key, Value>* root);
	void rightRotation(AVLNode<Key, Value>* unbalancedNode);
	void leftRotation(AVLNode<Key, Value>* unbalancedNode);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value>& new_item)
{
	//basically boilerplate from "bst.h" from here until
	if (this->root_ == nullptr) {
		//create new node
		this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
		return;
	}


	Node<Key, Value>* parent = nullptr;
	Node<Key, Value>* current = this->root_; 
	while (current != nullptr) {
		parent = current;
		if (new_item.first == current->getKey()) {
			current->setValue(new_item.second);
			return;
		}
		else if (current->getKey() < new_item.first) {
			current = current->getRight();
		}
		else {
			current = current->getLeft();
		}
	}
	AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, static_cast<AVLNode<Key, Value>*>(parent));
	if (new_item.first < parent->getKey()) {
		parent->setLeft(newNode);
	}
	else {
		parent->setRight(newNode);
	}
	//until here

	//now we need to update the balance factors of each AVLNode within the path from the newly inserted node to root
	AVLNode<Key,Value>* traversalNode = newNode;
	while (traversalNode != nullptr) {
		int leftHeight = getHeight(traversalNode->getLeft());
		int rightHeight = getHeight(traversalNode->getRight());
		traversalNode->setBalance(leftHeight-rightHeight);
		traversalNode = traversalNode->getParent();
	}

	//now we check if the tree is balanced, if it is still balanced then we are done, if not then we need to perform rotations
	if (this->isBalanced() == true) {
		return;
	}
	else {
		//Find the unbalanced node
		AVLNode<Key,Value>* traversalNode = newNode;
		AVLNode<Key,Value>* unbalancedNode = nullptr;
		while (traversalNode != nullptr) {
			if (abs(traversalNode->getBalance()) == 2) {
				unbalancedNode = traversalNode;
				break;
			}
			traversalNode = traversalNode->getParent();
		}

		int unbalancedValue = unbalancedNode->getBalance();
		if (unbalancedValue == 2) { //unbalancedNode is left heavy
			int unbalancedNodeChildValue = unbalancedNode->getLeft()->getBalance();
			AVLNode<Key,Value>* unbalancedNodeChild = unbalancedNode->getLeft();
			if (unbalancedNodeChildValue >= 0) { //child node is left heavy
				//perform left rotation on unbalancedNode only
				rightRotation(unbalancedNode);
			}
			else {//child node is right heavy
				//perform left rotation on child node, then a right rotation on unbalancedNode
				leftRotation(unbalancedNodeChild);
				rightRotation(unbalancedNode);
			}
		}
		else { // equals -2, right heavy 
			int unbalancedNodeChildValue = unbalancedNode->getRight()->getBalance();
			AVLNode<Key,Value>* unbalancedNodeChild = unbalancedNode->getRight();
			if (unbalancedNodeChildValue < 0) { //child node is right heavy
				//perform right rotation on unbalancedNode only
				leftRotation(unbalancedNode);
			}
			else {//child node is left heavy
				//perform right rotation on child node, then a left rotation on unbalancedNode
				rightRotation(unbalancedNodeChild);
				leftRotation(unbalancedNode);
			}
		}
	}
}

template<class Key, class Value>
int AVLTree<Key, Value>::getHeight(AVLNode<Key, Value>* root) {
	if (root == nullptr) {
        return 0;
    }
	int leftHeight = getHeight(root->getLeft());
	int rightHeight = getHeight(root->getRight());
	return std::max(leftHeight, rightHeight) + 1;
}

template<class Key, class Value>
void AVLTree<Key, Value>::rightRotation(AVLNode<Key, Value>* unbalancedNode) {
	AVLNode<Key, Value>* unbalancedNodeParent = unbalancedNode->getParent();
	AVLNode<Key, Value>* unbalancedNodeLeftChild = unbalancedNode->getLeft();
	AVLNode<Key, Value>* unbalancedNodeLeftChildRightSubtree = nullptr;

	if (unbalancedNodeLeftChild->getRight() != nullptr) {
		unbalancedNodeLeftChildRightSubtree = unbalancedNodeLeftChild->getRight();
	}

	unbalancedNodeLeftChild->setRight(unbalancedNode);
	unbalancedNodeLeftChild->setParent(unbalancedNodeParent);
	//update the parent's pointer to the new root (unbalancedNodeLeftChild)
    if (unbalancedNodeParent != nullptr) { //assuming its not root
        if (unbalancedNodeParent->getLeft() == unbalancedNode) {
            unbalancedNodeParent->setLeft(unbalancedNodeLeftChild);
        } else {
            unbalancedNodeParent->setRight(unbalancedNodeLeftChild);
        }
    }
	else {
		//account for root
		this->root_ = unbalancedNodeLeftChild;
	}
	unbalancedNode->setLeft(unbalancedNodeLeftChildRightSubtree);
	unbalancedNode->setParent(unbalancedNodeLeftChild);
	if (unbalancedNodeLeftChildRightSubtree) {
		unbalancedNodeLeftChildRightSubtree->setParent(unbalancedNode);
	}

	int unbalancedNodeLeftHeight = getHeight(unbalancedNode->getLeft());
	int unbalancedNodeRightHeight = getHeight(unbalancedNode->getRight());
	unbalancedNode->setBalance(unbalancedNodeLeftHeight - unbalancedNodeRightHeight);

	int unbalancedNodeLeftChildLeftHeight = getHeight(unbalancedNodeLeftChild->getLeft());
	int unbalancedNodeLeftChildRightHeight = getHeight(unbalancedNodeLeftChild->getRight());
	unbalancedNodeLeftChild->setBalance(unbalancedNodeLeftChildLeftHeight - unbalancedNodeLeftChildRightHeight); 
}

template<class Key, class Value>
void AVLTree<Key, Value>::leftRotation(AVLNode<Key, Value>* unbalancedNode) {
	AVLNode<Key, Value>* unbalancedNodeParent = unbalancedNode->getParent();
	AVLNode<Key, Value>* unbalancedNodeRightChild = unbalancedNode->getRight();
	AVLNode<Key, Value>* unbalancedNodeRightChildLeftSubtree = nullptr;

	if (unbalancedNodeRightChild->getLeft() != nullptr) {
		unbalancedNodeRightChildLeftSubtree = unbalancedNodeRightChild->getLeft();
	}

	unbalancedNodeRightChild->setLeft(unbalancedNode);
	unbalancedNodeRightChild->setParent(unbalancedNodeParent);
	//update the parent's pointer to the new root (unbalancedNodeLeftChild)
    if (unbalancedNodeParent != nullptr) { //assuming its not root
        if (unbalancedNodeParent->getLeft() == unbalancedNode) {
            unbalancedNodeParent->setLeft(unbalancedNodeRightChild);
        } else {
            unbalancedNodeParent->setRight(unbalancedNodeRightChild);
        }
    }
	else {
        this->root_ = unbalancedNodeRightChild; 
    }
	unbalancedNode->setRight(unbalancedNodeRightChildLeftSubtree);
	unbalancedNode->setParent(unbalancedNodeRightChild);
	if (unbalancedNodeRightChildLeftSubtree) {
		unbalancedNodeRightChildLeftSubtree->setParent(unbalancedNode);
	}

	int unbalancedNodeLeftHeight = getHeight(unbalancedNode->getLeft());
	int unbalancedNodeRightHeight = getHeight(unbalancedNode->getRight());
	unbalancedNode->setBalance(unbalancedNodeLeftHeight - unbalancedNodeRightHeight);

	int unbalancedNodeRightChildLeftHeight = getHeight(unbalancedNodeRightChild->getLeft());
	int unbalancedNodeRightChildRightHeight = getHeight(unbalancedNodeRightChild->getRight());
	unbalancedNodeRightChild->setBalance(unbalancedNodeRightChildLeftHeight - unbalancedNodeRightChildRightHeight); 
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    // TODO
	//Boilerplate from here to

	Node<Key,Value>* current = BinarySearchTree<Key,Value>::internalFind(key);
	if (current == nullptr) {
		return;
	}
	//three cases:
	Node<Key,Value>* nodeToDelete = current;
	//two children case, if true, it means the predecessor must live within the left subtree (don't have to look through parent lineage to find it)
	if (nodeToDelete->getLeft() != nullptr && nodeToDelete->getRight() != nullptr) {
		Node<Key,Value>* pred = BinarySearchTree<Key,Value>::predecessor(nodeToDelete);
		nodeSwap(static_cast<AVLNode<Key, Value>*>(pred), static_cast<AVLNode<Key, Value>*>(nodeToDelete));
	}
	//nodeToDelete has at most one child
	Node<Key,Value>* parent = nodeToDelete->getParent();
	Node<Key,Value>* nodeToDeleteChild = nullptr;
	if (nodeToDelete->getLeft()) {
		nodeToDeleteChild = nodeToDelete->getLeft();
	}
	if (nodeToDelete->getRight()) {
		nodeToDeleteChild = nodeToDelete->getRight();
	}
	if (nodeToDelete == this->root_) {
		//root can have 0 or 1 child at most at this point
		if (nodeToDeleteChild == nullptr) {
			//0 child case, we will delete the root later just set it to null for now
			this->root_ = nullptr;
		}
		else {
			//1 child case, we will set the root to child and delete the previous root at end of function
			this->root_ = nodeToDeleteChild;
			nodeToDeleteChild->setParent(nullptr);
		}
	}
	else {
		//this adjusts for 0 children case, if there are 0 children, then this sets the parent's child to nullptr
		if (parent->getLeft() == nodeToDelete) {
			parent->setLeft(nodeToDeleteChild);
		}
		else {
			parent->setRight(nodeToDeleteChild);
		}
		if (nodeToDeleteChild != nullptr) {
			nodeToDeleteChild->setParent(parent);
		}
	}
	delete nodeToDelete;
	//up until here

	//now we need to update the balance factors of each AVLNode within the path from the parent of the deleted node to root
	AVLNode<Key,Value>* traversalNode = static_cast<AVLNode<Key,Value>*>(parent);
	while (traversalNode != nullptr) {
		int leftHeight = getHeight(traversalNode->getLeft());
		int rightHeight = getHeight(traversalNode->getRight());
		traversalNode->setBalance(leftHeight-rightHeight);
		traversalNode = traversalNode->getParent();
	}

	//now we check if the tree is balanced, if it is still balanced then we are done, if not then we need to perform rotations
	if (this->isBalanced() == true) {
		return;
	}
	else {
		//Find the unbalanced node
		AVLNode<Key,Value>* traversalNode = static_cast<AVLNode<Key,Value>*>(parent);
		AVLNode<Key,Value>* unbalancedNode = nullptr;
		while (traversalNode != nullptr) {
			if (abs(traversalNode->getBalance()) == 2) {
				unbalancedNode = traversalNode;
				break;
			}
			traversalNode = traversalNode->getParent();
		}

		int unbalancedValue = unbalancedNode->getBalance();
		if (unbalancedValue == 2) { //unbalancedNode is left heavy
			int unbalancedNodeChildValue = unbalancedNode->getLeft()->getBalance();
			AVLNode<Key,Value>* unbalancedNodeChild = unbalancedNode->getLeft();
			if (unbalancedNodeChildValue > 0) { //child node is left heavy
				//perform left rotation on unbalancedNode only
				leftRotation(unbalancedNode);
			}
			else {//child node is right heavy
				//perform left rotation on child node, then a right rotation on unbalancedNode
				leftRotation(unbalancedNodeChild);
				rightRotation(unbalancedNode);
			}
		}
		else { // equals -2, right heavy 
			int unbalancedNodeChildValue = unbalancedNode->getRight()->getBalance();
			AVLNode<Key,Value>* unbalancedNodeChild = unbalancedNode->getRight();
			if (unbalancedNodeChildValue < 0) { //child node is right heavy
				//perform right rotation on unbalancedNode only
				rightRotation(unbalancedNode);
			}
			else {//child node is left heavy
				//perform right rotation on child node, then a left rotation on unbalancedNode
				rightRotation(unbalancedNodeChild);
				leftRotation(unbalancedNode);
			}
		}
	}
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
