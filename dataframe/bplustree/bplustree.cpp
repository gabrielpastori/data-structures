// AUTHOR: @cleomarfelipe
#include "./bplustree.h"
#include <iostream>
#include <math.h>

void print(std::vector<int> const &input)
{
    for (int i = 0; i < input.size(); i++) {
        std::cout << input.at(i) << ' ';
    }
    std::cout << std::endl;
}


int main() {

BPlusTree<int> tree;

for (int i = 1; i < 14; i++) {
    tree.Insert(i, 10 * i);
}

std::vector<int> vec = tree.RangeFind(5, 20);
print(vec);
tree.Draw();

return 0;
}
