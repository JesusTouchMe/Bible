// Copyright 2025 JesusTouchMe

#ifndef BIBLE_FRAMEWORK_INCLUDE_BIBLE_CODEGEN_CONTEXT_H
#define BIBLE_FRAMEWORK_INCLUDE_BIBLE_CODEGEN_CONTEXT_H

#include "Bible/codegen/Type.h"

#include <JesusASM/tree/ModuleNode.h>

#include <optional>

namespace codegen {
    class Builder;

    using namespace JesusASM::tree;

    struct ValueOrigin {
        ValueOrigin(AbstractInsnNode* origin, i64 value) : origin(origin), value(value) {}

        AbstractInsnNode* origin;
        i64 value;
    };

    struct Value {
        explicit Value(Type type, std::optional<ValueOrigin> value = std::nullopt) : type(type), value(value) {}

        Type type;
        std::optional<ValueOrigin> value; // only present if the value is 100% known
    };

    class Context {
    friend class Builder;
    public:
        explicit Context(std::unique_ptr<ModuleNode> module);

        ModuleNode* getModule();

        void push(Value value);

        template <typename... Args>
        constexpr void emplace(Args&&... args) {
            static_assert(std::is_constructible_v<Value, Args...>);
            push(Value(std::forward<Args>(args)...));
        }

        Value pop();

    private:
        std::unique_ptr<ModuleNode> mModule;
        std::vector<Value> mVirtualStack;
    };
}

#endif //BIBLE_FRAMEWORK_INCLUDE_BIBLE_CODEGEN_CONTEXT_H
