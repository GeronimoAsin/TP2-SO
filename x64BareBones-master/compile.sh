#!/bin/bash

# Verificar si se pasó el argumento "buddy"
if [ "$1" = "buddy" ]; then
    echo "Compilando con Buddy Memory Manager..."
    MAKE_TARGET="buddy"
else
    echo "Compilando con Standard Memory Manager..."
    MAKE_TARGET="all"
fi

docker start TP2-SO
docker exec -it TP2-SO make clean -C /root/x64BareBones-master/Toolchain
docker exec -it TP2-SO make clean -C /root/x64BareBones-master/
docker exec -it TP2-SO make -C /root/x64BareBones-master/Toolchain
docker exec -it TP2-SO make $MAKE_TARGET -C /root/x64BareBones-master/
docker stop TP2-SO
