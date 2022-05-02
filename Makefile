CC := g++
PROJECT := rrtracer

all: $(PROJECT)
.PHONY: all

include template/build

$(eval $(reset_build))
NAME := $(PROJECT)
SRC_DIR := src
SRC_EXT := cpp
INCLUDE_DIR := lib
CFLAGS := -O2 -g -DDEBUG -Wall -Wextra -std=c++17
LDFLAGS := -pthread
$(eval $(make_build))
