#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRBuilder.h> 
#include <llvm/Support/raw_ostream.h>
#include <llvm/Bitcode/BitcodeWriter.h>

using namespace llvm;

int main() {

    // ambiente global dos objetos llvm
    LLVMContext context;

    // criacao do modulo
    Module module ("factorial_module", context);
    IRBuilder<> ir_builder(context); // define o builder como sendo o context

    // define o tipo da funcao (int64_t) e seus parametros, retorno e quant de argumentos
    Type *i64 = Type::getInt64Ty(context);
    FunctionType *func_type = FunctionType::get(i64, {i64}, false);
    
    // cria a funcao, deixando ela visivel para outros módulos e a nomeando
    Function *function = Function::Create(func_type, Function::ExternalLinkage, "factorial", &module);

    // nomeia o parametro
    Value* n = function->arg_begin();
    n->setName("n");

    // criacao dos blocos (instrucoes da funcao)
    BasicBlock* entry = BasicBlock::Create(context, "entry", function);
    BasicBlock* loop = BasicBlock::Create(context, "loop", function);
    BasicBlock* end = BasicBlock::Create(context, "end", function);

    // logica da funcao

    // inicia builder no bloco de entrada
    ir_builder.SetInsertPoint(entry);

    // verifica se o valor é positivo, ou seja n >= 0
    Value* is_pos = ir_builder.CreateICmpSGE(n, ConstantInt::get(i64, 0), "is_pos");

    // redireciona a execucao para bloco do laco de repeticao se o nmr for positivo
    // senao vai direto pro end
    ir_builder.CreateCondBr(is_pos, loop, end);

    //inicio do bloco do laco
    ir_builder.SetInsertPoint(loop);

    // phinode para acumulador res iniciando em 1 e ocupando 2 nmrs reservados
    const auto res = ir_builder.CreatePHI(i64, 2, "res");
    res->addIncoming(ConstantInt::get(i64, 1), entry);

    // phinode para contador i iniciando em n
    const auto i = ir_builder.CreatePHI(i64, 2, "i");
    i->addIncoming(n, entry);

    // if i > 1
    Value* cmp = ir_builder.CreateICmpSGT(i, ConstantInt::get(i64, 1), "cmptmp");

    // res = res * i 
    Value* next_res = ir_builder.CreateMul(res, i, "next_res");

    //i = i - 1
    Value* next_i = ir_builder.CreateSub(i, ConstantInt::get(i64, 1), "next_i");
    
    // atualizacao dos phinodes
    res->addIncoming(next_res, loop);
    i->addIncoming(next_i, loop);

    // if i > 1, continua no loop, senao vai pro end
    ir_builder.CreateCondBr(cmp, loop, end);

    // bloco final
    ir_builder.SetInsertPoint(end);

    // phinode para valor final do retorno
    const auto final_res = ir_builder.CreatePHI(i64, 2);

    // se vem do loop, retorna o fatorial
    final_res->addIncoming(res, loop);

    // se vem do entry, ou seja, n < 0, retorna 0
    final_res->addIncoming(ConstantInt::get(i64, 0), entry);

    // retorno do valor final
    ir_builder.CreateRet(final_res);

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