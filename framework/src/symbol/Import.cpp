// Copyright 2025 JesusTouchMe

#include "Bibble/lexer/Lexer.h"

#include "Bibble/parser/ImportParser.h"

#include "Bibble/symbol/Import.h"

#include <algorithm>
#include <fstream>

namespace symbol {
    ImportManager::ImportManager() {
        //addModulePath(fs::current_path());
    }

    void ImportManager::addModulePath(fs::path path) {
        mModulePaths.push_back(std::move(path));
    }

    bool ImportManager::importModule(fs::path path, diagnostic::Diagnostics& diag, Scope* scope) {
        path += ".bibble";

        std::ifstream stream;

        for (const auto& modulePath : mModulePaths) {
            stream.open(modulePath / path);
            if (stream.is_open()) break;
        }

        if (!stream.is_open()) {
            return false;
        }

        std::stringstream buffer;
        buffer << stream.rdbuf();

        std::string text = buffer.str();

        diagnostic::Diagnostics importDiag;
        importDiag.setText(buffer.str());
        importDiag.setImported(true);

        std::string fileName = path.string();
        lexer::Lexer lexer(text, fileName);

        std::vector<lexer::Token> tokens = lexer.lex();

        parser::ImportParser parser(tokens, importDiag, *this, scope);

        std::ignore = parser.parse();
        return true;
    }

    void ImportManager::seizeScope(ScopePtr scope) {
        mScopes.push_back(std::move(scope));
    }
}