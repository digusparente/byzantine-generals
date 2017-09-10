CXX = g++
CXXFLAGS = -std=c++11

BIN = bin
OBJ = obj
LOGS = log

SRCL = Lieutenant
#SRCG = General

LOBJ = $(SRCL) $(addprefix Main, $(SRCL))
#GOBJ = $(SRCG) $(addprefix Main, $(SRCG))

OBJS1 = $(patsubst %, $(OBJ)/%.o, $(LOBJ))
#OBJS2 = $(patsubst %, $(OBJ)/%.o, $(GOBJ))

.PHONY: directories all clean

run:
	@rm -rf $(LOGS)/*.txt
	@sudo mn -c
	@sudo python mininet-topology.py

all: directories lieutenant # general
	@echo "Compile completed"

directories:
	@mkdir -p $(BIN) $(OBJ) $(LOGS)

lieutenant: $(OBJS1)
	@$(CXX) -o $(BIN)/$@ $^
	@echo "Linking Lieutenant completed"

#general: $(OBJS2)
#	@$(CXX) -o $(BIN)/$@ $^
#	@echo "Linking General completed"

$(OBJ)/%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "Compile $< completed"

clean:
	@rm -rf $(BIN) $(OBJ) $(LOGS)
	@echo "Clean completed"