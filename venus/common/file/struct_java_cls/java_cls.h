//
// Created by 周昊炜 on 2022/2/10.
//

#ifndef FILETYPER_JAVA_CLS_H
#define FILETYPER_JAVA_CLS_H

#include "../types.h"
#include <string>

const uint8_t CONSTANT_UTF8_TAG = 0x1;
const uint8_t CONSTANT_INTEGER_TAG = 0x3;
const uint8_t CONSTANT_FLOAT_TAG = 0x4;
const uint8_t CONSTANT_LONG_TAG = 0x5;
const uint8_t CONSTANT_DOUBLE_TAG = 0x6;
const uint8_t CONSTANT_CLASS_TAG = 0x7;
const uint8_t CONSTANT_STRING_TAG = 0x8;
const uint8_t CONSTANT_FIELD_REF_TAG = 0x9;
const uint8_t CONSTANT_METHOD_REF_TAG = 0xa;
const uint8_t CONSTANT_INTERFACE_METHOD_REF_TAG = 0xb;
const uint8_t CONSTANT_NAME_AND_TYPE_TAG = 0xc;
const uint8_t CONSTANT_METHOD_HANDLE_TAG = 0xf;
const uint8_t CONSTANT_METHOD_TYPE_TAG = 0x10;
const uint8_t CONSTANT_INVOKE_DYNAMIC_TAG = 0x12;

#pragma pack(1)
typedef struct constant_pool_info {
    uint8_t tag;
    uint8_t info[1];
} CONSTANT_POOL_INFO, *PCONSTANT_POOL_INFO;

typedef struct constant_utf8 {
    uint8_t tag;
    uint16_t length;
    uint8_t content[1];
} CONSTANT_UTF8, *PCONSTANT_UTF8;

typedef struct constant_integer {
    uint8_t tag;
    uint8_t content[4];
} CONSTANT_INTEGER, *PCONSTANT_INTEGER;

typedef struct constant_float {
    uint8_t tag;
    uint8_t content[4];
} CONSTANT_FLOAT, *PCONSTANT_FLOAT;

typedef struct constant_long {
    uint8_t tag;
    uint8_t high[4];
    uint8_t low[4];
} CONSTANT_LONG, *PCONSTANT_LONG;

typedef struct constant_double {
    uint8_t tag;
    uint8_t high[4];
    uint8_t low[4];
} CONSTANT_DOUBLE, *PCONSTANT_DOUBLE;

typedef struct constant_class {
    uint8_t tag;
    uint16_t name_index;
} CONSTANT_CLASS, *PCONSTANT_CLASS;

typedef struct constant_string {
    uint8_t tag;
    uint16_t string_index;
} CONSTANT_STRING, *PCONSTANT_STRING;

typedef struct constant_field_ref {
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type;
} CONSTANT_FIELD_REF, *PCONSTANT_FIELD_REF;

typedef struct constant_method_ref {
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type;
} CONSTANT_METHOD_REF, *PCONSTANT_METHOD_REF;

typedef struct constant_interface_method_ref {
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type;
} CONSTANT_INTERFACE_METHOD_REF, *PCONSTANT_INTERFACE_METHOD_REF;

typedef struct constant_name_and_type {
    uint8_t tag;
    uint16_t nme_index;
    uint16_t descriptor_index;
} CONSTANT_NAME_AND_TYPE, *PCONSTANT_NAME_AND_TYPE;

typedef struct constant_method_handle {
    uint8_t tag;
    uint8_t ref_kind;
    uint16_t ref_index;
} CONSTANT_METHOD_HANDLE, *PCONSTANT_METHOD_HANDLE;

typedef struct constant_method_type {
    uint8_t tag;
    uint16_t descriptor_index;
} CONSTANT_METHOD_TYPE, *PCONSTANT_METHOD_TYPE;

typedef struct constant_invoke_dynamic {
    uint8_t tag;
    uint16_t bootstrap_method_attr;
    uint16_t name_and_type;
} CONSTANT_INVOKE_DYNAMIC, *PCONSTANT_INVOKE_DYNAMIC;
#pragma pack()

