/**
 * Copyright 2015- Co. Ltd. sizebook
 * @file test2.cpp
 * @brief
 * @author sawai@sizebook.co.jp (Shogo Sawai)
 * @date 2016-12-01 11:36:08
 */
#include <iostream>
#include <sstream>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>

using namespace clang;

namespace {

class MyAstVisitor : public RecursiveASTVisitor<MyAstVisitor> {
 public:
    explicit MyAstVisitor(ASTContext *Context, const std::string &source_file_)
        : Context(Context), source_file(source_file_) {}

    template <typename T>
    bool isNotInSource(T *t) {
        if (!t) {
            return true;
        }
        if (Context->getSourceManager().getFilename(t->getLocStart()).str() != source_file) {
            return true;
        }
        return false;
    }

    bool VisitValueDecl(ValueDecl *vd) {
        if (isNotInSource(vd)) {
            return true;
        }
        auto &mng = Context->getSourceManager();
        std::string filename = mng.getFilename(vd->getLocStart()).str();
        int line_no = mng.getSpellingLineNumber(vd->getLocStart());

        QualType qt = vd->getType();
        parseType(qt.getTypePtr(), filename, line_no);
        return true;
    }

    bool VisitReturnStmt(ReturnStmt *S) {
        if (isNotInSource(S)) {
            return true;
        }
        auto &mng = Context->getSourceManager();
        std::string filename = mng.getFilename(S->getLocStart()).str();
        int line_no = mng.getSpellingLineNumber(S->getLocStart());
        auto *v = S->getRetValue();
        QualType qt = v->getType();
        parseType(qt.getTypePtr(), filename, line_no);
        return true;
    }

    void parseType(const Type *ty, const std::string &filename, int line_no) {
        if (clang::isa<clang::ElaboratedType>(ty)) {
            const clang::ElaboratedType *et = clang::dyn_cast<clang::ElaboratedType>(ty);
            return parseType(et->getNamedType().getTypePtr(), filename, line_no);
        }
        if (clang::isa<clang::TypedefType>(ty)) {
            const clang::TypedefType *et = clang::dyn_cast<clang::TypedefType>(ty);
            NamedDecl *nd = et->getDecl();
            std::string path = getAbsPath(nd);
            if (invalidUsage(path)) {
                std::cout << filename << ":" << line_no << " " << path << std::endl;
            }
            return;
        }
        if (clang::isa<clang::AutoType>(ty)) {
            const AutoType *at = clang::dyn_cast<AutoType>(ty);
            parseType(at->getDeducedType().getTypePtr(), filename, line_no);
        }
        if (clang::isa<clang::RecordType>(ty)) {
            const clang::RecordType *et = clang::dyn_cast<clang::RecordType>(ty);
            NamedDecl *nd = et->getDecl();
            std::string path = getAbsPath(nd);
            if (invalidUsage(path)) {
                std::cout << filename << ":" << line_no << " " << path << std::endl;
            }
            return;
        }
    }

    std::vector<std::string> getDeclContext(const clang::Decl *decl) {
        std::vector<std::string> ns;
        const clang::DeclContext *dc = decl->getDeclContext();
        while (dc && (!dc->isTranslationUnit())) {
            const auto *nd = llvm::dyn_cast<const clang::NamedDecl>(dc);
            if (!nd) {
                return std::vector<std::string>();
            }
            ns.push_back(nd->getNameAsString());
            dc = dc->getParent();
        }
        return std::vector<std::string>(ns.rbegin(), ns.rend());
    }

    std::string getAbsPath(const clang::NamedDecl *decl) {
        std::stringstream ss;
        auto ns = getDeclContext(decl);
        for (auto &&s : ns) {
            ss << s << "::";
        }
        ss << decl->getNameAsString();
        return ss.str();
    }

    bool invalidUsage(const std::string &path) {
        std::vector<std::string> allowed_list = {
            "Eigen::Matrix",
            "Eigen::Vector",
            "Eigen::Array",
        };
        for (auto &&allowed : allowed_list) {
            if (path.substr(0, allowed.size()) == allowed) {
                return false;
            }
        }
        return true;
    }

 private:
    ASTContext *Context;
    std::string source_file;
};

class PrintDeclsConsumer : public ASTConsumer {
 public:
    CompilerInstance &Instance;
    std::set<std::string> ParsedTemplates;
    std::string source_file;

 public:
    PrintDeclsConsumer(CompilerInstance &Instance, std::set<std::string> ParsedTemplates, const std::string &s)
        : Instance(Instance), ParsedTemplates(ParsedTemplates), source_file(s) {}

    bool HandleTopLevelDecl(DeclGroupRef DG) override {
        return true;
    }

    void HandleTranslationUnit(ASTContext &context) override {
        MyAstVisitor Visitor(&context, source_file);
        Visitor.TraverseDecl(context.getTranslationUnitDecl());
    }
};

class PrintDeclsNamesAction : public PluginASTAction {
    std::set<std::string> ParsedTemplates;

 protected:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, llvm::StringRef InFile) override {
        return llvm::make_unique<PrintDeclsConsumer>(CI, ParsedTemplates, InFile.str());
    }

    bool ParseArgs(const CompilerInstance &CI, const std::vector<std::string> &args) override {
        return true;
    }
    void PrintHelp(llvm::raw_ostream &ros) { ros << "print invalid usage locations\n"; }
};
}  // namespace

static FrontendPluginRegistry::Add<PrintDeclsNamesAction> X("eigen_cxx_auto_checker", "check invalid `auto` usage");
