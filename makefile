TARGET_EXEC := app
BUILD_DIR := build
SRC_DIR := src
STATIC_LIBS := glfw3 m GL
INCLUDE_DIRS := dependencies/include

STATIC_LIBS := $(addprefix -l, $(STATIC_LIBS))

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

# INCLUDE_DIRS := $(shell find $(SRC_DIR) -type d)
INCLUDE_FLAGS := $(addprefix -I, $(INCLUDE_DIRS))

CFLAGS := $(INCLUDE_FLAGS) -MMD -MP

.PHONY: all
all : $(BUILD_DIR)/$(TARGET_EXEC) .clangd
	./$(BUILD_DIR)/$(TARGET_EXEC)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(STATIC_LIBS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.clangd :
	printf "CompleFlags:\n\tAdd: [-I$$PWD/include]" > .clangd

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
