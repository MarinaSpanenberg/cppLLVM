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
A função `factorial` trata-se de obter o produto de todos os números inteiros positivos de 1 até o número _n_ informado.
- **Motivo da escolha:** A escolha desta foi função se deu por dois fatores: O primeiro é sua maior complexidade em relação as outras por incluir loops, permitindo explorar um pouco mais dos recursos da lib LLVM; O segundo motivo está relacionado a um dos trabalhos da disciplina de HPP, no qual foi analisado um algoritmo de permutações, ocasionando em ficar com o conceito de fatorial "fresco" na memória.

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

### 3.1 Função diff_min_max

### 3.2 Função factorial
Devido à maior complexidade da função `factorial`, que exige a utilização de múltiplos blocos básicos para representar diferentes momentos da execução (entrada, laço e saída), além do uso de PHI Nodes para controlar variáveis que sofrem alteração ao longo do fluxo, é compreensível assumir que mais dificuldades surgiriam durante sua implementação.

Entretanto, a principal dificuldade encontrada não esteve diretamente relacionada à lógica da função, mas sim ao processo de compilação com o `clang`. Durante a compilação, ocorreu um erro indicando a ausência da biblioteca de depuração do C++, **cassert**.

Esse problema ocorreu porque o Clang (versão 18) detectou a presença do GCC 14 no sistema e, por padrão, passou a utilizar os headers dessa versão. No entanto, em sistemas baseados em Ubuntu, nem sempre o pacote completo da libstdc++ correspondente é instalado automaticamente, o que pode resultar na ausência de alguns headers essenciais.

Como solução, optou-se pela abordagem mais direta e rápida, a instalação da versão correspondente da biblioteca (libstdc++-14-dev), garantindo a disponibilidade dos headers necessários e permitindo a compilação correta do código.