#!/bin/sh

# mover arquivos de inicialização do módulo de rede para dentro do QEMU
cp $BASE_DIR/../custom-scripts/S41network-config $BASE_DIR/target/etc/init.d
chmod +x $BASE_DIR/target/etc/init.d/S41network-config

# mover arquivo do programa do servidor para dentro do QEMU
cp $BASE_DIR/../custom-scripts/stats-server.py $BASE_DIR/target/usr/bin
cp $BASE_DIR/../custom-scripts/S42stats-server-config $BASE_DIR/target/etc/init.d
chmod +x $BASE_DIR/target/etc/init.d/S42stats-server-config