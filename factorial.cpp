#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRBuilder.h> 
#include <llvm/Support/raw_ostream.h>
#include <llvm/Bitcode/BitcodeWriter.h>

using namespace llvm;

int main() {

    LLVMContext context;

    // criacao do modulo
    Module module ("factorial_module", context);
    IRBuilder<> ir_builder(context);

    // define o tipo da funcao (int64_t)
    auto i64 = Type::getInt64Ty(context);
    auto func_type = FunctionType::get(i64, {i64}, false);
    
    // cria a funcao
    Function *function = Function::Create(func_type, Function::ExternalLinkage, "factorial", &module);

    // nomeia o parametro
    Value* n = function->arg_begin();
    n->setName("n");

    // criacao dos blocos
    BasicBlock* entry = BasicBlock::Create(context, "entry", function);
    BasicBlock* loop = BasicBlock::Create(context, "loop", function);
    BasicBlock* end = BasicBlock::Create(context, "end", function);

    ir_builder.SetInsertPoint(entry);
    ir_builder.CreateBr(loop);

    ir_builder.SetInsertPoint(loop);

    // acumulador res iniciando em 1
    const auto res = ir_builder.CreatePHI(i64, 2);
    res->addIncoming(ConstantInt::get(i64, 1), entry);

    // contador i iniciando em n
    const auto i = ir_builder.CreatePHI(i64, 2, "i");
    i->addIncoming(n, entry);

    // if i > 1
    Value* cmp = ir_builder.CreateICmpSGT(i, ConstantInt::get(i64, 1), "cmptmp");

    // res = res * i 
    Value* next_res = ir_builder.CreateMul(res, i, "next_res");

    //i = i - 1
    Value* next_i = ir_builder.CreateSub(i, ConstantInt::get(i64, 1), "next_i");
    
    res->addIncoming(next_res, loop);
    i->addIncoming(next_i, loop);

    ir_builder.CreateCondBr(cmp, loop, end);

    ir_builder.SetInsertPoint(end);
    ir_builder.CreateRet(res);

    // verifica a funcao
    if (verifyFunction(*function, &errs())) {
        errs() << "Failed to verify func";
        return EXIT_FAILURE;
    }
    printf("Successfully created factorial.ll and factorial.bc files!\n");
    
    // gera o .ll
    std::error_code EC;
    raw_fd_ostream outLL("factorial.ll", EC);
    module.print(outLL, nullptr);

    // gera o .bc
    raw_fd_ostream outBC("factorial.bc", EC);
    WriteBitcodeToFile(module, outBC);
    
    return EXIT_SUCCESS;
}