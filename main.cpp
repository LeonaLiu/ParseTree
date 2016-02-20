/*
 * c++练习-4
 * 表达式树
 * 练习OOP
 * created by Yang Liu
 * 2016.02.19
 * version: 1.0
 */
 
#include <iostream>
#include <string>
#include "ParseTree.h"

int main ()
{
    using ParseTree::ParseTree;
    using std::string;
    
 //   // ( 20 + (5 -10.5) ) * 7 / 12.3
 //   Node * pNode1 = new BinaryNode (new NumberNode(5), new NumberNode(10.5), BinaryNode::MINUS);
 //   Node * pNode2 = new BinaryNode (new NumberNode(20), pNode1, BinaryNode::ADD);
 //   Node * pNode3 = new BinaryNode (pNode2, new NumberNode(7), BinaryNode::MUTIL);
 //   Node * pNode4 = new BinaryNode (pNode3, new NumberNode(12.3), BinaryNode::DIV);
 //   std::cout << "Calculating the tree\n";
 //   // tell the root to calculate itself
 //   auto x = pNode4->calculate();
    //std::cout << x << std::endl;
 //   delete pNode4; // and all children

    string expression = "+( -20 + (5 -10.5) ) * 7 / 12.3";
    //string expression = "&( -20 + (5 -10.5) ) * 7 / 12.3"; //此条将触发异常
    ParseTree tree(expression);
    std::cout << expression << " = " << tree.calculate() << std::endl;

    return 0;
}
