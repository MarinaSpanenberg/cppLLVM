all: prog_diff prog_fact

prog_diff: diff_min_max.cpp test/test_diff.c
	mkdir -p bin
	# Compilar a funcao/gerador em c++:
	clang++ diff_min_max.cpp `llvm-config --cxxflags --ldflags --libs core` -o bin/generator_diff
	# Executar o gerador para gerar a representação textual .ll e .bc:
	./bin/generator_diff
	# Compilar o bitecode para .o output file:
	llc -filetype=obj diff_min_max.bc -o bin/diff_min_max.o
	# Linkar a função com o programa de teste:
	clang test/test_diff.c bin/diff_min_max.o -o prog_diff

prog_fact: factorial.cpp test/test_factorial.c
	mkdir -p bin
	# Compilar a funcao/gerador em c++:
	clang++ factorial.cpp `llvm-config --cxxflags --ldflags --libs core` -o bin/generator_fact
	# Executar o gerador para gerar a representação textual .ll e .bc:
	./bin/generator_fact
	# Compilar o bitecode para .o output file:
	llc -filetype=obj factorial.bc -o bin/factorial.o
	# Linkar a função com o programa de teste:
	clang test/test_factorial.c bin/factorial.o -o prog_fact
	# -> Executaveis gerados no diretorio principal: prog_diff e prog_fact

clean:
	rm -rf bin *.ll *.bc prog_diff prog_fact
