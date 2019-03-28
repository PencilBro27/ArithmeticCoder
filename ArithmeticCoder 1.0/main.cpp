#include <iostream>
#include <fstream>
#include "arithmeticEncoder.h"

using namespace std;

int main() {

    //读入文件信息
    string filename = "Genesis.txt";
    string line;
    string contents;
    ifstream file(filename);

    if (file.is_open()) {
        while (getline(file, line))
            contents += (line + "\n");
        file.close();
    } else {
        cout << "Can not open " << filename << " !!!" << endl;
    }

    contents.pop_back();

    //编码
    arithmeticCoder ac(contents);
    ac.encode();

    //获取编码结果
    uint32_t *data = ac.getData();

    //解码
    arithmeticCoder ac2(data);
    ac2.decode();

    //输出解码结果
    cout << ac2.getString() << endl;
    cout << contents.length() << " Byte  (原大小)" << endl;
    cout << (ac.getData()[0]) * sizeof(uint32_t) << " Byte  (压缩后带索引大小)" << endl;
    cout << (ac.getData()[0] - 258) * sizeof(uint32_t) << " Byte  (压缩后不带索引大小)" << endl;

    return 0;
}