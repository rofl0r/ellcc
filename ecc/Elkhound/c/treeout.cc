// treeout.cc            see license.txt for copyright and terms of use
// code for treeout.h

#include "treeout.h"     // this module

using namespace std;

// print heading to stdout, with some indication of tree level
ostream &treeOut(int level)
{
  while (level--) {
    cout << "*";
  }
  cout << " ";
  return cout;
}
