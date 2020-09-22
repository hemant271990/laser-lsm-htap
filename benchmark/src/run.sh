#./benchmark workloads/workload-k2 load >> log-k2 2>> err-k2
#sleep 30
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-k2 run >> log-k2 2>> err-k2
#sleep 30

#./benchmark workloads/workload-k3 load >> log-k3 2>> err-k3
#sleep 30
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-k3 run >> log-k3 2>> err-k3
#sleep 30

#./benchmark workloads/workload-k4 load >> log-k4 2>> err-k4
#sleep 30
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-k4 run >> log-k4 2>> err-k4
#sleep 30

#./benchmark workloads/workload-k2-col10 load >> log-k2 2>> err-k2
#sleep 30
#
#./benchmark workloads/workload-k3-col10 load >> log-k3 2>> err-k3
#sleep 30
#
#./benchmark workloads/workload-k4-col10 load >> log-k4 2>> err-k4
#sleep 30
#
#./benchmark workloads/workload-k2-col20 load >> log-k2 2>> err-k2
#sleep 30
#
#./benchmark workloads/workload-k3-col20 load >> log-k3 2>> err-k3
#sleep 30
#
#./benchmark workloads/workload-k4-col20 load >> log-k4 2>> err-k4
#sleep 30
#
#./benchmark workloads/workload-k2-col40 load >> log-k2 2>> err-k2
#sleep 30
#
#./benchmark workloads/workload-k3-col40 load >> log-k3 2>> err-k3
#sleep 30
#
#./benchmark workloads/workload-k4-col40 load >> log-k4 2>> err-k4
#sleep 30
#
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-k2-col10 run >> log-k2 2>> err-k2
#sleep 30
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-k3-col10 run >> log-k3 2>> err-k3
#sleep 30
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-k4-col10 run >> log-k4 2>> err-k4
#sleep 30
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-k2-col20 run >> log-k2 2>> err-k2
#sleep 30
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-k3-col20 run >> log-k3 2>> err-k3
#sleep 30
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-k4-col20 run >> log-k4 2>> err-k4
#sleep 30
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-k2-col40 run >> log-k2 2>> err-k2
#sleep 10
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-k3-col40 run >> log-k3 2>> err-k3
#sleep 30
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-k4-col40 run >> log-k4 2>> err-k4
#sleep 30
#

################
#     LOAD     #
################

# for workload cg1
# k = 2
#./benchmark workloads/workload-col30-cg1 load 2 ~/rocksdb-data/data-bench-col30-cg1 true >> log-cg1
#sleep 30
#./benchmark workloads/workload-col30-cg1 compact 2 ~/rocksdb-data/data-bench-col30-cg1 false >> log-cg1
#sleep 30
# k = 2
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-cg1 run 2 ~/rocksdb-data/data-bench-col30-cg1 true >> log-cg1
#sleep 30
#rm -rf /rocksdb-data/data-bench-col30-cg30

# for workload cg2
# k = 2
#./benchmark workloads/workload-col30-cg2 load 2 ~/rocksdb-data/data-bench-col30-cg2 true >> log-cg2
#sleep 30
#./benchmark workloads/workload-col30-cg2 compact 2 ~/rocksdb-data/data-bench-col30-cg2 false >> log-cg2
#sleep 30
# k = 2
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-cg2 run 2 ~/rocksdb-data/data-bench-col30-cg2 true >> log-cg2
#sleep 30
#rm -rf /rocksdb-data/data-bench-col30-cg2


# for workload cg5
# k = 2
#./benchmark workloads/workload-col30-cg5 load 2 ~/rocksdb-data/data-bench-col30-cg5 true >> log-cg5
#sleep 30
#./benchmark workloads/workload-col30-cg5 compact 2 ~/rocksdb-data/data-bench-col30-cg5 false >> log-cg5
#sleep 30
# k = 2
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-cg5 run 2 ~/rocksdb-data/data-bench-col30-cg5 true >> log-cg5
#sleep 30
#rm -rf /rocksdb-data/data-bench-col30-cg5


# for workload cg10
# k = 2
#./benchmark workloads/workload-col30-cg10 load 2 ~/rocksdb-data/data-bench-col30-cg10 true >> log-cg10
#sleep 30
#./benchmark workloads/workload-col30-cg10 compact 2 ~/rocksdb-data/data-bench-col30-cg10 false >> log-cg10
#sleep 30
# k = 2
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-cg10 run 2 ~/rocksdb-data/data-bench-col30-cg10 true >> log-cg10
#sleep 30
#rm -rf /rocksdb-data/data-bench-col30-cg10

# for workload cg15
# k = 2
#./benchmark workloads/workload-col30-cg15 load 2 ~/rocksdb-data/data-bench-col30-cg15 true >> log-cg15
#sleep 30
#./benchmark workloads/workload-col30-cg15 compact 2 ~/rocksdb-data/data-bench-col30-cg15 false >> log-cg15
#sleep 30
# k = 2
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-cg15 run 2 ~/rocksdb-data/data-bench-col30-cg15 true >> log-cg15
#sleep 30
#rm -rf /rocksdb-data/data-bench-col30-cg15

# for workload cg30
# k = 2
#./benchmark workloads/workload-col30-cg30 load 2 ~/rocksdb-data/data-bench-col30-cg30 true >> log-cg30
#sleep 30
#./benchmark workloads/workload-col30-cg30 compact 2 ~/rocksdb-data/data-bench-col30-cg30 false >> log-cg30
#sleep 30
# k = 2
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-cg30 run 2 ~/rocksdb-data/data-bench-col30-cg30 true >> log-cg30
#sleep 30
#rm -rf /rocksdb-data/data-bench-col30-cg30

# for distribution of keys based on time
#./benchmark workloads/workload-col30-Dtime load 2 ~/rocksdb-data/data-bench-col30-Dtime false >> log-Dtime

#######################
# Different layouts
#######################

#rm -rf ~/rocksdb-data/data-bench-col30-Dcg1/
#cp -r ~/rocksdb-data/data-bench-col30-Dcg1-gold/ ~/rocksdb-data/data-bench-col30-Dcg1/
#sleep 20
# for workload Dcg1
#./benchmark workloads/workload-col30-Dcg1 load 2 ~/rocksdb-data/data-bench-col30-Dcg1 true >> log-Dcg1
#sleep 30
#./benchmark workloads/workload-col30-Dcg1 compact 2 ~/rocksdb-data/data-bench-col30-Dcg1 false >> log-Dcg1
#sleep 30
# for Dcg1
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Dcg1 run 2 ~/rocksdb-data/data-bench-col30-Dcg1 false >> log-Dcg1
#sleep 30
#rm -rf ~/rocksdb-data/data-bench-col30-Dcg1/

