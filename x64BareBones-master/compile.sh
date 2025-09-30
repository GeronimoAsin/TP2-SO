docker start TP2-SO
docker exec -it TP2-SO make clean -C /root/x64BareBones-master/Toolchain
docker exec -it TP2-SO make clean -C /root/x64BareBones-master/
docker exec -it TP2-SO make -C /root/x64BareBones-master/Toolchain
docker exec -it TP2-SO make -C /root/x64BareBones-master/
docker stop TP2-SO 

