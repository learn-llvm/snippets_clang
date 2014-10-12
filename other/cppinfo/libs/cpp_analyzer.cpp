#include "cpp_analyzer.h"
#include <iostream>
#include <map>
#include <clang-c/Index.h>
#include <assert.h>

using namespace std;

namespace CppInfo{

    
/** @brief Data beign passed to visit_namespace_cursor. */
struct NamespaceVisitData{
    SourceAnalyzerPrivate* p;
    NamespaceInfo* info;
    
    NamespaceVisitData(SourceAnalyzerPrivate* p, NamespaceInfo* info) : p(p), info(info) {}
};
    
/** @brief Traverse the contents of namespace. */    
CXChildVisitResult visit_namespace_cursor(CXCursor cursor, CXCursor parent, CXClientData data);

/** @brief Traverse the contents of a class, struct or union. */
CXChildVisitResult visit_class_cursor(CXCursor cursor, CXCursor parent, CXClientData data);

/** Passed to visit_class_cursor function as clinet data. 
    Containes some of the data needed to traverse the class.
    This is needed to transfer data between consecutive invocations of visit_class_cursor.
 */
struct ClassVisitData{
    SourceAnalyzerPrivate* p;
    MemberInfo::Access current_access;
    RecordInfo* info;
    
    ClassVisitData(SourceAnalyzerPrivate* p, MemberInfo::Access initial_access, RecordInfo* info) 
    : p(p)
    , current_access(initial_access)
    , info(info) {}
};


CXChildVisitResult visit_enum_cursor(CXCursor cursor, CXCursor parent, CXClientData data);

struct EnumVisitData{
    SourceAnalyzerPrivate* p;
    EnumInfo* info;
    
