all: test test_dep

test: test.c
	cc test.c -Wl,-rpath /usr/local/lib -o test

test_dep: test.c
	cc -DLINK_DEPENDENT=1 -L/usr/local/lib -Wl,-rpath /usr/local/lib -lfreetrackclient test.c -o test_dep
clean:
	rm test
	rm test_dep
