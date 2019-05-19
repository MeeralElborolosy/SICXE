#include "fileParser.h"
assembler::assembler(bool mode, char* readFilePath, char* LISFILEPath, char* OBJFILEPath)
{
    this->mode = mode;
    this->readFilePath = readFilePath;
    this->LISFILEPath = LISFILEPath;
    this->OBJFILEPath = OBJFILEPath;
    endStatement = false;
    startLabel = "";
    loadOperandPatterns();
    loadOPTAB();
    loadRegisterNo();
    loadIllegalOperations();
    loadErrorMsg();
}
void assembler::loadRegisterNo()
{
    registerNo['a']=0;
    registerNo['x']=1;
    registerNo['l']=2;
    registerNo['b']=3;
    registerNo['s']=4;
    registerNo['t']=5;
}
void assembler::loadOperandPatterns()
{
    regex rr("(a|b|l|x|s|t),(a|b|l|x|s|t)");
    regex r("(a|b|l|x|s|t)");
    //regex m("[*]|((@|#)?(([a-z]([a-z0-9_]*))|([0-9]+)))|(([a-z]([a-z0-9_]*))((,x)?))"); //label:(@|#)?([a-z](.*)) //hexa:([0-9a-f]+h)
    //regex m("[*]|((@|#)?(([a-z]([a-z0-9_]*))|([0-9]+))(((+|-)[0-9]+)?))|(([a-z]([a-z0-9_]*))(((+|-)[0-9]+)?)((,x)?))");
    regex m("([*]((([+]|[-])[0-9]+)?))|((@|#)?(([a-z]([a-z0-9_]*))|([0-9]+))((([+]|[-])[0-9]+)?))|(([a-z]([a-z0-9_]*))((([+]|[-])[0-9]+)?)((,x)?))");
    //regex j("((@|#)?(([a-z](.*))|([0-9]+)|0([0-9a-f]+h))(,x)?)"); // j  *
    regex res_b_w("[0-9]{1,4}");
    regex start_org("[0-9a-f]{1,4}");
    regex byte("(c'(.){0,14}')|(x'([a-f0-9]{0,14})')");
    regex word("[0-9a-f]+");
    regex equ("[0-9]{1,4}");
    regex base("");
    regex end("([a-z](.*))*");

    operandPatterns["rmo"] = rr;
    operandPatterns["addr"] = rr;
    operandPatterns["subr"] = rr;
    operandPatterns["compr"] = rr;
    operandPatterns["tixr"] = r;
    operandPatterns["lda"] = m;
    operandPatterns["sta"] = m;
    operandPatterns["ldb"] = m;
    operandPatterns["stb"] = m;
    operandPatterns["ldl"] = m;
    operandPatterns["stl"] = m;
    operandPatterns["ldx"] = m;
    operandPatterns["stx"] = m;
    operandPatterns["lds"] = m;
    operandPatterns["sts"] = m;
    operandPatterns["ldt"] = m;
    operandPatterns["stt"] = m;
    operandPatterns["ldch"] = m;
    operandPatterns["stch"] = m;
    operandPatterns["+lda"] = m;
    operandPatterns["+sta"] = m;
    operandPatterns["+ldb"] = m;
    operandPatterns["+stb"] = m;
    operandPatterns["+ldl"] = m;
    operandPatterns["+stl"] = m;
    operandPatterns["+ldx"] = m;
    operandPatterns["+stx"] = m;
    operandPatterns["+lds"] = m;
    operandPatterns["+sts"] = m;
    operandPatterns["+ldt"] = m;
    operandPatterns["+stt"] = m;
    operandPatterns["+ldch"] = m;
    operandPatterns["+stch"] = m;
    operandPatterns["add"] = m;
    operandPatterns["sub"] = m;
    operandPatterns["+add"] = m;
    operandPatterns["+sub"] = m;
    operandPatterns["comp"] = m;
    operandPatterns["+comp"] = m;
    operandPatterns["j"] = m;
    operandPatterns["jeq"] = m;
    operandPatterns["jlt"] = m;
    operandPatterns["jgt"] = m;
    operandPatterns["+j"] = m;
    operandPatterns["+jeq"] = m;
    operandPatterns["+jlt"] = m;
    operandPatterns["+jgt"] = m;
    operandPatterns["tix"] = m;
    operandPatterns["+tix"] = m;
    operandPatterns["start"] = start_org;
    operandPatterns["org"] = start_org;
    operandPatterns["equ"] = equ;
    operandPatterns["resb"] = res_b_w;
    operandPatterns["resw"] = res_b_w;
    operandPatterns["byte"] = byte;
    operandPatterns["word"] = word;
    operandPatterns["base"] = base;
    operandPatterns["end"] = end;
}

