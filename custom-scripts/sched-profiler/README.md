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

Para cada uma das políticas de escalonamento suportadas, será fornecido um exemplo de execução e output com 2, 4 e 8 threads.

### `SCHED_FIFO`

### `SCHED_RR`

### `SCHED_IDLE`

### `SCHED_LOW_IDLE`