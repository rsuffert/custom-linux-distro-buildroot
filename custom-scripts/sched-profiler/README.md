# Profiler de escalonamento (Sched Profiler)

## Descrição do programa desenvolvido

Este programa consiste em um profiler que permite analisar o comportamento de um certo número de threads com uma determinada política de escalonamento e o modo como elas competem pelo controle da CPU. Para tal, o programa permite a criação de *n* threads com uma determinada política de prioridade e, ao final, exibe uma representação do uso de CPU de cada uma. As políticas de escalonamento suportadas são:

- `SCHED_FIFO` - padrão do kernel Linux (código 1);
- `SCHED_RR` - padrão do kernel Linux (código 2);
- `SCHED_IDLE` - padrão do kernel Linux (código 5);
- `SCHED_LOW_IDLE` - implementada no kernel Linux para o trabalho (código 7).

As threads criadas escrevem o caractere que as representa (de A-Z) em um buffer compartilhado até ele ficar cheio, representando, então, o padrão de controle de CPU de cada thread, que são escalonadas para escrever no buffer de acordo com a política que for selecionada dentre as acima. Ao final da execução, o buffer compartilhado é escrito na tela, bem como um resumo em que sequências de caracteres iguais são reduzidas para um único caractere e uma lista de quantas vezes cada thread assumiu o controle da CPU.

## Executando o programa

Para executar o programa, é necessário compilá-lo com o *cross-compiler* e executá-lo dentro do QEMU. Para isso, pode ser seguido o seguinte script:

```bash
cd custom-scripts/sched-profiler # entrar na pasta do programa
make # compilá-lo com o cross-compiler e copiar o código-objeto para dentro do filesystem do QEMU
cd ../.. # retornar à raiz do Buildroot
make # recompilar a distribuição para as aterações serem aplicadas globalmente
sudo ./qemu-up.sh # iniciar o QEMU
```

**DICA**: Isso pode ser encurtado em um único comando com `cd custom-scripts/sched-profiler && make && cd ../.. && make && sudo ./qemu-up.sh`.

Então, estando dentro do QEMU, basta executar a aplicação `sched_profiler`, localizada dentro da pasta `/bin`, fornecendo como parâmetros pela linha de comando (1) o tamanho do buffer em que as threads criadas irão escrever, (2) quantas threads serão criadas (limitado a 26, pois para mais que isso não há mais letras maiúsculas no alfabeto para representá-las) e (3) qual o código da política de escalonamento desejada, conforme a seção anterior. O comando fica algo como:

```bash
cd /bin
./sched_profiler <buffer-size> <n-threads> <sched-policy>
```

## Exemplos de execuções

Para cada uma das políticas de escalonamento suportadas, será fornecido um exemplo de execução e output com 2, 4 e 8 threads. Uma observação a se fazer em relação ao output das execuções é que foram impressos o buffer não resumido e o buffer resumido. No caso do buffer não resumido, a representação sai incompleta, pois excede o tamanho de representação configurado no sistema. Por outro lado, o buffer resumido sempre entrega uma representação fiel da alternância das threads.

### `SCHED_FIFO`

Exemplo com 2 threads e Buffer de 1000000 de posições:

```bash
./sched_profiler 1000000 2 1
================ Threads finished executing! ================
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
A
A: 1
B: 0
```

Exemplo com 4 threads e Buffer de 1000000 de posições:

```bash
./sched_profiler 1000000 4 1
================ Threads finished executing! ================
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
A
A: 1
B: 0
C: 0
D: 0
```

Exemplo com 8 threads e Buffer de 1000000 de posições:

```bash
./sched_profiler 1000000 8 1
================ Threads finished executing! ================
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
A
A: 1
B: 0
C: 0
D: 0
E: 0
F: 0
G: 0
H: 0
```

### `SCHED_RR`

Exemplo com 2 threads e Buffer de 1500000 de posições:

```bash
./sched_profiler 1000000 2 2
================ Threads finished executing! ================
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAe
BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
AB
A: 1
B: 1
```

Exemplo com 4 threads e Buffer de 15000000 de posições:

```bash
./sched_profiler 15000000 4 2
================ Threads finished executing! ================
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB
ABDBADBCABAB
A: 4
B: 5
C: 1
D: 2
```

Exemplo com 8 threads e Buffer de 20000000 de posições:

```bash
./sched_profiler 20000000 4 2
================ Threads finished executing! ================
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAF
ABDBDBHCHECHGFHF
A: 1
B: 3
C: 2
D: 2
E: 1
F: 2
G: 1
H: 4
```

### `SCHED_IDLE`


Exemplo com 2 threads e Buffer de 1000000 de posições:

```bash
./sched_profiler 1000000 2 5
================ Threads finished executing! ================
BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
BABABABABABABABAB
A: 8
B: 9
```

Exemplo com 4 threads e Buffer de 1000000 de posições:

```bash
./sched_profiler 100000 4 5
================ Threads finished executing! ================
BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBA
BCABABCDBACDBACDBACA
A: 6
B: 6
C: 5
D: 3
```

Exemplo com 8 threads e Buffer de 1000000 de posições:

```bash
./sched_profiler 1000000 8 5
================ Threads finished executing! ================
BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBC
BCGFEHADCGFABCGHDFABCDHC
A: 3
B: 3
C: 5
D: 3
E: 1
F: 3
G: 3
H: 3
```

### `SCHED_LOW_IDLE`

Exemplo com 2 threads e Buffer de 1000000 de posições:

```bash
./sched_profiler 1000000 2 7
================ Threads finished executing! ================
BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
BABABABABABABAB
A: 7
B: 8
```

Exemplo com 4 threads e Buffer de 1000000 de posições:

```bash
./sched_profiler 1000000 4 7
================ Threads finished executing! ================
BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBC
BCBACDBCABADCBDC
A: 3
B: 5
C: 5
D: 3
```

Exemplo com 8 threads e Buffer de 1000000 de posições:

```bash
./sched_profiler 1000000 8 7
================ Threads finished executing! ================
BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBD
BCEHABEGAFDHBECFGAD
A: 3
B: 3
C: 2
D: 2
E: 3
F: 2
G: 2
H: 2
```