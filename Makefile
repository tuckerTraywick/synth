CC := gcc
CFLAGS := -O -g3 -std=c99 -Wall -Wextra -Wpedantic
INCLUDE := -Iinclude `pkg-config raylib --cflags`
LIBRARIES := -lm  `pkg-config raylib --libs`

SOURCE_FILES := $(shell find source -name '*.c')
OBJECT_FILES := $(SOURCE_FILES:source/%.c=build/%.o)

.PHONY: build
build: binary/run

binary/run: $(OBJECT_FILES)
	@mkdir -p binary
	@$(CC) $^ -o $@ $(LIBRARIES)

build/%.o: source/%.c
	@mkdir -p build
	@$(CC) $(CFLAGS) $(INCLUDE) -c -MMD -MP -MT $@ -MF build/$*.d $< -o $@

.PRECIOUS: build/%.d

.PHONY: clean
clean:
	@rm -rf build binary

-include $(OBJECT_FILES:.o=.d)
