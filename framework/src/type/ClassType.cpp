// Copyright 2025 JesusTouchMe

#include "Bibble/symbol/Scope.h"

#include "Bibble/type/ClassType.h"
#include "Bibble/type/ViewType.h"

#include <algorithm>
#include <format>

ClassType::ClassType(std::string_view moduleName, std::string_view name, ClassType* baseType)
    : Type(std::format("{}::{}", moduleName, name))
    , mModuleName(moduleName)
    , mName(name)
    , mBaseType(baseType) {}

std::string_view ClassType::getModuleName() const {
    return mModuleName;
}

std::string_view ClassType::getName() const {
    return mName;
}

ClassType* ClassType::getBaseType() const {
    return mBaseType;
}

JesusASM::Type* ClassType::getJesusASMType() const {
    std::string moduleName = mModuleName;
    std::replace(moduleName.begin(), moduleName.end(), '.', '/');

    return JesusASM::Type::GetClassType(moduleName, mName);
}

codegen::Type ClassType::getRuntimeType() const {
    return codegen::Type::Reference;
}

Type::CastLevel ClassType::castTo(Type* destType) const {
    if (destType->isViewType()) {
        auto viewType = static_cast<ViewType*>(destType);
        destType = viewType->getBaseType();
    }

    if (destType->isClassType()) {
        const ClassType* current = this;

        while (current != nullptr) {
            if (current == destType) return CastLevel::Implicit;
            current = current->mBaseType;
        }
    }

    return CastLevel::Disallowed;
}

void ClassType::resolve(symbol::Scope* scope, diagnostic::Diagnostics& diag) {
    auto classSymbol = scope->findClass({ mModuleName, mName }); // TODO: does this work?
    if (classSymbol == nullptr) {
        diag.fatalError(std::format("cannot resolve class '{}{}::{}{}'",
                                    fmt::bold, mModuleName, mName, fmt::defaults));
    }
}

bool ClassType::isClassType() const {
    return true;
}

bool ClassType::isClassView() const {
    return true;
}

static std::vector<std::unique_ptr<ClassType>> classTypes;

ClassType* ClassType::Find(std::string_view moduleName, std::string_view name) {
    auto it = std::find_if(classTypes.begin(), classTypes.end(), [moduleName, name](const auto& type) {
        return type->mModuleName == moduleName && type->mName == name;
    });

    if (it != classTypes.end()) {
        return it->get();
    }

    return nullptr;
}

ClassType* ClassType::Create(std::string_view moduleName, std::string_view name, ClassType* baseType) {
    auto it = std::find_if(classTypes.begin(), classTypes.end(), [moduleName, name](const auto& type) {
        return type->mModuleName == moduleName && type->mName == name;
    });

    if (it != classTypes.end()) {
        return it->get();
    }

    classTypes.push_back(std::make_unique<ClassType>(moduleName, name, baseType));
    return classTypes.back().get();
}

