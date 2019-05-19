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
void fileParser::writeFile(vector<codeLine> &codeLines, char * LISFILE, bool endStatement)
{
    ofstream lisfile;
    lisfile.open(LISFILE);

    bool error = false;
    if(mode==1)
    {
        cout << "Line no.\tAddress\tLabel\tMnemonic Operands Comments\n";
        lisfile << "Line no.\tAddress\tLabel\tMnemonic Operands Comments\n";
    }
    for(codeLine line : codeLines)
    {
        for(int id: line.errorIds)
        {
            cout << errorMsg[id-1] << endl;
            lisfile << errorMsg[id-1] << endl;
            error = true;
        }
        cout << line.lineNo << "\t\t" << line.getHexAddress() << "\t" << line.line <<"\t" << std::hex << line.objcode[0] << endl;
        lisfile << line.lineNo << "\t\t" <<line.getHexAddress() << "\t" << line.line <<"\t" << std::hex << line.objcode[0] << endl;
    }
    if(!endStatement)
    {
        cout << errorMsg[12] << endl;
        lisfile << errorMsg[12] << endl;
    }
    if(!error)
    {
        cout << "Successful Assembly" << endl;
        lisfile << "Successful Assembly" << endl;
    }
    else
    {
        cout << "Unsuccessful Assembly" << endl;
        lisfile << "Unsuccessful Assembly" << endl;
    }
}
void fileParser::writeObjectFile(vector<codeLine> &codeLines, char * OBJFILE,string length){
    ofstream objfile;
    objfile.open(OBJFILE);

    pair<string, string> currTxtRec = make_pair("",""); // address, text record
    string startAddress;
    for(codeLine line : codeLines)
    {
        // write record in OBJFILE
        //cout << "OPCODE FINAL   "<<line.opcodeFinal <<endl;
        if(!line.line.empty() && line.line[0] != '.'){ // comment
            writeRecords(line, objfile, currTxtRec, startAddress,length);
        }
    }

}
/*string zeroPadding(int requiredSize, string str){
    while(str.size() < requiredSize){
        str.insert(str.begin(), '0');
    }
    return str;
}*/
void fileParser::writeRecords(codeLine &line, ofstream &objfile, pair<string, string> &currTxtRec, string &startAddress,string length){
    // write header
    if(line.opcodeFinal == "start" ){
        startAddress = line.getHexAddress();
        while(startAddress.size() < 6){
            startAddress.insert(startAddress.begin(), '0');
        }
        string label = line.getStartLabel();
        while(label.size() < 6){
            label.insert(label.begin(), ' ');
        }
        cout << endl << "H" << "^" << label << "^" << startAddress << "^" <<length;
        objfile << endl << "H" << "^" << label << "^" << startAddress << "^" <<length;
        return;
    }

    // write current record
    if(((currTxtRec.second != "" && currTxtRec.first != "")&&(line.opcodeFinal == "resw" || line.opcodeFinal == "resb" || line.opcodeFinal == "end")) || (currTxtRec.second.size() + line.format >=30)){ // > 1E hex
        int length = currTxtRec.second.size()/2; string zero = "";
        if(length < 16){
            zero = "0";
        }
        cout << endl << "T" << "^" << currTxtRec.first << "^" << zero <<  hex << currTxtRec.second.size()/2 << "^" << currTxtRec.second;
        objfile << endl << "T" << "^" <<  currTxtRec.first << "^" << zero << hex << currTxtRec.second.size()/2 << "^" << currTxtRec.second;
        currTxtRec = make_pair("", "");
    }
    // write end
    if(line.opcodeFinal == "end") {
        cout << endl << "E" << "^" << startAddress;
        objfile << endl << "E" << "^" << startAddress;
        return;
    }
    if(currTxtRec.second == "" && currTxtRec.first == ""  && (line.opcodeFinal != "end" && line.opcodeFinal != "resw" && line.opcodeFinal != "resb")){
        string add = line.getHexAddress();
        while(add.size() < 6){
            add.insert(add.begin(), '0');
        }
        currTxtRec = make_pair(add, line.getHexObjCode());
        return;
    }
    //append objcode to current text record
    if(currTxtRec.second != "" && currTxtRec.first != "" ){
        currTxtRec.second += line.getHexObjCode();
    }

}

