#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int helper_equalPaths(Node* root, int l){
  if (root == nullptr ||(root->left == nullptr && root->right == nullptr)){
    return l;
  }
  else if (root->left == nullptr){
    return helper_equalPaths(root->right, l+1);
  }
  else if (root->right == nullptr){
    return helper_equalPaths(root->left, l+1);
  }
  int lef = helper_equalPaths(root->left, l+1);
  int righ = helper_equalPaths(root->right, l+1);
  if (lef == -1 || righ == -1 || lef != righ){
    return -1;
  }
  return lef;
}

bool equalPaths(Node * root)
{
  // Add your code below
  return helper_equalPaths(root, 0) != -1;
  
}

