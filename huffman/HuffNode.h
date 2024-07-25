//
// Created by lenovo on 2021/11/9.
//

#ifndef COMPRESSION_HUFFNODE_H
#define COMPRESSION_HUFFNODE_H
#include <iostream>
using namespace std;
class HuffNode
{
public:
    char val; //字符的值
    int freq; //字符的频率
    HuffNode* lchild; //左孩子节点
    HuffNode* rchild; //右孩子节点

    HuffNode() {};
    HuffNode(char a, int b)
    {
        val = a;
        freq = b;
        lchild = NULL;
        rchild = NULL;
    }
    friend bool operator < (HuffNode A, HuffNode B)
    {
        return A.freq > B.freq; //提供给优先队列
    }
};
#endif //COMPRESSION_HUFFNODE_H
