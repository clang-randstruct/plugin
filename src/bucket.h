#pragma once

#include <cstdint>
#include <vector>

#include "clang/AST/AST.h"

using namespace clang;

/// Bucket to store fields up to size of a cache line during randomization.
class Bucket {
    public:
        /// Returns a randomized version of the bucket.
        virtual std::vector<FieldDecl *> randomize();
        /// Checks if an added element would fit in a cache line.
        virtual bool canFit(size_t size) const;
        /// Adds a field to the bucket.
        void add(FieldDecl *field, size_t size);
        /// Is this bucket for bitfields?
        virtual bool isBitfieldRun() const;
        /// Is this bucket full?
        bool full() const;
        bool empty() const;

    protected:
        size_t size;
        std::vector<FieldDecl *> fields;
};

/// BitfieldRun is a bucket for storing adjacent bitfields that may
/// exceed the size of a cache line.
class BitfieldRun : public Bucket {
    public:
        virtual std::vector<FieldDecl *> randomize() override;
        virtual bool canFit(size_t size) const override;
        virtual bool isBitfieldRun() const override;
};
