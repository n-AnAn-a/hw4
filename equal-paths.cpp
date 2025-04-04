#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

bool helper (Node* root, int* leafHeight, int currentHeight) {
  if (root == nullptr) {
    return true;
  }

  if (root->left == nullptr && root->right == nullptr) {
    if (*leafHeight == 0) {
      *leafHeight = currentHeight;
      return true;
    }
    if (currentHeight == *leafHeight) {
      return true;
    }
    else {
      return false;
    }
  }
  return helper(root->left, leafHeight, currentHeight + 1) && helper(root->right, leafHeight, currentHeight + 1);
}

bool equalPaths(Node * root)
{
    // Add your code below
  int leafHeight = 0;
  return helper(root, &leafHeight, 0);
}

