// AUTHOR: @cleomarfelipe
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

#include "./bplustree.h"
#include <iostream>

// Tree constructor
template<typename keyType>
BPlusTree<keyType>::BPlusTree() {
  this->root = nullptr;
}

// Node struct
template<typename keyType>
BPlusTree<keyType>::Node::Node(BPlusTree<keyType>::NodeElement*
firstNodeElement) {
  this->elementCount = 1;
  this->firstNodeElement = firstNodeElement;
  this->father = nullptr;
  this->nextNode = nullptr;
  this->prevNode = nullptr;
  this->nextSibling = nullptr;
  this->prevSibling = nullptr;
  this->firstChild = nullptr;
  this->upElement = nullptr;
  this->isLeaf = true;
}

// Node element struct
template<typename keyType>
BPlusTree<keyType>::NodeElement::NodeElement(keyType key, int data) {
  this->key = key;
  this->data = data;
  this->next = nullptr;
  this->prev = nullptr;
  this->downNode = nullptr;
}

// Auxiliary function for finding an element in the tree
template <typename keyType>
std::pair<typename BPlusTree<keyType>::Node*,
typename BPlusTree<keyType>::NodeElement*>
BPlusTree<keyType>::FindAux(keyType key) {
  if (root == nullptr) {
    return std::pair<BPlusTree::Node*,
    BPlusTree::NodeElement*>(nullptr, nullptr);
  }
  BPlusTree::Node *nodeHead = root;
  BPlusTree::Node *childHead = root->firstChild;
  BPlusTree::NodeElement* elementHead = root->firstNodeElement;
  // If head is in an internal node, find which node to go down to
  while (nodeHead->isLeaf == false) {
    for (int i = 0; i <= nodeHead->elementCount; i++) {
      if (i == nodeHead->elementCount) {
        nodeHead = childHead;
        elementHead = nodeHead->firstNodeElement;
        childHead = nodeHead->firstChild;
        break;
      }
      if (key < elementHead->key) {
        nodeHead = childHead;
        elementHead = nodeHead->firstNodeElement;
        childHead = nodeHead->firstChild;
        break;
      }
      elementHead = elementHead->next;
      childHead = childHead->nextSibling;
    }
  }
  // If head is in a leaf, find the element that has the key passed to the
  // function, or null if it was not found
  while (elementHead->key != key) {
    elementHead = elementHead->next;
    if (elementHead == nullptr)
      break;
  }
  return std::pair<BPlusTree<keyType>::Node*,
  BPlusTree<keyType>::NodeElement*>(nodeHead, elementHead);
}

// Find function
template<typename keyType>
int BPlusTree<keyType>::Find(keyType key) {
  std::pair<BPlusTree<keyType>::Node*, BPlusTree<keyType>::NodeElement*>
  findResponse = BPlusTree<keyType>::FindAux(key);
  if (findResponse.second == nullptr) {
    return -1;
  }
  // Return key's corresponding data
  return findResponse.second->data;
}

// Range search function
template<typename keyType>
std::vector<int> BPlusTree<keyType>::RangeFind(keyType key1, keyType key2) {
  std::pair<BPlusTree<keyType>::Node*, BPlusTree<keyType>::NodeElement*>
  findResponse = BPlusTree<keyType>::FindAux(key1);
  BPlusTree<keyType>::Node* nodeHead = findResponse.first;
  BPlusTree<keyType>::NodeElement* elementHead = nodeHead->firstNodeElement;
  // Inicializing empty vector of data
  std::vector<int> dataRange;
  while (elementHead->key <= key2) {
    // Insert data into the vector if head is between tresholds
    if (elementHead->key >= key1) {
      dataRange.push_back(elementHead->data);
    }
    // Move head and handle null
    elementHead = elementHead->next;
    if (elementHead == nullptr) {
      if (nodeHead->nextNode != nullptr) {
        nodeHead = nodeHead->nextNode;
        elementHead = nodeHead->firstNodeElement;
      } else {
        break;
      }
    }
  }
  // Return vector of data
  return dataRange;
}

