#ifndef YURI_KLOPOVSKY_CPP_INFO_BASIC_TYPE_INFO_H
#define YURI_KLOPOVSKY_CPP_INFO_BASIC_TYPE_INFO_H

#ifndef CPP_INFO_IMPLEMENTATION
extern BasicTypeInfo 
    cpp_type_void,
    cpp_type_bool,
    cpp_type_uchar,
    cpp_type_uchar16,
    cpp_type_uchar32,
    cpp_type_ushort,
    cpp_type_uint,
    cpp_type_ulong,
    cpp_type_ulong_long,
    cpp_type_uint128,
    cpp_type_char,
    cpp_type_wchar,
    cpp_type_short,
    cpp_type_int,
    cpp_type_long,
    cpp_type_long_long,
    cpp_type_int128,
    cpp_type_float,
    cpp_type_double,
    cpp_type_long_double,
    cpp_type_nullptr;

#else
BasicTypeInfo                          /* Signed ? */
    cpp_type_void         ("void",        true),
    cpp_type_bool         ("bool",        true),
    cpp_type_uchar        ("char",        false),
    cpp_type_uchar16      ("char",        false),
    cpp_type_uchar32      ("char",        false),
    cpp_type_ushort       ("short",       false),
    cpp_type_uint         ("int",         false),
    cpp_type_ulong        ("long",        false),
    cpp_type_ulong_long   ("long long",   false),
    cpp_type_uint128      ("uint128_t",   false),//This is probably wrong.
    cpp_type_char         ("char",        true),
    cpp_type_wchar        ("wchar_t",     true),
    cpp_type_short        ("short",       true),
    cpp_type_int          ("int",         true),
    cpp_type_long         ("long",        true),
    cpp_type_long_long    ("long long",   true),
    cpp_type_int128       ("int128_t",    true),//This is probably wrong.
    cpp_type_float        ("float",       true),
    cpp_type_double       ("double",      true),
    cpp_type_long_double  ("long double", true),
    cpp_type_nullptr      ("nullptr",     true);

#endif//CPP_INFO_IMPLEMENTATION
    
#endif//YURI_KLOPOVSKY_CPP_INFO_BASIC_TYPE_INFO_H