const uint16_t CLASS_ACC_FLAG_PUBLIC = 0x0001; // Declared public; may be acc_FLAGessed from outside its package.
const uint16_t CLASS_ACC_FLAG_FINAL = 0x0010; // Declared final; no subclasses allowed.
const uint16_t CLASS_ACC_FLAG_SUPER = 0x0020; // Treat superclass methods specially when invoked by the invokespecial instruction.
const uint16_t CLASS_ACC_FLAG_INTERFACE = 0x0200; // Is an interface, not a class.
const uint16_t CLASS_ACC_FLAG_ABSTRACT = 0x0400; // Declared abstract; must not be instantiated.
const uint16_t CLASS_ACC_FLAG_SYNTHETIC = 0x1000; // Declared synthetic; not present in the source code.
const uint16_t CLASS_ACC_FLAG_ANNOTATION = 0x2000; // Declared as an annotation type.
const uint16_t CLASS_ACC_FLAG_ENUM = 0x4000; // Declared as an enum type.

const uint16_t CLASS_ACC_FLAGS[] = {
        CLASS_ACC_FLAG_PUBLIC,
        CLASS_ACC_FLAG_FINAL,
        CLASS_ACC_FLAG_SUPER,
        CLASS_ACC_FLAG_INTERFACE,
        CLASS_ACC_FLAG_ABSTRACT,
        CLASS_ACC_FLAG_SYNTHETIC,
        CLASS_ACC_FLAG_ANNOTATION,
        CLASS_ACC_FLAG_ENUM
};

const uint16_t FIELD_ACC_FLAG_PUBLIC = 0x0001; //Declared public; may be accessed from outside its package.
const uint16_t FIELD_ACC_FLAG_PRIVATE = 0x0002; //Declared private; usable only within the defining class.
const uint16_t FIELD_ACC_FLAG_PROTECTED = 0x0004; //Declared protected; may be accessed within subclasses.
const uint16_t FIELD_ACC_FLAG_STATIC = 0x0008; //Declared static.
const uint16_t FIELD_ACC_FLAG_FINAL = 0x0010; //Declared final; never directly assigned to after object construction (JLS §17.5).
const uint16_t FIELD_ACC_FLAG_VOLATILE = 0x0040; //Declared volatile; cannot be cached.
const uint16_t FIELD_ACC_FLAG_TRANSIENT = 0x0080; //Declared transient; not written or read by a persistent object manager.
const uint16_t FIELD_ACC_FLAG_SYNTHETIC = 0x1000; //Declared synthetic; not present in the source code.
const uint16_t FIELD_ACC_FLAG_ENUM = 0x4000; //Declared as an element of an enum.

const uint16_t FIELD_ACC_FLAGS[] = {
        FIELD_ACC_FLAG_PUBLIC,
        FIELD_ACC_FLAG_PRIVATE,
        FIELD_ACC_FLAG_PROTECTED,
        FIELD_ACC_FLAG_STATIC,
        FIELD_ACC_FLAG_FINAL,
        FIELD_ACC_FLAG_VOLATILE,
        FIELD_ACC_FLAG_TRANSIENT,
        FIELD_ACC_FLAG_SYNTHETIC,
        FIELD_ACC_FLAG_ENUM
};

const uint16_t PROPERTY_ACC_FLAG_PUBLIC = 0x0001; //Declared public; may be accessed from outside its package.
const uint16_t PROPERTY_ACC_FLAG_PRIVATE = 0x0002; //Declared private; accessible only within the defining class.
const uint16_t PROPERTY_ACC_FLAG_PROTECTED = 0x0004; //Declared protected; may be accessed within subclasses.
const uint16_t PROPERTY_ACC_FLAG_STATIC = 0x0008; //Declared static.
const uint16_t PROPERTY_ACC_FLAG_FINAL = 0x0010; //Declared final; must not be overridden (§5.4.5).
const uint16_t PROPERTY_ACC_FLAG_SYNCHRONIZED = 0x0020; //Declared synchronized; invocation is wrapped by a monitor use.
const uint16_t PROPERTY_ACC_FLAG_BRIDGE = 0x0040; //A bridge method, generated by the compiler.
const uint16_t PROPERTY_ACC_FLAG_VARARGS = 0x0080; //Declared with variable number of arguments.
const uint16_t PROPERTY_ACC_FLAG_NATIVE = 0x0100; //Declared native; implemented in a language other than Java.
const uint16_t PROPERTY_ACC_FLAG_ABSTRACT = 0x0400; //Declared abstract; no implementation is provided.
const uint16_t PROPERTY_ACC_FLAG_STRICT = 0x0800; //Declared strictfp; floating-point mode is FP-strict.
const uint16_t PROPERTY_ACC_FLAG_SYNTHETIC = 0x1000; //Declared synthetic; not present in the source code.

