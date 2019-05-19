#include "assembler.h"
class fileParser
{
public:
    vector<string> errorMsg;
    char * path;
    bool mode;
    fileParser(vector<string> &errorMsg,bool mode);
    vector<codeLine> readFile(char * path);
    bool writeListFile(vector<codeLine> &codeLines,map<string,unsigned int> labels, char * LISFILE, bool endStatement);
    void writeObjectFile(vector<codeLine> &codeLines, char * OBJFILE,string length);
private:
    void writeRecords(codeLine &line, ofstream &objfile, pair<string, string> &currTxtRec, string &startAddress,string length);
};
