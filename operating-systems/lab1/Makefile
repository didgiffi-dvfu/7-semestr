CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic
CPPFLAGS ?= -Iinclude
BUILD_DIR := build
SOURCES := src/main.c src/greeting.c

ifeq ($(OS),Windows_NT)
TARGET := $(BUILD_DIR)/hello.exe
CREATE_BUILD_DIR = if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
REMOVE_BUILD_DIR = if exist $(BUILD_DIR) rmdir /S /Q $(BUILD_DIR)
RUN_TARGET = $(TARGET)
CHECK_OUTPUT = $(TARGET) | findstr /x /c:"Hello, World!" >nul
else
TARGET := $(BUILD_DIR)/hello
CREATE_BUILD_DIR = mkdir -p $(BUILD_DIR)
REMOVE_BUILD_DIR = rm -rf $(BUILD_DIR)
RUN_TARGET = ./$(TARGET)
CHECK_OUTPUT = test "$$($(TARGET))" = "Hello, World!"
endif

.PHONY: all run test clean

all: $(TARGET)

$(TARGET): $(SOURCES) include/greeting.h
	$(CREATE_BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	$(RUN_TARGET)

test: $(TARGET)
	@$(CHECK_OUTPUT)
	@echo "Test passed"

clean:
	$(REMOVE_BUILD_DIR)
