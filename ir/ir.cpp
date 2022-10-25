#include "ir.h"
#include <vector>


llvm::Function *buildIR(llvm::Module *module, llvm::IRBuilder<> &builder) {
    llvm::LLVMContext &context = builder.getContext();

    // Structs
    std::vector<llvm::Type*> mdb_set_fields_ty(7, builder.getInt64Ty());
    llvm::StructType *mdb_set_struct =  llvm::StructType::create(mdb_set_fields_ty);

    llvm::StructType *graph_window_struct = llvm::StructType::create(context);
    llvm::StructType *pixel_array_struct = llvm::StructType::create(context);

    // Funcs
    llvm::FunctionType *voidFuncType = 
        llvm::FunctionType::get(builder.getVoidTy(), false);
    llvm::Function *mainFunc = 
        llvm::Function::Create(voidFuncType, llvm::Function::ExternalLinkage, "main", module);

    llvm::FunctionType *func_init_pixel_array_ty = llvm::FunctionType::get(pixel_array_struct->getPointerTo(), false);
    llvm::Function *init_pixel_array = 
        llvm::Function::Create(func_init_pixel_array_ty, llvm::Function::ExternalLinkage, "initPixelArray", module);

    std::vector<llvm::Type*> init_window_args_ty(2, builder.getInt64Ty());
    llvm::FunctionType *func_init_window_ty = 
        llvm::FunctionType::get(graph_window_struct->getPointerTo(), init_window_args_ty, false);
    llvm::Function *init_window = 
        llvm::Function::Create(func_init_window_ty, llvm::Function::ExternalLinkage, "initWindow", module);

    std::vector<llvm::Type *> ptr_arg(1, llvm::Type::getInt64PtrTy(context));
    llvm::FunctionType *is_open_func_type = llvm::FunctionType::get(builder.getInt32Ty(), ptr_arg, false);
    llvm::Function *is_open_func = llvm::Function::Create(is_open_func_type, llvm::Function::ExternalLinkage, "isOpen", module);

    llvm::FunctionType *handle_event_func_type = 
        llvm::FunctionType::get(builder.getVoidTy(), ptr_arg, false);
    llvm::Function *handle_event_func = 
        llvm::Function::Create(handle_event_func_type, llvm::Function::ExternalLinkage, "handleWindowEvents", module);

    llvm::FunctionType *zoom_func_type = 
        llvm::FunctionType::get(builder.getVoidTy(), std::vector<llvm::Type*>(1, mdb_set_struct->getPointerTo()), false);
    llvm::Function *zoom_func = 
        llvm::Function::Create(zoom_func_type, llvm::Function::ExternalLinkage, "zoom", module);
    
    llvm::FunctionType *draw_mdb_set_func_type = 
        llvm::FunctionType::get(builder.getVoidTy(), 
            std::vector<llvm::Type*>({pixel_array_struct->getPointerTo(), builder.getInt64Ty(), builder.getInt64Ty(), mdb_set_struct->getPointerTo()}),
            false
        );
    llvm::Function *draw_mdb_set_func = 
        llvm::Function::Create(draw_mdb_set_func_type, llvm::Function::ExternalLinkage, "drawMandelbrotSet", module);
    
    llvm::FunctionType *draw_func_type = 
        llvm::FunctionType::get(builder.getVoidTy(), 
            std::vector<llvm::Type*>({graph_window_struct->getPointerTo(), pixel_array_struct->getPointerTo()}), false);
    llvm::Function *draw_func = llvm::Function::Create(draw_func_type, llvm::Function::ExternalLinkage, "draw", module);

    llvm::FunctionType *display_func_type = 
        llvm::FunctionType::get(builder.getVoidTy(), std::vector<llvm::Type*>({graph_window_struct->getPointerTo()}), false);
    llvm::Function *display_func = llvm::Function::Create(display_func_type, llvm::Function::ExternalLinkage, "display", module);

    llvm::FunctionType *destr_pixel_array_func_ty = 
        llvm::FunctionType::get(builder.getVoidTy(), std::vector<llvm::Type*>({pixel_array_struct->getPointerTo()}), false);
    llvm::Function *destr_pixel_array_func = llvm::Function::Create(destr_pixel_array_func_ty, llvm::Function::ExternalLinkage, "destrPixelArray", module);

    llvm::FunctionType *destr_window_func_ty = 
        llvm::FunctionType::get(builder.getVoidTy(), std::vector<llvm::Type*>({graph_window_struct->getPointerTo()}), false);
    llvm::Function *destr_window_func = llvm::Function::Create(destr_window_func_ty, llvm::Function::ExternalLinkage, "destrWindow", module);

    // BBs
    #define CREATE_BB_NUM(num) \
        llvm::BasicBlock *bb_##num = llvm::BasicBlock::Create(context, "BB"#num, mainFunc)
    CREATE_BB_NUM(0);
    CREATE_BB_NUM(5);
    CREATE_BB_NUM(10);
    CREATE_BB_NUM(12);
    CREATE_BB_NUM(14);
    CREATE_BB_NUM(19);
    #undef CREATE_BB_NUM

    builder.SetInsertPoint(bb_0);
    llvm::Value *mdb_set = builder.CreateAlloca(mdb_set_struct);
    auto setField = [& mdb_set_struct, & mdb_set, & builder](unsigned int Idx, double value) {
        llvm::Value *field = builder.CreateStructGEP(mdb_set_struct, mdb_set, Idx);
        builder.CreateStore(builder.getInt64(static_cast<int64_t>(value * PRECISION)), field);    
    };
    // Init mdb_set structure
    setField(0, 1.2);           // x_max
    setField(1, -2.2);          // x_min
    setField(2, 1.7);           // y_max
    setField(3, -1.7);          // y_min
    setField(4, 1);             // scale
    setField(5, -0.7436438870); // x_center
    setField(6, 0.13182590);    // y_center
    llvm::CallInst *pixel_array = builder.CreateCall(init_pixel_array);
    llvm::CallInst *window = builder.CreateCall(init_window, {builder.getInt64(600), builder.getInt64(600)});
    builder.CreateBr(bb_5);

    builder.SetInsertPoint(bb_5);
    llvm::PHINode *phi_step_number = builder.CreatePHI(builder.getInt64Ty(), 2);
    phi_step_number->addIncoming(builder.getInt64(35), bb_0);
    // second value filled later with val 15
    llvm::PHINode *phi_iterations = builder.CreatePHI(builder.getInt64Ty(), 2);
    phi_iterations->addIncoming(builder.getInt64(0), bb_0);
    // second value filled later with val 17
    llvm::CallInst *is_open = builder.CreateCall(is_open_func, {window});
    llvm::Value *cmp_res_9 = builder.CreateICmpEQ(is_open, builder.getInt32(0));
    builder.CreateCondBr(cmp_res_9, bb_19, bb_10);

    builder.SetInsertPoint(bb_10);
    builder.CreateCall(handle_event_func, {window});
    llvm::Value *cmp_res_11 = builder.CreateICmpEQ(phi_iterations, builder.getInt64(0));
    builder.CreateCondBr(cmp_res_11, bb_12, bb_14);

    builder.SetInsertPoint(bb_12);
    builder.CreateCall(zoom_func, {mdb_set});
    builder.CreateCall(draw_mdb_set_func, {pixel_array, builder.getInt64(600), builder.getInt64(600), mdb_set});
    llvm::Value *val_13 = builder.CreateAdd(phi_step_number, builder.getInt64(-1));
    builder.CreateBr(bb_14);

    builder.SetInsertPoint(bb_14);
    llvm::PHINode * phi_15 = builder.CreatePHI(builder.getInt64Ty(), 2);
    phi_15->addIncoming(val_13, bb_12);
    phi_15->addIncoming(phi_step_number, bb_10);
    builder.CreateCall(draw_func, {window, pixel_array});
    builder.CreateCall(display_func, {window});
    llvm::Value *val_16 = builder.CreateAnd(phi_iterations, builder.getInt64(1));
    llvm::Value *val_17 = builder.CreateXor(val_16, builder.getInt64(1));
    llvm::Value *val_18 = builder.CreateICmpEQ(phi_15, builder.getInt64(0));
    builder.CreateCondBr(val_18, bb_19, bb_5);
    // Finishing previously defined PHIs
    phi_step_number->addIncoming(phi_15, bb_14);
    phi_iterations->addIncoming(val_17, bb_14);
    

    builder.SetInsertPoint(bb_19);
    builder.CreateCall(destr_pixel_array_func, {pixel_array});
    builder.CreateCall(destr_window_func, {window});
    builder.CreateRetVoid();

    return mainFunc;
}