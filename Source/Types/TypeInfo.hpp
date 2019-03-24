#pragma once

#include "Precompiled.hpp"

// Forward declarations.
class RuntimeTypes;

// Type info class.
class TypeInfo
{
public:
    // Friend declarations.
    friend RuntimeTypes;

    // Identifier type.
    using IdentifierType = int;
    using TypeList = std::vector<TypeInfo*>;
    using AllocateFunction = void*(*)(void);

public:
    // Type info constructor.
    TypeInfo(const char* typeName, AllocateFunction allocateFunction, TypeInfo* baseType = nullptr);

    // Gets unique type identifier.
    int getIdentifier() const;
    
    // Gets readable type name.
    const char* getName() const;

    // Gets allocation function for this type.
    AllocateFunction getAllocateFunction() const;

    // Gets reference to base type.
    const TypeInfo* getBase() const;

    // Gets list of derived types.
    const TypeList& getDerived() const;

    // Checks if specified type identifier is same as this type.
    bool isSame(IdentifierType typeIdentifier) const;

    // Checks if specified type identifier is a base type of this type.
    bool isBase(IdentifierType typeIdentifier) const;

    // Checks if specified type identifier is a derived type from this type.
    bool isDerived(IdentifierType typeIdentifier) const;

    // Gets type info of another identifier.
    TypeInfo* getTypeInfo(IdentifierType typeIdentifier);

protected:
    // Type identifier.
    IdentifierType m_typeIdentifier;

    // Readable type name.
    const char* m_typeName;

    // Pointer to static allocation function.
    AllocateFunction m_allocateFunction;

    // Reference to base type.
    TypeInfo* m_baseType;

    // List of derived types.
    TypeList m_derivedTypes;
};

// Utility macros.
#define TYPE_DECLARE_EXPAND(x) x
#define TYPE_DECLARE_STRINGIFY(type) #type
#define TYPE_DECLARE_DEDUCE(arg1, arg2, arg3, ...) arg3

#define TYPE_DECLARE_DETERMINE(type) \
    virtual TypeInfo& getType() const \
    { \
        return type::Type(); \
    }

#define TYPE_DECLARE_ALLOCATE(type) \
    static void* allocate() \
    { \
        return new type(); \
    }

#define TYPE_DECLARE_CREATE(type) \
    static type* create(TypeInfo::IdentifierType typeIdentifier) \
    { \
        if(Type().isSame(typeIdentifier)) \
        { \
            return (type*)allocate(); \
        } \
        else if(Type().isDerived(typeIdentifier)) \
        { \
            TypeInfo* typeInfo = type::Type().getTypeInfo(typeIdentifier); \
            if(typeInfo != nullptr) \
            { \
                TypeInfo::AllocateFunction allocate = typeInfo->getAllocateFunction(); \
                return (type*)allocate(); \
            } \
        } \
        return nullptr; \
    }

#define TYPE_DECLARE_BASE(type) \
    public: \
        static TypeInfo& Type() \
        { \
            static TypeInfo typeInfo(TYPE_DECLARE_STRINGIFY(type), &type::allocate); \
            return typeInfo; \
        } \
        TYPE_DECLARE_DETERMINE(type) \
        TYPE_DECLARE_ALLOCATE(type) \
        TYPE_DECLARE_CREATE(type)

#define TYPE_DECLARE_DERIVED(type, base) \
    public: \
        static TypeInfo& Type() \
        { \
            static TypeInfo typeInfo(TYPE_DECLARE_STRINGIFY(type), &type::allocate, &base::Type()); \
            return typeInfo; \
        }\
        TYPE_DECLARE_DETERMINE(type) \
        TYPE_DECLARE_ALLOCATE(type) \
        TYPE_DECLARE_CREATE(type)

#define TYPE_DECLARE_CHOOSER(...) TYPE_DECLARE_EXPAND(TYPE_DECLARE_DEDUCE(__VA_ARGS__, TYPE_DECLARE_DERIVED, TYPE_DECLARE_BASE))
#define TYPE_DECLARE(...) TYPE_DECLARE_EXPAND(TYPE_DECLARE_CHOOSER(__VA_ARGS__)(__VA_ARGS__))