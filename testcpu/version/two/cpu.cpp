//
// Created by polite on 2021/7/11.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

#define MAX 1024
#define MAXIMUM 16
#define BYTE 8

string IR[MAX];//源代码
string IRbinary[MAX];//二进制代码
string HEX[MAX];//十六进制

class BinaryTree;//二叉树
BinaryTree* autoGrowTree(BinaryTree* tree,string instrument,string data);//自动生成树
BinaryTree* autoGrowTree(BinaryTree* tree,string instrument,string data,string (*func)(string,string));
BinaryTree* autoGrowTree(BinaryTree* tree,string instrument,string data,void (*func)(string &,string));
BinaryTree* autoGrowTree(BinaryTree* tree,string instrument,string data,void (*func)(string &,int));
BinaryTree* autoFoundTree(BinaryTree* tree,string instrument);//自动查找树
class CPU;//CPU

string numberToBinary(int x,string s);//将十进制的数转成二进制
string numberToBinary(int x,int res);//将十进制的数转成二进制
int binaryToNumber(string s);//将二进制的数转成十进制
int hexToNumber(string s);//将十六进制的数转成十进制
string hexToBinary(int x,string s);//将十六进制的数转成二进制
string binaryToHex(int x,string s);//将二进制的数转成十六进制

string add(string sa,string sb);//加
string seb(string sa,string sb);//减
string mul(string sa,string sb);//乘
string div(string sa,string sb);//除
void copy(string &sa,string sb);//寄存器拷贝
void cop(string &sa,int sb);//寄存器赋值
void free(string &sa,string sb);//清空寄存器

int read();//字节码
int compile(int length);//机器码二进制及十六进制

class BinaryTree {
private:
    BinaryTree* left;
    BinaryTree* right;
    string data;
public:
    string (*function)(string,string);
    void (*func)(string &,string);
    void (*funct)(string &,int);
    BinaryTree() {
        left = nullptr;
        right = nullptr;
        data = "";
        function = nullptr;
        func = nullptr;
        funct = nullptr;
    }
    void setLeft(BinaryTree *l) {
        left = l;
    }
    void setRight(BinaryTree *r) {
        right = r;
    }
    void setData(string d) {
        data = d;
    }
    BinaryTree* getLeft() {
        return left;
    }
    BinaryTree* getRight() {
        return right;
    }
    string getData() {
        return data;
    }
    void displayBefore() {
        if(data != "") {
            std::cout << data << '\t';
            if(left != nullptr)
                left->displayBefore();
            if(right != nullptr)
                right->displayBefore();
        }
    }
    void displayNow() {
        if(data != "") {
            if(left != nullptr)
                left->displayNow();
            std::cout << data << '\t';
            if(right != nullptr)
                right->displayNow();
        }
    }
    void displayAfter() {
        if(data != "") {
            if(left != nullptr)
                left->displayAfter();
            if(right != nullptr)
                right->displayAfter();
            std::cout << data << '\t';
        }
    }
};

BinaryTree* autoGrowTree(BinaryTree* tree,string instrument,string data) {
    BinaryTree* n = tree;
    //遍历树
    for(int i=0;i<BYTE;i++) {
        if(instrument.at(i) == '1') {
            if(n->getRight() == nullptr)
                n->setRight(new BinaryTree());
            n = n->getRight();
        } else {
            if(n->getLeft() == nullptr)
                n->setLeft(new BinaryTree());
            n = n->getLeft();
        }
    }

    if(n->getData() == "") {
        n->setData(data);
    } else {
        cerr << "There are same binary code about instruction!\n";
    }
    return tree;
}

