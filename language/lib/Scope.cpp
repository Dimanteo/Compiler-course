#include "Scope.h"
#include "CompilerCore.h"

namespace kolang {

bool Scope::contains(strid_t id) const {
    return (locals.find(id) != locals.end());
}

void Scope::setOrUpdateVariable(strid_t id) { locals[id] = IRGenerator::NIL(); }

void Scope::setOrUpdateVariable(strid_t id, IRValue memory) {
    locals[id] = memory;
}

IRValue Scope::getVariableValue(strid_t id) { return locals[id]; }

void Scope::yieldInsertBlock() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    insertion_point = IRG.getInsertBlock();
}

void Scope::applyInsertBlock() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    IRG.setInsertBlock(insertion_point);
}

}; // namespace kolang