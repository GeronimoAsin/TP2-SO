docker start TPE-ARQUI
docker exec -it TPE-ARQUI make clean -C /root/x64BareBones-master/Toolchain
docker exec -it TPE-ARQUI make clean -C /root/x64BareBones-master/
docker exec -it TPE-ARQUI make -C /root/x64BareBones-master/Toolchain
docker exec -it TPE-ARQUI make -C /root/x64BareBones-master/
docker stop TPE-ARQUI

