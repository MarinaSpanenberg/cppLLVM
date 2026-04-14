#include <llvm/IR/LLVMContext.h>// functions llvm
#include <llvm/IR/Module.h>     // Conversor para IR
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>   // Verifica o codigo IR
#include <llvm/Support/raw_ostream.h>
#include <llvm/Bitcode/BitcodeWriter.h>

// int64_t diff_min_max(int64_t a, int64_t b);
using namespace llvm;

int main() {
    LLVMContext context;

    // alocar memoria para o programa
    Module *module = new Module("modulo", context);

    // define o tipe int64
    Type *i64 = Type::getInt64Ty(context);
    // Tipo da func e seus parametros
    FunctionType *func_type = FunctionType::get(i64, {i64, i64}, false);
    // Define o tipo da funcao e seu nome
    Function *function = Function::Create(func_type, 
                                          Function::ExternalLinkage,
                                          0, "diff_min_max", module);

    // definindo os argumentos da func
    auto args = function->arg_begin();
    // pega o primeiro e segundo argumento da funcao e define seus nomes
    auto arg_a = args++;
    arg_a->setName("a");
    auto arg_b = args++;
    arg_a->setName("b");

    // Cria o Basic BLock (corpo da funcao/instrucoes da funcao)
    BasicBlock *basic_block = BasicBlock::Create(context, "entry", function);
    IRBuilder<> Builder(basic_block); // define o builder como sendo basic_block

    // Logica da funcao

    // Cmp: comparacao, comparar se a eh maior que b
    Value *Cmp = Builder.CreateICmpSGT(arg_a, arg_b, "cmp");

    // Realiza as possiveis subtracoes
    Value *SubAB = Builder.CreateSub(arg_a, arg_b, "subab");
    Value *SubBA = Builder.CreateSub(arg_b, arg_a, "subba");

    // Cria resultado com base no Cmp
    // Basicamente: (a > b) ? (a - b) : (b - a)
    Value *Result = Builder.CreateSelect(Cmp, SubAB, SubBA, "resultado");
    Builder.CreateRet(Result); // Return

    // REaliza a verificacao da funcao geradora
    if (verifyFunction(*function, &errs())) {
        errs() << "Falha em verificar a funcao\n";
        return EXIT_FAILURE;
    }

    // Gerar representacao textual .ll
    std::error_code EC;
    raw_fd_ostream outLL("diff_min_max.ll", EC);
    module->print(outLL, nullptr);

    // gera o .bc
    raw_fd_ostream outBC("diff_min_max.bc", EC);
    WriteBitcodeToFile(*module, outBC);

    return EXIT_SUCCESS;
}
