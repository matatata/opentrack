# Compile the source into position-independent object code
gcc -c -fPIC headpose_lib.c -o headpose_lib.o

# Link the object file into a shared library, linking the pthread library
gcc -shared -o libheadpose.so headpose_lib.o -lpthread


# client
gcc client_example.c -L. -lheadpose -o client
# Run it (pointing the dynamic linker to the current directory)
LD_LIBRARY_PATH=. ./client 127.0.0.1 4242

