#ifndef YURI_KLOPOVSKY_CPP_ANALYZER_CPP_INFO_H
#define YURI_KLOPOVSKY_CPP_ANALYZER_CPP_INFO_H

#include <string>
#include <vector>

namespace CppInfo{
       
        
/** @brief Base class for cpp information records. */
class CppInfo{    
protected:

public:
    template<class T> bool is() { return dynamic_cast<T>(this) != nullptr; }
    
    /** @brief Unsafe cast to any type. */
    template<class T> T to() { return (T) this; }
    
    virtual std::string toString() = 0;
};


class CppTypeInfo : public CppInfo{
    
};


/** @brief Plain old data type. 
 
    All available instances are declared in basic_type_info.h file, which is included below.
 */
class BasicTypeInfo : public CppTypeInfo{
    std::string _name;
    
public:    
    BasicTypeInfo(std::string name, bool is_signed) : _name(name), is_signed(is_signed) {}

    const bool is_signed;//Fixme for chars signed/unsigned.
    
    virtual std::string toString();
};

#include "basic_type_info.h"


class VariableInfo : public CppInfo{
     CppTypeInfo* _type_info;
     std::string _name;
     
public:
    VariableInfo(std::string name) { _name = name; }
    
    virtual std::string toString();
    
    inline void setTypeInfo(CppTypeInfo* info) { _type_info = info; }
    
    inline CppTypeInfo* typeInfo() const { return _type_info; }
};


struct FunctionInfo : public CppInfo{
   std::vector<VariableInfo*> parameters;
   std::string _name; 
   
public:
    FunctionInfo(std::string name) : CppInfo() 
    { 
        _name = name;
        _return_type = nullptr;
    }
    
    virtual std::string toString();
    
    inline void addParameterInfo(VariableInfo* info) { parameters.push_back(info); }
    
    inline void setReturnTypeInfo(CppTypeInfo* info) { _return_type = info; }
    
    inline CppTypeInfo* returnTypeInfo() const { return _return_type; }
    
    inline bool hasParameters() const { return !parameters.empty(); }
    
private:
    CppTypeInfo* _return_type;
};

class RecordInfo;


/** @brief Information for a member of a class, a struct or a union. 
 */
class MemberInfo{
    
public:
    enum class Access{
        Public,
        Protected,
        Private
    };
    
    MemberInfo(CppInfo* info, MemberInfo::Access access) 
    : _info(info)
    , _access(access)
    {
    }
    
    inline CppInfo* info() const { return _info; }
    
    virtual std::string toString();
    
    inline bool isVariable() const { return dynamic_cast<VariableInfo*>(_info) != nullptr; }
    
    inline VariableInfo* toVariable() const { return to<VariableInfo*>(); }
    
    inline bool isFunction() const { return dynamic_cast<FunctionInfo*>(_info); }
    
    inline FunctionInfo* toFunction() const { return to<FunctionInfo*>(); }
    
    inline bool isRecord() const;
    
    inline RecordInfo* toRecord() const { return to<RecordInfo*>(); }
    
    inline MemberInfo::Access access() const { return _access; }

    template<class T> T to() const { return (T)(_info); }
    
private:
    CppInfo* _info; //FunctionInfo, VariableInfo etc ...
    Access _access;
};


struct RecordInfo : public CppTypeInfo{
    std::vector<MemberInfo*> members;
    std::string _name;
    
protected:
    RecordInfo(std::string name) { _name = name; }
    
    enum class CSU{
        Class,
        Struct,
        Union
    } csu;
    
public:
    virtual std::string toString();
    
    inline void addMemberInfo(MemberInfo* info) { members.push_back(info); }
    
    inline const std::vector<MemberInfo*>::iterator membersBegin() { return members.begin(); }
    
    inline const std::vector<MemberInfo*>::iterator membersEnd() { return members.end(); }
    
    inline bool hasMemebers() const { return !members.empty(); }
    
    inline bool isClass() const { return csu == CSU::Class; }
    
    inline bool isStruct() const { return csu == CSU::Struct; }
    