BinaryTree* autoGrowTree(BinaryTree* tree,string instrument,string data,string (*func)(string,string)) {
    BinaryTree* n = tree;
    //遍历树
    for(int i=0;i<BYTE;i++) {
        if(instrument.at(i) == '1') {
            if(n->getRight() == nullptr)
                n->setRight(new BinaryTree());
            n = n->getRight();
        } else {
            if(n->getLeft() == nullptr)
                n->setLeft(new BinaryTree());
            n = n->getLeft();
        }
    }

    if(n->getData() == "") {
        n->setData(data);
        n->function = func;
    } else {
        cerr << "There are same binary code about instruction!\n";
    }
    return tree;
}

BinaryTree* autoGrowTree(BinaryTree* tree,string instrument,string data,void (*func)(string &,string)) {
    BinaryTree* n = tree;
    //遍历树
    for(int i=0;i<BYTE;i++) {
        if(instrument.at(i) == '1') {
            if(n->getRight() == nullptr)
                n->setRight(new BinaryTree());
            n = n->getRight();
        } else {
            if(n->getLeft() == nullptr)
                n->setLeft(new BinaryTree());
            n = n->getLeft();
        }
    }

    if(n->getData() == "") {
        n->setData(data);
        n->func = func;
    } else {
        cerr << "There are same binary code about instruction!\n";
    }
    return tree;
}

BinaryTree* autoGrowTree(BinaryTree* tree,string instrument,string data,void (*func)(string &,int)) {
    BinaryTree* n = tree;
    //遍历树
    for(int i=0;i<BYTE;i++) {
        if(instrument.at(i) == '1') {
            if(n->getRight() == nullptr)
                n->setRight(new BinaryTree());
            n = n->getRight();
        } else {
            if(n->getLeft() == nullptr)
                n->setLeft(new BinaryTree());
            n = n->getLeft();
        }
    }

    if(n->getData() == "") {
        n->setData(data);
        n->funct = func;
    } else {
        cerr << "There are same binary code about instruction!\n";
    }
    return tree;
}

BinaryTree* autoFoundTree(BinaryTree* tree,string instrument) {
    BinaryTree* n = tree;
    for (int i=0;i<BYTE;i++) {
        if (instrument.at(i) == '1') {
            if (n->getRight() != nullptr)
                n = n->getRight();
            else
                cerr << "There is an undefined syntax!\n";
        } else {
            if (n->getLeft() != nullptr)
                n = n->getLeft();
            else
                cerr << "There is an undefined syntax!\n";
        }
    }
    return n;
}

class CPU {
private:
    int numberBUS;//总线占用数,当>=1024/2时，说明总线占满了
    string BUS[MAX];//总线
    string CSR[MAXIMUM];//寄存器
    string PC = "0000";//程序计数器
    BinaryTree* instruction = new BinaryTree();//语法树
public:
    CPU() {
        numberBUS = 0;
        for(int i=0;i<MAX;i++) {
            BUS[i] = "--------";
        }
        for(int i=0;i<MAXIMUM;i++) {
            CSR[i] = "";
        }
        instruction = autoGrowTree(instruction,"00001100","add",add);
        instruction = autoGrowTree(instruction,"00001101","seb",seb);
        instruction = autoGrowTree(instruction,"00001110","mul",mul);
        instruction = autoGrowTree(instruction,"00001111","div",div);
        instruction = autoGrowTree(instruction,"00101100","copy",copy);
        instruction = autoGrowTree(instruction,"00111100","cop",cop);
        instruction = autoGrowTree(instruction,"01001100","free",free);
    }

    int readIR(int length) {
        int i=0;
        for(i=numberBUS;i<length;i++) {
            BUS[i] = IRbinary[i].substr(0,8);
            BUS[MAX/2+i] = IRbinary[i].substr(8,8);
        }
        numberBUS = i;
        return i;
    }

