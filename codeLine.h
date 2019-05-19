#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include <map>
#include <regex>
#include <iostream>

#define N_BIT 17
#define I_BIT 16
#define X_BIT 15
#define P_BIT 13

using namespace std;
class codeLine
{
    int pc; // pc value before this line
    bool mode;
    string startLabel;
public:
    unsigned int objcode;
    int newPc;
    int lineNo;
    int address;
    int format;
    bool end = 0;
    bool start=0;
    string line;
    vector<int> errorIds;
    codeLine(string &line, int lineNo);
    void setMode(bool mode);
    void loadPc(int pc);
    void validate(map<string,regex> &operandPatterns, map<string,unsigned int> &labels, map<string,vector<int>> &unknownLabels, map<string, pair<int,unsigned int>> &OPTAB, set<string> &illegalOperations); // validates line for errors and saves its address
    void evaluateDisp(map<string,unsigned int> &labels,map<string, pair<int,unsigned int>> &OPTAB,map<char,unsigned int> regNo);
    int getNewPc(); // return the new pc
    int hex2dec(string hex);
    string dec2hex(int dec);
    string getHexAddress();
    string getStartLabel();
    string getHexObjCode();
private:
    void validateFreeFormat(map<string,regex> &operandPatterns, map<string,unsigned int> &labels, map<string,vector<int>> &unknownLabels, map<string, pair<int,unsigned int>> &OPTAB, set<string> &illegalOperations);
    void validateFixedFormat(map<string,regex> &operandPatterns, map<string,unsigned int> &labels, map<string,vector<int>> &unknownLabels, map<string, pair<int,unsigned int>> &OPTAB, set<string> &illegalOperations);
    bool indexed(string operand);
    bool immediate(string operand);
    bool indirect(string operand);
    bool numeric(string operand);
    unsigned int setBit(unsigned int n,int bit);
    unsigned int clearBit(unsigned int n,int bit);
    bool getBit(unsigned int n,int bit);
public:
    string opcodeFinal;
    string operandFinal;
};
