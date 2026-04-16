# Relatório - Trabalho Geração de Código com LLVM

**Integrantes:**
- João Vítor Bortoluz da Rosa - 216486
- Marina Barbosa Spanenberg - 196379

## 1. Funções escolhidas e motivos
Foram escolhidas duas funções, `diff_min_max` escolhida por João e `factorial` por Marina.
### 1.1 Função diff_min_max (João)
A função `diff_min_max` se trata de uma comparação entre dois números, ao definir o maior deles, é tirada a diferença do maior pelo menor, de forma que nunca retorne um número negativo.
- **Motivo da escolha:** O motivo da escolha dessa função foi que ela permite implementar funções de comparação de inteiros (ICmp) e diferença (diff) da lib LLVM, além de ser de baixa complexidade, não necessitando um uso mais extenso da lib LLVM.

### 1.2 Função factorial (Marina)
A função `factorial` trata-se de obter o produto de todos os números inteiros positivos de 1 até o número _n_ informado.
- **Motivo da escolha:** A escolha desta foi função se deu por dois fatores: O primeiro é sua maior complexidade em relação as outras por incluir loops, permitindo explorar um pouco mais dos recursos da lib LLVM; O segundo motivo está relacionado a um dos trabalhos da disciplina de HPP, no qual foi analisado um algoritmo de permutações, ocasionando em ficar com o conceito de fatorial "fresco" na memória.

## 2. Implementação das Funções
Segue o passo a passo da implementação e reprodução (geração do IR, exportação, compilação, linkagem e execução), das duas funções escolhidas, apontando partes em que elas diferem:
### 2.1 Implemetação em C++
- Requisitos
    - Sistema Operacional<br>
        Linux 64 bits.

    - LLVM e clang. Exemplo para instalação:
        ```shell
        # Ubuntu e derivados
        sudo apt install llvm clang libstdc++-14-dev

        # Arch e derivados
        sudo pacman -S llvm clang libstdc++
        ```
    - Makefile.

### 2.2 Geração do IR
IR trata-se da **Representação Intermediária** de baixo nível, que serve como ponte entre o código fonte e o código da máquina final. Sua geração foi feita através das principais estruturas fornecidas pelo LLVM.

Sua geração foi realizada a partir das principais estruturas fornecidas pela biblioteca:
- LLVMContext
- Module
- Function
- BasicBlock
- IRBuilder

Ambas as implementações seguiram uma estrutura semelhante:
`using namespace llvm` apenas para tornar o código mais conciso.
1. **Criação do contexto global;**
    - Criando o contexto onde todos os dados são manipulados usando `LLVMContext`
        ```c++
        LLVMContext context;
        ```
2. ****Criação do módulo;**
    - Módulo principal onde a memória é alocada, armazenando as funções e variáveis do programa.
        ```c++
        // --- DIFF_MIN_MAX ---
        Module *module = new Module("modulo", context);

        // --- FACTORIAL ---
        Module module ("factorial_module", context);
        // ** Define IRBuilder usando context
        IRBuilder<> ir_builder(context);
        ```
3. **Definição da assinatura da função;**
    - define o tipo do retorno, dos parâmetros e os argumentos das funções.
        - Aqui o primeira diferença entre o código das duas funções, enquanto a representação da `diff_min_max` implementa uma função com 2 parâmetros de tipo int64_t, `factorial` aceita um parâmetro int64_t.
        ```c++
        // --- DIFF_MIN_MAX ---
        Type *i64 = Type::getInt64Ty(context);
        FunctionType *func_type = FunctionType::get(i64, {i64, i64}, false);

        // --- FACTORIAL ---
        auto i64 = Type::getInt64Ty(context);
        auto func_type = FunctionType::get(i64, {i64}, false);
        ```
    - Seguindo para a criação da função `Function::Create()`
        ```c++
        // --- DIFF_MIN_MAX ---
        Function *function = Function::Create(func_type, Function::ExternalLinkage, 0, "diff_min_max", module);

        // --- FACTORIAL ---
        Function *function = Function::Create(func_type, Function::ExternalLinkage, "factorial", &module);
        ```
    - Definição do nome dos/do parâmetros/parâmetro:
        ```c++
        // --- DIFF_MIN_MAX ---
        auto args = function->arg_begin();
        auto arg_a = args++;
        arg_a->setName("a");
        auto arg_b = args++;
        arg_a->setName("b");

        // --- FACTORIAL ---
        Value* n = function->arg_begin();
        n->setName("n");
        ```
