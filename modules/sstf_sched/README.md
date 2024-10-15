## Instruções

1. Executar `make` neste diretório (`modules/sstf_sched`)
2. Iniciar o QEMU com o script `qemu-up.sh` da raiz do repositório
3. Iniciar o módulo do kernel com o comando `modprobe sstf-iosched`
4. Configurar o módulo `sstf-iosched` como o algoritmo de escalonamento de disco padrão com o comando `echo sstf > /sys/block/sdb/queue/scheduler`
5. Executar a aplicação de teste:

```bash
cd /bin
./sstf_read
```
