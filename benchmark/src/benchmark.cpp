//#include <assert>
#include "rocksdb/db.h"
#include <fstream>
//#include "rocksdb/options.h"
#include "rocksdb/table.h"
#include "rocksdb/iostats_context.h"
#include "rocksdb/perf_context.h"

#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>
#include <stdlib.h>
#include <math.h>
#include <random>
#include <ctime>
#include <atomic>
#include <algorithm>
#include <iomanip>
#include <set>
#include <unordered_set>
#include <mutex>
#include <inttypes.h>
#include "zipf.h"
using namespace std;
using namespace rocksdb;
using namespace std::chrono;


bool DEBUG								= false;
vector<uint32_t> read_filter;
vector<uint32_t> scan_filter;
set<uint32_t> update_cols = {3};

// tunable params
bool disable_compaction 	= false;
bool hashed_key						= true;
bool inserts_with_reads		= false;
uint32_t column_count 		= 10;
uint32_t max_levels 			= 7;
uint32_t column_from_k 		= 2;
uint32_t proj_offset 		= 2;
double read_offset 		= 0.0;
string db_path 						= "/home/h2saxena/Desktop/shichao-klsm/klsm/testRDB/data-benchmark";
uint64_t num_records			= 20;
uint64_t num_operations		= 10;
uint64_t stage1_inserts		= 10000000;
uint64_t stage2_inserts		= 30000000;
uint32_t column_width			= 5;
uint32_t key_width				= 10;
uint32_t num_read_cols		= 10;
uint32_t num_scan_cols		= 1;
float scan_length			= 0.9;
float read_proportion			= 0.2;
float scan_proportion			= 0.2;
//float insert_proportion		= 0.4;
float update_proportion		= 0.2;
int num_threads						= 1;
int num_bg_jobs				= 4;
std::random_device rd{};
std::mt19937 gen{rd()};
double norm_mean					= 0.9;
string read_pattern				= "uniform";
string update_pattern			= "zipf";
int level_multiplier			= 2;
uint32_t cg_size			= 2;
uint32_t design				= -1;

// runtime variables
unordered_set<uint32_t> key_set;
vector<uint32_t> key_vec;
std::mutex key_vec_lock;
atomic<uint64_t> key_counter;
atomic<uint64_t> operation_counter;
atomic<uint64_t> read_count;
atomic<uint64_t> scan_count;
atomic<uint64_t> insert_count;
atomic<uint64_t> update_count;
float avg_read_through;
float avg_scan_through;
float avg_insert_through;
float avg_update_through;
atomic<uint64_t> interval_count;
vector<long long> read_latency;
vector<long long> scan_latency;
vector<long long> update_latency;
std::mutex read_lock;
std::mutex scan_lock;
std::mutex update_lock;

ZipfianGenerator* skew_key_gen;

bool parseConfig(string config_file) {
	ifstream is_file(config_file);

	std::string line;
	while( std::getline(is_file, line) ) {
		std::istringstream is_line(line);
		std::string key;
		if( std::getline(is_line, key, '=') ) {
			std::string value;
			if( std::getline(is_line, value) ) {
				if(key == "column_count") {
					column_count = static_cast<uint32_t>(std::stoul(value));
					cout << "column_count: " << column_count << endl;
				} else if (key == "db_path") {
					db_path = value;
					cout << "db_path: " << db_path << endl;
				} else if (key == "num_records") {
					num_records = static_cast<uint64_t>(std::stoul(value));
					cout << "num_records: " << num_records << endl;
				} else if (key == "stage1_inserts") {
					stage1_inserts = static_cast<uint64_t>(std::stoul(value));
					cout << "stage1_inserts: " << stage1_inserts << endl;
				} else if (key == "stage2_inserts") {
					stage2_inserts = static_cast<uint64_t>(std::stoul(value));
					cout << "stage2_inserts: " << stage2_inserts << endl;
				} else if (key == "num_operations") {
					num_operations = static_cast<uint64_t>(std::stoul(value));
					cout << "num_operations: " << num_operations << endl;
				} else if (key == "disable_compaction") {
					if (value == "true")
						disable_compaction = true;
					else if (value == "false")
						disable_compaction = false;
					cout << "disable_compaction: " << disable_compaction << endl;
				} else if (key == "inserts_with_reads") {
					if (value == "true")
						inserts_with_reads = true;
					else if (value == "false")
						inserts_with_reads = false;
					cout << "inserts_with_reads: " << inserts_with_reads << endl;
				} else if (key == "hashed_key") {
					if (value == "true")
						hashed_key = true;
					else if (value == "false")
						hashed_key = false;
					cout << "hashed_key: " << hashed_key << endl;
				} else if (key == "column_from_k") {
					column_from_k = static_cast<uint32_t>(std::stoul(value));
					cout << "column_from_k: " << column_from_k << endl;
				} else if (key == "max_levels") {
					max_levels = static_cast<uint32_t>(std::stoul(value));
					cout << "max_levels: " << max_levels << endl;
				} else if (key == "column_width") {
					column_width = static_cast<uint32_t>(std::stoul(value));
					cout << "column_width: " << column_width << endl;
				} else if (key == "key_width") {
					key_width = static_cast<uint32_t>(std::stoul(value));
					cout << "key_width: " << key_width << endl;
				} else if (key == "read_proportion") {
					read_proportion = std::stof(value);
					cout << "read_proportion: " << read_proportion << endl;
				} else if (key == "scan_proportion") {
					scan_proportion = std::stof(value);
					cout << "scan_proportion: " << scan_proportion << endl;
				} else if (key == "update_proportion") {
					update_proportion = std::stof(value);
					cout << "update_proportion: " << update_proportion << endl;
				} else if (key == "norm_mean") {
					norm_mean = std::stod(value);
					cout << "norm_mean: " << norm_mean << endl;
				} else if (key == "num_threads") {
					num_threads = std::stoi(value);
					cout << "num_threads: " << num_threads << endl;
				} else if (key == "num_bg_jobs") {
					num_bg_jobs = std::stoi(value);
					cout << "num_bg_jobs: " << num_bg_jobs << endl;
				} else if (key == "num_read_cols") {
					num_read_cols = static_cast<uint32_t>(std::stoul(value));
					cout << "num_read_cols: " << num_read_cols << endl;
				} else if (key == "num_scan_cols") {
					num_scan_cols = static_cast<uint32_t>(std::stoul(value));
					cout << "num_scan_cols: " << num_scan_cols << endl;
				} else if (key == "scan_length") {
					scan_length = std::stof(value);
					cout << "scan_length: " << scan_length << endl;
				} else if (key == "read_pattern") {
					read_pattern = value;
					cout << "read_pattern: " << read_pattern << endl;
				} else if (key == "update_pattern") {
					update_pattern = value;
					cout << "update_pattern: " << update_pattern << endl;
				} else if (key == "level_multiplier") {
					level_multiplier = std::stoi(value);
					cout << "level_multiplier: " << level_multiplier << endl;
				} else if (key == "cg_size") {
                                        cg_size = static_cast<uint32_t>(std::stoul(value));
                                        cout << "cg_size: " << cg_size << endl;
				} else if (key == "design") {
                                        design = static_cast<uint32_t>(std::stoul(value));
                                        cout << "design: " << design << endl;
                                } else {
					cout << "No param: " << key << " - " << value << endl;
					is_file.close();
					return false;
				}
			}
		}
	}
	is_file.close();
	return true;
}

vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix(int num_levels, uint32_t column_num) {
    vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);

    for(int i = 0; i < 1; i++) {
        vector<tuple<uint32_t, uint32_t>> cg_i(1);
        cg_i[0] = make_tuple(0,column_num-1);
        matrix[i] = cg_i;
    }   

	//vector<tuple<uint32_t, uint32_t>> cg_j{make_tuple(0,15), make_tuple(16,column_num-1)};
	//matrix[2] = cg_j;
	//vector<tuple<uint32_t, uint32_t>> cg_j2{make_tuple(0, 14), make_tuple(15,29)};
	//vector<tuple<uint32_t, uint32_t>> cg_j2{make_tuple(0, 5), make_tuple(6,11), make_tuple(12,17), make_tuple(18,23), make_tuple(24,29)};
	//vector<tuple<uint32_t, uint32_t>> cg_j2{make_tuple(0, 2), make_tuple(3,5), make_tuple(6,8), make_tuple(9,11), make_tuple(12,14),
	//					make_tuple(15, 17), make_tuple(18,20), make_tuple(21,23), make_tuple(24,26), make_tuple(27,29)};
	/*matrix[2] = cg_j2;
	matrix[3] = cg_j2;
	matrix[4] = cg_j2;
	matrix[5] = cg_j2;
	*/
	int num_cgs = column_num/cg_size;	
	for(int i = 1; i < num_levels; i++) {
	  vector<tuple<uint32_t, uint32_t>> cg_i(num_cgs);
          for(int j = 0; j < num_cgs; j++) {
            cg_i[j] = make_tuple(static_cast<uint32_t>(j*cg_size), static_cast<uint32_t>(j*cg_size + cg_size -1));
          }
          matrix[i] = cg_i;
	}
    return matrix;
}



vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix1(int num_levels, uint32_t column_num) {
    vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);

    for(int i = 0; i < 2; i++) {
        vector<tuple<uint32_t, uint32_t>> cg_i(1);
        cg_i[0] = make_tuple(0,column_num-1);
        matrix[i] = cg_i;
    }   

	vector<tuple<uint32_t, uint32_t>> cg_j2{make_tuple(0,13), make_tuple(14,29)};
	matrix[2] = cg_j2;
	vector<tuple<uint32_t, uint32_t>> cg_j3{make_tuple(0,5), make_tuple(6,13), make_tuple(14,21), make_tuple(22,29)};
	matrix[3] = cg_j3;
	vector<tuple<uint32_t, uint32_t>> cg_j4{make_tuple(0,1), make_tuple(2,5), make_tuple(6,9), make_tuple(10,13), 
																					make_tuple(14,17), make_tuple(18,21), make_tuple(22,25), make_tuple(26,29)};
	matrix[4] = cg_j4;
	matrix[5] = cg_j4;
	vector<tuple<uint32_t, uint32_t>> cg_j6{make_tuple(0,1), make_tuple(2,3), make_tuple(4,5), make_tuple(6,7), make_tuple(8,9), make_tuple(10,11), 
																					make_tuple(12,13), make_tuple(14,15), make_tuple(16,17), make_tuple(18,19), make_tuple(20,21), make_tuple(22,23), 
																					make_tuple(24,25), make_tuple(26,27), make_tuple(28,29)};
	matrix[6] = cg_j6;
	for(int i = 7; i < num_levels; i++) {
		vector<tuple<uint32_t, uint32_t>> cg_i(column_num);
		for(int j = 0; j < column_num; j++) {
			cg_i[j] = make_tuple(static_cast<uint32_t>(j), static_cast<uint32_t>(j));
		}
		matrix[i] = cg_i;
	}   
	return matrix;
}

vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix1_(int num_levels, uint32_t column_num) {
    vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);

    for(int i = 0; i < 2; i++) {
        vector<tuple<uint32_t, uint32_t>> cg_i(1);
        cg_i[0] = make_tuple(0,column_num-1);
        matrix[i] = cg_i;
    }   

	vector<tuple<uint32_t, uint32_t>> cg_j2{make_tuple(0,13), make_tuple(14,29)};
	matrix[2] = cg_j2;
	vector<tuple<uint32_t, uint32_t>> cg_j3{make_tuple(0,5), make_tuple(6,13), make_tuple(14,21), make_tuple(22,29)};
	matrix[3] = cg_j3;
	vector<tuple<uint32_t, uint32_t>> cg_j4{make_tuple(0,1), make_tuple(2,5), make_tuple(6,9), make_tuple(10,13), 
																					make_tuple(14,17), make_tuple(18,21), make_tuple(22,25), make_tuple(26,29)};
	matrix[4] = cg_j4;
	matrix[5] = cg_j4;
	vector<tuple<uint32_t, uint32_t>> cg_j6{make_tuple(0,1), make_tuple(2,3), make_tuple(4,5), make_tuple(6,7), make_tuple(8,9), make_tuple(10,11), 
																					make_tuple(12,13), make_tuple(14,15), make_tuple(16,17), make_tuple(18,19), make_tuple(20,21), make_tuple(22,23), 
																					make_tuple(24,25), make_tuple(26,27), make_tuple(28,29)};
	matrix[6] = cg_j6;
	for(int i = 7; i < num_levels; i++) {
		vector<tuple<uint32_t, uint32_t>> cg_i(column_num);
		for(int j = 0; j < column_num; j++) {
			cg_i[j] = make_tuple(static_cast<uint32_t>(j), static_cast<uint32_t>(j));
		}
		matrix[i] = cg_i;
	}   
	return matrix;
}

vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix2(int num_levels, uint32_t column_num) {
    vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);

    for(int i = 0; i < 2; i++) {
        vector<tuple<uint32_t, uint32_t>> cg_i(1);
        cg_i[0] = make_tuple(0,column_num-1);
        matrix[i] = cg_i;
    }   

	vector<tuple<uint32_t, uint32_t>> cg_j{make_tuple(0,9), make_tuple(10,29)};
	matrix[2] = cg_j;
	vector<tuple<uint32_t, uint32_t>> cg_j2{make_tuple(0,9), make_tuple(10,19), make_tuple(20,29)};
	matrix[3] = cg_j2;
	matrix[4] = cg_j2;
	vector<tuple<uint32_t, uint32_t>> cg_j4{make_tuple(0,4), make_tuple(5,9), make_tuple(10,14), make_tuple(15,19), make_tuple(20,24), make_tuple(25,29)};
	matrix[5] = cg_j4;
	matrix[6] = cg_j4;
	matrix[7] = cg_j4;
	return matrix;
}

vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix3(int num_levels, uint32_t column_num) {
    vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);

    for(int i = 0; i < 2; i++) {
        vector<tuple<uint32_t, uint32_t>> cg_i(1);
        cg_i[0] = make_tuple(0,column_num-1);
        matrix[i] = cg_i;
    }   

	vector<tuple<uint32_t, uint32_t>> cg_j{make_tuple(0,4), make_tuple(5,9), make_tuple(10,29)};
	matrix[2] = cg_j;
	matrix[3] = cg_j;
	matrix[4] = cg_j;
	matrix[5] = cg_j;
	matrix[6] = cg_j;
	matrix[7] = cg_j;
	return matrix;
}

vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix4(int num_levels, uint32_t column_num) {
    vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);

    for(int i = 0; i < 1; i++) {
        vector<tuple<uint32_t, uint32_t>> cg_i(1);
        cg_i[0] = make_tuple(0,column_num-1);
        matrix[i] = cg_i;
    }   

	vector<tuple<uint32_t, uint32_t>> cg_j2{make_tuple(0,13), make_tuple(14,29)};
	matrix[1] = cg_j2;
	vector<tuple<uint32_t, uint32_t>> cg_j3{make_tuple(0,5), make_tuple(6,13), make_tuple(14,21), make_tuple(22,29)};
	matrix[2] = cg_j3;
	vector<tuple<uint32_t, uint32_t>> cg_j4{make_tuple(0,1), make_tuple(2,5), make_tuple(6,9), make_tuple(10,13), 																	make_tuple(14,17), make_tuple(18,21), make_tuple(22,25), make_tuple(26,29)};
	matrix[3] = cg_j4;
	vector<tuple<uint32_t, uint32_t>> cg_j6{make_tuple(0,1), make_tuple(2,3), make_tuple(4,5), make_tuple(6,7), make_tuple(8,9), make_tuple(10,11), 
						make_tuple(12,13), make_tuple(14,15), make_tuple(16,17), make_tuple(18,19), make_tuple(20,21), make_tuple(22,23),
						make_tuple(24,25), make_tuple(26,27), make_tuple(28,29)};
	matrix[4] = cg_j6;
	matrix[5] = cg_j6;
	for(int i = 6; i < num_levels; i++) {
		vector<tuple<uint32_t, uint32_t>> cg_i(column_num);
		for(int j = 0; j < column_num; j++) {
			cg_i[j] = make_tuple(static_cast<uint32_t>(j), static_cast<uint32_t>(j));
		}
		matrix[i] = cg_i;
	}   
	return matrix;
}

vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix5(int num_levels, uint32_t column_num) {
    vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);

    for(int i = 0; i < 1; i++) {
        vector<tuple<uint32_t, uint32_t>> cg_i(1);
        cg_i[0] = make_tuple(0,column_num-1);
        matrix[i] = cg_i;
    }   

	vector<tuple<uint32_t, uint32_t>> cg_j2{make_tuple(0,5), make_tuple(6,11), make_tuple(12,17), make_tuple(18,23), make_tuple(24,29)};
	matrix[1] = cg_j2;
	matrix[2] = cg_j2;
	vector<tuple<uint32_t, uint32_t>> cg_j4{make_tuple(0,2), make_tuple(3,5), make_tuple(6,8), make_tuple(9,11), make_tuple(12,14),
						make_tuple(15,17), make_tuple(18,20), make_tuple(21,23), make_tuple(24,26), make_tuple(27,29)};
	matrix[3] = cg_j4;
	matrix[4] = cg_j4;
	matrix[5] = cg_j4;
	matrix[6] = cg_j4;
	matrix[7] = cg_j4;
	return matrix;
}

vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix6(int num_levels, uint32_t column_num) {
    vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);

    for(int i = 0; i < 1; i++) {
        vector<tuple<uint32_t, uint32_t>> cg_i(1);
        cg_i[0] = make_tuple(0,column_num-1);
        matrix[i] = cg_i;
    }   

	vector<tuple<uint32_t, uint32_t>> cg_j{make_tuple(0,14), make_tuple(15,16), make_tuple(17,19), make_tuple(20,29)};
	matrix[1] = cg_j;
	matrix[2] = cg_j;
	matrix[3] = cg_j;
	matrix[4] = cg_j;
	matrix[5] = cg_j;
	matrix[6] = cg_j;
	matrix[7] = cg_j;
	return matrix;
}

vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix7(int num_levels, uint32_t column_num) {
	vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);

	vector<tuple<uint32_t, uint32_t>> cg_j1{make_tuple(0,29)};
	vector<tuple<uint32_t, uint32_t>> cg_j2{make_tuple(0,9), make_tuple(10,19), make_tuple(20,29)};
	vector<tuple<uint32_t, uint32_t>> cg_j3{make_tuple(0,4), make_tuple(5,9), make_tuple(10,14), make_tuple(15,19), make_tuple(20,24), make_tuple(25,29)};
	vector<tuple<uint32_t, uint32_t>> cg_j4{make_tuple(0,2), make_tuple(3,4), make_tuple(5,7), make_tuple(8,9), make_tuple(10,12), make_tuple(13,14), make_tuple(15,17), make_tuple(18,19),
						 make_tuple(20,22), make_tuple(23,24), make_tuple(25,27), make_tuple(28,29)};
	matrix[0] = cg_j1;
	matrix[1] = cg_j1;
	matrix[2] = cg_j2;
	matrix[3] = cg_j3;
	matrix[4] = cg_j4;
	matrix[5] = cg_j4;
	matrix[6] = cg_j4;
	matrix[7] = cg_j4;
	return matrix;
}

vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix8(int num_levels, uint32_t column_num) {
	vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);

	vector<tuple<uint32_t, uint32_t>> cg_j1{make_tuple(0,29)};
	vector<tuple<uint32_t, uint32_t>> cg_j2{make_tuple(0,9), make_tuple(10,19), make_tuple(20,29)};
	vector<tuple<uint32_t, uint32_t>> cg_j3{make_tuple(0,4), make_tuple(5,9), make_tuple(10,14), make_tuple(15,19), make_tuple(20,24), make_tuple(25,29)};
	vector<tuple<uint32_t, uint32_t>> cg_j4{make_tuple(0,2), make_tuple(3,4), make_tuple(5,7), make_tuple(8,9), make_tuple(10,12), make_tuple(13,14), make_tuple(15,17), make_tuple(18,19),
						 make_tuple(20,22), make_tuple(23,24), make_tuple(25,27), make_tuple(28,29)};
	matrix[0] = cg_j1;
	matrix[1] = cg_j1;
	matrix[2] = cg_j2;
	matrix[3] = cg_j2;
	matrix[4] = cg_j3;
	matrix[5] = cg_j3;
	matrix[6] = cg_j4;
	matrix[7] = cg_j4;
	return matrix;
}

vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix9(int num_levels, uint32_t column_num) {
	vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);

	vector<tuple<uint32_t, uint32_t>> cg_j1{make_tuple(0,29)};
	vector<tuple<uint32_t, uint32_t>> cg_j2{make_tuple(0,9), make_tuple(10,19), make_tuple(20,29)};
	vector<tuple<uint32_t, uint32_t>> cg_j3{make_tuple(0,4), make_tuple(5,9), make_tuple(10,14), make_tuple(15,19), make_tuple(20,24), make_tuple(25,29)};
	vector<tuple<uint32_t, uint32_t>> cg_j4{make_tuple(0,2), make_tuple(3,4), make_tuple(5,7), make_tuple(8,9), make_tuple(10,12), make_tuple(13,14), make_tuple(15,17), make_tuple(18,19),
						 make_tuple(20,22), make_tuple(23,24), make_tuple(25,27), make_tuple(28,29)};
	matrix[0] = cg_j1;
	matrix[1] = cg_j1;
	matrix[2] = cg_j1;
	matrix[3] = cg_j1;
	matrix[4] = cg_j1;
	matrix[5] = cg_j2;
	matrix[6] = cg_j3;
	matrix[7] = cg_j4;
	return matrix;
}

vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix10(int num_levels, uint32_t column_num) {
	vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);

	vector<tuple<uint32_t, uint32_t>> cg_j1{make_tuple(0,29)};
	vector<tuple<uint32_t, uint32_t>> cg_j2{make_tuple(0,14), make_tuple(15,19)};
	vector<tuple<uint32_t, uint32_t>> cg_j3{make_tuple(0,6), make_tuple(7,14), make_tuple(15,21), make_tuple(22,29)};
	matrix[0] = cg_j1;
	matrix[1] = cg_j1;
	matrix[2] = cg_j2;
	matrix[3] = cg_j3;
	return matrix;
}

vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix11(int num_levels, uint32_t column_num) {
	vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);

	vector<tuple<uint32_t, uint32_t>> cg_j1{make_tuple(0,29)};
	vector<tuple<uint32_t, uint32_t>> cg_j2{make_tuple(0,9), make_tuple(10,19), make_tuple(20,29)};
	vector<tuple<uint32_t, uint32_t>> cg_j3{make_tuple(0,2), make_tuple(3,5), make_tuple(6,9), make_tuple(10,12), make_tuple(13,15), make_tuple(16,19), 
						make_tuple(20,22),  make_tuple(23,25), make_tuple(26,29)};
	matrix[0] = cg_j1;
	matrix[1] = cg_j1;
	matrix[2] = cg_j2;
	matrix[3] = cg_j3;
	return matrix;
}

vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix12(int num_levels, uint32_t column_num) {
	vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);

	vector<tuple<uint32_t, uint32_t>> cg_j1{make_tuple(0,29)};
	vector<tuple<uint32_t, uint32_t>> cg_j2{make_tuple(0,7), make_tuple(8,15), make_tuple(16,22), make_tuple(23,29)};
	vector<tuple<uint32_t, uint32_t>> cg_j3{make_tuple(0,1), make_tuple(2,3), make_tuple(4,5), make_tuple(6,7), make_tuple(8,9), make_tuple(10,11), make_tuple(12,13), make_tuple(14,15), 
						make_tuple(16,17), make_tuple(18,19), make_tuple(20,21), make_tuple(22,22), make_tuple(23,24), make_tuple(25,26), make_tuple(27,28), make_tuple(29,29)};
	matrix[0] = cg_j1;
	matrix[1] = cg_j1;
	matrix[2] = cg_j2;
	matrix[3] = cg_j3;
	return matrix;
}

vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix13(int num_levels, uint32_t column_num) {
        vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);

        vector<tuple<uint32_t, uint32_t>> cg_j1{make_tuple(0,29)};
        vector<tuple<uint32_t, uint32_t>> cg_j2{make_tuple(0,5), make_tuple(6,11), make_tuple(12,17), make_tuple(18,23), make_tuple(24,29)};
        vector<tuple<uint32_t, uint32_t>> cg_j3(column_num);
	for(int j = 0; j < column_num; j++) {
		cg_j3[j] = make_tuple(static_cast<uint32_t>(j), static_cast<uint32_t>(j));
	}

	matrix[0] = cg_j1;
        matrix[1] = cg_j1;
        matrix[2] = cg_j2;
        matrix[3] = cg_j3;
        return matrix;
}

vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix14(int num_levels, uint32_t column_num) {
        vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);

        vector<tuple<uint32_t, uint32_t>> cg_j1{make_tuple(0,29)};
	vector<tuple<uint32_t, uint32_t>> cg_j2{make_tuple(0,9), make_tuple(10,19), make_tuple(20,29)};
        vector<tuple<uint32_t, uint32_t>> cg_j3{make_tuple(0,4), make_tuple(5,9), make_tuple(10,14), make_tuple(15,19), make_tuple(20,24), make_tuple(25,29)};
        vector<tuple<uint32_t, uint32_t>> cg_j4{make_tuple(0,2), make_tuple(3,4), make_tuple(5,7), make_tuple(8,9), make_tuple(10,12), make_tuple(13,14),
						make_tuple(15,17), make_tuple(18,19), make_tuple(20,22), make_tuple(23,24), make_tuple(25,27), make_tuple(28,29)};
        vector<tuple<uint32_t, uint32_t>> cg_j5(column_num);
	for(int j = 0; j < column_num; j++) {
		cg_j5[j] = make_tuple(static_cast<uint32_t>(j), static_cast<uint32_t>(j));
	}

	matrix[0] = cg_j1;
        matrix[1] = cg_j1;
        matrix[2] = cg_j1;
        matrix[3] = cg_j2;
        matrix[4] = cg_j3;
	matrix[5] = cg_j4;
	matrix[6] = cg_j4;
	matrix[7] = cg_j5;
        return matrix;
}

vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix15(int num_levels, uint32_t column_num) {
    vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);
	
	vector<tuple<uint32_t, uint32_t>> cg_j0{make_tuple(0,99)};
	matrix[0] = cg_j0;

	int fanout = 2;
	int num_cgs = 1;
	for(int i = 1; i < num_levels; i++) {
	  vector<tuple<uint32_t, uint32_t>> cg_i(num_cgs);
	  int cg_size = column_num/num_cgs;
	  for(int j = 0; j < num_cgs; j++) {
		if(j < num_cgs-1)
		    cg_i[j] = make_tuple(static_cast<uint32_t>(j*cg_size), static_cast<uint32_t>(j*cg_size + cg_size -1));
		else
		    cg_i[j] = make_tuple(static_cast<uint32_t>(j*cg_size), static_cast<uint32_t>(column_num-1));
	  }
	  matrix[i] = cg_i;
	  num_cgs = num_cgs*fanout;
	}
    return matrix;
}

vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix16(int num_levels, uint32_t column_num) {
    vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);
	
	vector<tuple<uint32_t, uint32_t>> cg_j0{make_tuple(0,99)};
	matrix[0] = cg_j0;

	int fanout = 3;
	int num_cgs = 1;
	for(int i = 1; i < num_levels; i++) {
	  vector<tuple<uint32_t, uint32_t>> cg_i(num_cgs);
	  int cg_size = column_num/num_cgs;
	  for(int j = 0; j < num_cgs; j++) {
		if(j < num_cgs-1)
		    cg_i[j] = make_tuple(static_cast<uint32_t>(j*cg_size), static_cast<uint32_t>(j*cg_size + cg_size -1));
		else
		    cg_i[j] = make_tuple(static_cast<uint32_t>(j*cg_size), static_cast<uint32_t>(column_num-1));
	  }
	  matrix[i] = cg_i;
	  num_cgs = num_cgs*fanout;
	}
    return matrix;
}

vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix17(int num_levels, uint32_t column_num) {
    vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);
	
	vector<tuple<uint32_t, uint32_t>> cg_j0{make_tuple(0,99)};
	matrix[0] = cg_j0;

	int fanout = 4;
	int num_cgs = 1;
	for(int i = 1; i < num_levels; i++) {
	  vector<tuple<uint32_t, uint32_t>> cg_i(num_cgs);
	  int cg_size = column_num/num_cgs;
	  for(int j = 0; j < num_cgs; j++) {
		if(j < num_cgs-1)
		    cg_i[j] = make_tuple(static_cast<uint32_t>(j*cg_size), static_cast<uint32_t>(j*cg_size + cg_size -1));
		else
		    cg_i[j] = make_tuple(static_cast<uint32_t>(j*cg_size), static_cast<uint32_t>(column_num-1));
	  }
	  matrix[i] = cg_i;
	  num_cgs = num_cgs*fanout;
	}
    return matrix;
}

vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix18(int num_levels, uint32_t column_num) {
    vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);
	
	vector<tuple<uint32_t, uint32_t>> cg_j0{make_tuple(0,99)};
	matrix[0] = cg_j0;

	int fanout = 5;
	int num_cgs = 1;
	for(int i = 1; i < num_levels; i++) {
	  vector<tuple<uint32_t, uint32_t>> cg_i(num_cgs);
	  int cg_size = column_num/num_cgs;
	  for(int j = 0; j < num_cgs; j++) {
		if(j < num_cgs-1)
		    cg_i[j] = make_tuple(static_cast<uint32_t>(j*cg_size), static_cast<uint32_t>(j*cg_size + cg_size -1));
		else
		    cg_i[j] = make_tuple(static_cast<uint32_t>(j*cg_size), static_cast<uint32_t>(column_num-1));
	  }
	  matrix[i] = cg_i;
	  num_cgs = num_cgs*fanout;
	  if(num_cgs > 100)
		num_cgs = 100;
	}
    return matrix;
}

vector<vector<tuple<uint32_t, uint32_t>>> createCGMatrix19(int num_levels, uint32_t column_num) {
	vector<vector<tuple<uint32_t, uint32_t>>> matrix(num_levels);

	vector<tuple<uint32_t, uint32_t>> cg_j1{make_tuple(0,29)};
    vector<tuple<uint32_t, uint32_t>> cg_j2{make_tuple(0,14), make_tuple(15,29)};
    vector<tuple<uint32_t, uint32_t>> cg_j3{make_tuple(0,14), make_tuple(15,19), make_tuple(20,29)};
    vector<tuple<uint32_t, uint32_t>> cg_j4{make_tuple(0,14), make_tuple(15,19), make_tuple(20,26), make_tuple(27,29)};

	matrix[0] = cg_j1;
	matrix[1] = cg_j1;
	matrix[2] = cg_j2;
	matrix[3] = cg_j2;
	matrix[4] = cg_j3;
	matrix[5] = cg_j3;
	matrix[6] = cg_j4;
	matrix[7] = cg_j4;
	return matrix;
}

void printCGMatrix(vector<vector<tuple<uint32_t, uint32_t>>> m) {
    for(int i = 0; i < m.size(); i++) {
        for(int j = 0; j < m[i].size(); j++) {
            cout << "(" << get<0>(m[i][j]) << "," << get<1>(m[i][j]) << ")";
        }
				cout << endl;
    }   
}