    void run(int length) {
        for(int i=0;i<length;i++) {
            PC = binaryToHex(16,numberToBinary(16,i));
            BinaryTree* bt = autoFoundTree(instruction,BUS[i]);

            if(bt->function != nullptr) {
                string result = bt->function(CSR[binaryToNumber(BUS[MAX/2+i].substr(0,4))],
                                             CSR[binaryToNumber(BUS[MAX/2+i].substr(4,4))]);
                CSR[binaryToNumber(BUS[MAX/2+i].substr(0, 4))] = result;
                cout << PC << ":" << bt->getData() << "--" << binaryToNumber(result) << endl;
            } else if(bt->func != nullptr) {
                bt->func(CSR[binaryToNumber(BUS[MAX/2+i].substr(0, 4))],
                         CSR[binaryToNumber(BUS[MAX/2+i].substr(4, 4))]);
                cout << PC << ":" << bt->getData() << "--" << "func" << endl;
            } else {
                bt->funct(CSR[binaryToNumber(BUS[MAX/2+i].substr(0, 4))],
                          binaryToNumber(BUS[MAX/2+i].substr(4, 4)));
                cout << PC << ":" << bt->getData() << "--" << "funct" << endl;
            }
        }
    }
};

string numberToBinary(int x,string s) {
    int res;
    string re;
    stringstream ss;
    ss << s;
    ss >> res;
    for (;res;res /= 2) {
        if(res % 2 == 0)
            re = "0" + re;
        else
            re = "1" + re;
    }
    if (re.length() > x) {
        re = re.substr(re.length() - x);
    } else {
        int n = x - re.length();
        string inter = "";
        for (;n>0;n--) {
            inter = inter + '0';
        }
        re = inter + re;
    }
    return  re;
}

string numberToBinary(int x,int res) {
    string re;
    for (;res;res /= 2) {
        if(res % 2 == 0)
            re = '0' + re;
        else
            re = '1' + re;
    }
    if(re.length() > x) {
        re = re.substr(re.length() - x);
        cerr << "The number has been >= 2^" << x << endl;
    } else {
        int n = x - re.length();
        string inter;
        for(;n>0;n--) {
            inter =inter + '0';
        }
        re = inter + re;
    }
    return re;
}

int binaryToNumber(string s) {
    int array[12]={1,2,4,8,16,32,64,128,256,512,1024,2048};
    int sum=0;
    int j=0;
    for(int i=s.length()-1;i>=0;i--) {
        if(s.at(j)=='1')
            sum+=array[i];
        j++;
    }
    return sum;
}

