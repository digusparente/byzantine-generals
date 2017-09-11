CXX = g++
CXXFLAGS = -std=c++11

BIN = bin
OBJ = obj
LOGS = log

SRC = Soldier Main

OBJS = $(patsubst %, $(OBJ)/%.o, $(SRC))

.PHONY: directories all clean

run:
	@rm -rf $(LOGS)/*.txt
	@sudo python topology.py 7 2 0 1

all: directories soldier
	@echo "Compile completed"

directories:
	@mkdir -p $(BIN) $(OBJ) $(LOGS)

soldier: $(OBJS)
	@$(CXX) -o $(BIN)/$@ $^
	@echo "Linking soldier completed"

$(OBJ)/%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "Compile $< completed"

clean:
	@rm -rf $(BIN) $(OBJ) $(LOGS)
	@echo "Clean completed"