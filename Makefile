pstty: ./src/pstty.c ./src/flag_handlers.c ./src/flags.c ./src/print_util.c
	cc -Wall -Wextra ./src/pstty.c ./src/flag_handlers.c ./src/flags.c ./src/print_util.c ./src/stty.c ./src/stty_value.c -o pstty

clean:
	rm -rf pstty demotwo