#rm -rf ~/rocksdb-data/data-bench-col30-Dcg2/
#cp -r ~/rocksdb-data/data-bench-col30-Dcg2-gold/ ~/rocksdb-data/data-bench-col30-Dcg2/
#sleep 20
# for workload Dcg2
#./benchmark workloads/workload-col30-Dcg2 load 2 ~/rocksdb-data/data-bench-col30-Dcg2 true >> log-Dcg2
#sleep 30
#./benchmark workloads/workload-col30-Dcg2 compact 2 ~/rocksdb-data/data-bench-col30-Dcg2 false >> log-Dcg2
#sleep 30
# for Dcg2
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Dcg2 run 2 ~/rocksdb-data/data-bench-col30-Dcg2 false >> log-Dcg2
#sleep 30
#rm -rf ~/rocksdb-data/data-bench-col30-Dcg2/

#sleep 20
# for workload Dcg5
#./benchmark workloads/workload-col30-Dcg5 load 2 ~/rocksdb-data/data-bench-col30-Dcg5 true >> log-Dcg5
#sleep 30
#./benchmark workloads/workload-col30-Dcg5 compact 2 ~/rocksdb-data/data-bench-col30-Dcg5 false >> log-Dcg5
#sleep 30
# for Dcg5
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Dcg5 run 2 ~/rocksdb-data/data-bench-col30-Dcg5 false >> log-Dcg5
#sleep 30
#cp -r ~/rocksdb-data/data-bench-col30-Dcg5/ ~/rocksdb-data/data-bench-col30-Dcg5-gold/ 

#rm -rf ~/rocksdb-data/data-bench-col30-Dcg10/
#cp -r ~/rocksdb-data/data-bench-col30-Dcg10-gold/ ~/rocksdb-data/data-bench-col30-Dcg10/
#sleep 20
# for workload Dcg10
#./benchmark workloads/workload-col30-Dcg10 load 2 ~/rocksdb-data/data-bench-col30-Dcg10 true >> log-Dcg10
#sleep 30
#./benchmark workloads/workload-col30-Dcg10 compact 2 ~/rocksdb-data/data-bench-col30-Dcg10 false >> log-Dcg10
#sleep 30
# for Dcg10
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Dcg10 run 2 ~/rocksdb-data/data-bench-col30-Dcg10 false >> log-Dcg10
#sleep 30

#rm -rf ~/rocksdb-data/data-bench-col30-Dcg15/
#cp -r ~/rocksdb-data/data-bench-col30-Dcg15-gold/ ~/rocksdb-data/data-bench-col30-Dcg15/
#sleep 20
# for workload Dcg15
#./benchmark workloads/workload-col30-Dcg15 load 2 ~/rocksdb-data/data-bench-col30-Dcg15 true >> log-Dcg15
#sleep 30
#./benchmark workloads/workload-col30-Dcg15 compact 2 ~/rocksdb-data/data-bench-col30-Dcg15 false >> log-Dcg15
#sleep 30
# for Dcg15
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Dcg15 run 2 ~/rocksdb-data/data-bench-col30-Dcg15 false >> log-Dcg15
#sleep 30
#cp -r ~/rocksdb-data/data-bench-col30-Dcg15/ ~/rocksdb-data/data-bench-col30-Dcg15-gold/

#rm -rf ~/rocksdb-data/data-bench-col30-Dcg30/
#cp -r ~/rocksdb-data/data-bench-col30-Dcg30-gold ~/rocksdb-data/data-bench-col30-Dcg30
#sleep 20
# for workload Dcg30
#./benchmark workloads/workload-col30-Dcg30 load 2 ~/rocksdb-data/data-bench-col30-Dcg30 true >> log-Dcg30
#sleep 30
#./benchmark workloads/workload-col30-Dcg30 compact 2 ~/rocksdb-data/data-bench-col30-Dcg30 false >> log-Dcg30
#sleep 30
# for Dcg30
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Dcg30 run 2 ~/rocksdb-data/data-bench-col30-Dcg30 false >> log-Dcg30
#sleep 30


#rm -rf ~/rocksdb-data/data-bench-col30-D1/
#cp -r ~/rocksdb-data/data-bench-col30-D1-gold ~/rocksdb-data/data-bench-col30-D1
#sleep 20
# for workload D1
#./benchmark workloads/workload-col30-D1 load 2 ~/rocksdb-data/data-bench-col30-D1 true >> log-D1
#sleep 30
#./benchmark workloads/workload-col30-D1 compact 2 ~/rocksdb-data/data-bench-col30-D1 false >> log-D1
#sleep 30
# for D1
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-D1 run 2 ~/rocksdb-data/data-bench-col30-D1 false >> log-D1
#sleep 30
#rm -rf /rocksdb-data/data-bench-col30-D1
#cp -r ~/rocksdb-data/data-bench-col30-D1 ~/rocksdb-data/data-bench-col30-D1-gold

#rm -rf ~/rocksdb-data/data-bench-col30-D2/
#cp -r ~/rocksdb-data/data-bench-col30-D2-gold ~/rocksdb-data/data-bench-col30-D2
# for workload D2
#./benchmark workloads/workload-col30-D2 load 2 ~/rocksdb-data/data-bench-col30-D2 true >> log-D2
#sleep 30
#./benchmark workloads/workload-col30-D2 compact 2 ~/rocksdb-data/data-bench-col30-D2 false >> log-D2
#sleep 30
# for D2
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-D2 run 2 ~/rocksdb-data/data-bench-col30-D2 false >> log-D2
#sleep 30
#rm -rf /rocksdb-data/data-bench-col30-D2
#cp -r ~/rocksdb-data/data-bench-col30-D2 ~/rocksdb-data/data-bench-col30-D2-gold

#rm -rf ~/rocksdb-data/data-bench-col30-D3/
#cp -r ~/rocksdb-data/data-bench-col30-D3-gold/ ~/rocksdb-data/data-bench-col30-D3/
# for workload D3
#./benchmark workloads/workload-col30-D3 load 2 ~/rocksdb-data/data-bench-col30-D3 true >> log-D3
#sleep 30
#./benchmark workloads/workload-col30-D3 compact 2 ~/rocksdb-data/data-bench-col30-D3 false >> log-D3
#sleep 30
# for D3
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-D3 run 2 ~/rocksdb-data/data-bench-col30-D3 false >> log-D3
#sleep 30
#rm -rf /rocksdb-data/data-bench-col30-D3
#cp -r ~/rocksdb-data/data-bench-col30-D3 ~/rocksdb-data/data-bench-col30-D3-gold


#rm -rf ~/rocksdb-data/data-bench-col30-D4/
#cp -r ~/rocksdb-data/data-bench-col30-D4-gold ~/rocksdb-data/data-bench-col30-D4
#sleep 20
# for workload D4
#./benchmark workloads/workload-col30-D4 load 2 ~/rocksdb-data/data-bench-col30-D4 true >> log-D4
#sleep 30
#./benchmark workloads/workload-col30-D4 compact 2 ~/rocksdb-data/data-bench-col30-D4 false >> log-D4
#sleep 30
# for D4
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-D4 run 2 ~/rocksdb-data/data-bench-col30-D4 false >> log-D4
#sleep 30
#cp -r ~/rocksdb-data/data-bench-col30-D4 ~/rocksdb-data/data-bench-col30-D4-gold


