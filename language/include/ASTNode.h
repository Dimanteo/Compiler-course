#ifndef KOLANG_AST_NODE_H
#define KOLANG_AST_NODE_H

#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/Value.h>

#include <variant>

namespace kolang {

using strid_t = uint64_t;
using value_t = std::variant<llvm::Value *, strid_t>;
using ValueVector = llvm::SmallVector<value_t, 1>;

// Value that can be produced during parsing
class ASTNode {
  private:
    ValueVector values;

  public:
    static ASTNode getNIL() { return ASTNode(); }
    bool isNIL() { return values.size() == 0; }

    static bool isID(value_t val) {
        return std::holds_alternative<strid_t>(val);
    }
    static bool isPtr(value_t val) {
        return std::holds_alternative<llvm::Value *>(val);
    }
    static strid_t asID(value_t val);
    static llvm::Value *asPtr(value_t val);

    ASTNode() : values(0){};
    ASTNode(value_t v) : values(1, v){};
    ASTNode(llvm::Value *ptrVal) : values(1, ptrVal){};
    ASTNode(strid_t idVal) : values(1, idVal){};
    ASTNode(const ASTNode &other) : values(other.values) {}
    ASTNode &operator=(const ASTNode &other);

    value_t &get(size_t index = 0);
    value_t &operator[](size_t index) { return get(index); }
    size_t getNumValues() const;
    void addValue(const value_t &val);
    void addValue(const ASTNode &other);
    ValueVector &getValues();
    const ValueVector &getValues() const;
};

}; // namespace kolang
#endif