#include "codeLine.h"
#include <math.h>
codeLine::codeLine(string &line, int lineNo)
{
    this->line = line;
    transform(this->line.begin(), this->line.end(), this->line.begin(), ::tolower);
    this->lineNo = lineNo;
    this->objcode=0;
}
void codeLine::setMode(bool mode)
{
    this->mode = mode;
}
void codeLine::loadPc(int pc)
{
    this->newPc=pc;
    this->pc = pc;
    this->address=pc;
}
void codeLine::validate(map<string,regex> &operandPatterns, map<string,unsigned int> &labels, map<string,vector<int>> &unknownLabels, map<string, pair<int,unsigned int>> &OPTAB, set<string> &illegalOperations)
{
    if(mode == 0)  // free format
    {
        validateFreeFormat(operandPatterns, labels, unknownLabels, OPTAB, illegalOperations);
    }
    else   // fixed format
    {
        validateFixedFormat(operandPatterns, labels, unknownLabels, OPTAB, illegalOperations);
    }
}
void codeLine::validateFreeFormat(map<string,regex> &operandPatterns, map<string,unsigned int> &labels, map<string,vector<int>> &unknownLabels, map<string, pair<int,unsigned int>> &OPTAB, set<string> &illegalOperations)
{

    regex labelPattern("(@|#)?([a-z]([a-z0-9_]*))"); // push mn 8eir @ #

    string l = line;
    string firstField;
    string opcode = "";
    string operand;


    istringstream ss(l);

    ss >> firstField;

// this line is a comment
    if(firstField[0]=='.')
    {
        newPc = pc;
        goto done;
    }
// first field is an operand
    if(operandPatterns.find(firstField) != operandPatterns.end())
    {
        opcode = firstField;
        firstField = "";
        goto secondField;
    }
//check if not format 4
    if(illegalOperations.find(firstField) != illegalOperations.end())
    {
        newPc = pc;
        errorIds.push_back(11);
        goto done;
    }
// first field is a label
// check pattern

    if(!regex_match(firstField, labelPattern))
    {
        newPc = pc;
        errorIds.push_back(14);
        goto secondField;
    }
//check dupplicated
    if(labels.find(firstField) != labels.end())
    {
        newPc = pc;
        errorIds.push_back(4);
        //goto secondField;
    }
    labels.insert(make_pair(firstField,address));
    unknownLabels.erase(firstField);

    ss >> opcode;
secondField:
// error opcode
    if(operandPatterns.find(opcode) == operandPatterns.end())
    {
        newPc = pc;
        errorIds.push_back(8);
        goto done;
    }
// opcode is instruction
    if(OPTAB.find(opcode) != OPTAB.end())
    {
        newPc = pc + OPTAB[opcode].first;
        objcode=OPTAB[opcode].second;
    }
    opcodeFinal=opcode;
    ss >> operand;
    if(!regex_match(operand, operandPatterns[opcode]))
    {
        errorIds.push_back(9);
        goto done;
    }
    if(!operand.empty() && (operand[0]=='@' || operand[0]=='#'))
    {

        if(immediate(operand))
        {
            objcode=setBit(objcode,I_BIT);
        }
        else if(indirect(operand))
        {
            objcode=setBit(objcode,N_BIT);
        }
        if(numeric(operand))
        {
            objcode=clearBit(objcode,P_BIT);
        }
        operand.erase(operand.begin());
    }
    else if(OPTAB[opcodeFinal].first!=2)
    {
        objcode=setBit(objcode,I_BIT);
        objcode=setBit(objcode,N_BIT);
    }
    if(indexed(operand))
    {
        if(OPTAB[opcodeFinal].first==3)
        {
            objcode=setBit(objcode,X_BIT);
        }
        else if(OPTAB[opcodeFinal].first==4)
        {
            objcode=setBit(objcode,X_BIT+4);
        }
        operand=operand.substr(0,operand.length()-2);
    }
    if(regex_match(operand, labelPattern)&&OPTAB[opcode].first!=2)
    {
        if(labels.find(operand) == labels.end())
        {
            unknownLabels[operand].push_back(lineNo);
        }
    }
    operandFinal=operand;
// opcode is directive
// update addresses
    if(opcode == "start")
    {
        address  =  newPc = hex2dec(operand);
        startLabel = firstField;
        start=1;
    }
    else if(opcode == "org")
    {
        address  =  newPc = hex2dec(operand);
        if(firstField != "")
        {
            errorIds.push_back(5);
        }
    }
    else if(opcode == "end")
    {
        newPc = pc;
        startLabel=operand;
        if(firstField != "")
        {
            errorIds.push_back(5);
        }
        end = 1;
    }
    else if(opcode == "byte")
    {
        newPc = pc + operand.length()-3;
    }
    else if(opcode == "word")
    {
        newPc = pc + 3 ;
    }
    else if(opcode == "equ ")
    {
        newPc = pc ;
    }
    else if(opcode == "resb")
    {
        stringstream numbers(operand);
        int x = 0;
        numbers >> x;
        newPc = pc + x;
    }
    else if(opcode == "resw")
    {
        stringstream numbers(operand);
        int x = 0;
        numbers >> x;
        newPc = pc + 3 * x;
    }
    else if(opcode == "base")
    {
        newPc = pc;
        if(firstField != "")
        {
            errorIds.push_back(5);
        }
    }
done:
    return;
}
void codeLine::validateFixedFormat(map<string,regex> &operandPatterns, map<string,unsigned int> &labels, map<string,vector<int>> &unknownLabels, map<string, pair<int,unsigned int>> &OPTAB, set<string> &illegalOperations)
{
    regex labelPattern("(@|#)?([a-z]([a-z0-9_]*))"); // push mn 8eir @ #
    string op_code,operand,label,comment;
    stringstream ss(line);
    string token;
    vector<string> tokens;
    int temp;
    int index;
    unsigned long n=line.length();
    int i=0;
    //check if comment
    if(line[i]=='.')
    {
        newPc = pc;
        goto done;
    }
    label=line.substr(0,8);
    if(label!="        ")
    {
        if(label[0]==' ')
        {
            //misplaced label
            newPc = pc;
            errorIds.push_back(1);
            goto done;
            label="";
        }
    }
    else
    {
        label="";
    }
    if(n>8&&line[8]!=' ')
    {
        //misplaced op_code
        errorIds.push_back(2);
        newPc = pc;
        goto done;
    }
    index=label.find(" ");
    temp=index;
    if(index!=string::npos)
    {
        while(index<label.length())
        {
            if(label[index]!=' ')
            {
                //misplaced op_code
                errorIds.push_back(2);
                newPc = pc;
                goto done;
            }
            index++;
        }
        label=label.substr(0,temp);
    }
    if(n>9)
    {
        op_code=line.substr(9,6);
        if((n>15&&line[15]!=' ')||(n>16&&line[16]!=' '))
        {
            //misplaced operand
            errorIds.push_back(3);
            newPc = pc;
            goto done;
        }
        if(op_code[0]==' ')
        {
            errorIds.push_back(2);
            newPc = pc;
            goto done;
        }
        index=op_code.find(" ");
        temp=index;
        if(index!=string::npos)
        {
            while(index<op_code.length())
            {
                if(op_code[index]!=' ')
                {
                    //misplaced operand
                    errorIds.push_back(3);
                    newPc = pc;
                    goto done;
                }
                index++;
            }
            op_code=op_code.substr(0,temp);
        }
    }
    else
    {
        //missing op_code
        errorIds.push_back(2);
        newPc = pc;
        goto done;
    }
    if(n>17)
    {
        operand=line.substr(17,18);
        if(operand[0]==' ')
        {
            errorIds.push_back(3);
            goto next;
        }
        index=operand.find(" ");
        temp=index;
        if(index!=string::npos)
        {
            while(index<operand.length())
            {
                if(operand.substr(index,1)!=" ")
                {
                        //misplaced comment
                        goto next;
                }
                index++;
            }
            operand=operand.substr(0,temp);
        }
    }
    else
    {
        operand="";
    }
    if(n>35)
    {
        comment=line.substr(35,31);
    }
    // first field is a label
    // check pattern
    next:
    if(label!="")
    {
        if(!regex_match(label, labelPattern))
        {
            newPc = pc;
            errorIds.push_back(14);
        }
        //check dupplicated
        else
        {
            if(labels.find(label) != labels.end())
            {
                newPc = pc;
                errorIds.push_back(4);
            }
            labels.insert(make_pair(label,address));
            unknownLabels.erase(label);
        }
    }
    // error opcode
    if(operandPatterns.find(op_code) == operandPatterns.end())
    {
        newPc = pc;
        errorIds.push_back(8);
        goto done;
    }
    // opcode is instruction
    else
    {
        if(OPTAB.find(op_code) != OPTAB.end())
        {
            newPc = pc + OPTAB[op_code].first;
            objcode=OPTAB[op_code].second;
            opcodeFinal=op_code;
        }
    }
    //check format
    if(!regex_match(operand, operandPatterns[op_code]))
    {
        errorIds.push_back(9);
    }
    else
    {
        if(!operand.empty() && (operand[0]=='@' || operand[0]=='#'))
        {
            if(immediate(operand))
            {
                objcode=setBit(objcode,I_BIT);
            }
            else if(indirect(operand))
            {
                objcode=setBit(objcode,N_BIT);
            }
            if(numeric(operand))
            {
                objcode=clearBit(objcode,P_BIT);
            }
            operand.erase(operand.begin());
        }
        else if(OPTAB[opcodeFinal].first!=2)
        {
            objcode=setBit(objcode,I_BIT);
            objcode=setBit(objcode,N_BIT);
        }
        if(indexed(operand))
        {
            if(OPTAB[opcodeFinal].first==3)
            {
                objcode=setBit(objcode,X_BIT);
            }
            else if(OPTAB[opcodeFinal].first==4)
            {
                objcode=setBit(objcode,X_BIT+4);
            }
            operand=operand.substr(0,operand.length()-2);
        }
        if(regex_match(operand, labelPattern)&&OPTAB[op_code].first!=2)
        {
            if(labels.find(operand) == labels.end())
            {
                unknownLabels[operand].push_back(lineNo);
            }
        }
        operandFinal=operand;
    }
    // opcode is directive
    // update addresses
    //cout <<op_code << ' ' << operand<<endl;
    if(op_code == "start")
    { //cout <<operand<<endl;
        if(find(errorIds.begin(),errorIds.end(),9)!=errorIds.end())
        {
            goto done;
        }
        address  =  newPc = hex2dec(operand);
        startLabel = label;
        start=1;
    }
    else if(op_code == "org")
    {
        if(label != "")
        {
            errorIds.push_back(5);
        }
        if(find(errorIds.begin(),errorIds.end(),9)!=errorIds.end())
        {
            goto done;
        }
        address  =  newPc = hex2dec(operand);
    }
    else if(op_code == "end")
    {
        if(label != "")
        {
            errorIds.push_back(5);
        }
        end = 1;
        if(find(errorIds.begin(),errorIds.end(),9)!=errorIds.end())
        {
            goto done;
        }
        newPc = pc;
        startLabel=operand;
    }
    else if(op_code == "byte")
    {
        if(find(errorIds.begin(),errorIds.end(),9)!=errorIds.end())
        {
            goto done;
        }
        newPc = pc + operand.length()-3;
    }
    else if(op_code == "word")
    {
        newPc = pc + 3 ;
    }
    else if(op_code == "equ ")
    {
        newPc = pc ;
    }
    else if(op_code == "resb")
    {
        if(find(errorIds.begin(),errorIds.end(),9)!=errorIds.end())
        {
            goto done;
        }
        stringstream numbers(operand);
        int x = 0;
        numbers >> x;
        newPc = pc + x;
    }
    else if(op_code == "resw")
    {
        if(find(errorIds.begin(),errorIds.end(),9)!=errorIds.end())
        {
            goto done;
        }
        stringstream numbers(operand);
        int x = 0;
        numbers >> x;
        newPc = pc + 3 * x;
    }
    else if(op_code == "base")
    {
        newPc = pc;
        if(label != "")
        {
            errorIds.push_back(5);
        }
    }
done:
    return;
}
void codeLine::evaluateDisp(map<string,unsigned int> &labels,map<string, pair<int,unsigned int>> &OPTAB,map<char,unsigned int> regNo)
{
    regex labelPattern("(@|#)?([a-z]([a-z0-9_]*))");
    if(opcodeFinal==""||operandFinal=="")
    {
        return;
    }
    if(OPTAB[opcodeFinal].first==2)
    {
        unsigned int disp=(regNo[operandFinal[0]]<<4)|regNo[operandFinal[2]];
        objcode|=disp;
        return;
    }
    unsigned int disp;
    if(regex_match(operandFinal,labelPattern))
    {
        disp=labels[operandFinal];
    }
    else
    {
        stringstream ss(operandFinal);
        ss>>disp;
    }
    if(OPTAB[opcodeFinal].first==3)
    {
        if(getBit(objcode,P_BIT))
        {
            objcode|=((disp-((unsigned int)newPc))&4095);
        }
        else
        {
            objcode|=(disp&4095);
        }
    }
    else if(OPTAB[opcodeFinal].first==4)
    {
        objcode|=(disp&1048575);
    }
    else
    {
        cout<<opcodeFinal<<" invalid"<<endl;
    }
}
int codeLine::getNewPc()
{
    return newPc;
}