#rm -rf ~/rocksdb-data/data-bench-col30-D5/
#cp -r ~/rocksdb-data/data-bench-col30-D5-gold ~/rocksdb-data/data-bench-col30-D5
#sleep 20
# for workload D5
#./benchmark workloads/workload-col30-D5 load 2 ~/rocksdb-data/data-bench-col30-D5 true >> log-D5
#sleep 30
#./benchmark workloads/workload-col30-D5 compact 2 ~/rocksdb-data/data-bench-col30-D5 false >> log-D5
#sleep 30
# for D5
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-D5 run 2 ~/rocksdb-data/data-bench-col30-D5 false >> log-D5
#sleep 30
#cp -r ~/rocksdb-data/data-bench-col30-D5 ~/rocksdb-data/data-bench-col30-D5-gold

#rm -rf ~/rocksdb-data/data-bench-col30-D6/
#cp -r ~/rocksdb-data/data-bench-col30-D6-gold ~/rocksdb-data/data-bench-col30-D6
#sleep 20
# for workload D6
#./benchmark workloads/workload-col30-D6 load 2 ~/rocksdb-data/data-bench-col30-D6 true >> log-D6
#sleep 30
#./benchmark workloads/workload-col30-D6 compact 2 ~/rocksdb-data/data-bench-col30-D6 false >> log-D6
#sleep 30
# for D6
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-D6 run 2 ~/rocksdb-data/data-bench-col30-D6 false >> log-D6
#sleep 30
#cp -r ~/rocksdb-data/data-bench-col30-D6 ~/rocksdb-data/data-bench-col30-D6-gold


# for workload Dt
#./benchmark workloads/workload-col30-Dt load 2 ~/rocksdb-data/data-bench-col30-Dt true >> log-Dt
#sleep 30
#./benchmark workloads/workload-col30-Dt compact 2 ~/rocksdb-data/data-bench-col30-Dt false >> log-Dt
#sleep 30
# for Dt
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Dt run 2 ~/rocksdb-data/data-bench-col30-Dt false >> log-Dt
#sleep 30
#rm -rf /rocksdb-data/data-bench-col30-Dt

# for workload Dcg1
# k = 2
#./benchmark workloads/workload-col30-Dcg1 load 2 ~/rocksdb-data/data-bench-col30-Dcg1 true >> log-Dcg1
#sleep 30
#./benchmark workloads/workload-col30-Dcg1 compact 2 ~/rocksdb-data/data-bench-col30-Dcg1 false >> log-Dcg1
#sleep 30
# k = 2
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Dcg1 run 2 ~/rocksdb-data/data-bench-col30-Dcg1 true >> log-Dcg1
#sleep 30
#rm -rf /rocksdb-data/data-bench-col30-cg30

#rm -rf ~/rocksdb-data/data-bench-col30-D7/
#sleep 20
# for workload D7
#./benchmark workloads/workload-col30-D7 load 2 ~/rocksdb-data/data-bench-col30-D7 true >> log-D7
#sleep 30
#./benchmark workloads/workload-col30-D7 compact 2 ~/rocksdb-data/data-bench-col30-D7 false >> log-D7
#sleep 30
# for D7
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-D7 run 2 ~/rocksdb-data/data-bench-col30-D7 false >> log-D7
#sleep 30
#cp -r ~/rocksdb-data/data-bench-col30-D7 ~/rocksdb-data/data-bench-col30-D7-gold

#rm -rf ~/rocksdb-data/data-bench-col30-D8/
#sleep 20
# for workload D8
#./benchmark workloads/workload-col30-D8 load 2 ~/rocksdb-data/data-bench-col30-D8 true >> log-D8
#sleep 30
#./benchmark workloads/workload-col30-D8 compact 2 ~/rocksdb-data/data-bench-col30-D8 false >> log-D8
#sleep 30
# for D8
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-D8 run 2 ~/rocksdb-data/data-bench-col30-D8 false >> log-D8
#sleep 30
#cp -r ~/rocksdb-data/data-bench-col30-D8 ~/rocksdb-data/data-bench-col30-D8-gold

#rm -rf ~/rocksdb-data/data-bench-col30-D9/
#sleep 20
# for workload D9
#./benchmark workloads/workload-col30-D9 load 2 ~/rocksdb-data/data-bench-col30-D9 true >> log-D9
#sleep 30
#./benchmark workloads/workload-col30-D9 compact 2 ~/rocksdb-data/data-bench-col30-D9 false >> log-D9
#sleep 30
# for D9
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-D9 run 2 ~/rocksdb-data/data-bench-col30-D9 false >> log-D9
#sleep 30
#cp -r ~/rocksdb-data/data-bench-col30-D9 ~/rocksdb-data/data-bench-col30-D9-gold

#rm -rf ~/rocksdb-data/data-bench-col30-D8-cT/
#cp -r ~/rocksdb-data/data-bench-col30-D8-cT-gold ~/rocksdb-data/data-bench-col30-D8-cT
#sleep 20
# for workload D8-cT
#./benchmark workloads/workload-col30-D8 load 2 ~/rocksdb-data/data-bench-col30-D8-cT true >> log-D8-cT
#sleep 30
#./benchmark workloads/workload-col30-D8 compact 2 ~/rocksdb-data/data-bench-col30-D8-cT false >> log-D8-cT
#sleep 30
# for D8-cT
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-D7-norm95 run 2 ~/rocksdb-data/data-bench-col30-D7-norm95 false >> log-D7-norm
#sleep 30
#
#./benchmark workloads/workload-col30-D7-norm60 run 2 ~/rocksdb-data/data-bench-col30-D7-norm60 false >> log-D7-norm
#sleep 30

#./benchmark workloads/workload-col30-D7-norm30 run 2 ~/rocksdb-data/data-bench-col30-D7-norm30 false >> log-D7-norm
#sleep 30



#./benchmark workloads/workload-col30-D8-norm95 run 2 ~/rocksdb-data/data-bench-col30-D8-norm95 false >> log-D8-norm
#sleep 30
#
#./benchmark workloads/workload-col30-D8-norm60 run 2 ~/rocksdb-data/data-bench-col30-D8-norm60 false >> log-D8-norm
#sleep 30
#
#./benchmark workloads/workload-col30-D8-norm30 run 2 ~/rocksdb-data/data-bench-col30-D8-norm30 false >> log-D8-norm
#sleep 30
#
#
#./benchmark workloads/workload-col30-D9-norm95 run 2 ~/rocksdb-data/data-bench-col30-D9-norm95 false >> log-D9-norm
#sleep 30
#
#./benchmark workloads/workload-col30-D9-norm60 run 2 ~/rocksdb-data/data-bench-col30-D9-norm60 false >> log-D9-norm
#sleep 30
#
#./benchmark workloads/workload-col30-D9-norm30 run 2 ~/rocksdb-data/data-bench-col30-D9-norm30 false >> log-D9-norm
#sleep 30

