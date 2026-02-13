# Makefile (Cmake wrapper)

SHELL := /bin/bash

# You can override these like:
# 	make release GEN="Unix Makefiles"
GEN 			?= Ninja
SRC_DIR		?= .
BUILD_DIR	?= build
INST_DIR	?= build-installed

# Cmake helpers
define cmake_configure
	cmake -S "$(SRC_DIR)" -B "$(BUILD_DIR)" -G "$(GEN)" $(1)
endef

define cmake_build
	cmake --build "$(BUILD_DIR)" $(1)
endef

.PHONY: help build release debug test install clean

help:
	@echo "Targets:"
	@echo "  make build			- Configure + build (default: Release)"
	@echo "  make release		- Configure + build Release"
	@echo "  make debug			- Configure + build Debug"
	@echo "  make test			- Configure + build Debug with tests enabled"
	@echo "  make install		- Configure + build + install into $(INST_DIR)"
	@echo "  make clean			- Remove $(BUILD_DIR)/ and $(INST_DIR)/"
	@echo ""
	@echo "Overrides:"
	@echo "  GEN=<generator> BUILD_DIR=<dir> INST_DIR=<dir>"
	@echo "  Example make debug Gen=\"Unix Makefiles\""

# Default: Release build
build: release

release:
	$(call cmake_configure,-DCMAKE_BUILD_TYPE=Release -DPHYSIM_TESTING=OFF)
	$(call cmake_build,)

debug:
	$(call cmake_configure,-DCMAKE_BUILD_TYPE=Debug -DPHYSIM_TESTING=OFF)
	$(call cmake_build,)

# "test" here means: configure with tests enabled + build them.
# (Running tests can be done with: ctest --test-dir build)
test:
	$(call cmake_configure,-DCMAKE_BUILD_TYPE=Debug -DPHYSIM_TESTING=ON)
	$(call cmake_build,)

# Optional: local install into build-installed/
install:
	$(call cmake_configure,
	-DCMAKE_BUILD_TYPE=Release 
	-DPHYSIM_TESTIN=OFF 
	-DPHYSIM_INSTALL_PREFIX="$(abspath $(INST_DIR))")
	$(call cmake_build,)
	cmake --install "$(BUILD_DIR)"

clean:
	rm -rf "$(BUILD_DIR)" "$(INST_DIR)"

