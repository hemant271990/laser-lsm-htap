IDIR=-I../../rocksdb-5.14-hemant/include -I../../rocksdb-5.14-hemant/table
CC=gcc
CFLAGS=-std=c++11

ODIR=obj
LDIR=-L../../rocksdb-5.14-hemant/

# -lsnappy for snappy -lbz2 for bzip2 -lz for zlib
#LIBS=-lrocksdb_debug -lpthread -lsnappy
LIBS=-lrocksdb -lpthread -lsnappy

default: benchmark

benchmark: benchmark.cpp
	g++ -g -o benchmark benchmark.cpp $(CFLAGS) $(IDIR) $(LDIR) $(LIBS)
#g++ -o KBench KBench.cpp -std=c++11 -I../../rocksdb-4.9/include/ -I../../rocksdb-4.9/table/ -L../../rocksdb-4.9/ -lrocksdb -lpthread
.PHONY: clean

clean:
	rm benchmark