#./benchmark workloads/workload-col30-D7 run 2 ~/rocksdb-data/data-bench-col30-D7-norm95 true >> log-D7-norm
#sleep 30
#./benchmark workloads/workload-col30-D7 run 2 ~/rocksdb-data/data-bench-col30-D7-norm60 true >> log-D7-norm
#sleep 30
#./benchmark workloads/workload-col30-D7 run 2 ~/rocksdb-data/data-bench-col30-D7-norm30 true >> log-D7-norm
#sleep 30
#
#./benchmark workloads/workload-col30-D8 run 2 ~/rocksdb-data/data-bench-col30-D8-norm95 true >> log-D8-norm
#sleep 30
#./benchmark workloads/workload-col30-D8 run 2 ~/rocksdb-data/data-bench-col30-D8-norm60 true >> log-D8-norm
#sleep 30
#./benchmark workloads/workload-col30-D8 run 2 ~/rocksdb-data/data-bench-col30-D8-norm30 true >> log-D8-norm
#sleep 30
#
#./benchmark workloads/workload-col30-D9 run 2 ~/rocksdb-data/data-bench-col30-D9-norm95 true >> log-D9-norm
#sleep 30
#./benchmark workloads/workload-col30-D9 run 2 ~/rocksdb-data/data-bench-col30-D9-norm60 true >> log-D9-norm
#sleep 30
#./benchmark workloads/workload-col30-D9 run 2 ~/rocksdb-data/data-bench-col30-D9-norm30 true >> log-D9-norm
#sleep 30

#./benchmark workloads/workload-col30-D7 load 2 ~/rocksdb-data/data-bench-col30-D7-scan true >> log-D7-scan
#sleep 30
#./benchmark workloads/workload-col30-D7 compact 2 ~/rocksdb-data/data-bench-col30-D7-scan false >> log-D7-scan
#sleep 30
#./benchmark workloads/workload-col30-D7 run 2 ~/rocksdb-data/data-bench-col30-D7-scan true >> log-D7-scan
#sleep 30

#./benchmark workloads/workload-col30-D8 load 2 ~/rocksdb-data/data-bench-col30-D8-scan true >> log-D8-scan
#sleep 30
#./benchmark workloads/workload-col30-D8 compact 2 ~/rocksdb-data/data-bench-col30-D8-scan false >> log-D8-scan
#sleep 30
#./benchmark workloads/workload-col30-D8 run 2 ~/rocksdb-data/data-bench-col30-D8-scan true >> log-D8-scan
#sleep 30


#./benchmark workloads/workload-col30-D9 load 2 ~/rocksdb-data/data-bench-col30-D9-scan true >> log-D9-scan
#sleep 30
#./benchmark workloads/workload-col30-D9 compact 2 ~/rocksdb-data/data-bench-col30-D9-scan false >> log-D9-scan
#sleep 30
#./benchmark workloads/workload-col30-D9 run 2 ~/rocksdb-data/data-bench-col30-D9-scan true >> log-D9-scan


###################
## compaction exp
###################
#./benchmark workloads/workload-col30-D10-comp load 2 ~/rocksdb-data/data-bench-col30-D10 true >> log-D10-comp
#sleep 30
#./benchmark workloads/workload-col30-D10-comp compact 2 ~/rocksdb-data/data-bench-col30-D10 false >> log-D10-comp
#sleep 30
#
#./benchmark workloads/workload-col30-D11-comp load 2 ~/rocksdb-data/data-bench-col30-D11 true >> log-D11-comp
#sleep 30
#./benchmark workloads/workload-col30-D11-comp compact 2 ~/rocksdb-data/data-bench-col30-D11 false >> log-D11-comp
#sleep 30
#
#./benchmark workloads/workload-col30-D12-comp load 2 ~/rocksdb-data/data-bench-col30-D12 true >> log-D12-comp
#sleep 30
#./benchmark workloads/workload-col30-D12-comp compact 2 ~/rocksdb-data/data-bench-col30-D12 false >> log-D12-comp
#sleep 30
#
#./benchmark workloads/workload-col30-D13-comp load 2 ~/rocksdb-data/data-bench-col30-D13 true >> log-D13-comp
#sleep 30
#./benchmark workloads/workload-col30-D13-comp compact 2 ~/rocksdb-data/data-bench-col30-D13 false >> log-D13-comp
#sleep 30

#./benchmark workloads/workload-col30-D15-comp load 2 ~/rocksdb-data/data-bench-col30-D15 true >> log-D15-comp
#sleep 30
#./benchmark workloads/workload-col30-D15-comp compact 2 ~/rocksdb-data/data-bench-col30-D15 false >> log-D15-comp
#sleep 30
#
#./benchmark workloads/workload-col30-D16-comp load 2 ~/rocksdb-data/data-bench-col30-D16 true >> log-D16-comp
#sleep 30
#./benchmark workloads/workload-col30-D16-comp compact 2 ~/rocksdb-data/data-bench-col30-D16 false >> log-D16-comp
#sleep 30
#
#./benchmark workloads/workload-col30-D17-comp load 2 ~/rocksdb-data/data-bench-col30-D17 true >> log-D17-comp
#sleep 30
#./benchmark workloads/workload-col30-D17-comp compact 2 ~/rocksdb-data/data-bench-col30-D17 false >> log-D17-comp
#sleep 30
#
#./benchmark workloads/workload-col30-D18-comp load 2 ~/rocksdb-data/data-bench-col30-D18 true >> log-D18-comp
#sleep 30
#./benchmark workloads/workload-col30-D18-comp compact 2 ~/rocksdb-data/data-bench-col30-D18 false >> log-D18-comp
#sleep 30


### threads ###
#./benchmark workloads/workload-col30-D18-comp-t1 load 2 ~/rocksdb-data/data-bench-col30-D18-t1 true >> log-D18-comp-t
#sleep 30
#cp -r ~/rocksdb-data/data-bench-col30-D18-t1 ~/rocksdb-data/data-bench-col30-D18-t-gold

#cp -r ~/rocksdb-data/data-bench-col30-D18-t-gold ~/rocksdb-data/data-bench-col30-D18-t1
#./benchmark workloads/workload-col30-D18-comp-t1 compact 2 ~/rocksdb-data/data-bench-col30-D18-t1 false >> log-D18-comp-t
#sleep 30

#cp -r ~/rocksdb-data/data-bench-col30-D18-t-gold ~/rocksdb-data/data-bench-col30-D18-t2
#./benchmark workloads/workload-col30-D18-comp-t2 compact 2 ~/rocksdb-data/data-bench-col30-D18-t2 false >> log-D18-comp-t
#sleep 30

#cp -r ~/rocksdb-data/data-bench-col30-D18-t-gold ~/rocksdb-data/data-bench-col30-D18-t3
#./benchmark workloads/workload-col30-D18-comp-t3 compact 2 ~/rocksdb-data/data-bench-col30-D18-t3 false >> log-D18-comp-t
#sleep 30
#
#cp -r ~/rocksdb-data/data-bench-col30-D18-t-gold ~/rocksdb-data/data-bench-col30-D18-t4
#./benchmark workloads/workload-col30-D18-comp-t4 compact 2 ~/rocksdb-data/data-bench-col30-D18-t4 false >> log-D18-comp-t
#sleep 30