    EnumVisitData(SourceAnalyzerPrivate* p, EnumInfo* info) : p(p), info(info) {}
};


/** */
int find_pointee_type_and_pointer_depth(CXType &pointee_type, CXType pointer_type);


template<typename T> struct KnownItem{
    CXCursor cursor;
    T info;
    
    KnownItem(CXCursor cursor = clang_getNullCursor(), T info = nullptr) : cursor(cursor), info(info) {}
    
    inline bool isGood() { return (info != nullptr && !clang_Cursor_isNull(cursor)); }
};


/** @brief Maps cursors to class declarations.
     
    This is needed to find the CppInfo of a class, typedef or enum 
    when an instance of that type is returned from a function.
    See the case for CXType_Record, in the cx2type function.
*/
template<typename T> class KnownItems{
public:
    
    void add(CXCursor cursor, T info)
    {
        known_items.push_back(KnownItem<T>(cursor, info));
    }
    
    inline KnownItem<T> operator[](CXCursor cursor)
    {
        for(int i=0; i<(int)known_items.size(); i++)
        {
            auto &item = known_items[i];
            /*I don't know what xdata actually means. It's not documented anywhere.
                But this type of comparison works.
                */
            if(item.cursor.xdata == cursor.xdata) return item;
        }
        
        return KnownItem<T>();
    }
    
    inline KnownItem<T> operator[](RecordInfo* info)
    {
        for(int i=0; i<(int)known_items.size(); i++)
        {
            auto &item = known_items[i];
            if(item.info == info) return item;
        }
        
        return KnownItem<T>();
    }
    
private:
    vector<KnownItem<T>> known_items;
};


struct SourceAnalyzerPrivate{    
    KnownItems<RecordInfo*> known_records;
    KnownItems<EnumInfo*> known_enumerations;
    KnownItems<TypedefInfo*> known_typedefs;
   
    
    /** @brief Convert CXType to CppTypeInfo */
    CppTypeInfo* cx2type(CXType type)
    {    
        switch(type.kind)
        {
            case CXType_Invalid:
            case CXType_Unexposed:
            {
                return nullptr;
            }
            
            case CXType_Void:
            {
                return &cpp_type_void;
            }
            
            case CXType_Bool:
            {
                return &cpp_type_bool;
            }
            
            case CXType_Char_U:
            {
                cerr << "[Got CXType_Char_U]\n";
                return nullptr;
            }
            
            case CXType_UChar:
            {
                return &cpp_type_uchar;
            }
            
            case CXType_Char16:
            {
                return &cpp_type_uchar16;
            }
            
            case CXType_Char32:
            {
                return &cpp_type_uchar32;
            }
            
            case CXType_UShort:
            {
                return &cpp_type_ushort;
            }
            
            case CXType_UInt:
            {
                return &cpp_type_uint;
            }
            
            case CXType_ULong:
            {
                return &cpp_type_ulong;
            }
            
            case CXType_ULongLong:
            {
                return &cpp_type_ulong_long;
            }
            
            case CXType_UInt128:
            {
                return &cpp_type_uint128;
            }
            
            case CXType_Char_S:
            {
                return &cpp_type_char;
            }
            
            case CXType_SChar:
            {
                return &cpp_type_char;
            }
            
            case CXType_WChar:
            {
                return &cpp_type_wchar;
            }
            
            case CXType_Short:
            {
                return &cpp_type_short;
            }
            
            case CXType_Int:
            {
                return &cpp_type_int;
            }
            
            case CXType_Long:
            {
                return &cpp_type_long;
            }
            
            case CXType_LongLong:
            {
                return &cpp_type_long_long;
            }
            
            case CXType_Int128:
            {
                return &cpp_type_int128;
            }
            
            case CXType_Float:
            {
                return &cpp_type_float;
            }
            
            case CXType_Double:
            {
                return &cpp_type_double;
            }
            
            case CXType_LongDouble:
            {
                return &cpp_type_long_double;
            }
            
            case CXType_NullPtr:
            {
                return &cpp_type_nullptr;
            }
            
            case CXType_Overload:
            case CXType_Dependent:
            case CXType_ObjCId:
            case CXType_ObjCClass:
            case CXType_ObjCSel:
            case CXType_Complex:
            case CXType_ObjCInterface:
            case CXType_ObjCObjectPointer:
            {
                cerr << "cx2type: type not implemented!\n";
                abort();
                return nullptr;
            }
            
            case CXType_Pointer:
            {
                CXType pointee_type;
                int pointer_depth = find_pointee_type_and_pointer_depth(pointee_type, type);
                CppInfo* type = cx2type(pointee_type);
                if(type == nullptr)
                {
                    cerr << "cx2type: CXType_Pointer Warning got nullptr!\n";
                    return nullptr;
                }
                
                return new PointerTypeInfo(type, pointer_depth);
            }
            
            case CXType_BlockPointer:
            {
                cerr << "cx2type: Got CXType_BlockPointer!\n";
                return nullptr;
            }
            
            case CXType_LValueReference:
            {
                auto pointee_cxtype = clang_getPointeeType(type);
                auto pointee_type = cx2type(pointee_cxtype);
                return new LvalueRefInfo(pointee_type);
            }
                
            case CXType_RValueReference:
            {
                auto pointee_cxtype = clang_getPointeeType(type);
                auto pointee_type = cx2type(pointee_cxtype);
                return new RvalueRefInfo(pointee_type);
            }
           
            
            case CXType_Record:
            {
                auto record_cursor = clang_getTypeDeclaration(type);
                auto record = known_records[record_cursor];
                if(record.isGood())
                {
                    return record.info;
                }
                else
                {
                    cerr << "cx2type: Record is bad!\n";
                    abort();
                }
                return nullptr;
            }
            
            case CXType_Enum:
            {
                auto enum_cursor = clang_getTypeDeclaration(type);
                auto e = known_enumerations[enum_cursor];
                if(e.isGood())
                {
                    return e.info;
                }
                else
                {
                    cerr << "cx2type: Enum is bad!\n";
                    abort();
                }
                return nullptr;
            }
            
            case CXType_Typedef:
            {
                auto typedef_cursor = clang_getTypeDeclaration(type);
                auto tpd = known_typedefs[typedef_cursor];
                if(tpd.isGood())
                {
                    return tpd.info;
                }
                else
                {
                    cerr << "cx2type: Typedef is bad!\n";
                    abort();
                }
                return nullptr;
            }
            
            case CXType_FunctionNoProto:
            {
                cerr << "cx2type: Got CXType_FunctionNoProto!\n";
                return nullptr;
            }
            
            case CXType_FunctionProto:
            {
                cerr << "cx2type: Got CXType_FunctionProto!\n";
                return nullptr;
            }
            
            case CXType_ConstantArray:
            {
                CppInfo* element_type = cx2type(clang_getElementType(type));
                long unsigned int size = clang_getArraySize(type);
                return new ConstantArrayInfo(element_type, size);
            }
            
            case CXType_Vector:
            {
                cerr << "cx2type: Got CXType_Vector!\n";
                return nullptr;
            }
            
            default: return nullptr;
        }
    }

    
    VariableInfo* get_variable_info(CXCursor cursor)
    {
        auto variable_info = new VariableInfo(clang_getCString( clang_getCursorSpelling(cursor) ));
                
        CXType type = clang_getCursorType(cursor);
        variable_info->setTypeInfo(cx2type(type));
        
        //TODO: analyze initializer
        
        return variable_info;
    }

    
    FunctionInfo* get_function_info(CXCursor cursor)
    {
        auto function_info = new FunctionInfo(clang_getCString( clang_getCursorSpelling(cursor) ));
                
        CXType type = clang_getCursorType(cursor);
        
        if(type.kind == CXType_FunctionProto)
        {
            CXType return_type = clang_getResultType(type);
            auto typeinfo = cx2type(return_type);
            function_info->setReturnTypeInfo(typeinfo);
            
            int arg_count = clang_getNumArgTypes(type);
            
            for(int i=0; i<arg_count; i++)
            {
                auto arg_cursor = clang_Cursor_getArgument(cursor, i);
                if(arg_cursor.kind == CXCursor_ParmDecl)
                {
                    function_info->addParameterInfo(get_variable_info(arg_cursor));
                    
                }
            }
        }
        
        return function_info;
    }