    inline bool isUnion() const { return csu == CSU::Union; }    
};


inline bool MemberInfo::isRecord() const { return dynamic_cast<RecordInfo*>(_info) != nullptr; }


class ClassInfo : public RecordInfo {
public:
    ClassInfo(std::string name) : RecordInfo(name) { csu = CSU::Class; }
};


class StructInfo : public RecordInfo {
public:
    StructInfo(std::string name) : RecordInfo(name) { csu = CSU::Struct; }
};


class UnionInfo : public RecordInfo {
public:
    UnionInfo(std::string name) : RecordInfo(name) { csu = CSU::Union; }
};


struct EnumInfo : public CppTypeInfo{
    class Item{
        std::string _name;
        long long _value;
        
    public:
        Item(std::string name, long long value) : _name(name), _value(value) {}
        
        inline std::string toString() { return _name; }
        
        inline long long value() const { return _value; }
    };
    
    std::vector<EnumInfo::Item> items;
    
    virtual std::string toString();
    
    inline void addItem(std::string name, long long value) { items.push_back(EnumInfo::Item(name, value)); }
    
private:
    std::string _name;
};


/** @brief Pointer type descriptor. 
 
    The pointee member points to the actual type beneath all the "stars".
    The depth member describes how many "stars" are there.
    
    One can use the stars() method to get a string with the number "stars" corresponding to the pointer depth.
 */
struct PointerTypeInfo : public CppTypeInfo{
    CppInfo* pointee;
    
    int depth;
    
    PointerTypeInfo(CppInfo* pointee, int depth) : pointee(pointee), depth(depth) {}
    
    virtual std::string toString();
    
    /** @brief Get a string with the number "stars" corresponding to the pointer depth. */
    inline std::string stars() 
    {  
        std::string text = "";
        for(int i=0; i<depth; i++) text.push_back('*');
        return text;
    }
};


struct LvalueRefInfo : public CppTypeInfo{
    CppInfo* pointee;
    
    LvalueRefInfo(CppInfo* pointee) : pointee(pointee) {}
    
    virtual std::string toString();
};


struct RvalueRefInfo : public CppTypeInfo{
    CppInfo* pointee;
    
    RvalueRefInfo(CppInfo* pointee) : pointee(pointee) {}
    
    virtual std::string toString();
};


class TypedefInfo : public CppTypeInfo{
    CppInfo* _type;
    std::string _name;
    
public:
    TypedefInfo(std::string name, CppInfo* type) : _type(type) { _name = name; }
    
    virtual std::string toString();
    
    inline CppInfo* underlyingType() const { return _type; }
};



class ConstantArrayInfo : public CppTypeInfo{
    CppInfo* _element_type;
    long unsigned int _size;
    
public:
    ConstantArrayInfo(CppInfo* element_type, long unsigned int size) 
    : _element_type(element_type), _size(size) 
    {}
    
    virtual std::string toString();
    
    inline CppInfo* elementType() const { return _element_type; }
    
    inline long unsigned int size() const { return _size; }
    
    std::string indexText();
};


struct NamespaceInfo : public CppInfo{
    std::vector<NamespaceInfo*> namespaces;
    std::vector<VariableInfo*> variables;
    std::vector<FunctionInfo*> functions;
    std::vector<ClassInfo*> classes;
    std::vector<StructInfo*> structs;
    std::vector<UnionInfo*> unions;
    std::vector<EnumInfo*> enumerations;
    std::vector<TypedefInfo*> typedefs;
    
    NamespaceInfo(std::string name = "") { _name = name; }
    
    virtual std::string toString(); 
    
    inline void addNamespaceInfo(NamespaceInfo* info) { namespaces.push_back(info); }
    
    inline void addVariableInfo(VariableInfo* info) { variables.push_back(info); }
    
    inline void addFunctionInfo(FunctionInfo* info) { functions.push_back(info); }
    
    inline void addClassInfo(ClassInfo* info) { classes.push_back(info); }
    
    inline void addStructInfo(StructInfo* info) { structs.push_back(info); }
    
    inline void addUnionInfo(UnionInfo* info) { unions.push_back(info); }
    
    inline void addEnumInfo(EnumInfo* info) { enumerations.push_back(info); }
    
    inline void addTypedef(TypedefInfo* info) { typedefs.push_back(info); }
    
public:
    std::string _name;
};


std::vector<FunctionInfo*> find_function_overloads(std::vector<FunctionInfo*> functions, std::string name);


std::vector<MemberInfo*> member_functions(std::vector<MemberInfo*> members);

}//namespace CppAnalyzer

#endif//YURI_KLOPOVSKY_CPP_ANALYZER_CPP_INFO_H