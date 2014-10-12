#define CPP_INFO_IMPLEMENTATION
#include "cpp_info.h"
#undef CPP_INFO_IMPLEMENTATION

#include <sstream>

namespace CppInfo{
    
std::string BasicTypeInfo::toString() 
{ 
    std::string prefix; 
    if(!is_signed) prefix = "unsigned "; 
    return prefix + _name; 
}
     
     
std::string VariableInfo::toString()
{
    return _name;
}


std::string FunctionInfo::toString()
{
    return _name;
}


std::string MemberInfo::toString()
{
    return _info->toString();
}


std::string RecordInfo::toString()
{
    return _name;
}


std::string EnumInfo::toString()
{
    return _name;
}


std::string TypedefInfo::toString()
{
    return _name;
}


std::string NamespaceInfo::toString()
{
    return _name;
}

     
std::string PointerTypeInfo::toString()
{
    return pointee->toString() + stars();
}


std::string LvalueRefInfo::toString()
{
    return pointee->toString() + "&";
}


std::string RvalueRefInfo::toString()
{
    return pointee->toString() + "&&";
}


std::string ConstantArrayInfo::toString()
{
    return elementType()->toString() + indexText();
}


std::string ConstantArrayInfo::indexText()
{
    std::stringstream ss;
    ss << "[" << size() << "]";
    return ss.str();
}

    
std::vector<FunctionInfo*> find_function_overloads(std::vector<FunctionInfo*> functions, std::string name)
{
    std::vector<FunctionInfo*> overloads;
    
    for(auto function : functions)
    {
        if(function->toString() == name)
        {
            overloads.push_back(function);
        }
    }
    
    return overloads;
}


std::vector<MemberInfo*> member_functions(std::vector<MemberInfo*> members)
{
    std::vector<MemberInfo*> functions;
    
    for(auto member : members)
    {
        if(member->isFunction())
        {
            functions.push_back(member);
        }
    }
    
    return functions;
}


}//namespace CppInfo