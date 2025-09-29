docker start TP2-SO
docker exec -it TP2-SO make clean -C /root/Toolchain
docker exec -it TP2-SO make clean -C /root/
docker exec -it TP2-SO make -C /root/Toolchain
docker exec -it TP2-SO make -C /root/
docker stop TP2-SO

