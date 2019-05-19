#include <fstream>
#include <string>
#include<iostream>
#include<stdio.h>
#include<vector>
#include<cstring>
#include<algorithm>
#include<set>
#include<map>
#include<queue>
#include<stack>
#include<math.h>
#include<regex>
#include "codeLine.h"

using namespace std;
class assembler
{
    int pc = 0;
    map<string,regex> operandPatterns;
    map<string, pair<int,unsigned int>> OPTAB; // length of instructions
    map<char,unsigned int> registerNo;
    set<string> illegalOperations; //can't be format 4
    map<string,unsigned int> labels; //khaliha map
    map<string,vector<int>> unknownLabels;
    vector<string> errorMsg;
    vector<codeLine> codeLines;
    bool endStatement;
    string startLabel;
public:
    assembler(bool mode, char* readFilePath, char* LISFILEPath, char* OBJFILEPath);
    void run();
private:
    bool mode;
    char* readFilePath;
    char* LISFILEPath;
    char* OBJFILEPath;
    void loadOperandPatterns();
    void loadOPTAB();
    void loadIllegalOperations();
    void loadErrorMsg();
    void loadRegisterNo();
};
