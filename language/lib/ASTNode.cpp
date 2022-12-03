#include "ASTNode.h"

#include "assert.h"

namespace kolang {

strid_t ASTNode::asID(value_t val) {
    assert(isID(val));
    return std::get<strid_t>(val);
}

llvm::Value *ASTNode::asPtr(value_t val) {
    assert(isPtr(val));
    return std::get<llvm::Value *>(val);
}

ASTNode &ASTNode::operator=(const ASTNode &other) {
    values = other.values;
    return *this;
}

value_t &ASTNode::get(size_t index/* = 0*/) {
    return values[index];
}

size_t ASTNode::getNumValues() const {
    return values.size();
}

void ASTNode::addValue(const value_t &val) {
    values.push_back(val);
}

void ASTNode::addValue(const ASTNode &other) {
    for (const value_t &v : other.getValues()) {
        addValue(v);
    }
}

ValueVector &ASTNode::getValues() {
    return values;
}

const ValueVector &ASTNode::getValues() const {
    return values;
}

} // namespace kolang