int codeLine::hex2dec(string hex)
{
    int dec = 0;
    int x;
    unsigned long n = hex.length()-1;
    for(int i = 0 ; i <= n ; i++)
    {
        if(hex[i] >= '0' && hex[i] <= '9')
            dec += (hex[i]-'0') * pow(16,n-i);
        else
        {
            x = hex[i] - 'a';
            x += 10;
            dec += x *pow(16,n-i);
        }

    }
    return dec;
}
string codeLine::dec2hex(int dec)
{
    stringstream ss;
    ss << std::hex << dec;
    return ss.str();
}
string codeLine::getHexAddress()
{
    return dec2hex(address);
}
string codeLine::getStartLabel()
{
    return startLabel;
}
bool codeLine::indexed(string operand)
{
    regex pattern("((.*),x)");
    return regex_match(operand, pattern);
}
bool codeLine::immediate(string operand)
{
    regex pattern("([#](.*))");
    return regex_match(operand, pattern);
}
bool codeLine::indirect(string operand)
{
    regex pattern("([@](.*))");
    return regex_match(operand, pattern);
}
bool codeLine::numeric(string operand)
{
    regex pattern("(([@]|[#])([0-9]+))");
    return regex_match(operand, pattern);
}
unsigned int codeLine::setBit(unsigned int n,int bit)
{
    return n|(1<<bit);
}
unsigned int codeLine::clearBit(unsigned int n,int bit)
{
    return n&~(1<<bit);
}
bool codeLine::getBit(unsigned int n,int bit)
{
    return (n>>bit)&1!=0;
}
