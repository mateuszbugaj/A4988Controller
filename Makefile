A4988_LIBRARY_DIR := ./
A4988_LIBRARY_NAME := a4988
A4988_LIBRARY_OBJ_DIR := $(A4988_LIBRARY_DIR)src/
A4988_LIBRARY_INC_DIR := $(A4988_LIBRARY_DIR)inc/
A4988_LIBRARY_SRC := $(wildcard $(A4988_LIBRARY_OBJ_DIR)*.c)
A4988_LIBRARY_OBJ := $(A4988_LIBRARY_SRC:.c=.o)
A4988_LIBRARY_AR := ar -rcs

# default rule to build .a library
all: $(A4988_LIBRARY_NAME).a

$(A4988_LIBRARY_NAME).a: $(A4988_LIBRARY_OBJ)
    $(A4988_LIBRARY_AR) $@ $^

%.o: %.c
    $(CC) $(CFLAGS) -I$(A4988_LIBRARY_INC_DIR) -c $< -o $@

clean:
    rm -f $(A4988_LIBRARY_NAME).a $(A4988_LIBRARY_OBJ)