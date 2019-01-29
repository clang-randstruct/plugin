#include <algorithm>
#include <random>

#include "bucket.h"

const size_t CACHE_LINE = 64;

std::vector<FieldDecl *> Bucket::randomize() {
    // TODO use seed
    auto rng = std::default_random_engine{};
    std::shuffle(std::begin(fields), std::end(fields), rng);
    return fields;
}

bool Bucket::canFit(size_t size) const {
    return this->size + size <= CACHE_LINE;
}

void Bucket::add(FieldDecl *field, size_t size) {
    fields.push_back(field);
    this->size += size;
}

bool Bucket::isBitfieldRun() const {
    return false;
}

bool Bucket::full() const {
    return size == CACHE_LINE;
}

std::vector<FieldDecl *> BitfieldRun::randomize() {
    return fields;
}

bool BitfieldRun::canFit(size_t size) const {
    return true;
}

bool BitfieldRun::isBitfieldRun() const {
    return true;
}
