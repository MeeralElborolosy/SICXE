#include "assembler.h"
class fileParser
{
public:
    vector<string> errorMsg;
    char * path;
    bool mode;
    fileParser(vector<string> &errorMsg,bool mode);
    vector<codeLine> readFile(char * path);
    void writeListFile(vector<codeLine> &codeLines, char * LISFILE, bool endStatement);
    void writeObjectFile(vector<codeLine> &codeLines, char * OBJFILE);
private:
    void writeRecords(codeLine &line, ofstream &objfile, pair<string, string> &currTxtRec, string &startAddress);

};