DB* initDB() {
	DB* db;
	Options options;
	options.create_if_missing = true;
	//options.target_file_size_base = 128 * 1024*1024;	// default 64MB
	options.write_buffer_size = 64 * 1024 * 1024;	// default 64MB
	options.max_write_buffer_number = 2; // default 2
	options.min_write_buffer_number_to_merge = 1; // We can change this if merging slows down writes default 1
	options.max_bytes_for_level_base = 1 * 128 * 1024 * 1024ul; // default 256MB
	options.target_file_size_base = 64 * 1024 * 1024;
	options.target_file_size_multiplier = 1;	// we can reduce this given that target_file_size_base=256MB now default 1
	options.max_bytes_for_level_multiplier = level_multiplier; // default 10
	options.max_background_jobs = num_bg_jobs;
	options.max_subcompactions = 4;
	options.num_levels = max_levels; // default 7
	options.disable_auto_compactions = disable_compaction; // default false
	options.compaction_pri = rocksdb::CompactionPri::kOldestLargestSeqFirst;
	options.column_num = column_count;
	options.column_from_k = column_from_k;
	if (design == 7) {
		options.cg_range_matrix = createCGMatrix7(max_levels, column_count);
		options.levels_cg_count = {1, 1, 3, 6, 12, 12, 12, 12};
	} else if (design == 8) {
		options.cg_range_matrix = createCGMatrix8(max_levels, column_count);
		options.levels_cg_count = {1, 1, 3, 3, 6, 6, 12, 12};
	} else if (design == 9) {
		options.cg_range_matrix = createCGMatrix9(max_levels, column_count);
		options.levels_cg_count = {1, 1, 1, 1, 1, 3, 6, 12};
	} else if (design == 10) {
                options.cg_range_matrix = createCGMatrix10(max_levels, column_count);
                options.levels_cg_count = {1, 1, 2, 4};
	} else if (design == 11) {
                options.cg_range_matrix = createCGMatrix11(max_levels, column_count);
                options.levels_cg_count = {1, 1, 3, 9};
	} else if (design == 12) {
                options.cg_range_matrix = createCGMatrix12(max_levels, column_count);
                options.levels_cg_count = {1, 1, 4, 16};
	} else if (design == 13) {
                options.cg_range_matrix = createCGMatrix13(max_levels, column_count);
                options.levels_cg_count = {1, 1, 5, 30};
	} else if (design == 14) {
                options.cg_range_matrix = createCGMatrix14(max_levels, column_count);
                options.levels_cg_count = {1, 1, 1, 3, 6, 15, 15, 30};
	} else if (design == 15) {
		options.cg_range_matrix = createCGMatrix15(max_levels, column_count);
                options.levels_cg_count = {1, 1, 2, 4, 8};
	} else if (design == 16) {
		options.cg_range_matrix = createCGMatrix16(max_levels, column_count);
                options.levels_cg_count = {1, 1, 3, 9, 27};
	} else if (design == 17) {
		options.cg_range_matrix = createCGMatrix17(max_levels, column_count);
                options.levels_cg_count = {1, 1, 4, 16, 64};
	} else if (design == 18) {
		options.cg_range_matrix = createCGMatrix18(max_levels, column_count);
                options.levels_cg_count = {1, 1, 5, 25, 100};
	} else if (design == 19) {
		options.cg_range_matrix = createCGMatrix19(max_levels, column_count);
                options.levels_cg_count = {1, 1, 2, 2, 3, 3, 4, 4};
	} else if (design == -1) {
		options.cg_range_matrix = createCGMatrix(max_levels, column_count);
		options.levels_cg_count = {1, column_count/cg_size, column_count/cg_size, column_count/cg_size, column_count/cg_size, column_count/cg_size, column_count/cg_size, column_count/cg_size};
	}

	printCGMatrix(options.cg_range_matrix);
	//options.levels_cg_count = {1, column_count/cg_size, column_count/cg_size, column_count/cg_size, column_count/cg_size};
	//options.levels_cg_count = {1, column_count/cg_size, column_count/cg_size, column_count/cg_size, column_count/cg_size, column_count/cg_size, column_count/cg_size, column_count/cg_size};
	//options.levels_cg_count = {1, 1, 2, 4, 8, 8, 15, 30};
	//options.levels_cg_count = {1, 1, 2, 3, 3, 6, 6, 6};
	//options.levels_cg_count = {1, 1, 3, 3, 3, 3, 3, 3};
	//options.levels_cg_count = {1, 2, 4, 8, 15, 15, 30, 30};
	//options.levels_cg_count = {1, 5, 5, 10, 10, 10, 10, 10};
	//options.levels_cg_count = {1, 4, 4, 4, 4, 4, 4, 4};
	options.delim = "|";

	Status s = DB::Open(options, db_path, &db);
	if (!s.ok()) cout << s.ToString() << endl;
	return db;
}

uint64_t getNextKey() {
	return key_counter.fetch_add(1);
}

uint64_t getNextOpCount() {
	return operation_counter.fetch_add(1);
}

uint32_t myhash(uint32_t x) {
	if(hashed_key) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
	} else {
		return x;
	}
}

/* uint64_t myhash(uint64_t x) {
	// append last two digits at the front, trying to simulate year as prefix for IDs
	uint64_t rem = x;
	rem = (rem ^ (rem >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
	rem = (rem ^ (rem >> 27)) * UINT64_C(0x94d049bb133111eb);
	rem = rem ^ (rem >> 31);
	// uint64_t rem = x % 100;
	//rem = pow(rem, 3);
	//rem = rem % 100; 
	rem = rem % 10000000000;
	//rem = rem * pow(10, 10) + x;
	//rem = rem * pow(10, key_width - 3) + x;
  return rem;
} */

string getInsertKeyStr() {
	//bool found_unique = false;
	//while (!found_unique) {
		uint32_t hashed = myhash((uint32_t) getNextKey());
		/* if(key_set.find(hashed) == key_set.end()) {
			key_set.insert(hashed);
			key_vec.push_back(hashed);
			found_unique = true;
		} */
	//}
	//uint32_t user_key = key_vec[key_vec.size()-1];
	std::ostringstream kstr;
	kstr << std::setfill('0') << std::setw(key_width) << hashed/* user_key */ ;
	return kstr.str();
}

size_t getNextReadPosition() {
	size_t max_pos = 0;
	key_vec_lock.lock();
	max_pos = key_vec.size();
	key_vec_lock.unlock();
	size_t read_delay = 100;
	if (max_pos < 2*read_delay) {
		read_delay = 0;
	}
	return max_pos - read_delay;
}

uint32_t getNextReadKeyNormal(double _norm_mean = norm_mean) {
	std::normal_distribution<> norm_dist{_norm_mean,0.02};
	uint64_t curr_key = key_counter.load();
	float rand = norm_dist(gen);
	//cout << rand << endl;
	uint32_t gen_num = (uint32_t) (rand*curr_key);
	while (gen_num > curr_key - 100 || gen_num < 0) {
		rand = norm_dist(gen);
		gen_num = (uint32_t) (rand*curr_key);
	}
	uint32_t user_key = myhash(gen_num);
	return user_key;
}

uint32_t getNextReadKeyZipf() {
	//size_t pos = getNextReadPosition();
	//size_t get_read_pos = (size_t) (pos - skew_key_gen->Next((uint64_t) pos));
	//key_vec_lock.lock();
	uint64_t curr_key = key_counter.load() - 100;
	uint32_t user_key = (uint32_t) (curr_key - skew_key_gen->Next(curr_key));
	user_key = myhash(user_key);
	//uint32_t user_key = key_vec[get_read_pos];
	//key_vec_lock.unlock();
	return user_key;
}

uint32_t getNextReadKeyUniform() {
	uint64_t curr_key = key_counter.load() - 100;
	uint32_t user_key = (uint32_t) (rand() % curr_key + 1);
	user_key = myhash(user_key);
	return user_key;
}

uint32_t getNextReadKey() {
	if(read_pattern == "zipf")
		return getNextReadKeyZipf();
	else if(read_pattern == "normal")
		return getNextReadKeyNormal();
	else if(read_pattern == "uniform")
		return getNextReadKeyUniform();
}

uint32_t getNextUpdateKey() {
	if(update_pattern == "zipf")
		return getNextReadKeyZipf();
	else if(update_pattern == "normal")
		return getNextReadKeyNormal();
	else if(update_pattern == "uniform")
		return getNextReadKeyUniform();
}

