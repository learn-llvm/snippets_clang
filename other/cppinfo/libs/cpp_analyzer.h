#ifndef YURI_KLOPOVKSY_CPP_ANALYZER_H 
#define YURI_KLOPOVKSY_CPP_ANALYZER_H

#include "cpp_info.h"
#include <vector>

namespace CppInfo{
    
struct SourceAnalyzerPrivate;
    
class SourceAnalyzer{
    SourceAnalyzerPrivate* p;
    
public:  
    SourceAnalyzer();
   ~SourceAnalyzer();
    
    NamespaceInfo* analyzeSources(std::vector<const char*> file_paths);

};
    
}//namespace CppAnalyzer

#endif//YURI_KLOPOVKSY_CPP_ANALYZER_H
