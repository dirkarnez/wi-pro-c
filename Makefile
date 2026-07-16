CC = gcc
CFLAGS = -Wall -g

RUSTC = rustc
RUSTFLAGS = -O

PYTHON = python

# The default target that runs when you just type 'make'
all: hello_c hello_rust

# Rule to compile the executable
hello_c: ./c/main.c
	$(CC) $(CFLAGS) -o ./main_c ./c/main.c -lm

hello_rust: ./rust/main.rs
	$(RUSTC) $(RUSTFLAGS) -o ./main_rust ./rust/main.rs

python: ./python/create_idft_mat_modified.py
	$(PYTHON) -m pip install --upgrade pip && \
	$(PYTHON) -m pip install numpy && \
	$(PYTHON) ./python/create_idft_mat_modified.py
  
# Rule to clean up build artifacts
clean:
	rm -f hello_c hello_rust
