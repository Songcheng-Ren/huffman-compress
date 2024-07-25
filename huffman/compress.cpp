#include "compress.h"
#include "HuffNode.h"
#include<string>
#include<fstream>
#include<queue>
#include<iostream>
using namespace std;
string Code[256]; //保存每个字符的字符串01编码

//压缩操作

//统计每个字符的频率，由于char类型范围-128~127，故在原数上加上128
void Count(string toCompress, int num[]){
    ifstream infile(toCompress, ios::in | ios::binary); //二进制形式打开文件
    char ch;
    while(infile.get(ch)){
        num[ch + 128]++;
    }
}

//使用优先队列根据统计的频率创建哈夫曼树
HuffNode* CreateTree(int num[]){
    priority_queue<HuffNode> HuffQueue;
    //创建哈夫曼节点
    for(int j = 0; j < 256; j++){
        if(num[j] != 0){
            HuffNode temp(j-128, num[j]);
            HuffQueue.push(temp);
        }
    }
    //创建哈夫曼树
    while(HuffQueue.size() > 1){
        HuffNode* temp1 = new HuffNode(HuffQueue.top());
        HuffQueue.pop();
        HuffNode* temp2 = new HuffNode(HuffQueue.top());
        HuffQueue.pop();
        HuffNode root(0, temp1->freq + temp2->freq);
        root.lchild = temp1;
        root.rchild = temp2;
        HuffQueue.push(root);
    }
    return new HuffNode(HuffQueue.top()); //返回根节点
}

//使用递归前序遍历哈夫曼树，并生成每个字符的编码
void GetCode(HuffNode* root, string code){
    if(root->lchild == NULL && root->rchild == NULL){ //判断是否为叶节点
        Code[root->val + 128] = code;
        return;
    }
    else{
        GetCode(root->lchild, code + "0");
        GetCode(root->rchild, code + "1");
    }
}

//再次打开文件，对文件每个字符进行编码，并将结果保存在一个01字符串中
string EnCode(string toCompress){
    ifstream infile(toCompress, ios::in | ios::binary);
    char ch;
    string binaryString = "";
    while(infile.get(ch)){
        if(Code[ch + 128] != ""){
            binaryString += Code[ch + 128];
        }
    }
    return binaryString; //返回该文件编码后生成的01字符串
}

//将01字符串转化为二进制字符保存于一个字符串中
string SwitchStringToBinary(string binaryString, int &Sign) {
    string BinaryPath = "";
    int bytebuff = 0;
    int shiftcount = 0;
    //字符串每8个字符转化为一个字符
    for (int i = 0; i < binaryString.size(); i++) {
        bytebuff += (binaryString[i] == '1' ? 1 : 0);
        bytebuff <<= 1;
        shiftcount++;
        if (shiftcount == 8) {
            bytebuff >>= 1;
            BinaryPath += (char)bytebuff;
            bytebuff = 0;
            shiftcount = 0;
            if (i == binaryString.size() - 1)
                break;
            if (i + 8 > binaryString.size()) {
                i++;
                while (i <= binaryString.size() - 1) {
                    bytebuff += (binaryString[i] == '1' ? 1 : 0);
                    bytebuff <<= 1;
                    shiftcount++;
                    i++;
                }
                bytebuff <<= 7 - shiftcount;
                BinaryPath += (char)bytebuff;
            }
        }
    }
    Sign = 8 - shiftcount; //文件末尾不足8个字符的不足，并记录补足的字符，以传址的方式传出函数
    return BinaryPath; //返回转化后的字符串
}

//将哈夫曼树信息、补足字符数和编码后字符串写入新的文件
void WriteToFile(string toCompress, int num[], string BinaryPath, int Sign){
    ofstream outfile(toCompress + ".huf", ios::binary); //以二进制的方式打开新的文件，命名为原来文件名+.huf
    //将每个字符的频率写入文件
    for(int i = 0; i < 256; i++){
        //outfile << num[i];
        outfile.write((char *)&num[i],sizeof(int));
    }
    outfile.write((char *)&Sign,sizeof(int)); //将补足字符数写入文件
    //将编码后字符串写入文件
    for (int i = 0; i < BinaryPath.size(); i++) {
        outfile.put(BinaryPath[i]);
    }
    outfile.close();
}

//解码操作

//读取压缩文件的各类信息，各类信息以传址方式传出函数
void ReadChar(string toDeCompress, int num[], string &SearchPath,
              int &Sign){
    ifstream infile(toDeCompress, ios::in | ios::binary); //以二进制方式打开压缩文件
    //读取各字符的频率
    for(int i = 0; i < 256; i++) {
        infile.read((char *) &num[i], sizeof(int));
    }
    infile.read((char *) &Sign, sizeof(int)); //读取补足的字符数
    //使用位运算进行解码，并将结果保存在一个字符串中
    int bitmask = 0x80;
    char ch;
    int a;
    while (infile.get(ch)) {
        //a = (int)ch + 128;
        a = (int)(unsigned char)ch;
        while (bitmask != 0) {
            if ((bitmask & a) != 0) {
                SearchPath += '1';
            }
            if ((bitmask & a) == 0) {
                SearchPath += '0';
            }
            bitmask >>= 1;
        }
        bitmask = 0x80;
    }
    SearchPath.erase(SearchPath.end() - Sign, SearchPath.end()); //将字符串末尾减去补足的字符数
    infile.close();
}

//用解码后的字符串来遍历哈夫曼树，找到对应的字符，并写入新文件
void ReadToFile(string name, string SearchPath, HuffNode* root){
    //将原文件名减去.huf后缀，使解压后文件能按正确的方式打开
    name.erase(name.end() - 1);
    name.erase(name.end() - 1);
    name.erase(name.end() - 1);
    name.erase(name.end() - 1);
    ofstream outfile(name, ios::out | ios::binary); //以二进制方式打开新建的文件
    HuffNode* temp = root; //从根节点开始遍历
    for (int i = 0; i < SearchPath.size(); i++) {
        if (SearchPath[i] == '0') { //如果为0则查找左子树
            temp = temp->lchild;
        }
        if (SearchPath[i] == '1') { //如果为1则查找右子树
            temp = temp->rchild;
        }
        if (temp->lchild == NULL && temp->rchild == NULL) { //如果为叶节点则读取字符，并使遍历节点重新指向根节点
            char res = temp->val;
            outfile.put(res);
            temp = root;
        }
    }
    outfile.close();
}

//压缩函数,返回编码字符串指针
string* Compress(string name){
    int num[256] = {0}; //声明保存字符频率的数组
    for(int i = 0; i < 256; i++){ //重置编码字符串数组
        Code[i] = "";
    }
    Count(name, num); //统计频率
    //string charInformation;
    HuffNode* root = CreateTree(num); //创建哈夫曼树
    string code = "";
    GetCode(root, code); //遍历对字符编码
    string binaryString = EnCode(name); //对文件编码
    int Sign = 0;
    string BinaryPath = SwitchStringToBinary(binaryString, Sign); //转化为二进制
    WriteToFile(name, num, BinaryPath, Sign); //写入文件
    return Code;
}

//解压函数
void Decompress(string name){
    int num[256] = {0}; //声明保存字符频率的数组
    int Sign;
    string SearchPath;
    ReadChar(name, num, SearchPath, Sign); //读取压缩文件信息
    HuffNode* root = CreateTree(num); //创建哈夫曼树
    ReadToFile(name, SearchPath, root); //解码并写入文件
}
