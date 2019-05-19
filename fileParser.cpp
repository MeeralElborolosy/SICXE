/*#include "fileParser.h"

vector<codeLine> fileParser::readFile(char*path){
  vector<codeLine> codeLines;
  return codeLines;
}
void fileParser::writeFile(char*path, vector<codeLine> &codeLines){
}*/
#include "fileParser.h"
fileParser::fileParser(vector<string> &errorMsg, bool mode)
{
    this->mode = mode;
    this->errorMsg = errorMsg;
}
vector<codeLine> fileParser::readFile(char * path)
{
    vector<codeLine> codeLines;
    int count=1;
    ifstream inFile;
    inFile.open(path);
    string line;
    while (!inFile.eof())
    {
        getline(inFile,line);
        if(line==""){
            continue;
        }
        codeLine code_line(line,count);
        codeLines.push_back(code_line);
        count++;
    }
    count--;
    inFile.close();
    return codeLines;
}
void fileParser::writeFile(vector<codeLine> &codeLines,char * path,bool endStatement)
{
    ofstream outFile;
    outFile.open(path);
    bool error=false;
    if(mode==1)
    {
        cout<<"Line no.\tAddress\tLabel\tMnemonic Operands Comments\n";
        outFile<<"Line no.\tAddress\tLabel\tMnemonic Operands Comments\n";
    }
    for(codeLine line : codeLines)
    {
        for(int id: line.errorIds)
        {
            cout<<errorMsg[id-1]<<endl;
            outFile<<errorMsg[id-1]<<endl;
            error=true;
        }
        cout<<line.lineNo<<"\t\t"<<line.getHexAddress()<<"\t"<<line.line<<"\t"<<std::hex<<line.objcode[0]<<"\t";
        outFile<<line.lineNo<<"\t\t"<<line.getHexAddress()<<"\t"<<line.line<<"\t"<<std::hex<<line.objcode[0]<<"\t";
        for(int i=1;i<line.objcode.size();i++)
        {
            cout<<std::hex<<line.objcode[i]<<"\t";
        }
        cout<<endl;
    }
    if(!endStatement)
    {
        cout<<errorMsg[12]<<endl;
        outFile<<errorMsg[12]<<endl;
    }
    if(!error)
    {
        cout<<"Successful Assembly"<<endl;
        outFile<<"Successful Assembly"<<endl;
    }
    else
    {
        cout<<"Unsuccessful Assembly"<<endl;
        outFile<<"Unsuccessful Assembly"<<endl;
    }
}