#cp -r ~/rocksdb-data/data-bench-col30-D18-t-gold ~/rocksdb-data/data-bench-col30-D18-t5
#./benchmark workloads/workload-col30-D18-comp-t5 compact 2 ~/rocksdb-data/data-bench-col30-D18-t5 false >> log-D18-comp-t
#sleep 30

#cp -r ~/rocksdb-data/data-bench-col30-D18-t-gold ~/rocksdb-data/data-bench-col30-D18-t6
#./benchmark workloads/workload-col30-D18-comp-t6 compact 2 ~/rocksdb-data/data-bench-col30-D18-t6 false >> log-D18-comp-t
#sleep 30

######################
# EXP-4 MIXED WORKLAOD
######################
#./benchmark workloads/workload-col30-Dcg30 run 2 ~/rocksdb-data/data-bench-col30-Dcg30-exp4 false >> log-Dcg30-exp4
#sleep 30

#./benchmark workloads/workload-col30-D7 run 2 ~/rocksdb-data/data-bench-col30-D7-exp4-gold true >> log-D7-exp4
#sleep 30
#
#./benchmark workloads/workload-col30-D9 run 2 ~/rocksdb-data/data-bench-col30-D9-exp4-gold true >> log-D9-exp4
#sleep 30
#
#./benchmark workloads/workload-col30-D8 run 2 ~/rocksdb-data/data-bench-col30-D8-exp4-gold true >> log-D8-exp4
#sleep 30

#./benchmark workloads/workload-col30-D19 run 2 ~/rocksdb-data/data-bench-col30-D19-exp4 true >> log-D19-exp4
#sleep 30
#
#./benchmark workloads/workload-col30-Dcg2 run 2 ~/rocksdb-data/data-bench-col30-Dcg2-gold true >> log-Dcg2-exp4
#sleep 30
#
#./benchmark workloads/workload-col30-Dcg5 run 2 ~/rocksdb-data/data-bench-col30-Dcg5-gold true >> log-Dcg5-exp4
#sleep 30
#
#./benchmark workloads/workload-col30-Dcg10 run 2 ~/rocksdb-data/data-bench-col30-Dcg10-gold true >> log-Dcg10-exp4
#sleep 30
#
#./benchmark workloads/workload-col30-Dcg15 run 2 ~/rocksdb-data/data-bench-col30-Dcg15-gold true >> log-Dcg15-exp4
#sleep 30
#
#./benchmark workloads/workload-col30-Dcg1 run 2 ~/rocksdb-data/data-bench-col30-Dcg1-gold true >> log-Dcg1-exp4
#sleep 30

#./benchmark workloads/workload-col30-D19 compact 2 ~/rocksdb-data/data-bench-col30-D19-exp4 false >> log-D19-exp4
#sleep 30

#./benchmark workloads/workload-col30-D9 compact 2 ~/rocksdb-data/data-bench-col30-D9-exp4 false >> log-D9-exp4
#sleep 30

#./benchmark workloads/workload-col30-D7 compact 2 ~/rocksdb-data/data-bench-col30-D7-exp4 false >> log-D7-exp4
#sleep 30
#
#./benchmark workloads/workload-col30-D8 compact 2 ~/rocksdb-data/data-bench-col30-D8-exp4 false >> log-D8-exp4
#sleep 30
#
#./benchmark workloads/workload-col30-Dcg2 compact 2 ~/rocksdb-data/data-bench-col30-Dcg2-exp4 false >> log-Dcg2-exp4
#sleep 30
#
#./benchmark workloads/workload-col30-Dcg5 compact 2 ~/rocksdb-data/data-bench-col30-Dcg5-exp4 false >> log-Dcg5-exp4
#sleep 30
#
#./benchmark workloads/workload-col30-Dcg10 compact 2 ~/rocksdb-data/data-bench-col30-Dcg10-exp4 false >> log-Dcg10-exp4
#sleep 30

##################
# STRESS TEST
##################

# SCAN
#for offset in 0 2 4 6 8 10 12 14 16 18 20 22
#do
#	./benchmark workloads/workload-col30-D19 run $offset ~/rocksdb-data/data-bench-col30-D19-exp4 true >> log-D19-exp4
#	sleep 30
#done

# READ
for offset in 0.0 0.1 0.2 0.3 0.4 0.5 0.6
do
	#rm -rf ~/rocksdb-data/data-bench-col30-D19-read-stress
	#./benchmark workloads/workload-col30-D19-read-stress run $offset ~/rocksdb-data/data-bench-col30-D19-read-stress false >> log-D19-read-stress
	#sleep 30

	rm -rf ~/rocksdb-data/data-bench-col30-Dcg1-read-stress
	./benchmark workloads/workload-col30-Dcg1-read-stress run $offset ~/rocksdb-data/data-bench-col30-Dcg1-read-stress false >> log-Dcg1-read-stress
	sleep 30

	rm -rf ~/rocksdb-data/data-bench-col30-Dcg2-read-stress
	./benchmark workloads/workload-col30-Dcg2-read-stress run $offset ~/rocksdb-data/data-bench-col30-Dcg2-read-stress false >> log-Dcg2-read-stress
	sleep 30

	rm -rf ~/rocksdb-data/data-bench-col30-Dcg5-read-stress
	./benchmark workloads/workload-col30-Dcg5-read-stress run $offset ~/rocksdb-data/data-bench-col30-Dcg5-read-stress false >> log-Dcg5-read-stress
	sleep 30

	rm -rf ~/rocksdb-data/data-bench-col30-Dcg10-read-stress
	./benchmark workloads/workload-col30-Dcg10-read-stress run $offset ~/rocksdb-data/data-bench-col30-Dcg10-read-stress false >> log-Dcg10-read-stress
	sleep 30

	rm -rf ~/rocksdb-data/data-bench-col30-Dcg30-read-stress
	./benchmark workloads/workload-col30-Dcg30-read-stress run $offset ~/rocksdb-data/data-bench-col30-Dcg30-read-stress false >> log-Dcg30-read-stress
	sleep 30

done



##################
# MICRO BENCHMARK
##################
#rm -rf ~/rocksdb-data/data-bench-col30-cg1/
#cp -r ~/rocksdb-data/data-bench-col30-Dcg1-gold/ ~/rocksdb-data/data-bench-col30-cg1/
#./benchmark workloads/workload-col30-cg1 run 2 ~/rocksdb-data/data-bench-col30-cg1 true >> log-cg1
#sleep 30

