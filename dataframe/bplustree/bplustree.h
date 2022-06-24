// AUTHOR: @cleomarfelipe
#ifndef BPLUSTREE_H_
#define BPLUSTREE_H_

// Copyright (c) 2022

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.

#include<iostream>
#include<vector>
#include<utility>

template <typename keyType>
class BPlusTree {
  /*
  Main B+ tree class (maximum of 4 children for node).
  */
 public:
  struct NodeElement;
  struct Node;

  struct NodeElement {
  /*
  A struct that is used to store the content of a node (node 
  elements). Each node element has a key (that is used by the key), 
  data (what the key is linked to), a next element (the next element 
  of the node), a prev element (the prev element of the node) and a 
  down node (the node whose keys are smaller than the element's key).
  The constructor takes key and data as arguments.
  
  Obs.: in our situation, data will be a row number, so it will be 
  always an integer.
  */
  keyType key;
  int data;
  NodeElement* next;
  NodeElement* prev;
  Node* downNode;
  NodeElement(keyType, int);
  };

  struct Node {
  /*
  This struct is used for the tree nodes. Each Node has the number of
  elements in it, a boolean isLeaf that indicates if the node is a 
  leaf, a father (the node father), a next node (in the same level, 
  i.e. brothers and cousins), a previous node (in the same level), a 
  next sibling, a previous sibling, a first node element, an up 
  element (father's node element whose key is greater than all the 
  values of the node) and a first child.
  */
  int elementCount;
  bool isLeaf;
  Node* father;
  Node* nextNode;
  Node* prevNode;
  Node* nextSibling;
  Node* prevSibling;
  NodeElement* firstNodeElement;
  NodeElement* upElement;
  Node* firstChild;
  explicit Node(NodeElement*);
  };

  // Root of the tree
  Node* root;

  // Splits a node when it is overfilled
  void Split(Node*);

  // Inserts an element in a node
  void InsertInto(Node*, NodeElement*);

  // Inserts a key into the key
  void Insert(keyType, int);

  // Auxiliary function for finding a key in the tree. Returns a pair
  // containing the node and the node element where the key is in
  std::pair<Node*, NodeElement*> FindAux(keyType);

  // Tries to find a key in the tree. Returns the corresponding data or -1
  // (if not found)
  int Find(keyType);

  // Execute a range search in the key. Returns a vector of the data
  // corresponding to the keys that are between the values (inclusive)
  // passed to the function
  std::vector<int> RangeFind(keyType, keyType);

  // Draw the tree nodes and its keys
  void Draw();

  // Class constructor
  BPlusTree();
};

#include "bplustree.hpp"

#endif  // BPLUSTREE_H_;
