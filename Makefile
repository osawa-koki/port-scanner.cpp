CC = gcc
SOURCES = src/main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = main
BUILD_DIR = build
LDFLAGS = -lstdc++ -fopenmp

all: $(BUILD_DIR) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)/$@

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -rf $(OBJECTS) $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

.PHONY: $(BUILD_DIR)

run: $(EXECUTABLE)
	$(BUILD_DIR)/$(EXECUTABLE)
