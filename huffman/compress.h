//
// Created by lenovo on 2021/11/9.
//

//#ifndef COMPRESSION_COMPRESS_H
//#define COMPRESSION_COMPRESS_H

#include "HuffNode.h"
#include<string>
void Count(string toCompress, int num[]);
HuffNode* CreateTree(int num[]);
void GetCode(HuffNode* root, string code);
string EnCode(string toCompress);
string* Compress(string name);
void ReadChar(string toDeCompress, int num[], string &SearchPath,
              int &Sign);
void ReadToFile(string name, string SearchPath, HuffNode* root);
void Decompress(string name);
//#endif //COMPRESSION_COMPRESS_H
