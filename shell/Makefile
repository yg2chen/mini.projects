CC := gcc
ELF := shlite
SRC_DIR := .
BUILD_DIR := ./build
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS += $(addprefix $(BUILD_DIR)/, $(patsubst %.c, %.o, $(notdir $(wildcard $(SRC_DIR)/*.c))))

all: check_dir build
	@echo 'build completed!'

check_dir:
	@if [[ ! -d "build" ]]; then \
		echo "creating build directory"; \
		mkdir -p build; \
	fi

build: $(OBJS)
	$(CC) -o $(ELF) $^
	

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $^ -o $@

.PHONY: clean
clean:
	rm -f $(ELF) $(OBJS)
	@echo 'clean completed!'