const uint16_t PROPERTY_ACC_FLAGS[] = {
        PROPERTY_ACC_FLAG_PUBLIC,
        PROPERTY_ACC_FLAG_PRIVATE,
        PROPERTY_ACC_FLAG_PROTECTED,
        PROPERTY_ACC_FLAG_STATIC,
        PROPERTY_ACC_FLAG_FINAL,
        PROPERTY_ACC_FLAG_SYNCHRONIZED,
        PROPERTY_ACC_FLAG_BRIDGE,
        PROPERTY_ACC_FLAG_VARARGS,
        PROPERTY_ACC_FLAG_NATIVE,
        PROPERTY_ACC_FLAG_ABSTRACT,
        PROPERTY_ACC_FLAG_STRICT,
        PROPERTY_ACC_FLAG_SYNTHETIC,
};

#pragma pack(1)
typedef struct attribute_info {
    uint16_t  name_index;
    uint32_t length;
    uint8_t info[1];
} ATTRIBUTE_INFO, *PATTRIBUTE_INFO;

typedef struct field_info {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attribute_count;
    ATTRIBUTE_INFO attribute_info[1];
} FILED_INFO, *PFIELD_INFO;

typedef struct method_info {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attribute_count;
    ATTRIBUTE_INFO attribute_info[1];
} METHOD_INFO, *PMETHOD_INFO;
#pragma pack()

const std::string ATTIBUTE_TYPE_CONSTANT_VALUE = "ConstantValue";
const std::string ATTIBUTE_TYPE_CODE = "Code";
const std::string ATTIBUTE_TYPE_STACK_MAP_TABLE = "StackMapTable";
const std::string ATTIBUTE_TYPE_EXCEPTIONS = "Exceptions";
const std::string ATTIBUTE_TYPE_INNER_CLASS = "InnerClasses";
const std::string ATTIBUTE_TYPE_ENCLOSING_METHOD = "EnclosingMethod";
const std::string ATTIBUTE_TYPE_SYNTHETIC = "Synthetic";
const std::string ATTIBUTE_TYPE_SIGNATURE = "Signature";
const std::string ATTIBUTE_TYPE_SOURCE_FILE = "SourceFile";
const std::string ATTIBUTE_TYPE_SOURCE_DEBUG_EXTENSION = "SourceDebugExtension";
const std::string ATTIBUTE_TYPE_LINE_NUMBER_TABLE = "LineNumberTable";
const std::string ATTIBUTE_TYPE_LOCAL_VARIABLE_TABLE = "LocalVariableTable";
const std::string ATTIBUTE_TYPE_LOCAL_VARIABLE_TYPE_TABLE = "LocalVariableTypeTable";
const std::string ATTIBUTE_TYPE_DEPRECATED = "Deprecated";
const std::string ATTIBUTE_TYPE_RUNTIME_VISIVLE_ANNOTATIONS = "RuntimeVisibleAnnotations";
const std::string ATTIBUTE_TYPE_RUNTIME_INVISIVLE_ANNOTATIONS = "RuntimeInvisibleAnnotations";
const std::string ATTIBUTE_TYPE_RUNTIME_VISIVLE_PARAMETER_ANNOTATIONS = "RuntimeVisibleParameterAnnotations";
const std::string ATTIBUTE_TYPE_RUNTIME_INVISIVLE_PARAMETER_ANNOTATIONS = "RuntimeInvisibleParameterAnnotations";
const std::string ATTIBUTE_TYPE_DEFAULT = "AnnotationDefault";
const std::string ATTIBUTE_TYPE_BOOTSTRAP_METHODS = "BootstrapMethods";

#endif //FILETYPER_JAVA_CLS_H