#rm -rf ~/rocksdb-data/data-bench-col30-cg2/
#cp -r ~/rocksdb-data/data-bench-col30-Dcg2-gold/ ~/rocksdb-data/data-bench-col30-cg2/
#
#rm -rf ~/rocksdb-data/data-bench-col30-cg5/
#cp -r ~/rocksdb-data/data-bench-col30-Dcg5-gold/ ~/rocksdb-data/data-bench-col30-cg5/
#
#rm -rf ~/rocksdb-data/data-bench-col30-cg10/
#cp -r ~/rocksdb-data/data-bench-col30-Dcg10-gold/ ~/rocksdb-data/data-bench-col30-cg10/
#
#rm -rf ~/rocksdb-data/data-bench-col30-cg15/
#cp -r ~/rocksdb-data/data-bench-col30-Dcg15-gold/ ~/rocksdb-data/data-bench-col30-cg15/
#
#rm -rf ~/rocksdb-data/data-bench-col30-cg30/
#cp -r ~/rocksdb-data/data-bench-col30-Dcg30-gold/ ~/rocksdb-data/data-bench-col30-cg30/

#for mode in read
#do
#for cols in 30
#do
#for cg in 1 2 5 10 15 30
#do
#	echo "./benchmark workloads/workload-col30-cg$cg-$mode$cols run 2 ~/rocksdb-data/data-bench-col30-cg$cg true"
#	./benchmark workloads/workload-col30-cg$cg-$mode$cols run 2 ~/rocksdb-data/data-bench-col30-cg$cg true >> log-cg$cg
#	sleep 20
#done
#done
#done

 ##################
## READ WORKLOADS
###################
 #rm -rf ~/rocksdb-data/data-bench-col30-D1/
##cp -r ~/rocksdb-data/data-bench-col30-D1-gold/ ~/rocksdb-data/data-bench-col30-D1/
##echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-D1-R1 run 2 ~/rocksdb-data/data-bench-col30-D1 false >> log-D1-R1
#sleep 30

#rm -rf ~/rocksdb-data/data-bench-col30-D1/
#cp -r ~/rocksdb-data/data-bench-col30-D1-gold/ ~/rocksdb-data/data-bench-col30-D1/
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-D1-R2 run 2 ~/rocksdb-data/data-bench-col30-D1 false >> log-D1-R2
#sleep 30

#rm -rf ~/rocksdb-data/data-bench-col30-D1/
#cp -r ~/rocksdb-data/data-bench-col30-D1-gold/ ~/rocksdb-data/data-bench-col30-D1/
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-D1-R3 run 2 ~/rocksdb-data/data-bench-col30-D1 false >> log-D1-R3
#sleep 30

#rm -rf ~/rocksdb-data/data-bench-col30-D1/
#cp -r ~/rocksdb-data/data-bench-col30-D1-gold/ ~/rocksdb-data/data-bench-col30-D1/
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-D1-R4 run 2 ~/rocksdb-data/data-bench-col30-D1 false >> log-D1-R4
#sleep 30
#
#rm -rf ~/rocksdb-data/data-bench-col30-D1/
#cp -r ~/rocksdb-data/data-bench-col30-D1-gold/ ~/rocksdb-data/data-bench-col30-D1/
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-D1-R5 run 2 ~/rocksdb-data/data-bench-col30-D1 false >> log-D1-R5
#sleep 30


# for workload w1
# k = 2
#./benchmark workloads/workload-col30-w1 load 2 ~/rocksdb-data/data-bench-k2-col30-w1 true >> log-k2
#sleep 30
#./benchmark workloads/workload-col30-w1 compact 2 ~/rocksdb-data/data-bench-k2-col30-w1 false >> log-k2
#sleep 30
# k = 2
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w1 run 2 ~/rocksdb-data/data-bench-k2-col30-w1 false >> log-k2
#sleep 30
#rm -rf /rocksdb-data/data-bench-k2-col30-w1

## k = 3
#./benchmark workloads/workload-col30-w1 load 3 /rocksdb-data/data-bench-k3-col30-w1 true >> log-k3
#sleep 30
#./benchmark workloads/workload-col30-w1 compact 3 /rocksdb-data/data-bench-k3-col30-w1 false >> log-k3
#sleep 30
## k = 3
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w1 run 3 /rocksdb-data/data-bench-k3-col30-w1 false >> log-k3
#sleep 30
#rm -rf /rocksdb-data/data-bench-k3-col30-w1

# k = 4
#./benchmark workloads/workload-col30-w1 load 4 /rocksdb-data/data-bench-k4-col30-w1 true >> log-k4
#sleep 30
#./benchmark workloads/workload-col30-w1 compact 4 /rocksdb-data/data-bench-k4-col30-w1 false >> log-k4
#sleep 30
## k = 4
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w1 run 4 /rocksdb-data/data-bench-k4-col30-w1 false >> log-k4
#sleep 30
#rm -rf /rocksdb-data/data-bench-k4-col30-w1/*.sst

## k = 5
#./benchmark workloads/workload-col30-w1 load 5 /rocksdb-data/data-bench-k5-col30-w1 true >> log-k5
#sleep 30
#./benchmark workloads/workload-col30-w1 compact 5 /rocksdb-data/data-bench-k5-col30-w1 false >> log-k5
#sleep 30
## k = 5
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w1 run 5 /rocksdb-data/data-bench-k5-col30-w1 false >> log-k5
#sleep 30
#rm -rf /rocksdb-data/data-bench-k5-col30-w1

# k = 6
#./benchmark workloads/workload-col30-w1 load 6 /rocksdb-data/data-bench-k6-col30-w1 true >> log-k6
#sleep 30
#./benchmark workloads/workload-col30-w1 compact 6 /rocksdb-data/data-bench-k6-col30-w1 false >> log-k6
#sleep 30
## k = 6
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w1 run 6 /rocksdb-data/data-bench-k6-col30-w1 false >> log-k6
#sleep 30
##rm -rf /rocksdb-data/data-bench-k6-col30-w1

## k = 7
#./benchmark workloads/workload-col30-w1 load 7 /rocksdb-data/data-bench-k7-col30-w1 true >> log-k7
#sleep 30
#./benchmark workloads/workload-col30-w1 compact 7 /rocksdb-data/data-bench-k7-col30-w1 false >> log-k7
#sleep 30
## k = 7
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w1 run 7 /rocksdb-data/data-bench-k7-col30-w1 false >> log-k7
#sleep 30
#rm -rf /rocksdb-data/data-bench-k7-col30-w1

## k = 8
#./benchmark workloads/workload-col30-w1 load 8 /rocksdb-data/data-bench-k8-col30-w1 true >> log-k8
#sleep 30
#./benchmark workloads/workload-col30-w1 compact 8 /rocksdb-data/data-bench-k8-col30-w1 false >> log-k8
#sleep 30
## k = 8
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w1 run 8 /rocksdb-data/data-bench-k8-col30-w1 false >> log-k8
#sleep 30
##rm -rf /rocksdb-data/data-bench-k8-col30-w1


