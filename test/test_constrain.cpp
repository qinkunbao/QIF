/*************************************************************************
	> File Name: test_constrain.cpp
	> Author: 
	> Mail: 
	> Created Time: Tue Apr 23 12:21:10 2019
 ************************************************************************/

#include<iostream>
#include "BitVector.h"
#include "Constrains.h"

using namespace std;
using namespace tana;

int main() {
    std::shared_ptr<BitVector> bit_num = std::make_shared<BitVector>(SYMBOL, "K1");

    Constrain c = Constrain(bit_num, RelationType ::greater, 123);

    cout << c;

    return 1;


}