void assembler::loadOPTAB()
{
    OPTAB["rmo"] = make_pair(2,0xac00);
    OPTAB["addr"] = make_pair(2,0x9000);
    OPTAB["subr"] = make_pair(2,0x9400);
    OPTAB["compr"] = make_pair(2,0xa000);
    OPTAB["tixr"] = make_pair(2,0xb800);
    OPTAB["lda"] = make_pair(3,0x000000|(1<<13));
    OPTAB["sta"] = make_pair(3,0x0c0000|(1<<13));
    OPTAB["ldb"] = make_pair(3,0x680000|(1<<13));
    OPTAB["stb"] = make_pair(3,0x780000|(1<<13));
    OPTAB["ldl"] = make_pair(3,0x080000|(1<<13));
    OPTAB["stl"] = make_pair(3,0x140000|(1<<13));
    OPTAB["ldt"] = make_pair(3,0x740000|(1<<13));
    OPTAB["stt"] = make_pair(3,0x840000|(1<<13));
    OPTAB["ldx"] = make_pair(3,0x040000|(1<<13));
    OPTAB["stx"] = make_pair(3,0x100000|(1<<13));
    OPTAB["lds"] = make_pair(3,0x6c0000|(1<<13));
    OPTAB["sts"] = make_pair(3,0x7c0000|(1<<13));
    OPTAB["ldch"] = make_pair(3,0x500000|(1<<13));
    OPTAB["stch"] = make_pair(3,0x540000|(1<<13));
    OPTAB["+lda"] = make_pair(4,0x00000000|(0x03100000));
    OPTAB["+sta"] = make_pair(4,0x0c000000|(0x03100000));
    OPTAB["+ldb"] = make_pair(4,0x68000000|(0x03100000));
    OPTAB["+stb"] = make_pair(4,0x78000000|(0x03100000));
    OPTAB["+ldl"] = make_pair(4,0x08000000|(0x03100000));
    OPTAB["+stl"] = make_pair(4,0x14000000|(0x03100000));
    OPTAB["+ldx"] = make_pair(4,0x04000000|(0x03100000));
    OPTAB["+stx"] = make_pair(4,0x10000000|(0x03100000));
    OPTAB["+lds"] = make_pair(4,0x6c000000|(0x03100000));
    OPTAB["+sts"] = make_pair(4,0x7c000000|(0x03100000));
    OPTAB["+ldt"] = make_pair(4,0x74000000|(0x03100000));
    OPTAB["+stt"] = make_pair(4,0x84000000|(0x03100000));
    OPTAB["+ldch"] = make_pair(4,0x5000000|(0x03100000));
    OPTAB["+stch"] = make_pair(4,0x5400000|(0x03100000));
    OPTAB["add"] = make_pair(3,0x180000|(1<<13));
    OPTAB["sub"] = make_pair(3,0x1c0000|(1<<13));
    OPTAB["+add"] = make_pair(4,0x18000000|(0x03100000));
    OPTAB["+sub"] = make_pair(4,0x1c000000|(0x03100000));
    OPTAB["comp"] = make_pair(3,0x280000|(1<<13));
    OPTAB["+comp"] = make_pair(4,0x28000000|(0x03100000));
    OPTAB["j"] = make_pair(3,0x3c0000|(1<<13));
    OPTAB["jeq"] = make_pair(3,0x300000|(1<<13));
    OPTAB["jlt"] = make_pair(3,0x380000|(1<<13));
    OPTAB["jgt"] = make_pair(3,0x340000|(1<<13));
    OPTAB["+j"] = make_pair(4,0x3c000000|(0x03100000));
    OPTAB["+jeq"] = make_pair(4,0x30000000|(0x03100000));
    OPTAB["+jlt"] = make_pair(4,0x38000000|(0x03100000));
    OPTAB["+jgt"] = make_pair(4,0x34000000|(0x03100000));
    OPTAB["tix"] = make_pair(3,0x2c0000|(1<<13));
    OPTAB["+tix"] = make_pair(4,0x2c000000|(0x03100000));
}
void assembler::loadIllegalOperations()
{
    illegalOperations.insert("+rmo");
    illegalOperations.insert("+addr");
    illegalOperations.insert("+subr");
    illegalOperations.insert("+tixr");
    illegalOperations.insert("+mulr");
    illegalOperations.insert("+divr");

}
void assembler::loadErrorMsg()
{
    errorMsg.push_back("misplaced label"); //1
    errorMsg.push_back("missing or misplaced operation mnemonic"); //2
    errorMsg.push_back("missing or misplaced operand field"); //3
    errorMsg.push_back("duplicate label definition"); //4
    errorMsg.push_back("this statement can't have a label"); //5
    errorMsg.push_back("this statement can't have an operand"); //6
    errorMsg.push_back("wrong operation prefix"); //7
    errorMsg.push_back("unrecognized operation code"); //8
    errorMsg.push_back("undefined symbol in operand"); //9
    errorMsg.push_back("not a hexadecimal string"); //10
    errorMsg.push_back("can't be format 4 instruction"); //11
    errorMsg.push_back("illegal address for a register"); //12
    errorMsg.push_back("missing END statement"); //13
    errorMsg.push_back("invalid label"); //14
    errorMsg.push_back("end operand doesn't match start label"); //15
}
void assembler::run()
{
    fileParser fp(errorMsg, mode);
    codeLines = fp.readFile(readFilePath);
    for(int i=0; i<codeLines.size(); i++)
    {
        codeLines[i].setMode(mode);
        codeLines[i].loadPc(pc);
        codeLines[i].validate(operandPatterns, labels, unknownLabels, OPTAB, illegalOperations);
        pc = codeLines[i].getNewPc();
        if(codeLines[i].start)
        {
            startLabel=codeLines[i].getStartLabel();
        }
        if(codeLines[i].end)
        {
            if(codeLines[i].getStartLabel()!=""&&codeLines[i].getStartLabel()!=startLabel)
            {
                codeLines[i].errorIds.push_back(15);
                cout<<endl<<endl<<codeLines[i].getStartLabel()<<" "<<startLabel<<endl<<endl;
            }
            endStatement=true;
            break;
        }
    }

    for(int i=0;i<codeLines.size();i++)
    {
        codeLines[i].evaluateDisp(labels,OPTAB,registerNo);
    }
    bool error = fp.writeListFile(codeLines, LISFILEPath, endStatement);
    if(!error) {
        fp.writeObjectFile(codeLines, OBJFILEPath);
    }
}