## for workload w2
## k = 2
#./benchmark workloads/workload-col30-w2 load 2 /rocksdb-data/data-bench-k2-col30-w2 true >> log-k2
#sleep 30
#./benchmark workloads/workload-col30-w2 compact 2 /rocksdb-data/data-bench-k2-col30-w2 false >> log-k2
#sleep 30
## k = 2
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w2 run 2 /rocksdb-data/data-bench-k2-col30-w2 false >> log-k2
#sleep 30
#rm -rf /rocksdb-data/data-bench-k2-col30-w2
#
### k = 3
#./benchmark workloads/workload-col30-w2 load 3 /rocksdb-data/data-bench-k3-col30-w2 true >> log-k3
#sleep 30
#./benchmark workloads/workload-col30-w2 compact 3 /rocksdb-data/data-bench-k3-col30-w2 false >> log-k3
#sleep 30
## k = 3
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w2 run 3 /rocksdb-data/data-bench-k3-col30-w2 false >> log-k3
#sleep 30
#rm -rf /rocksdb-data/data-bench-k3-col30-w2
#
## k = 4
#./benchmark workloads/workload-col30-w2 load 4 /rocksdb-data/data-bench-k4-col30-w2 true >> log-k4
#sleep 30
#./benchmark workloads/workload-col30-w2 compact 4 /rocksdb-data/data-bench-k4-col30-w2 false >> log-k4
#sleep 30
## k = 4
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w2 run 4 /rocksdb-data/data-bench-k4-col30-w2 false >> log-k4
#sleep 30
#rm -rf /rocksdb-data/data-bench-k4-col30-w2
#
## k = 5
#./benchmark workloads/workload-col30-w2 load 5 /rocksdb-data/data-bench-k5-col30-w2 true >> log-k5
#sleep 30
#./benchmark workloads/workload-col30-w2 compact 5 /rocksdb-data/data-bench-k5-col30-w2 false >> log-k5
#sleep 30
## k = 5
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w2 run 5 /rocksdb-data/data-bench-k5-col30-w2 false >> log-k5
#sleep 30
#rm -rf /rocksdb-data/data-bench-k5-col30-w2
#
## k = 6
#./benchmark workloads/workload-col30-w2 load 6 /rocksdb-data/data-bench-k6-col30-w2 true >> log-k6
#sleep 30
#./benchmark workloads/workload-col30-w2 compact 6 /rocksdb-data/data-bench-k6-col30-w2 false >> log-k6
#sleep 30
## k = 6
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w2 run 6 /rocksdb-data/data-bench-k6-col30-w2 false >> log-k6
#sleep 30
#rm -rf /rocksdb-data/data-bench-k6-col30-w2
#
## k = 7
#./benchmark workloads/workload-col30-w2 load 7 /rocksdb-data/data-bench-k7-col30-w2 true >> log-k7
#sleep 30
#./benchmark workloads/workload-col30-w2 compact 7 /rocksdb-data/data-bench-k7-col30-w2 false >> log-k7
#sleep 30
## k = 7
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w2 run 7 /rocksdb-data/data-bench-k7-col30-w2 false >> log-k7
#sleep 30
#rm -rf /rocksdb-data/data-bench-k7-col30-w2
#
## k = 8
#./benchmark workloads/workload-col30-w2 load 8 /rocksdb-data/data-bench-k8-col30-w2 true >> log-k8
#sleep 30
#./benchmark workloads/workload-col30-w2 compact 8 /rocksdb-data/data-bench-k8-col30-w2 false >> log-k8
#sleep 30
## k = 8
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w2 run 8 /rocksdb-data/data-bench-k8-col30-w2 false >> log-k8
#sleep 30
#rm -rf /rocksdb-data/data-bench-k8-col30-w2
#
#
#
## for workload w3
## k = 2
#./benchmark workloads/workload-col30-w3 load 2 /rocksdb-data/data-bench-k2-col30-w3 true >> log-k2
#sleep 30
#./benchmark workloads/workload-col30-w3 compact 2 /rocksdb-data/data-bench-k2-col30-w3 false >> log-k2
#sleep 30
## k = 2
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w3 run 2 /rocksdb-data/data-bench-k2-col30-w3 false >> log-k2
#sleep 30
#rm -rf /rocksdb-data/data-bench-k2-col30-w3
#
### k = 3
#./benchmark workloads/workload-col30-w3 load 3 /rocksdb-data/data-bench-k3-col30-w3 true >> log-k3
#sleep 30
#./benchmark workloads/workload-col30-w3 compact 3 /rocksdb-data/data-bench-k3-col30-w3 false >> log-k3
#sleep 30
## k = 3
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w3 run 3 /rocksdb-data/data-bench-k3-col30-w3 false >> log-k3
#sleep 30
#rm -rf /rocksdb-data/data-bench-k3-col30-w3
#
## k = 4
#./benchmark workloads/workload-col30-w3 load 4 /rocksdb-data/data-bench-k4-col30-w3 true >> log-k4
#sleep 30
#./benchmark workloads/workload-col30-w3 compact 4 /rocksdb-data/data-bench-k4-col30-w3 false >> log-k4
#sleep 30
## k = 4
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w3 run 4 /rocksdb-data/data-bench-k4-col30-w3 false >> log-k4
#sleep 30
#rm -rf /rocksdb-data/data-bench-k4-col30-w3
#
## k = 5
#./benchmark workloads/workload-col30-w3 load 5 /rocksdb-data/data-bench-k5-col30-w3 true >> log-k5
#sleep 30
#./benchmark workloads/workload-col30-w3 compact 5 /rocksdb-data/data-bench-k5-col30-w3 false >> log-k5
#sleep 30
## k = 5
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w3 run 5 /rocksdb-data/data-bench-k5-col30-w3 false >> log-k5
#sleep 30
#rm -rf /rocksdb-data/data-bench-k5-col30-w3
#
## k = 6
#./benchmark workloads/workload-col30-w3 load 6 /rocksdb-data/data-bench-k6-col30-w3 true >> log-k6
#sleep 30
#./benchmark workloads/workload-col30-w3 compact 6 /rocksdb-data/data-bench-k6-col30-w3 false >> log-k6
#sleep 30
## k = 6
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w3 run 6 /rocksdb-data/data-bench-k6-col30-w3 false >> log-k6
#sleep 30
#rm -rf /rocksdb-data/data-bench-k6-col30-w3
#
## k = 7
#./benchmark workloads/workload-col30-w3 load 7 /rocksdb-data/data-bench-k7-col30-w3 true >> log-k7
#sleep 30
#./benchmark workloads/workload-col30-w3 compact 7 /rocksdb-data/data-bench-k7-col30-w3 false >> log-k7
#sleep 30
## k = 7
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w3 run 7 /rocksdb-data/data-bench-k7-col30-w3 false >> log-k7
#sleep 30
#rm -rf /rocksdb-data/data-bench-k7-col30-w3
#
## k = 8
#./benchmark workloads/workload-col30-w3 load 8 /rocksdb-data/data-bench-k8-col30-w3 true >> log-k8
#sleep 30
#./benchmark workloads/workload-col30-w3 compact 8 /rocksdb-data/data-bench-k8-col30-w3 false >> log-k8
#sleep 30
## k = 8
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-w3 run 8 /rocksdb-data/data-bench-k8-col30-w3 false >> log-k8
#sleep 30
#rm -rf /rocksdb-data/data-bench-k8-col30-w3