uint32_t getNextScanKey() {
	//size_t pos = getNextReadPosition();
	//size_t get_read_pos = (size_t) (rand() % pos + 1);
	//key_vec_lock.lock();
	// biased to pick older keys more often
	//uint64_t curr_key = key_counter.load() - 100;
	//uint32_t user_key = (uint32_t) (rand() % curr_key + 1);
	uint32_t rand_num = (uint32_t) (rand() % 10 + 1);
	//uint32_t user_key = (uint32_t) (1 + skew_key_gen->Next(curr_key));
	//user_key = myhash(user_key);
	return rand_num * 100000000;
	//uint32_t user_key = key_vec[get_read_pos];
	//key_vec_lock.unlock();
	//return user_key;
	//return getNextReadKeyUniform();
}

string getKeyStr(uint32_t key) {
	std::ostringstream kstr;
	kstr << std::setfill('0') << std::setw(key_width) << key ;
	return kstr.str();
}

void storeReadLatency(long long latency) {
	read_lock.lock();
	read_latency.push_back(latency);
	read_lock.unlock();
}

void storeScanLatency(long long latency) {
	scan_lock.lock();
	scan_latency.push_back(latency);
	scan_lock.unlock();
}

void storeUpdateLatency(long long latency) {
	update_lock.lock();
	update_latency.push_back(latency);
	update_lock.unlock();
}

void printWriteThroughput() {
	int64_t interval = 60;
	uint64_t curr_num_operations = key_counter.load();
	while(curr_num_operations < num_records) {
		// get current count
		uint64_t old_insert_count = insert_count.load();

		// sleep for 10 secs
		this_thread::sleep_for(chrono::seconds(interval));

		// get new count
		uint64_t new_insert_count = insert_count.load();

		float insert_through = (float) (new_insert_count - old_insert_count) / interval;

		cout 	<< " === CURRENT STATS " 
					<< " INSERT (ops/sec): " << insert_through
					<< endl;

		uint64_t count = interval_count.fetch_add(1);
		if (count == 0) {
			avg_insert_through = insert_through;
		} else {
			avg_insert_through = (avg_insert_through*count + insert_through) / (count + 1);
		}

		curr_num_operations = key_counter.load();
		cout 	<< " ** AVERAGE STATS " 
					<< " OPERATIONS: " << curr_num_operations
					<< " INSERT (ops/sec): " << avg_insert_through
					<< endl;
	}
	cout 	<< " FINAL COUNT " 
				<< " TOTAL OPERATIONS: " << curr_num_operations
				<< " INSERT (#ops): " << insert_count.load()
				<< endl;
}

void printThroughput() {
	int64_t interval = 60;
	uint64_t curr_num_operations = operation_counter.load();
	while(curr_num_operations < num_operations) {
		// get current count
		uint64_t old_read_count = read_count.load();
		uint64_t old_scan_count = scan_count.load();
		uint64_t old_insert_count = insert_count.load();
		uint64_t old_update_count = update_count.load();

		// sleep for 10 secs
		this_thread::sleep_for(chrono::seconds(interval));

		// get new count
		uint64_t new_read_count = read_count.load();
		uint64_t new_scan_count = scan_count.load();
		uint64_t new_insert_count = insert_count.load();
		uint64_t new_update_count = update_count.load();

		float read_through = (float) (new_read_count - old_read_count) / interval;
		float scan_through = (float) (new_scan_count - old_scan_count) / interval;
		float insert_through = (float) (new_insert_count - old_insert_count) / interval;
		float update_through = (float) (new_update_count - old_update_count) / interval;

		cout 	<< " === CURRENT STATS " 
					<< "READ (ops/sec): " << read_through 
					<< " SCAN (ops/sec): " << scan_through
					<< " INSERT (ops/sec): " << insert_through
					<< " UPDATE (ops/sec): " << update_through
					<< endl;

		uint64_t count = interval_count.fetch_add(1);
		if (count == 0) {
			avg_read_through = read_through;
			avg_scan_through = scan_through;
			avg_insert_through = insert_through;
			avg_update_through = update_through;
		} else {
			avg_read_through = (avg_read_through*count + read_through) / (count + 1);
			avg_scan_through = (avg_scan_through*count + scan_through) / (count + 1);
			avg_insert_through = (avg_insert_through*count + insert_through) / (count + 1);
			avg_update_through = (avg_update_through*count + update_through) / (count + 1);
		}

		curr_num_operations = operation_counter.load();
		cout 	<< " ** AVERAGE STATS " 
					<< " OPERATIONS: " << curr_num_operations
					<< " READ (ops/sec): " << avg_read_through 
					<< " SCAN (ops/sec): " << avg_scan_through
					<< " INSERT (ops/sec): " << avg_insert_through
					<< " UPDATE (ops/sec): " << avg_update_through
					<< endl;
	}
	cout 	<< " FINAL COUNT " 
				<< " TOTAL OPERATIONS: " << curr_num_operations
				<< " READ (#ops): " << read_count.load() 
				<< " SCAN (#ops): " << scan_count.load()
				<< " INSERT (#ops): " << insert_count.load()
				<< " UPDATE (#ops): " << update_count.load()
				<< endl;
}

float computeAvg(const vector<long long>& vec) {
	size_t count = vec.size();
	long long sum = 0;
	for(size_t i = 0; i < count; i++) {
		sum += vec[i];
	}
	return (float) sum / count;
}

void printLatencyStats() {
	sort(read_latency.begin(), read_latency.end());
	size_t read_count = read_latency.size();
	if (read_count > 0) {
		cout 	<< "[READ LATENCY] \n"
					<< "  MinLatency(us), " << read_latency[0] << endl
					<< "  MaxLatency(us), " << read_latency[read_count-1] << endl
					<< "  AvgLatency(us), " << computeAvg(read_latency) << endl
					<< "  95thPercentile(us), " << read_latency[(size_t)(read_count * 95 / 100)] << endl
					<< "  99thPercentile(us), " << read_latency[(size_t)(read_count * 99 / 100)] << endl ;
	}

	sort(scan_latency.begin(), scan_latency.end());
	size_t scan_count = scan_latency.size();
	if (scan_count > 0) {
		cout 	<< "[SCAN LATENCY] \n"
					<< "  MinLatency(ms), " << scan_latency[0] << endl
					<< "  MaxLatency(ms), " << scan_latency[scan_count-1] << endl
					<< "  AvgLatency(ms), " << computeAvg(scan_latency) << endl
					<< "  95thPercentile(ms), " << scan_latency[(size_t)(scan_count * 95 / 100)] << endl
					<< "  99thPercentile(ms), " << scan_latency[(size_t)(scan_count * 99 / 100)] << endl ;
	}

	sort(update_latency.begin(), update_latency.end());
	size_t update_count = update_latency.size();
	if (update_count > 0) {
		cout 	<< "[UPDATE LATENCY] \n"
					<< "  MinLatency(us), " << update_latency[0] << endl
					<< "  MaxLatency(us), " << update_latency[update_count-1] << endl
					<< "  AvgLatency(us), " << computeAvg(update_latency) << endl
					<< "  95thPercentile(us), " << update_latency[(size_t)(update_count * 95 / 100)] << endl
					<< "  99thPercentile(us), " << update_latency[(size_t)(update_count * 99 / 100)] << endl ;
	}

	// print total time. Add all scan and read latencies
	long long total = 0;
	for(size_t i = 0; i < read_latency.size(); i++) {
		total += read_latency.at(i);
	}
	for(size_t i = 0; i < scan_latency.size(); i++) {
		total += scan_latency.at(i)*1000;
	}
	cout 	<< "[TOTAL TIME] \n"
					<< "  total(secs), " << total/(1000*1000) << endl;
}

void loadDBWithTime(DB* db) {
	cout << "Begin load for: " << num_records << " entries" << endl;
	for(uint64_t count = 0; count < num_records; count++) {
		string kstr = getInsertKeyStr();

		std::ostringstream vstr;
		for (uint32_t i = 0; i < column_count; i++) {
			int v = rand() % 100000 + 1;
			if (i == 0)
				v = count;
			vstr << std::setfill('0') << std::setw(column_width) << v << "|";
		}
		if (DEBUG) {
			cout << "key: " << kstr << " value: " << vstr.str() << endl;
		}
		Status s = db->Put(WriteOptions(), kstr, vstr.str());
    if (!s.ok()) {
			cout << "[ERROR] INSERT key: " << kstr << " value: " << vstr.str() << endl;
			cout << s.ToString() << endl;
		} else {
			// if success increase count
			insert_count.fetch_add(1);
		}
		int thinkCount = 0;
                while(thinkCount < 10000){
                        thinkCount++;
                }

		/* if (count % 1000000 == 0) {
			cout << count << endl;
		} */
	}
	db->Flush(FlushOptions());
	cout << endl;
}

