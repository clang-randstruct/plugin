#pragma once

#include <cstdint>
#include <vector>

#include "clang/AST/AST.h"

using namespace clang;

class Bucket {
    public:
        virtual std::vector<FieldDecl *> randomize();
        virtual bool canFit(size_t size) const;
        void add(FieldDecl *field, size_t size);
        virtual bool isBitfieldRun() const;
        bool full() const;

    protected:
        size_t size;
        std::vector<FieldDecl *> fields;
};

class BitfieldRun : public Bucket {
    public:
        virtual std::vector<FieldDecl *> randomize() override;
        virtual bool canFit(size_t size) const override;
        virtual bool isBitfieldRun() const override;
};
