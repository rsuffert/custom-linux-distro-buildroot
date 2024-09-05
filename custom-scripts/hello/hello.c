#include <stdio.h>

int main(void){
	printf("Hello World!\n");

	return 0;
}

// mover para /output/target/usr/bin (programa de usuário) e fazer make -> vai aparecer em /usr/bin no qemu
// precisa compilar com cross-compiler para a arquitetura do target (qemu) -> para C, é o cross-gcc
    // /output/host/bin/i686-buildroot-linux-gnu-gcc