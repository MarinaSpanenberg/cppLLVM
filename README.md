# Relatório - Trabalho Geração de Código com LLVM

**Integrantes:**
- João Vítor Bortoluz da Rosa - 216486
- Marina Barbosa Spanenberg - 196379

## 1. Funções escolhidas e motivos
Foram escolhidas duas funções, `diff_min_max` escolhida por João e `factorial` escolhida por Marina.
### 1.1 Função diff_min_max (João)
A função `diff_min_max` se trata de uma comparação entre dois números, ao definir o maior deles, é tirado a diferença do maior com o menor, de forma que nunca retorne um número negativo.
- **Motivo da escolha:** O motivo da escolha dessa função foi que ela permite implementar funções de comparação de inteiros (ICmp) e diferença (diff) da lib llvm, além de ser de "baixa complexidade", não necessitando um uso mais extenso da lib llvm.

### 1.2 Função factorial (Marina)
A função `factorial` trata-se de obter o produto de todos os números inteiros positivos dentro do número _n_ passado até chegar no 1.
- **Motivo da escolha:** A escolha desta foi função se deu devido a dois fatores: O primeiro seria por se tratar de uma complexidade maior em relação as outras por incluir loops, permitindo explorar um pouco mais sobre a lib LLVM; O último se deve a que em um dos trabalhos da disciplina de HPP, escolhi um código de permutações para analisar, ocasionando em ficar com o fatorial "fresco" na memória.

## 2. Implemetação das Funções
Segue o passo a passo da implementação e reprodução (geração do IR, exportação, compilação, linkagem e execução), das duas funções escolhidas, apontando partes em que elas diferem:
### 2.1 Implemetação em C++
- Requisitos
    - Sistema Operacional<br>
        Linux 64 bits.

    - LLVM e clang. Exemplo para instalação:
        ```shell
        # Ubuntu e derivados
        sudo apt install llvm clang libstdc++-14-dev

        # Arch
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
1. Criação do contexto global;
2. Criação do módulo;
3. Definição da assinatura da função;
4. Criação dos blocos básicos;
5. Geração das instruções LLVM;
6. Validação com verifyFunction;
7. Exportação para .ll e .bc.
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
## 3. Problemas encontrados e suas soluções

### Função diff_min_max

### 3.2 Função factorial
