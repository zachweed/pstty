all: pstty demotwo

pstty: pstty.c flag_handlers.c flags.c print_util.c
	cc -Wall -Wextra pstty.c flag_handlers.c flags.c print_util.c -o pstty

demotwo: demotwo.c flag_handlers.c flags.c print_util.c
	cc -Wall -Wextra demotwo.c flag_handlers.c flags.c print_util.c -o demotwo

clean:
	rm -rf pstty demotwo
