CC := g++
FLAGS := -std=c++11 -w
TEST_FLAGS := -lgtest_main -lgtest -lpthread
INC_DIR := include
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
INCLUDE := -I./$(INC_DIR)
$(BIN_DIR)/main: $(BUILD_DIR)/main.o $(BUILD_DIR)/Date.o $(BUILD_DIR)/User.o $(BUILD_DIR)/Meeting.o $(BUILD_DIR)/Storage.o $(BUILD_DIR)/AgendaService.o $(BUILD_DIR)/AgendaUI.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(FLAGS) $(INCLUDE) $^ -o $@

$(BUILD_DIR)/main.o: main.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) $(INCLUDE) -c $^ -o $@

$(BUILD_DIR)/Storage.o: $(INC_DIR)/Storage.hpp $(SRC_DIR)/Storage.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) $(INCLUDE) -c $(SRC_DIR)/Storage.cpp -o $@

$(BUILD_DIR)/Date.o: $(INC_DIR)/Date.hpp $(SRC_DIR)/Date.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) $(INCLUDE) -c $(SRC_DIR)/Date.cpp -o $@

$(BUILD_DIR)/User.o: $(INC_DIR)/User.hpp $(SRC_DIR)/User.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) $(INCLUDE) -c $(SRC_DIR)/User.cpp -o $@

$(BUILD_DIR)/Meeting.o: $(INC_DIR)/Meeting.hpp $(SRC_DIR)/Meeting.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) $(INCLUDE) -c $(SRC_DIR)/Meeting.cpp -o $@

$(BUILD_DIR)/AgendaService.o: $(INC_DIR)/AgendaService.hpp $(SRC_DIR)/AgendaService.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) $(INCLUDE) -c $(SRC_DIR)/AgendaService.cpp -o $@

$(BUILD_DIR)/AgendaUI.o: $(INC_DIR)/AgendaUI.hpp $(SRC_DIR)/AgendaUI.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) $(INCLUDE) -c $(SRC_DIR)/AgendaUI.cpp -o $@



clean:
	@rm -f *.gch
	@rm -f *.o
	@rm -f main
	@echo "cleaned"