int hexToNumber(string s) {
    char array1[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    int array2[4]={1,16,256,4096};
    int sum=0;
    for(int i=s.length()-1;i>=0;i--) {
        int x=s.length()-1-i;
        for(int j=0;i<16;j++) {
            if(s.at(i)==array1[j]) {
                sum=sum+j*array2[x];
                break;
            }
        }
    }
    return sum;
}

string hexToBinary(int x,string s) {
    string result;
    int m = 0;
    char he[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    for(int i=0;i<s.length();i++) {
        char s1 = s.at(i);
        for(;m < 16;m++) {
            if(he[m] == s1)
                break;
        }
        result = result + numberToBinary(4,m);
        m=0;
    }
    int sub = x - result.length();
    if(x > result.length()) {
        for(int i=0;i<sub;i++) {
            result = '0' + result;
        }
    }
    return result;
}

string binaryToHex(int x,string s) {
    string re;
    int m=x/4;
    char hex[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    for(int i=0;i<m;i++) {
        string str=s.substr(i*4,4);
        int num = binaryToNumber(str);
        re = re + hex[num];
    }
    return re;
}

string add(string sa,string sb) {
    int a = binaryToNumber(sa);
    int b = binaryToNumber(sb);

    return numberToBinary(8,a+b);
}

string seb(string sa,string sb) {
    int a = binaryToNumber(sa);
    int b = binaryToNumber(sb);

    return numberToBinary(8,a-b);
}

string mul(string sa,string sb) {
    int a = binaryToNumber(sa);
    int b = binaryToNumber(sb);

    return numberToBinary(8,a*b);
}

string div(string sa,string sb) {
    int a = binaryToNumber(sa);
    int b = binaryToNumber(sb);

    return numberToBinary(8,a/b);
}

void copy(string &sa,string sb) {
    sa = sb;
}

void cop(string &sa,int sb) {
    sa = numberToBinary(8,sb);
}

void free(string &sa,string sb) {
    sa = "--------";
}

int read() {
    ifstream infile("test.txt",ios::in);
    string line;
    int length;
    for (length=0;getline(infile,line);length++) {
        IR[length] = line;
    }
    return length;
}

int compile(int length) {
    int x = 0;
    for (int i=0;i<length;i++) {
        string et = IR[i];
        string temp = "";
        string s = "";
        int index = 0;
        int tempIndex = 0;
        for (;index<et.length() && !((et.at(index) >= 'A' && et.at(index) <= 'Z') || et.at(index) >= 'a' && et.at(index) <= 'z');index++) ;
        for (;index<et.length() && ((et.at(index) >= 'A' && et.at(index) <= 'Z') || et.at(index) >= 'a' && et.at(index) <= 'z');index++) {
            temp = temp + et.at(index);
        }

        if(temp == "add") {
            IRbinary[i]="00001100";

            for (;!(et.at(index) >= '0' && et.at(index) <= '9');index++) ;
            for (tempIndex=0;et.at(tempIndex+index) != ',' && et.at(tempIndex+index)!=' ' && et.at(tempIndex+index) != '\t' && et.at(tempIndex+index) != '\n';tempIndex++) ;
            s = et.substr(index,tempIndex);
            IRbinary[i] = IRbinary[i] + numberToBinary(4,s);
            index = index + tempIndex;
            for (;!(et.at(index) >= '0' && et.at(index) <= '9');index++) ;
            for (tempIndex=0;(index+tempIndex) < et.length() && et.at(tempIndex+index) >= '0' && et.at(tempIndex+index) <= '9';tempIndex++) ;
            s = et.substr(index,tempIndex);
            IRbinary[i] = IRbinary[i] + numberToBinary(4,s);
            continue;
        }

        if(temp == "seb") {
            IRbinary[i]="00001101";

            for (;!(et.at(index) >= '0' && et.at(index) <= '9');index++) ;
            for (tempIndex=0;et.at(tempIndex+index) != ',' && et.at(tempIndex+index)!=' ' && et.at(tempIndex+index) != '\t' && et.at(tempIndex+index) != '\n';tempIndex++) ;
            s = et.substr(index,tempIndex);
            IRbinary[i] = IRbinary[i] + numberToBinary(4,s);
            index = index + tempIndex;
            for (;!(et.at(index) >= '0' && et.at(index) <= '9');index++) ;
            for (tempIndex=0;(index+tempIndex) < et.length() && et.at(tempIndex+index) >= '0' && et.at(tempIndex+index) <= '9';tempIndex++) ;
            s = et.substr(index,tempIndex);
            IRbinary[i] = IRbinary[i] + numberToBinary(4,s);
            continue;
        }

        if(temp == "mul") {
            IRbinary[i]="00001110";

            for (;!(et.at(index) >= '0' && et.at(index) <= '9');index++) ;
            for (tempIndex=0;et.at(tempIndex+index) != ',' && et.at(tempIndex+index)!=' ' && et.at(tempIndex+index) != '\t' && et.at(tempIndex+index) != '\n';tempIndex++) ;
            s = et.substr(index,tempIndex);
            IRbinary[i] = IRbinary[i] + numberToBinary(4,s);
            index = index + tempIndex;
            for (;!(et.at(index) >= '0' && et.at(index) <= '9');index++) ;
            for (tempIndex=0;(index+tempIndex) < et.length() && et.at(tempIndex+index) >= '0' && et.at(tempIndex+index) <= '9';tempIndex++) ;
            s = et.substr(index,tempIndex);
            IRbinary[i] = IRbinary[i] + numberToBinary(4,s);
            continue;
        }

        if(temp == "div") {
            IRbinary[i]="00001111";

            for (;!(et.at(index) >= '0' && et.at(index) <= '9');index++) ;
            for (tempIndex=0;et.at(tempIndex+index) != ',' && et.at(tempIndex+index)!=' ' && et.at(tempIndex+index) != '\t' && et.at(tempIndex+index) != '\n';tempIndex++) ;
            s = et.substr(index,tempIndex);
            IRbinary[i] = IRbinary[i] + numberToBinary(4,s);
            index = index + tempIndex;
            for (;!(et.at(index) >= '0' && et.at(index) <= '9');index++) ;
            for (tempIndex=0;(index+tempIndex) < et.length() && et.at(tempIndex+index) >= '0' && et.at(tempIndex+index) <= '9';tempIndex++) ;
            s = et.substr(index,tempIndex);
            IRbinary[i] = IRbinary[i] + numberToBinary(4,s);
            continue;
        }

        if(temp == "copy") {
            IRbinary[i]="00101100";

            for (;!(et.at(index) >= '0' && et.at(index) <= '9');index++) ;
            for (tempIndex=0;et.at(tempIndex+index) != ',' && et.at(tempIndex+index)!=' ' && et.at(tempIndex+index) != '\t' && et.at(tempIndex+index) != '\n';tempIndex++) ;
            s = et.substr(index,tempIndex);
            IRbinary[i] = IRbinary[i] + numberToBinary(4,s);
            index = index + tempIndex;
            for (;!(et.at(index) >= '0' && et.at(index) <= '9');index++) ;
            for (tempIndex=0;(index+tempIndex) < et.length() && et.at(tempIndex+index) >= '0' && et.at(tempIndex+index) <= '9';tempIndex++) ;
            s = et.substr(index,tempIndex);
            IRbinary[i] = IRbinary[i] + numberToBinary(4,s);
            continue;
        }

        if(temp == "cop") {
            IRbinary[i]="00111100";

            for (;!(et.at(index) >= '0' && et.at(index) <= '9');index++) ;
            for (tempIndex=0;et.at(tempIndex+index) != ',' && et.at(tempIndex+index)!=' ' && et.at(tempIndex+index) != '\t' && et.at(tempIndex+index) != '\n';tempIndex++) ;
            s = et.substr(index,tempIndex);
            IRbinary[i] = IRbinary[i] + numberToBinary(4,s);
            index = index + tempIndex;
            for (;!(et.at(index) >= '0' && et.at(index) <= '9');index++) ;
            for (tempIndex=0;(index+tempIndex) < et.length() && et.at(tempIndex+index) >= '0' && et.at(tempIndex+index) <= '9';tempIndex++) ;
            s = et.substr(index,tempIndex);
            IRbinary[i] = IRbinary[i] + numberToBinary(4,s);
            continue;
        }

        if(temp == "free") {
            IRbinary[i]="01001100";

            for (;!(et.at(index) >= '0' && et.at(index) <= '9');index++) ;
            for (tempIndex=0;(index+tempIndex) < et.length() && et.at(tempIndex+index) >= '0' && et.at(tempIndex+index) <= '9';tempIndex++) ;
            s = et.substr(index,tempIndex);
            IRbinary[i] = IRbinary[i] + numberToBinary(4,s) + "0000";
            continue;
        }
        x++;
    }
    return length - x;
}

void sixteen(int length) {
    for (int i=0;i<length;i++) {
        HEX[i] = binaryToHex(16,IRbinary[i]);
    }
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    int length = read();

    for (int i=0;i<length;i++) {
        cout << IR[i] << endl;
    }

    length = compile(length);
    sixteen(length);
    for(int i=0;i<length;i++) {
        cout<<binaryToHex(16,numberToBinary(16,2*i))<<':'<<IRbinary[i]<<'\t'<<HEX[i]<<'\n';
    }

    CPU test = CPU();
    test.run(test.readIR(length));
    return 0;
}
