## Instruções

1. Executar `make` neste diretório (`modules/sstf_sched`)
2. Executar `make` no diretório raiz do repositório
3. Iniciar o QEMU com o script `qemu-up.sh` da raiz do repositório
    - Dica: Se obtiver algum erro relativo a não ser possível obter o lock de escrita, possivelmente o processo do QEMU não foi terminado adequadamente. Nesse caso, execute `ps aux | grep qemu | grep -v grep | awk '{print $2}' | xargs sudo kill -9` para terminar os processos do QEMU e tente rodar o script `qemu-up.sh` novamente.
4. Iniciar o módulo do kernel com o comando `modprobe sstf-iosched`
5. Configurar o módulo `sstf-iosched` como o algoritmo de escalonamento de disco padrão com o comando `echo sstf > /sys/block/sdb/queue/scheduler`
6. Executar a aplicação de teste:

```bash
cd /bin
./sector_read
```