4. **Criação dos blocos básicos;**
    - Outra grande diferença entre as funções, pelo fato de `diff_min_max` ter menos complexidade, não necessita de loops, fazendo com que possua apenas um `BasicBlock`.
    A `factorial` necessita de três, sendo eles `entry`, `loop` e `end`.
        ```c++
        // --- DIFF_MIN_MAX ---
        BasicBlock *basic_block = BasicBlock::Create(context, "entry", function);

        // --- FACTORIAL ---
        BasicBlock* entry = BasicBlock::Create(context, "entry", function);
        BasicBlock* loop = BasicBlock::Create(context, "loop", function);
        BasicBlock* end = BasicBlock::Create(context, "end", function);
        ```
    - A utilização do IRBuilder possui uma diferença importante entre elas: 
        - Na função `diff_min_max`, o IRBuilder é definido como sendo o único `BasicBlock` criado.
        - Na função `factorial`, o IRBuilder é definido logo após a criação do `context`, assim é possível mover o `ir_builder` entre os diferentes `BasicBlocks` criados.
        ```c++
        // --- DIFF_MIN_MAX ---
        // ** Define IRBuilder usando o basic_block
        IRBuilder<> Builder(basic_block);

        // --- FACTORIAL ---
        // IRBuilder definido antes no código, usando o contexto, assim o IRBuilder é movido entre os BasicBlocks conferme a necessidade
        ir_builder.SetInsertPoint(entry);
        ir_builder.CreateBr(loop);

        ir_builder.SetInsertPoint(loop);
        ```
5. **Geração das instruções LLVM;**
    - Com o builder na posição correta, é possível criar a lógica da função.
    - As duas funções possuis implementações diferentes para sua lógica, suas diferenças em detalhes na seção **2.3 Diferenças entre as implementações**.
        ```c++
        // --- DIFF_MIN_MAX ---
        // Cmp: comparacao, comparar se a eh maior que b
        Value *Cmp = Builder.CreateICmpSGT(arg_a, arg_b, "cmp");

        // Realiza as possiveis subtracoes
        Value *SubAB = Builder.CreateSub(arg_a, arg_b, "subab");
        Value *SubBA = Builder.CreateSub(arg_b, arg_a, "subba");

        // Basicamente: (a > b) ? (a - b) : (b - a)
        Value *Result = Builder.CreateSelect(Cmp, SubAB, SubBA, "resultado");
        Builder.CreateRet(Result); // Return

        // --- FACTORIAL ---
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
        // Retorno da função
        ir_builder.CreateRet(res);
        ```

6. **Validação com verifyFunction;**
    - Validação simples da função implementada usando `verifyFunction` da `llvm/IR/Verifier.h`.

        ```c++
        if (verifyFunction(*function, &errs())) {
            errs() << "Falha em verificar a funcao\n";
            return EXIT_FAILURE;
        }
        ```

7. **Exportação para .ll e .bc.**
	- Usando `llvm/Support/raw_ostream.h` para definir o tipo de output dos dois formatos, foi possível gerar a representação textual somente "printando" no arquivo .ll, e o bitecode (.bc) gerado usando a função `WriteBitcodeToFile`, da lib llvm/Bitcode/BitcodeWriter.h.

        ```c++
        // Gerar representacao textual .ll
        // factorial: "factorial.ll"
        std::error_code EC;
        raw_fd_ostream outLL("diff_min_max.ll", EC);
        module->print(outLL, nullptr);

        // gera o .bc
        // factorial: "factorial.bc"
        raw_fd_ostream outBC("diff_min_max.bc", EC);
        WriteBitcodeToFile(*module, outBC);
        ```
### 2.3 Diferenças entre as implementações
As duas funções diferem principalmente em sua complexidade estrutural.

A função `diff_min_max` utiliza:
- Comparação entre inteiros (ICmpSGT);
- Subtração (CreateSub);
- Seleção condicional (CreateSelect);
- Retorno do resultado.

Já a função `factorial` exige uma estrutura mais elaborada, contendo:

- Variáveis temporárias;
- Laço de repetição;
- PHINodes;
- Multiplicação;
- Decremento;
- Desvio condicional entre blocos.
Isso permitiu explorar de forma mais aprofundada a manipulação do fluxo de controle em LLVM.

### 2.4 Pipeline (compilação, linkagem e execução)
Todo o processo está no Makefile desse projeto, usando duas formas diferentes de compilar e linkar as duas funções escolhidas.

