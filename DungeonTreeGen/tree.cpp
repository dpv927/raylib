﻿#include <cstdio>
#include <iostream>
#include <ostream>
#include <random>
#include <algorithm>
#include <vector>
#include <raylib.h>
#include "tree.hpp"
#include "entry.hpp"

std::uniform_int_distribution<int> Node::distribution(1, 3);
std::mt19937 Node::rng(std::random_device{}());


inline void Node::addParentConnection(Entry srcEntry, Node* dstRoom, Entry dstEntry) {
  auto& connection = this->links[srcEntry];
  connection.dstRoom = dstRoom;
  connection.dstEntry = dstEntry;
  connection.isParent = true;
}

inline void Node::addConnection(Entry srcEntry, Node* dstRoom, Entry dstEntry) {
  auto& connection = this->links[srcEntry];
  connection.dstRoom = dstRoom;
  connection.dstEntry = dstEntry;
  connection.isParent = false;
}

void Node::generateSubtree(int maxDepth) {
  std::vector<Node*> nodes;
  int entry {0};

  // As the root room of the tree, generates 4 rooms and 
  // creates a connection to each of those rooms. Then 
  // generates the corresponding random subtree of rooms.
  //
  // A normal room can have [0-4] connections (including)
  // its parent room connection, but the root room has
  // always 4 connections.
  nodes.push_back(this);

  for(auto& link : this->links) {
    Entry iterEntry {(Entry)entry};
    Entry childEntry {getSymetric(iterEntry)};

    std::pair<int,int> offset = getOffset(iterEntry);
    Node* child {new Node(this->x + offset.first, this->y + offset.second)};

    child->addParentConnection(childEntry, this, iterEntry);
    this->addConnection(iterEntry, child, childEntry);
    nodes.push_back(child);
    ++entry;
  }

  for(auto& link : this->links) {
    link.dstRoom->generateSubtree(1, maxDepth, nodes);
  }
}

void Node::generateSubtree(int depth, int maxDepth, std::vector<Node*>& nodes) {

  // A non-root room has always one connection used to connect 
  // itself with its parent. The idea here its to get a random 
  // number of unused doors (places to stablish a connection) 
  // and create connection to new rooms. 
  std::vector<Entry> unusedLinks(FREE_LINKS);
  int doors {0};

  for(int i = 0; i < LINKS; ++i) {
    if(this->links[i].dstRoom == nullptr) {
      unusedLinks.push_back((Entry)i);
      ++doors;
    }
  }

  const int randLinks {std::min(doors, distribution(rng))};
  std::shuffle(unusedLinks.begin(), unusedLinks.end(), rng);
  
  for (int i {0}; i < randLinks; ++i) {
    Entry iterEntry {unusedLinks[i]};
    Entry childEntry {getSymetric(iterEntry)};

    std::pair<int,int> offset = getOffset(iterEntry);
    offset.first += this->x;
    offset.second += this->y;

    for(auto& node : nodes) {
      if(node->x == offset.first && node->y == offset.second) {
        // In case we found an already exsistent node in the 
        // target coordinates, abort the creation of a new node.
        return;
      }
    }

    // Only create a new node if there are not any other nodes 
    // at the target coordinates.
    Node* child = new Node(offset.first, offset.second);
    nodes.push_back(child);

    child->addParentConnection(childEntry, this, iterEntry);
    this->addConnection(iterEntry, child, childEntry);

    if((depth + 1) < maxDepth){
      child->generateSubtree(depth + 1, maxDepth, nodes);
    }
  }
}

void Node::destroySubtree(void) {
  for (auto& link : this->links) {
    if (link.dstRoom != nullptr && !link.isParent) {
      link.dstRoom->destroySubtree();
      delete link.dstRoom;
      link.dstRoom = nullptr;
    }
  }
}

void Node::printSubtree(int depth) {
  std::string fmt;
  int port = 0;

  for (int i = 0; i < depth; i++)
    fmt += "\t";

  std::cout << fmt 
    << "\033[1m\033[32m<@" << (unsigned long long) this
    << ">\033[0m" << std::endl;
       
  for (auto& link : this->links) {
    if (link.dstRoom == nullptr) {
      std::cout << fmt
        << "[" << port << "] = @null"
        << std::endl;
      
    } else {
      std::cout << fmt 
        << "[" << port << "] = @"
        << (unsigned long long)link.dstRoom
        << std::endl;
    }
    ++port;
  }

  for (auto& link : this->links) {
    if (link.dstRoom != nullptr && !link.isParent) {
      link.dstRoom->printSubtree(depth + 1);
    }
  }
}

const int RoomDim = 100;

void Node::drawSubtree(Node* target) { 

  int x = this->x * (RoomDim << 1);
  int y = this->y * (RoomDim << 1);

  Vector2 src;
  Vector2 dst;
  src.x = x + (RoomDim >> 1);
  src.y = y + (RoomDim >> 1);

  for (auto& link : this->links) {
    if (link.dstRoom != nullptr && !link.isParent) {
      dst.x = (link.dstRoom->x * (RoomDim << 1)) + (RoomDim >> 1);
      dst.y = (link.dstRoom->y * (RoomDim << 1)) + (RoomDim >> 1);
      
      DrawLineEx(src, dst, 10.0, WHITE);
      link.dstRoom->drawSubtree(this);
    }
  }

  if(this != target) {
    DrawRectangleLines(x, y, RoomDim, RoomDim, WHITE);
  } else {
    DrawRectangle(x, y, RoomDim, RoomDim, PURPLE);
  }
}
