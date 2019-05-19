#include "assembler.h"

int main()
{
    char readFilePath[] = "code.txt";
    char LISFILEPath[] = "LISFILE.txt";
    char OBJFILEPath[] = "OBJFILE.txt";
    int mode;
    do{
        cout<<"\t\tSIC/XE Assembler\nPlease enter your desired mode:\n0: Free Format\t1: Fixed Format"<<endl;
        cin>>mode;
    }while(mode!=0&&mode!=1);
    cout<<(mode==0?"Free Format\n":"Fixed Format\n");
    assembler *a = new assembler(mode, readFilePath, LISFILEPath, OBJFILEPath); // 0 ==> free // 1 ==> fixed
    a->run();
    return 0;
}
