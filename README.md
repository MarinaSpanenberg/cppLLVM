# Relatório - Trabalho Geração de Código com LLVM

**Integrantes:**
- João Vítor Bortoluz da Rosa - 216486
- Marina Barbosa Spanenberg - 

## 1. Funções escolhidas e motivos
Foram escolhidas duas funções, `diff_min_max` escolhida por João e `factorial` escolhida por Marina.
### 1.1 Função diff_min_max (João)
A função `diff_min_max` se trata de uma comparação entre dois números, ao definir o maior deles, é tirado a diferença do maior com o menor, de forma que nunca retorne um número negativo.
- **Motivo da escolha:** O motivo da escolha dessa função foi que ela permite implementar funções de comparação de inteiros (ICmp) e diferença (diff) da lib llvm, além de ser de "baixa complexidade", não necessitando um uso mais extenso da lib llvm.

### 1.2 Função factorial (Marina)

## 2. Implemetação das Funções
Segue o passo a passo da implementação e reprodução (geração do IR, exportação, compilação, linkagem e execução), das duas funções escolhidas, apontando partes em que elas diferem:
### 2.1 Implemetação em C++

### 2.2 Geração do IR

## 3. Problemas encontrados e suas soluções
