# Diretórios
BINFOLDER := bin/
INCFOLDER := inc/
SRCFOLDER := src/
OBJFOLDER := obj/

# Compiladores
CC := gcc       # Compilador para arquivos C
CXX := g++      # Compilador para arquivos C++

# Flags de compilação
CFLAGS := -std=c99 -Wall
CXXFLAGS := -std=c++11 -Wall    # Flags para C++
LINKFLAGS := -lwiringPi -lpthread -lbcm2835

# Arquivos-fonte
C_SRCFILES := $(wildcard src/*.c)
CPP_SRCFILES := $(wildcard src/*.cpp)

# Gerar arquivos-objeto para C e C++
C_OBJFILES := $(C_SRCFILES:src/%.c=obj/%.o)
CPP_OBJFILES := $(CPP_SRCFILES:src/%.cpp=obj/%.o)

# Meta principal: compilar tudo
all: | $(OBJFOLDER) $(BINFOLDER) compile_c compile_cpp link

# Compilar os arquivos C antes
compile_c: $(C_OBJFILES)

# Compilar os arquivos C++ depois
compile_cpp: $(CPP_OBJFILES)

# Linkar os objetos C e C++ juntos
link:
	$(CXX) $(C_OBJFILES) $(CPP_OBJFILES) -o $(BINFOLDER)main $(LINKFLAGS)

# Regras para compilar arquivos .c em arquivos .o
obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I./$(INCFOLDER)

# Regras para compilar arquivos .cpp em arquivos .o
obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I./$(INCFOLDER)

# Diretórios criados, se não existirem
$(OBJFOLDER):
	mkdir -p $(OBJFOLDER)

$(BINFOLDER):
	mkdir -p $(BINFOLDER)

# Meta para executar o binário
run: bin/main
	bin/main

# Meta para limpar arquivos-objeto e binário
.PHONY: clean
clean:
	rm -rf obj/*.o
	rm -rf bin/main
	touch obj/.gitkeep
	touch bin/.gitkeep