- **Compilação:**
    Se trata de uma compilação normal de um código c++ com o clang do projeto LLVM, com a adição das flags, sendo elas necessárias para importar as bibliotecas usadas para realizar a implementação da função IR, exemplo `llvm/IR/LLVMContext.h`.<br>

    ```shell
    clang++ diff_min_max.cpp `llvm-config --cxxflags --ldflags --libs core` -o bin/generator_diff
    ```

- **Gerador do IR:**
    Os arquivos IR (.ll e .bc) já são gerados dentro da implementação em c++, basta executar o `gerador` gerado na etapa de compilação. É também possível gerá-los usando o próprio clang passando a flag `-emit-llvm`, e os nomeando (flag `-o`) de acordo com a representação intermediária desejada (.bc ou .ll).

- **Linkagem**:
Aqui onde a pipeline se difere entre as duas funções:
    - `diff_min_max`: a linkagem foi feita entre o arquivo de teste `test_diff.c`, e o object file obitido ao compilar o bitecode com o `llc` (`llvm static compiler`) passando a flag `-filetype=obj` (OBS: não necessária, pula a geração do assembly).
        ```shell
        # Compilar o bitecode para .o output file:
        llc factorial.ll -o bin/factorial.s
        # Linkar a função com o programa de teste:
        clang test/test_factorial.c bin/factorial.s -o prog_fact
        ```

    - `factorial`: usando também o `llc` foi compilado a representação textual para assembly diretamente, após foi realizada a linkagem com o arquivo de testes `test_factorial.c`.
        ```shell
        # Compilar o bitecode para .o output file:
        llc factorial.ll -o bin/factorial.s
        # Linkar a função com o programa de teste:
        clang test/test_factorial.c bin/factorial.s -o prog_fact
        ```
    - **NOTA**: É possível passar diretamente os arquivos IR para a linkagem com o arquivo de teste, sem a necessidade do llc atuando como intermediador, o clang é experto o suficiente para linkar `.bc` e `.ll` com o `.c`. Decidimos seguir usando o `llc` para não pular etapas relevantes do pipeline.

- **Execução:**
    - O binário para a execução é gerado na pasta root do projeto.
        ```shell
        ./prog_diff
        ./prog_fact
        ```

    - Os arquivos de teste consistem apenas da declaração da função desejada, e o uso dentro de um `printf` para validar seu output.
        - `test_diff.c`:
        ```c
        #include <stdio.h>
        #include <stdint.h>

        int64_t diff_min_max(int64_t a, int64_t b);

        int main() {
            printf("%ld\n", diff_min_max(2, 7));
            printf("%ld\n", diff_min_max(20, 10));
            printf("%ld\n", diff_min_max(-20, -5));
            printf("%ld\n", diff_min_max(1, 1));
            return 0;
        }
        ```
        - `test_factorial.c`:
        ```c
        #include <stdio.h>
        #include <stdint.h>

        int64_t factorial(int64_t n); 

        int main() {
            printf("%ld\n", factorial(0));
            printf("%ld\n", factorial(5));
            printf("%ld\n", factorial(10));
            return 0;
        }
        ```

## 3. Problemas encontrados e suas soluções
O primeiro "problema" encontrado foi entender e decidir entre as diferentes formas de obter o `.bc` e `.ll`, linkar e se deveríamos necessariamente passar pelo LLVM static compiler (llc), a resolução desse problema foi detalhada na seção anterior **2.4 Pipeline**.

### 3.1 Função factorial
Devido à maior complexidade da função `factorial`, que exige a utilização de múltiplos blocos básicos para representar diferentes momentos da execução (entrada, laço e saída), além do uso de PHI Nodes para controlar variáveis que sofrem alteração ao longo do fluxo, é compreensível assumir que mais dificuldades surgiriam durante sua implementação.

Entretanto, a principal dificuldade encontrada não esteve diretamente relacionada à lógica da função, mas sim ao processo de compilação com o `clang`. Durante a compilação, ocorreu um erro indicando a ausência da biblioteca de depuração do C++, **cassert**.

Esse problema ocorreu porque o Clang (versão 18) detectou a presença do GCC 14 no sistema e, por padrão, passou a utilizar os headers dessa versão. No entanto, em sistemas baseados em Ubuntu, nem sempre o pacote completo da libstdc++ correspondente é instalado automaticamente, o que pode resultar na ausência de alguns headers essenciais.

Como solução, optou-se pela abordagem mais direta e rápida, a instalação da versão correspondente da biblioteca (libstdc++-14-dev), garantindo a disponibilidade dos headers necessários e permitindo a compilação correta do código.

