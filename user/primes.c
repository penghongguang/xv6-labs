#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1
#define LEN sizeof(int)

int fd_left_get_data(int fd_l[2], int *dat) {
    if(read(fd_l[RD], dat, LEN) == LEN) {
        printf("prime %d\n", *dat);
        return 0;
    }
    return -1;
}

void transmit_data(int fd_l[2], int fd_r[2], int first_data) {
    int data;
    while(read(fd_l[RD], &data, LEN) == LEN) {
        if(data % first_data) {
            write(fd_r[WR], &data, LEN);
        }
    }
    close(fd_l[RD]);
    close(fd_r[WR]);
}

void primes(int fd_l[2]) {
    close(fd_l[WR]);
    int first_data;
    if(fd_left_get_data(fd_l, &first_data) == 0) {
        int fd_r[2];
        pipe(fd_r);
        transmit_data(fd_l, fd_r, first_data);

        if(fork() == 0) {
            primes(fd_r);
        } else {
            close(fd_r[RD]);
            wait(0);
        }
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    int fd_l[2];
    pipe(fd_l);

    for(int i = 2; i <= 35; ++i) {
        write(fd_l[WR], &i, LEN);
    }

    if(fork() == 0) {
        primes(fd_l); // right become left for next loop
    } else {
        close(fd_l[WR]);
        close(fd_l[RD]);
        wait(0);
    }

    exit(0);
}