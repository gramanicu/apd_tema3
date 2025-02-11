# Copyright 2020 Grama Nicolae

.PHONY: gitignore clean memory beauty run
.SILENT: beauty clean memory gitignore

# Program arguements

PCOUNT = 5
SFILE = ./tests/in/input1.txt

# Compilation variables
CC = mpic++
CFLAGS = -Wno-unused-parameter -Wno-cast-function-type -Wall -Wextra -pedantic -pthread -g -std=c++17
EXE = main
SRC = src/Main.cpp src/GenreParser/Parser.cpp src/GenreParser/Helpers.cpp
OBJ = $(SRC:.cpp=.o)

CSFILES = */*.cpp */*/*.cpp */*/*.hpp

# Compiles the program
build: $(OBJ)
	@$(CC) $^ -o $(EXE) $(CFLAGS) ||:
	-@rm -f *.o ||:
	@$(MAKE) -s gitignore ||:

%.o: %.cpp
	@$(CC) -c $< -o $@ $(CFLAGS) ||:

# Executes the binary
run: clean build
	@mpirun -np $(PCOUNT) ./$(EXE) $(SFILE)||:

# Deletes the binary and object files
clean:
	rm -f $(EXE) $(OBJ) apd_tema3.zip

# Automatic coding style, in my personal style
beauty:
	clang-format -i -style=file $(CSFILES)

# Adds and updates gitignore rules
gitignore:
	@echo "$(EXE)" > .gitignore ||:
	@echo "src/*.o" >> .gitignore ||:
	@echo "src/*/*.o" >> .gitignore ||:
	@echo ".vscode*" >> .gitignore ||:	
	@echo "*.zip" >> .gitignore ||:	
	@echo "*.log" >> .gitignore ||:	
	@echo "*.out" >> .gitignore ||:		

# Creates an archive of the project
archive: clean
	zip -FSr GRAMA_NICOLAE_332CA_TEMA3_APD_2020.zip ./src Makefile Readme.md