// Split function
template<typename keyType>
void BPlusTree<keyType>::Split(BPlusTree<keyType>::Node *splitNode) {
  // Get split element (median) and create new element and node
  // (consequence of split)
  BPlusTree<keyType>::NodeElement *splitElement =
  splitNode->firstNodeElement->next->next;
  BPlusTree<keyType>::NodeElement *upElement =
  new BPlusTree<keyType>::NodeElement(splitElement->key, -1);
  BPlusTree<keyType>::Node *rightNode =
  new BPlusTree<keyType>::Node(splitElement);
  rightNode->elementCount = 2;

  // Adjusting links
  if (splitNode->nextNode != nullptr) {
    splitNode->nextNode->prevNode = rightNode;
  }
  if (splitNode->nextSibling != 0) {
    splitNode->nextSibling->prevSibling = rightNode;
  }

  rightNode->nextNode = splitNode->nextNode;
  rightNode->nextSibling = splitNode->nextSibling;

  rightNode->prevNode = splitNode;
  rightNode->prevSibling = splitNode;

  splitNode->nextNode = rightNode;
  splitNode->nextSibling = rightNode;
  splitNode->firstNodeElement->next->next = nullptr;
  splitNode->elementCount = 2;

  // Adjusting links in case of node being leaf
  if (splitNode->isLeaf == false) {
    rightNode->isLeaf = false;
    rightNode->firstNodeElement = rightNode->firstNodeElement->next;
    rightNode->elementCount--;
    rightNode->firstChild =
    splitNode->firstChild->nextSibling->nextSibling->nextSibling;
    BPlusTree<keyType>::Node *aux = rightNode->firstChild;
    for (int i = 0; i < 2; i++) {
      aux->father = rightNode;
      aux = aux->nextSibling;
    }
    splitNode->firstChild->nextSibling->nextSibling->
    nextSibling->prevSibling = nullptr;
    splitNode->firstChild->nextSibling->nextSibling->nextSibling = nullptr;
    splitNode->firstChild->nextSibling->nextSibling->upElement = nullptr;
  }
  rightNode->firstNodeElement->prev = nullptr;

  // Handling split node being root and not being root
  if (splitNode->father == nullptr) {
    BPlusTree<keyType>::Node *upNode = new BPlusTree::Node(upElement);
    upNode->isLeaf = false;
    splitNode->father = upNode;
    rightNode->father = upNode;
    upNode->firstChild = splitNode;
    upElement->downNode = splitNode;
    splitNode->upElement = upNode->firstNodeElement;
    root = upNode;
  } else {
    BPlusTree<keyType>::Node *upNode = splitNode->father;
    rightNode->father = upNode;
    BPlusTree<keyType>::InsertInto(upNode, upElement);
    if (splitNode->prevSibling == nullptr) {
      splitNode->upElement = splitNode->father->firstNodeElement;
    } else {
      splitNode->upElement = splitNode->prevSibling->upElement->next;
    }
    if (rightNode->prevSibling == nullptr) {
      rightNode->upElement = rightNode->father->firstNodeElement;
      rightNode->upElement->downNode = rightNode;
    } else {
      rightNode->upElement = rightNode->prevSibling->upElement->next;
      splitNode->upElement->downNode = splitNode;
      if (splitNode->upElement->next != nullptr) {
        splitNode->upElement->next->downNode = rightNode;
      }
    }
  }
}

// Insert into function
template<typename keyType>
void BPlusTree<keyType>::InsertInto(BPlusTree<keyType>::Node *insertNode,
BPlusTree<keyType>::NodeElement *element) {
  int key = element->key;
  NodeElement* elementHead = insertNode->firstNodeElement;
  // If the element being inserted is lower than the first element,
  // it will become the first element
  if (elementHead->key > key) {
    element->next = elementHead;
    elementHead->prev = element;
    insertNode->firstNodeElement = element;
  // If not, find where to put it in
  } else {
    while (elementHead->next != nullptr) {
      if (elementHead->next->key < key) {
        elementHead = elementHead->next;
      } else {
        break;
      }
    }
    if (elementHead->next != nullptr) {
      elementHead->next->prev = element;
      element->next = elementHead->next;
    }
    element->prev = elementHead;
    elementHead->next = element;
  }
  insertNode->elementCount++;

  // After insert, split if the node is overfilled
  if (insertNode->elementCount > 3) {
    BPlusTree<keyType>::Split(insertNode);
  }
}

// Insert (into the tree) function
template<typename keyType>
void BPlusTree<keyType>::Insert(keyType key, int data) {
  BPlusTree<keyType>::NodeElement *element =
  new BPlusTree<keyType>::NodeElement(key, data);
  if (root == nullptr) {
    BPlusTree<keyType>::Node *node = new BPlusTree<keyType>::Node(element);
    root = node;
  } else {
    BPlusTree<keyType>::Node* insertNode = BPlusTree::FindAux(key).first;
    BPlusTree<keyType>::InsertInto(insertNode, element);
  }
}

// Draw function
template<typename keyType>
void BPlusTree<keyType>::Draw() {
  BPlusTree<keyType>::Node* nodeHead = root;
  BPlusTree<keyType>::NodeElement* elementHead;
  BPlusTree<keyType>::Node* firstNode = nodeHead;

  std::cout << "| ";
  // Iterate over the nodes and print its elements
  while (nodeHead != nullptr || firstNode != nullptr) {
    elementHead = nodeHead->firstNodeElement;
    while (elementHead != nullptr) {
      std::cout << elementHead->key << " ";
      elementHead = elementHead->next;
      if (elementHead == nullptr) {
        std::cout << "| ";
      }
    }
    if (nodeHead->nextNode == nullptr) {
      nodeHead = firstNode->firstChild;
      firstNode = nodeHead;
      std::cout << std::endl << "| ";
    } else {
      nodeHead = nodeHead->nextNode;
    }
  }
}