void loadDB(DB* db) {
	cout << "Begin load for: " << num_records << " entries" << endl;
	for(uint64_t count = 0; count < num_records; count++) {
		string kstr = getInsertKeyStr();

		std::ostringstream vstr;
		for (uint32_t i = 0; i < column_count; i++) {
			int v = rand() % 1000 + 1;
			vstr << std::setfill('0') << std::setw(column_width) << v << "|";
		}
		if (DEBUG) {
			cout << "key: " << kstr << " value: " << vstr.str() << endl;
		}
		Status s = db->Put(WriteOptions(), kstr, vstr.str());
    if (!s.ok()) {
			cout << "[ERROR] INSERT key: " << kstr << " value: " << vstr.str() << endl;
			cout << s.ToString() << endl;
		} else {
			// if success increase count
			insert_count.fetch_add(1);
		}
		/* if (count % 1000000 == 0) {
			cout << count << endl;
		} */
	}
	db->Flush(FlushOptions());
	cout << endl;
}

void read(DB* db, vector<uint32_t> _read_filter, double _norm_mean) {
	string kstr = getKeyStr(getNextReadKeyNormal(_norm_mean));
	// DEBUG
	// cout << "norm-mean:" << _norm_mean << endl;
	// cout << " read-filter: ";
	// for (uint32_t i = 0; i < _read_filter.size(); i++) {
	// 	cout << _read_filter[i] << " ";
	// }
	// cout << endl;

	ReadOptions ro;
	ro.filter_columns = _read_filter;
	string get_val;
	auto start = std::chrono::high_resolution_clock::now();
	Status s = db->Get(ro, kstr, &get_val);
  if (!s.ok()) {
		cout << s.ToString() << endl;
		cout << "[ERROR] READ: " << kstr << endl;
	} else {
		auto elapsed = std::chrono::high_resolution_clock::now() - start;
		long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
		storeReadLatency(microseconds);
		// if success increase count
		read_count.fetch_add(1);
	}
	if (DEBUG) {
		cout << "READ: " << kstr << " - " << get_val << endl;
		this_thread::sleep_for(chrono::milliseconds(100));
	}
	// sleep so that read ops are spaced. We are only interested in the latency, not throughput
	//this_thread::sleep_for(chrono::milliseconds(100));
}

void scan(DB* db, vector<uint32_t> _scan_filter, float _scan_length) {
	uint64_t count = 0;
	string kstr = getKeyStr(getNextScanKey());

	//vector<uint32_t> scan_filter = {1, 2, 3, 4, 5};
	/*for (uint32_t col = 1; col <= num_scan_cols; col++) {
		//uint32_t rand_c = (uint32_t) (rand() % (column_count-1) + 1);
		//scan_filter.push_back(rand_c);
		scan_filter.push_back(col);
	}*/

	// DEBUG
	// cout << " scan_length: " << _scan_length << endl;
	// cout << " scan-filter: ";
	// for (uint32_t i = 0; i < _scan_filter.size(); i++) {
	// 	cout << _scan_filter[i] << " ";
	// }
	// cout << endl;

	ReadOptions ro;
	ro.filter_columns = _scan_filter;
	Iterator *it = db->NewIterator(ro);
	auto start = std::chrono::high_resolution_clock::now();
	it->Seek(kstr);
	//it->SeekToFirst();
	uint64_t scan_amount = static_cast<uint64_t>(_scan_length*key_counter.load());
	cout << "Column: " << _scan_filter[0] << " Scanning for " << scan_amount << " entries out of " << key_counter.load() << " from key: " << kstr << endl;
	while(it->Valid() && count < scan_amount) {
		//string val = it->value().ToString();
		//cout << val << endl;
		count++;
		it->Next();
	}
	if (count == scan_amount) {
		auto elapsed = std::chrono::high_resolution_clock::now() - start;
		long long milliseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / 1000;
		storeScanLatency(milliseconds);
		cout << "scan latency(ms): " << milliseconds << endl;
		// if success increase count
		scan_count.fetch_add(1);
	}

	if (DEBUG) {
		cout << "SCAN: " << kstr << " - " << count << endl;
		this_thread::sleep_for(chrono::milliseconds(500));
	}
}

void insert(DB* db) {
	// keep inserting till read thread finishes its operations
	while(key_counter.load() < stage2_inserts) {
		string kstr = getInsertKeyStr();
		std::ostringstream vstr;
		for (uint32_t i = 0; i < column_count; i++) {
			int v = rand() % 1000 + 1;
			vstr << std::setfill('0') << std::setw(column_width) << v << "|";
		}
		if (DEBUG) {
			cout << "INSERT key: " << kstr << " value: " << vstr.str() << endl;
			this_thread::sleep_for(chrono::milliseconds(100));
		}

		Status s = db->Put(WriteOptions(), kstr, vstr.str());
		if (!s.ok()) {
			cout << "[ERROR] INSERT key: " << kstr << " value: " << vstr.str() << endl;
			cout << s.ToString() << endl;
		} else {
			// if success increase count
			insert_count.fetch_add(1);
		}
		int thinkCount = 0;
		//while(thinkCount < 8000){
		//	thinkCount++;
		//}
		int sleep_interval = 1000000;
		int sleep_time = 10;
		if(key_counter.load() > 0.3*stage2_inserts)
			sleep_interval = 500000;
		if(key_counter.load() > 0.6*stage2_inserts)
			sleep_interval = 300000;
		if(key_counter.load() > 0.75*stage2_inserts)
			sleep_interval = 100000;
		if(key_counter.load() > 0.91*stage2_inserts)
			sleep_interval = 50000;
	
		if (key_counter.load() % sleep_interval == 0) {
			//if (key_counter.load() > 0.75*stage2_inserts)
			//	sleep_time = 15*3;
			this_thread::sleep_for(chrono::seconds(sleep_time));
			cout << "pausing inserts for " << sleep_time << " secs, current count: " << key_counter.load() << endl;
		}
	}
	
}

