docker start TPE
docker exec -it TPE make clean -C /root/x64BareBones-master/Toolchain
docker exec -it TPE make clean -C /root/x64BareBones-master/
docker exec -it TPE make -C /root/x64BareBones-master/Toolchain
docker exec -it TPE make -C /root/x64BareBones-master/
docker stop TPE

