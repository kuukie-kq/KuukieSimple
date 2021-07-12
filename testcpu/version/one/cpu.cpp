//
// Created by polite on 2021/7/11.
//

//=====/The Start/=====//
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

#define MAX 1024

string IR[MAX];//源代码
string IRbinary[MAX];//二进制代码
string HEX[MAX];//十六进制
string CSR[MAX];//内存
string Instruction[MAX];//语法集
string PC = "0000";

string numberToBinary(int x,string s) { //将十进制数转成二进制
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

string numberToBinary(int x,int res) { //将十进制数转成二进制
    string re;
    for (;res;res /= 2) {
        if(res % 2 == 0)
            re = '0' + re;
        else
            re = '1' + re;
    }
    if(re.length() > x) {
        re = re.substr(re.length() - x);
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

int binaryToNumber(string s) { //将二进制的数转成十进制
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

int hexToNumber(string s) { //将十六进制转成十进制
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

string hexToBinary(int x,string s) { //将十六进制转成二进制
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

string binaryToHex(int x,string s) { //将二进制转成十六进制
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

int takeCSR(string s) {
    int array[15]={1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384};
    int sum=0;
    int j=1;
    for(int i=s.length()-2;i>=0;i--) {
        if(s.at(j)=='1')
            sum+=array[i];
        j++;
    }
    if(s.at(0)=='1')
        return 0 - sum;
    else
        return sum;
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
            Instruction[binaryToNumber(IRbinary[i])] = "add";
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
            Instruction[binaryToNumber(IRbinary[i])] = "seb";
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
            Instruction[binaryToNumber(IRbinary[i])] = "mul";
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
            Instruction[binaryToNumber(IRbinary[i])] = "div";
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
            Instruction[binaryToNumber(IRbinary[i])] = "copy";
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
            Instruction[binaryToNumber(IRbinary[i])] = "cop";
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
            Instruction[binaryToNumber(IRbinary[i])] = "free";

            for (;!(et.at(index) >= '0' && et.at(index) <= '9');index++) ;
            for (tempIndex=0;(index+tempIndex) < et.length() && et.at(tempIndex+index) >= '0' && et.at(tempIndex+index) <= '9';tempIndex++) ;
            s = et.substr(index,tempIndex);
            IRbinary[i] = IRbinary[i] + numberToBinary(8,s);
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

void run(int length) {
    int index = 0;

    ofstream outFile("result.txt",ios::app);
    cout << "CPU execution process==>" << "\n";
    outFile << "CPU执行过程==>" << "\n";
    for (;index<length;index++) {
        CSR[16]="00000000";
        string inter = Instruction[binaryToNumber(IRbinary[index].substr(0,8))];

        if (inter == "add") {
            int d = binaryToNumber(IRbinary[index].substr(8,4));
            int r = binaryToNumber(IRbinary[index].substr(12,4));
            PC = binaryToHex(16,numberToBinary(16,2*index));
            cout << "PC:" << PC << "\texecution:" << IR[index] << "\tdecoding:" << IRbinary[index] << "\t";
            cout << "address R" << d << " + address R" << r << "\tmemory access CSR" << d << ":" << CSR[d] << " memory access CSR" << r << ":" << CSR[r] << "\t";
            outFile << "PC:" << PC << "\t执行:" << IR[index] << "\t译码:" << IRbinary[index] << "\t";
            outFile << "地址R" << d << " + 地址R" << r << "\t访存CSR" << d << ":"<< CSR[d] << "访存CSR" << d << ":" << CSR[r] << "\t";

            int sum1 = takeCSR(CSR[d]) + takeCSR(CSR[r]);
            int sum2 = binaryToNumber(CSR[d]) + binaryToNumber(CSR[r]);
            int sum3 = binaryToNumber(CSR[d].substr(4)) + binaryToNumber(CSR[r].substr(4));
            string result = numberToBinary(8,sum2);
            CSR[d] = result;
            cout << "rewrite:CSR" << d << " the result is :" << CSR[d] << "\n";
            outFile << "写回:CSR" << d << "结果为:" << CSR[d] << "\n";

            if(sum2 > 255)
                CSR[16].at(0) = '1';//标志位C
            if(sum1 == 0)
                CSR[16].at(1) = '1';//标志位Z
            if(sum1 < 0)
                CSR[16].at(2) = '1';//标志位N
            if(sum1 < -127 || sum1 > 127) {
                CSR[16].at(3) = '1';//标志位V
                cerr << "Data overflow!" << endl;
            }
            if(CSR[16].at(2) != CSR[16].at(3)) {
                CSR[16].at(4) = '1';//标志位S
            } else {
                CSR[16].at(2) = '0';
            }
            if(sum3 > 15)
                CSR[16].at(5) = '1';//标志位H
            cout << "C:" << CSR[16].at(0) << '\t';
            cout << "Z:" << CSR[16].at(1) << '\t';
            cout << "N:" << CSR[16].at(2) << '\t';
            cout << "V:" << CSR[16].at(3) << '\t';
            cout << "S:" << CSR[16].at(4) << '\t';
            cout << "H:" << CSR[16].at(5) << '\n';
            outFile << "C:" << CSR[16].at(0) << '\t';
            outFile << "Z:" << CSR[16].at(1) << '\t';
            outFile << "N:" << CSR[16].at(2) << '\t';
            outFile << "V:" << CSR[16].at(3) << '\t';
            outFile << "S:" << CSR[16].at(4) << '\t';
            outFile << "H:" << CSR[16].at(5) << '\n';

            continue;
        }

        if (inter == "seb") {
            int d = binaryToNumber(IRbinary[index].substr(8,4));
            int r = binaryToNumber(IRbinary[index].substr(12,4));
            PC = binaryToHex(16,numberToBinary(16,2*index));
            cout << "PC:" << PC << "\texecution:" << IR[index] << "\tdecoding:" << IRbinary[index] << "\t";
            cout << "address R" << d << " - address R" << r << "\tmemory access CSR" << d << ":" << CSR[d] << " memory access CSR" << r << ":" << CSR[r] << "\t";
            outFile << "PC:" << PC << "\t执行:" << IR[index] << "\t译码:" << IRbinary[index] << "\t";
            outFile << "地址R" << d << " - 地址R" << r << "\t访存CSR" << d << ":"<< CSR[d] << "访存CSR" << d << ":" << CSR[r] << "\t";

            int sum1 = takeCSR(CSR[d]) - takeCSR(CSR[r]);
            int sum2 = binaryToNumber(CSR[d]) - binaryToNumber(CSR[r]);
            int sum3 = binaryToNumber(CSR[d].substr(4)) - binaryToNumber(CSR[r].substr(4));
            string result = numberToBinary(8,sum2);
            CSR[d] = result;
            cout << "rewrite:CSR" << d << " the result is :" << CSR[d] << "\n";
            outFile << "写回:CSR" << d << "结果为:" << CSR[d] << "\n";

            if(sum2 > 255)
                CSR[16].at(0) = '1';//标志位C
            if(sum1 == 0)
                CSR[16].at(1) = '1';//标志位Z
            if(sum1 < 0)
                CSR[16].at(2) = '1';//标志位N
            if(sum1 < -127 || sum1 > 127) {
                CSR[16].at(3) = '1';//标志位V
                cerr << "Data overflow!" << endl;
            }
            if(CSR[16].at(2) != CSR[16].at(3)) {
                CSR[16].at(4) = '1';//标志位S
            } else {
                CSR[16].at(2) = '0';
            }
            if(sum3 > 15)
                CSR[16].at(5) = '1';//标志位H
            cout << "C:" << CSR[16].at(0) << '\t';
            cout << "Z:" << CSR[16].at(1) << '\t';
            cout << "N:" << CSR[16].at(2) << '\t';
            cout << "V:" << CSR[16].at(3) << '\t';
            cout << "S:" << CSR[16].at(4) << '\t';
            cout << "H:" << CSR[16].at(5) << '\n';
            outFile << "C:" << CSR[16].at(0) << '\t';
            outFile << "Z:" << CSR[16].at(1) << '\t';
            outFile << "N:" << CSR[16].at(2) << '\t';
            outFile << "V:" << CSR[16].at(3) << '\t';
            outFile << "S:" << CSR[16].at(4) << '\t';
            outFile << "H:" << CSR[16].at(5) << '\n';

            continue;
        }

        if (inter == "mul") {
            int d = binaryToNumber(IRbinary[index].substr(8,4));
            int r = binaryToNumber(IRbinary[index].substr(12,4));
            PC = binaryToHex(16,numberToBinary(16,2*index));
            cout << "PC:" << PC << "\texecution:" << IR[index] << "\tdecoding:" << IRbinary[index] << "\t";
            cout << "address R" << d << " * address R" << r << "\tmemory access CSR" << d << ":" << CSR[d] << " memory access CSR" << r << ":" << CSR[r] << "\t";
            outFile << "PC:" << PC << "\t执行:" << IR[index] << "\t译码:" << IRbinary[index] << "\t";
            outFile << "地址R" << d << " * 地址R" << r << "\t访存CSR" << d << ":"<< CSR[d] << "访存CSR" << d << ":" << CSR[r] << "\t";

            int sum1 = takeCSR(CSR[d]) * takeCSR(CSR[r]);
            int sum2 = binaryToNumber(CSR[d]) * binaryToNumber(CSR[r]);
            int sum3 = binaryToNumber(CSR[d].substr(4)) * binaryToNumber(CSR[r].substr(4));
            string result = numberToBinary(8,sum2);
            CSR[d] = result;
            cout << "rewrite:CSR" << d << " the result is :" << CSR[d] << "\n";
            outFile << "写回:CSR" << d << "结果为:" << CSR[d] << "\n";

            if(sum2 > 255)
                CSR[16].at(0) = '1';//标志位C
            if(sum1 == 0)
                CSR[16].at(1) = '1';//标志位Z
            if(sum1 < 0)
                CSR[16].at(2) = '1';//标志位N
            if(sum1 < -127 || sum1 > 127) {
                CSR[16].at(3) = '1';//标志位V
                cerr << "Data overflow!" << endl;
            }
            if(CSR[16].at(2) != CSR[16].at(3)) {
                CSR[16].at(4) = '1';//标志位S
            } else {
                CSR[16].at(2) = '0';
            }
            if(sum3 > 15)
                CSR[16].at(5) = '1';//标志位H
            cout << "C:" << CSR[16].at(0) << '\t';
            cout << "Z:" << CSR[16].at(1) << '\t';
            cout << "N:" << CSR[16].at(2) << '\t';
            cout << "V:" << CSR[16].at(3) << '\t';
            cout << "S:" << CSR[16].at(4) << '\t';
            cout << "H:" << CSR[16].at(5) << '\n';
            outFile << "C:" << CSR[16].at(0) << '\t';
            outFile << "Z:" << CSR[16].at(1) << '\t';
            outFile << "N:" << CSR[16].at(2) << '\t';
            outFile << "V:" << CSR[16].at(3) << '\t';
            outFile << "S:" << CSR[16].at(4) << '\t';
            outFile << "H:" << CSR[16].at(5) << '\n';

            continue;
        }

        if (inter == "div") {
            int d = binaryToNumber(IRbinary[index].substr(8,4));
            int r = binaryToNumber(IRbinary[index].substr(12,4));
            PC = binaryToHex(16,numberToBinary(16,2*index));
            cout << "PC:" << PC << "\texecution:" << IR[index] << "\tdecoding:" << IRbinary[index] << "\t";
            cout << "address R" << d << " / address R" << r << "\tmemory access CSR" << d << ":" << CSR[d] << " memory access CSR" << r << ":" << CSR[r] << "\t";
            outFile << "PC:" << PC << "\t执行:" << IR[index] << "\t译码:" << IRbinary[index] << "\t";
            outFile << "地址R" << d << " / 地址R" << r << "\t访存CSR" << d << ":"<< CSR[d] << "访存CSR" << d << ":" << CSR[r] << "\t";

            int sum1 = takeCSR(CSR[d]) / takeCSR(CSR[r]);
            int sum2 = binaryToNumber(CSR[d]) / binaryToNumber(CSR[r]);
            int sum3 = binaryToNumber(CSR[d].substr(4)) / binaryToNumber(CSR[r].substr(4));
            string result = numberToBinary(8,sum2);
            CSR[d] = result;
            cout << "rewrite:CSR" << d << " the result is :" << CSR[d] << "\n";
            outFile << "写回:CSR" << d << "结果为:" << CSR[d] << "\n";

            if(sum2 > 255)
                CSR[16].at(0) = '1';//标志位C
            if(sum1 == 0)
                CSR[16].at(1) = '1';//标志位Z
            if(sum1 < 0)
                CSR[16].at(2) = '1';//标志位N
            if(sum1 < -127 || sum1 > 127) {
                CSR[16].at(3) = '1';//标志位V
                cerr << "Data overflow!" << endl;
            }
            if(CSR[16].at(2) != CSR[16].at(3)) {
                CSR[16].at(4) = '1';//标志位S
            } else {
                CSR[16].at(2) = '0';
            }
            if(sum3 > 15)
                CSR[16].at(5) = '1';//标志位H
            cout << "C:" << CSR[16].at(0) << '\t';
            cout << "Z:" << CSR[16].at(1) << '\t';
            cout << "N:" << CSR[16].at(2) << '\t';
            cout << "V:" << CSR[16].at(3) << '\t';
            cout << "S:" << CSR[16].at(4) << '\t';
            cout << "H:" << CSR[16].at(5) << '\n';
            outFile << "C:" << CSR[16].at(0) << '\t';
            outFile << "Z:" << CSR[16].at(1) << '\t';
            outFile << "N:" << CSR[16].at(2) << '\t';
            outFile << "V:" << CSR[16].at(3) << '\t';
            outFile << "S:" << CSR[16].at(4) << '\t';
            outFile << "H:" << CSR[16].at(5) << '\n';

            continue;
        }

        if (inter == "copy") {
            int d = binaryToNumber(IRbinary[index].substr(8,4));
            int r = binaryToNumber(IRbinary[index].substr(12,4));
            PC = binaryToHex(16,numberToBinary(16,2*index));
            cout << "PC:" << PC << "\texecution:" << IR[index] << "\tdecoding:" << IRbinary[index] << "\t";
            cout << "address R" << d << " < address R" << r << "\tmemory access CSR" << d << ":" << CSR[d] << " memory access CSR" << r << ":" << CSR[r] << "\t";
            outFile << "PC:" << PC << "\t执行:" << IR[index] << "\t译码:" << IRbinary[index] << "\t";
            outFile << "地址R" << d << " < 地址R" << r << "\t访存CSR" << d << ":" << CSR[d] << "访存CSR" << d << ":" << CSR[r] << "\t";

            CSR[d] = CSR[r];
            cout << "rewrite :CSR" << d << " the result is :" << CSR[d] << "\n";
            outFile << "写回:CSR" << d << "结果为:" << CSR[d] << "\n";

            continue;
        }

        if (inter == "cop") {
            int d = binaryToNumber(IRbinary[index].substr(8,4));
            int r = binaryToNumber(IRbinary[index].substr(12,4));
            PC = binaryToHex(16,numberToBinary(16,2*index));
            cout << "PC:" << PC << "\texecution:" << IR[index] << "\tdecoding:" << IRbinary[index] << "\t";
            outFile << "PC:" << PC << "\t执行:" << IR[index] << "\t译码:" << IRbinary[index] << "\t";

            CSR[d] = numberToBinary(8,r);
            cout << "rewrite :CSR" << d << " the result is :" << CSR[d] << "\n";
            outFile << "写回:CSR" << d << "结果为:" << CSR[d] << "\n";

            continue;
        }

        if (inter == "free") {
            int d = binaryToNumber(IRbinary[index].substr(8,8));
            PC = binaryToHex(16,numberToBinary(16,2*index));
            cout << "PC:" << PC << "\texecution:" << IR[index] << "\tdecoding:" << IRbinary[index] << "\t";
            outFile << "PC:" << PC << "\t执行:" << IR[index] << "\t译码:" << IRbinary[index] << "\t";

            CSR[d] = "nullnull";
            cout << "relife :CSR" << d << " the result is :" << CSR[d] << "\n";
            outFile << "释放:CSR" << d << "结果为:" << CSR[d] << "\n";

            continue;
        }
    }
    CSR[16] = "00000000";
    outFile.close();
}

//TestMain
int main() {
    cout << "read test.txt file" << endl;
    int length = read();
    cout << "the result is : " << endl;
    for (int i=0;i<length;i++) {
        cout << IR[i] << endl;
    }

    length = compile(length);
    sixteen(length);
    ofstream outfile("result.txt",ios::out|ios::trunc);
    cout << "bulid result is : " << endl;
    for(int i=0;i<length;i++) {
        cout<<binaryToHex(16,numberToBinary(16,2*i))<<':'<<IRbinary[i]<<"  "<<HEX[i]<<'\n';
        outfile<<binaryToHex(16,numberToBinary(16,2*i))<<':'<<IRbinary[i]<<"  "<<HEX[i]<<'\n';
    }
    outfile.close();

    //内存初始化
    for(int i=0;i<MAX;i++){
        CSR[i] = "00000000";
    }

    run(length);

    return 0;
}
//=====/The End/=====//