    ClassInfo* get_class_info(CXCursor cursor)
    {
        auto class_info = new ClassInfo(clang_getCString( clang_getCursorSpelling(cursor) ));

        ClassVisitData data(this, MemberInfo::Access::Private, class_info);
        clang_visitChildren(cursor, visit_class_cursor, &data);
        
        return class_info;
    }
    
    StructInfo* get_struct_info(CXCursor cursor)
    {
        auto struct_info = new StructInfo(clang_getCString( clang_getCursorSpelling(cursor) ));
                
        ClassVisitData data(this, MemberInfo::Access::Private, struct_info);
        clang_visitChildren(cursor, visit_class_cursor, &data);
        
        return struct_info;
    }


    UnionInfo* get_union_info(CXCursor cursor)
    {
        auto union_info = new UnionInfo(clang_getCString( clang_getCursorSpelling(cursor) ));
                
        ClassVisitData data(this, MemberInfo::Access::Private, union_info);
        clang_visitChildren(cursor, visit_class_cursor, &data);
        
        return union_info;
    }
};


SourceAnalyzer::SourceAnalyzer()
{
    p = new SourceAnalyzerPrivate;
}


SourceAnalyzer::~SourceAnalyzer()
{
    delete p;
}


NamespaceInfo* SourceAnalyzer::analyzeSources(std::vector<const char*> file_paths)
{
    assert(file_paths.size() >= 1);
    
    CXIndex index = clang_createIndex(0, 0);
    
    const char* compiler_args[] = {};
    CXTranslationUnit translation_unit = clang_createTranslationUnitFromSourceFile(index, file_paths[0], 0, compiler_args, 0, nullptr);
    
    CXCursor cursor = clang_getTranslationUnitCursor(translation_unit);
    
    NamespaceVisitData data(p, new NamespaceInfo);
    
    clang_visitChildren(cursor, &visit_namespace_cursor, &data);
    
    return data.info;
}

 
CXChildVisitResult visit_namespace_cursor(CXCursor cursor, CXCursor parent, CXClientData data)
{   
#ifdef DEBUG
    assert(data  != nullptr);
#endif//DEBUG
    
    CXCursorKind cursor_kind = clang_getCursorKind(cursor);
    
    auto namespace_visit_data = (NamespaceVisitData*) data;
    auto p = namespace_visit_data->p;
 
#ifdef DEBUG
    assert(namespace_visit_data->info != nullptr);
    assert(namespace_visit_data->info->is<NamespaceInfo*>());
#endif//DEBUG   

    auto parent_info = namespace_visit_data->info->to<NamespaceInfo*>();
    
    switch(cursor_kind)
    {
        case CXCursor_Namespace:
        {
            auto namespace_info = new NamespaceInfo(clang_getCString( clang_getCursorSpelling(cursor) ));

            NamespaceVisitData visit_data(p, namespace_info);
            clang_visitChildren(cursor, visit_namespace_cursor, &visit_data);
            
            parent_info->addNamespaceInfo(namespace_info);
            break;
        }
        
        case CXCursor_VarDecl:
        {         
            parent_info->addVariableInfo(p->get_variable_info(cursor));
            break;
        }
        
        case CXCursor_FunctionDecl:
        {
            auto function_info= p->get_function_info(cursor);
                        
            parent_info->addFunctionInfo(function_info);
            break;
        }
        
        case CXCursor_ClassDecl:
        {
            auto class_info = p->get_class_info(cursor);
            parent_info->addClassInfo(class_info);
            p->known_records.add(cursor, class_info);
            break;
        }
        
        case CXCursor_StructDecl:
        {
            auto struct_info = p->get_struct_info(cursor);
            parent_info->addStructInfo(struct_info);
            p->known_records.add(cursor, struct_info);
            break;
        }
        
        case CXCursor_UnionDecl:
        {
            auto union_info = p->get_union_info(cursor);
            parent_info->addUnionInfo(union_info);
            p->known_records.add(cursor, union_info);
            break;
        }
        
        
        case CXCursor_EnumDecl:
        {
            auto enum_info = new EnumInfo;
            EnumVisitData data(p, enum_info);
            clang_visitChildren(cursor, visit_enum_cursor, &data);
            parent_info->addEnumInfo(enum_info);
            p->known_enumerations.add(cursor, enum_info);
            break;
        }
        
        
        case CXCursor_TypedefDecl:
        {
            string name = clang_getCString(clang_getCursorSpelling(cursor));
            CppInfo* type = p->cx2type(clang_getTypedefDeclUnderlyingType(cursor));
            if(type == nullptr)
            {
                cerr << "Warning: failed to get underlying type for typedef " << name << "\n";
            }
            auto tpd = new TypedefInfo(name, type);
            parent_info->addTypedef(tpd);
            p->known_typedefs.add(cursor, tpd);
            break;
        }
        
        default:
        {
            break;
        }
    }
    
    return CXChildVisit_Continue;
}


CXChildVisitResult visit_class_cursor(CXCursor cursor, CXCursor parent, CXClientData data)
{    
    auto visit_data = (ClassVisitData*) data;
    auto &p = visit_data->p;
    auto &access = visit_data->current_access;
    auto &parent_info = visit_data->info;    
    
    switch(cursor.kind)
    {
        case CXCursor_CXXMethod:
        case CXCursor_ConversionFunction:
        {
            auto function_info = p->get_function_info(cursor);
            parent_info->addMemberInfo(new MemberInfo(function_info, access));
            break;
        }
         
        //TODO: Process destructors.
        
        case CXCursor_ClassDecl:
        {
            parent_info->addMemberInfo(new MemberInfo(p->get_class_info(cursor), access));
            break;
        }
        
        case CXCursor_StructDecl:
        {
            parent_info->addMemberInfo(new MemberInfo(p->get_struct_info(cursor), access));
            break;
        }
        
        case CXCursor_UnionDecl:
        {
            parent_info->addMemberInfo(new MemberInfo(p->get_union_info(cursor), access));
            break;
        }
        
        case CXCursor_CXXAccessSpecifier:
        {
            auto new_access_specifier = clang_getCXXAccessSpecifier(cursor);
            if(new_access_specifier == CX_CXXPublic)
            {
                access = MemberInfo::Access::Public;
            }
            else if(new_access_specifier == CX_CXXProtected)
            {
                access = MemberInfo::Access::Protected;
            }
            else if(new_access_specifier == CX_CXXPrivate)
            {
                access = MemberInfo::Access::Private;
            }
            
            break;
        }
        
        default: break;
    }
    
    return CXChildVisit_Continue;
}


CXChildVisitResult visit_enum_cursor(CXCursor cursor, CXCursor parent, CXClientData data)
{
    auto visit_data = (EnumVisitData*) data;
    auto parent_info = visit_data->info;
    
    if(cursor.kind == CXCursor_EnumConstantDecl)
    {
        string name = clang_getCString(clang_getCursorSpelling(cursor));
        long long value = clang_getEnumConstantDeclValue(cursor);
        parent_info->addItem(name, value);
    }
    else
    {
        cerr << "No enum constant decl.\n";
        abort();
    }
    
    return CXChildVisit_Continue;
}


int find_pointee_type_and_pointer_depth(CXType &pointee_type, CXType pointer_type)
{
    if(pointer_type.kind == CXType_Pointer)
    {
        return find_pointee_type_and_pointer_depth(pointee_type, clang_getPointeeType(pointer_type)) + 1;
    }
    else
    {
        pointee_type = pointer_type;
        return 0;
    }
}
    
}//namespace CppAnalyzer