##################
## for workload lm2
## k = 2
#./benchmark workloads/workload-col30-lm2 load 2 /rocksdb-data/data-bench-k2-col30-lm2 true >> log-k2-lm2
#sleep 30
#./benchmark workloads/workload-col30-lm2 compact 2 /rocksdb-data/data-bench-k2-col30-lm2 false >> log-k2-lm2
#sleep 30
## k = 2
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-lm2 run 2 /rocksdb-data/data-bench-k2-col30-lm2 true >> log-k2-lm2
#sleep 30
#rm -rf /rocksdb-data/data-bench-k2-col30-lm2
#
## k = 8
#./benchmark workloads/workload-col30-lm2 load 8 /rocksdb-data/data-bench-k8-col30-lm2 true >> log-k8-lm2
#sleep 30
#./benchmark workloads/workload-col30-lm2 compact 8 /rocksdb-data/data-bench-k8-col30-lm2 false >> log-k8-lm2
#sleep 30
## k = 8
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-lm2 run 8 /rocksdb-data/data-bench-k8-col30-lm2 true >> log-k8-lm2
#sleep 30
#rm -rf /rocksdb-data/data-bench-k8-col30-lm2
#
#
## for workload lm3
## k = 2
#./benchmark workloads/workload-col30-lm3 load 2 /rocksdb-data/data-bench-k2-col30-lm3 true >> log-k2-lm3
#sleep 30
#./benchmark workloads/workload-col30-lm3 compact 2 /rocksdb-data/data-bench-k2-col30-lm3 false >> log-k2-lm3
#sleep 30
## k = 2
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-lm3 run 2 /rocksdb-data/data-bench-k2-col30-lm3 true >> log-k2-lm3
#sleep 30
#rm -rf /rocksdb-data/data-bench-k2-col30-lm3
#
## k = 7
#./benchmark workloads/workload-col30-lm3 load 7 /rocksdb-data/data-bench-k7-col30-lm3 true >> log-k7-lm3
#sleep 30
#./benchmark workloads/workload-col30-lm3 compact 7 /rocksdb-data/data-bench-k7-col30-lm3 false >> log-k7-lm3
#sleep 30
## k = 7
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-lm3 run 7 /rocksdb-data/data-bench-k7-col30-lm3 true >> log-k7-lm3
#sleep 30
#rm -rf /rocksdb-data/data-bench-k7-col30-lm3
#
#
## for workload lm4
## k = 2
#./benchmark workloads/workload-col30-lm4 load 2 /rocksdb-data/data-bench-k2-col30-lm4 true >> log-k2-lm4
#sleep 30
#./benchmark workloads/workload-col30-lm4 compact 2 /rocksdb-data/data-bench-k2-col30-lm4 false >> log-k2-lm4
#sleep 30
## k = 2
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-lm4 run 2 /rocksdb-data/data-bench-k2-col30-lm4 true >> log-k2-lm4
#sleep 30
#rm -rf /rocksdb-data/data-bench-k2-col30-lm4
#
## k = 6
#./benchmark workloads/workload-col30-lm4 load 6 /rocksdb-data/data-bench-k6-col30-lm4 true >> log-k6-lm4
#sleep 30
#./benchmark workloads/workload-col30-lm4 compact 6 /rocksdb-data/data-bench-k6-col30-lm4 false >> log-k6-lm4
#sleep 30
## k = 6
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-lm4 run 6 /rocksdb-data/data-bench-k6-col30-lm4 true >> log-k6-lm4
#sleep 30
#rm -rf /rocksdb-data/data-bench-k6-col30-lm4



################
# RUN WORKLOAD #
################

## for workload w1


# for workload w2

# for workload w3

# for workload lm2

# for workload lm3

# for workload lm4



##########
# UPDATE #
##########

# for latest updates
# load data 
#./benchmark workloads/workload-col30-update load 5 /rocksdb-data/data-bench-k5-col30-update-latest false >> log-update-latest-k5
#sleep 30

# compact data
#./benchmark workloads/workload-col30-update compact 5 /rocksdb-data/data-bench-k5-col30-update-latest false >> log-update-latest-k5
#sleep 30

# update data in latest
#./benchmark workloads/workload-col30-update update 5 /rocksdb-data/data-bench-k5-col30-update-latest false >> log-update-latest-k5
#sleep 30

# for uniform updates
# load data
#./benchmark workloads/workload-col30-update load 5 /rocksdb-data/data-bench-k5-col30-update-uniform false >> log-update-uniform-k5
#sleep 30

# compact data
#./benchmark workloads/workload-col30-update compact 5 /rocksdb-data/data-bench-k5-col30-update-uniform false >> log-update-uniform-k5

# update data in uniform
#./benchmark workloads/workload-col30-update update 5 /rocksdb-data/data-bench-k5-col30-update-uniform false >> log-update-uniform-k5
#sleep 30



########
# SCAN #
########
# length = 120M
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Sb run 2 /rocksdb-data/data-bench-k2-col30 true >> log-k2
#sleep 30
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Sb run 3 /rocksdb-data/data-bench-k3-col30 true >> log-k3
#sleep 30
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Sb run 4 /rocksdb-data/data-bench-k4-col30 true >> log-k4
#sleep 30
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Sb run 5 /rocksdb-data/data-bench-k5-col30 true >> log-k5
#sleep 30
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Sb run 6 /rocksdb-data/data-bench-k6-col30 true >> log-k6
#sleep 30
#
## length = 150M
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Sc run 2 /rocksdb-data/data-bench-k2-col30 true >> log-k2
#sleep 30
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Sc run 3 /rocksdb-data/data-bench-k3-col30 true >> log-k3
#sleep 30
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Sc run 4 /rocksdb-data/data-bench-k4-col30 true >> log-k4
#sleep 30
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Sc run 5 /rocksdb-data/data-bench-k5-col30 true >> log-k5
#sleep 30
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Sc run 6 /rocksdb-data/data-bench-k6-col30 true >> log-k6
#sleep 30
#
## length = 180M
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Sd run 2 /rocksdb-data/data-bench-k2-col30 true >> log-k2
#sleep 30
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Sd run 3 /rocksdb-data/data-bench-k3-col30 true >> log-k3
#sleep 30
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Sd run 4 /rocksdb-data/data-bench-k4-col30 true >> log-k4
#sleep 30
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Sd run 5 /rocksdb-data/data-bench-k5-col30 true >> log-k5
#sleep 30
#
#echo "ubuntu" | sudo -S sync
#echo 3 | sudo tee /proc/sys/vm/drop_caches
#./benchmark workloads/workload-col30-Sd run 6 /rocksdb-data/data-bench-k6-col30 true >> log-k6
#sleep 30




# husky-big exp wrt max-background-job
#./benchmark workloads/workload-col80 load 2 /rocksdb-data/data-bench-k2-col80-bg4 false >> log-k2-bg4
