#include "bucket.h"

const size_t CACHE_LINE = 64;

std::vector<FieldDecl *> Bucket::randomize() {
    // TODO
    return fields;
}

bool Bucket::canFit(size_t size) const {
    return this->size + size <= CACHE_LINE;
}

void Bucket::add(FieldDecl *field) {
    fields.push_back(field);
}

std::vector<FieldDecl *> BitfieldRun::randomize() {
    return fields;
}

bool BitfieldRun::canFit(size_t size) const {
    return true;
}
