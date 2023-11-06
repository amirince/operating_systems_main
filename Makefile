shm_proc: shm_processes.c
	gcc shm_processes.c -D_SVID_SOURCE -D_GNU_SOURCE -Wall -std=c99 -Werror=cpp -pedantic  -o shm_proc
my_code: my_code.c
	gcc my_code.c -D_DEFAULT_SOURCE -D_GNU_SOURCE -Wall -std=c99 -pedantic -o my_code