void update(DB* db) {
	string kstr = getKeyStr(getNextUpdateKey());
	ReadOptions ro;
	ro.filter_columns = read_filter;
	string get_val;
	auto t1 = std::chrono::high_resolution_clock::now();
	Status s = db->Get(ro, kstr, &get_val);
  if (!s.ok()) {
		cout << "[ERROR] READ: " << kstr << endl;
		cout << s.ToString() << endl;
		return;
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	std::ostringstream vstr;
	for (uint32_t i = 1; i <= column_count; i++) {
		if(update_cols.find(i) != update_cols.end()) {
			int v = rand() % 1000 + 1;
			vstr << std::setfill('0') << std::setw(column_width) << v;
		}
		vstr << "|";
	}

	if (DEBUG) {
		cout << "UPDATE: " << kstr << " - " << vstr.str() << endl;
		//this_thread::sleep_for(chrono::milliseconds(200));
	}
	auto t3 = std::chrono::high_resolution_clock::now();
	s = db->Merge(WriteOptions(), kstr, vstr.str());
  if (!s.ok()) {
		cout << "[ERROR] UPDATE: " << kstr << " - " << vstr.str() << endl;
		cout << s.ToString() << endl;
	} else {
		auto t4 = std::chrono::high_resolution_clock::now();
		auto elapsed = t4-t3 + t2-t1;
		long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
		storeUpdateLatency(microseconds);
		// if success increase count
		update_count.fetch_add(1);
	}
}

void runWorkload(DB* db) {
	//std::discrete_distribution<> dist({ read_proportion, scan_proportion, update_proportion });
	uint64_t scan_ops_count = 9999999;
	uint64_t update_ops_count = 9999999;
	if(scan_proportion != 0) {
		scan_ops_count = static_cast<uint64_t>(1.0/scan_proportion)+1;
	}
	if(update_proportion != 0) { 
	 update_ops_count = static_cast<uint64_t>(1.0/update_proportion)+1;
	}
	cout << "Scan interval: " << scan_ops_count << " Update interval: " << update_ops_count << endl;
	std::mt19937 eng(std::time(0));

	rocksdb::SetPerfLevel(rocksdb::PerfLevel::kEnableTimeExceptForMutex);
	rocksdb:: get_perf_context()->Reset();
	rocksdb::get_iostats_context()->Reset();
	
	auto t1 = std::chrono::high_resolution_clock::now();
	uint64_t curr_num_operations = getNextOpCount();
	while( curr_num_operations < num_operations) {
		/*int op = dist(eng);
		if (op == 0) {
			read(db);
		} else if (op == 1) {
			scan(db);
		} else if (op == 2) {
			update(db);
		} else {
			cout << "WRONG OPERATION " << op << endl;
		}*/
		// if (scan_ops_count != 9999999 && curr_num_operations % scan_ops_count == 0) {
		// 	scan(db);
		// }
		if (update_ops_count != 9999999 && curr_num_operations % update_ops_count == 0) {
			update(db);
		}
		if(curr_num_operations % 2 == 0) {
			double _norm_mean = 0.98 - read_offset;
			vector<uint32_t> _read_filter;
			for (uint32_t col = 1; col <= 30; col++) {
				_read_filter.push_back(col);
			}
			read(db, _read_filter, _norm_mean);
		} else {
			double _norm_mean = 0.85 - read_offset;
			vector<uint32_t> _read_filter;
			for (uint32_t col = 15; col <= 30; col++) {
				_read_filter.push_back(col);
			}
			read(db, _read_filter, _norm_mean);
		}
		
		curr_num_operations = getNextOpCount();
		if(curr_num_operations % 5000 == 0) {
			cout << "sleeping every 5000 ops" << endl;
			this_thread::sleep_for(chrono::seconds(30));
		}
	}
	// do scan ops at the end
	uint32_t scan_count = 0;
	while(scan_count > 0) {
		if(key_counter.load() < stage2_inserts) {
			this_thread::sleep_for(chrono::seconds(10));
			continue;
		}
		if(scan_count % 2 == 0) {
			float _scan_length = 0.05;
			int start = 21;
			int end = 30;
			vector<uint32_t> _scan_filter;
			for (uint32_t col = start; col <= end; col++) {
				_scan_filter.push_back(col);
			}
			scan(db, _scan_filter, _scan_length);
		} else {
			float _scan_length = 0.5;
			int start = 28;
                        int end = 30;
                        int offset = proj_offset;
			vector<uint32_t> _scan_filter;
			for (uint32_t col = start-offset; col <= end-offset; col++) {
				_scan_filter.push_back(col);
			}
			scan(db, _scan_filter, _scan_length);
		}
		scan_count--;
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	auto elapsed = t2-t1;
	long long seconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (1000*1000);
	cout << " Workload time thread-internal(secs): " << seconds << endl;

	rocksdb::SetPerfLevel(rocksdb::PerfLevel::kDisable);
	cout << rocksdb::get_perf_context()->ToString(true) << endl;
	cout << rocksdb::get_iostats_context()->ToString(true) << endl;
}

void runUpdateWorkload(DB* db) {
  while(getNextOpCount() < num_operations) {
    update(db);
    update(db);
    update(db);
  }
}

int main(int argc, char** argv) {
	if (argc < 6) {
		cout << "Usage: ./benchmark <config-file> <mode 'load'/'run'> <k> <dbpath> <disable compaction 'true'/'false'>" << endl;
		return 0;
	}
	string config_file 	= argv[1];
	string mode					=	argv[2];
	
	if(!parseConfig(config_file)) {
		return 0;
	}
	//column_from_k = static_cast<uint32_t>(std::stoul(argv[3]));
	//proj_offset = static_cast<uint32_t>(std::stoul(argv[3]));
	//cout << "proj offset: " << proj_offset << endl;
	read_offset = static_cast<double>(std::stod(argv[3]));
	cout << "read offset: " << read_offset << endl;

	db_path = argv[4];
	string dc = argv[5];
	if (dc == "true") {
		disable_compaction = true;
	} else if (dc == "false") {
		disable_compaction = false;
	}
	cout << "column_from_k: " << column_from_k << endl;
	cout << "db_path: " << db_path << endl;
	cout << "disable_compaction: " << disable_compaction << endl;

	for (uint32_t col = 1; col <= num_read_cols; col++) {
		read_filter.push_back(col);
	}
	
	cout << " read-filter: ";
	for (uint32_t i = 0; i < read_filter.size(); i++) {
		cout << read_filter[i] << " ";
	}
	cout << endl;
	

	for (uint32_t col = 1; col <= num_scan_cols; col++) {
		scan_filter.push_back(col);
	}
	
	cout << " scan-filter: ";
	for (uint32_t i = 0; i < scan_filter.size(); i++) {
		cout << scan_filter[i] << " ";
	}
	cout << endl;
	

	/* unordered_set<uint32_t> test_set;
	uint64_t i = 0;
	while (i < num_records) {
		uint32_t hashed = myhash((uint32_t) getNextKey());
		//if(test_set.find(hashed) == test_set.end()) {
			test_set.insert(hashed);
		//}
		i++;
		if(test_set.size() % 10000000 == 0) {
			cout << test_set.size() << endl;
		}
	}
	cout << "inserted: " << num_records << " set has: " << test_set.size() << " counter: " << key_counter.load() << endl;
	return 0; */
	
	DB* db = initDB();
	skew_key_gen = new ZipfianGenerator(num_records);
	if(mode == "load") {
		cout << "mode: load" << endl;
		auto t1 = std::chrono::high_resolution_clock::now();
		std::thread t[2];
		//t[0] = std::thread(loadDBWithTime, db);
		t[0] = std::thread(loadDB, db);
		t[1] = std::thread(printWriteThroughput);
		t[0].join();
		t[1].join();
		auto t2 = std::chrono::high_resolution_clock::now();
		auto elapsed = t2-t1;
		long long seconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (1000*1000);
		cout << " Load time(secs): " << seconds << endl;
	} else if(mode == "run") {
		cout << "mode: run" << endl;
		// load keys
		/* while (key_set.size() < num_records) {
			uint32_t hashed = myhash((uint32_t) getNextKey());
			if(key_set.find(hashed) == key_set.end()) {
				key_set.insert(hashed);
				key_vec.push_back(hashed);
			}
		} */
		key_counter.store(num_records);
		std::thread t[num_threads+2];

		// start insert thread
		t[num_threads] = std::thread(printThroughput);
		if(inserts_with_reads) {
			t[num_threads+1] = std::thread(insert, db);
			cout << "Give inserts a head start. Sleeping till writes reach: "<< stage1_inserts << endl;
			while(key_counter.load() < stage1_inserts) {
				this_thread::sleep_for(chrono::seconds(30));
			}
			cout << "Starting workload. Inserted so far: "<< key_counter.load() << endl;
		}
		auto t1 = std::chrono::high_resolution_clock::now();
		//Launch a group of threads for read/scan/update
		for (int i = 0; i < num_threads; ++i) {
				t[i] = std::thread(runWorkload, db);
		}
		//Join the threads with the main thread
		for (int i = 0; i < num_threads; ++i) {
			t[i].join();
		}
		t[num_threads].join();
		if(inserts_with_reads) {
			t[num_threads+1].join();
		}
		auto t2 = std::chrono::high_resolution_clock::now();
		auto elapsed = t2-t1;
		long long seconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (1000*1000);
		cout << " Workload time(secs): " << seconds << endl;
		printLatencyStats();
	} else if(mode == "compact") {
		cout << "mode: compact" << endl;
		// given 20 minutes for compaction
		cout << "Sleeping for (secs):" << 18000 << endl;
		this_thread::sleep_for(chrono::seconds(18000));
	} else if(mode == "update") {
    cout << "mode: update" << endl;
    key_counter.store(num_records);
    std::thread t[3];
    t[0] = std::thread(runUpdateWorkload, db);
    t[1] = std::thread(insert, db);
    t[2] = std::thread(printThroughput);

    t[0].join();
    t[1].join();
    t[2].join();
    printLatencyStats();
  }
	
	return 0;
}
