#include "cpp_analyzer.h"

#include <iostream>

using namespace std;
using namespace CppInfo;


int main(int argc, char* argv[])
{
#ifdef DEBUG
    cout << "debug build\n";
#endif//DEBUG
    
    if(argc < 2)
    {
        cerr << "No input!\n";
        return 1;
    }
    
    vector<const char*> files = {argv[1] };
    SourceAnalyzer a;
    auto info = a.analyzeSources(files);
        
    for(auto e : info->enumerations)
    {
        cout << "enum " << e->toString() << "{\n";
        for(auto item : e->items)
        {
            cout << "   " << item.toString() << "\n";
        }
        cout << "}\n";
    }
    
    return 0;
}