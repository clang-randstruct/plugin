#include <algorithm>
#include <random>

#include "bucket.h"

// TODO: Is there a way to detect this? (i.e. on 32bit system vs 64?)
const size_t CACHE_LINE = 64;

std::vector<FieldDecl *> Bucket::randomize() {
    // TODO use seed
    auto rng = std::default_random_engine{};
    std::shuffle(std::begin(fields), std::end(fields), rng);
    return fields;
}

bool Bucket::canFit(size_t size) const {
    // A bucket can only fill up to a cache line.
    return this->size + size <= CACHE_LINE;
}

void Bucket::add(FieldDecl *field, size_t size) {
    fields.push_back(field);
    this->size += size;
}

bool Bucket::isBitfieldRun() const {
    // The normal bucket is not a bitfieldrun. This is to avoid RTTI.
    return false;
}

bool Bucket::full() const {
    // We're full if our size is a cache line.
    return size >= CACHE_LINE;
}

bool Bucket::empty() const {
    return size == 0;
}

std::vector<FieldDecl *> BitfieldRun::randomize() {
    // Keep bit fields adjacent, we will not scramble them.
    return fields;
}

bool BitfieldRun::canFit(size_t size) const {
    // We can always fit another adjacent bitfield.
    return true;
}

bool BitfieldRun::isBitfieldRun() const {
    // Yes.
    return true;
}
