# Variables for compiler and flags
CC = gcc
CFLAGS = -Wall -g

# New Rust variables
RUSTC = rustc
RUSTFLAGS = -O

# The default target that runs when you just type 'make'
all: hello_c hello_rust

# Rule to compile the executable
hello_c: c/main.c
	$(CC) $(CFLAGS) -o main_c c/main.c

hello_rust: rust/main.rs
	$(RUSTC) $(RUSTFLAGS) -o main_rust rust/main.rs
  
# Rule to clean up build artifacts
clean:
	rm -f hello_c hello_rust
