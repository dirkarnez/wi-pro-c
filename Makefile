# Variables for compiler and flags
CC = gcc
CFLAGS = -Wall -g

# New Rust variables
RUSTC = rustc
RUSTFLAGS = -O

# The default target that runs when you just type 'make'
all: hello_c hello_rust

# Rule to compile the executable
hello_c: hello.c
	$(CC) $(CFLAGS) -o hello hello.c

hello_rust: hello.rs
	$(RUSTC) $(RUSTFLAGS) -o hello_rust hello.rs
  
# Rule to clean up build artifacts
clean:
	rm -f hello_c hello_rust
