#include <cstdlib>
#include "archlab.hpp"
#include <unistd.h>
#include"pin_tags.h"
#include"omp.h"
#include"papi.h"
#include <unordered_set>
#include<algorithm>
#include<cstdint>

int event_set = PAPI_NULL;
double start, stop;

#define CLINK extern "C"
#define OPT(a) __attribute__(a)

extern "C" uint64_t *__attribute__((noinline)) baseline_int(uint64_t * array, unsigned long int size) {
	//uint64_t * array = new uint64_t[size];
	for(uint i = 0; i < size; i++) {
		array[i] = 0;
	}

	for (uint j = 0; j < 3; j++) {
		for(uint i= 1 ; i < size; i++) {
			array[i] += i/(1+j)+array[i - 1];
		}
	}
	return array;
}

extern "C" uint64_t *__attribute__ ((optimize(4))) baseline_int_O4 (uint64_t * array, unsigned long int size) {
	//uint64_t * array = new uint64_t[size];
	for(uint i = 0; i < size; i++) {
		array[i] = 0;
	}

	for (uint j = 0; j < 3; j++) {
		for(uint i= 1 ; i < size; i++) {
			array[i] += i/(1+j)+array[i - 1];
		}

	}
	return array;
}

extern "C" uint64_t *baseline_double(uint64_t * _array, unsigned long int size) {
	//double * array = new double[size];
	double * array = (double*)_array;
	for(uint i = 0; i < size; i++) {
		array[i] = 0;
	}

	for (double j = 0; j < 3; j++) {
		for(uint i= 1 ; i < size; i++) {
			array[i] += i/(1+j)+array[i - 1];
		}
	}
	return (uint64_t*)array;
}

extern "C" uint64_t *baseline_float(uint64_t * _array, unsigned long int size) {
	//double * array = new double[size];
	float * array = (float*)_array;
	for(uint i = 0; i < size; i++) {
		array[i] = 0;
	}

	for (double j = 0; j < 3; j++) {
		for(uint i= 1 ; i < size; i++) {
			array[i] += i/(1+j)+array[i - 1];
		}
	}
	return (uint64_t*)array;
}

extern "C" uint64_t *baseline_char(uint64_t * _array, unsigned long int size) {
	//double * array = new double[size];
	char * array = (char*)_array;
	for(uint i = 0; i < size; i++) {
		array[i] = 0;
	}

	for (double j = 0; j < 3; j++) {
		for(uint i= 1 ; i < size; i++) {
			array[i] += i/(1+j)+array[i - 1];
		}
	}
	return (uint64_t*)array;
}

extern "C" uint64_t *__attribute__ ((optimize(4),noinline)) baseline_double_O4(uint64_t * _array, unsigned long int size) {
	//double * array = new double[size];
	double * array = (double*)_array;
	for(uint i = 0; i < size; i++) {
		array[i] = 0;
	}

	for (double j = 0; j < 3; j++) {
		for(uint i= 1 ; i < size; i++) {
			array[i] += i/(1+j)+array[i - 1];
		}
	}
	return (uint64_t*)array;
}


volatile int ROW_SIZE = 1024;
extern "C" uint64_t *__attribute__ ((optimize(4))) matrix_column_major(uint64_t * _array, unsigned long int size) {
#define ROW_SIZE 1024
	double * array = (double*)_array;

	for (int k = 0; k < ROW_SIZE; k++) {
		for(uint i= 0 ; i < size/ROW_SIZE; i++) {
			array[i*ROW_SIZE + k] += 1.0; // This Line
		}
	}
	
	return (uint64_t*)array;
}


//int ROW_SIZE = 1024;
extern "C" uint64_t *__attribute__ ((optimize(4))) matrix_row_major(uint64_t * _array, unsigned long int size) {

	double * array = (double*)_array;

	for(uint i= 0; i < size/ROW_SIZE; i++) {
		for (int k = 0; k < ROW_SIZE; k++) {
			array[i*ROW_SIZE + k] += 1.0; // This Line
		}
	}
	
	return (uint64_t*)array;
}

extern "C" uint64_t *__attribute__ ((optimize(0))) everything(uint64_t * array, unsigned long int size) {
	matrix_column_major(array, size);
	baseline_int(array,size);
	return array;
}

extern "C" uint64_t *__attribute__ ((optimize(0))) everything_opt(uint64_t * array, unsigned long int size) {
	matrix_column_major(array, size);
	baseline_int_O4(array,size);
	return array;
}

extern "C" uint64_t *__attribute__ ((optimize(0))) option_1(uint64_t * array, unsigned long int size) {
	matrix_column_major(array, size);
	baseline_int_O4(array,size);
	return array;
}

extern "C" uint64_t *__attribute__ ((optimize(0))) option_2(uint64_t * array, unsigned long int size) {

	matrix_row_major(array, size);
	baseline_int(array,size);
	return array;
}

uint64_t * (*power_virus)(uint64_t * array, unsigned long int size) = NULL;

// There's a lot of really unsafe multithreading going on here.  Don't ever write code like this...It will fail in the real world --Professor Swanson
//uint64_t * power_virus_array = NULL;
uint array_size;
unsigned long int  power_virus_size = 0;
unsigned int power_virus_reps = 0;
int running_viruses = 0;
void *go_power_virus(void*arg) {
	uint64_t delay = (uint64_t)arg;

	uint64_t * power_virus_array = NULL;
	power_virus_array = new uint64_t[array_size]; // and 256 because we matrix_* needs a little extra space
	for(uint i = 0; i < array_size; i++) {
		power_virus_array[i] = 0;
	}
	sleep(delay);
	running_viruses++;
	for(uint i = 0; i < power_virus_reps; i++) {
		power_virus(power_virus_array, power_virus_size);
	}
	return NULL;
}

int main(int argc, char *argv[])
{

	
	
	std::vector<int> mhz_s;
	std::vector<int> default_mhz;
	load_frequencies();
	default_mhz.push_back(cpu_frequencies_array[1]);
	std::stringstream clocks;
	for(int i =0; cpu_frequencies_array[i] != 0; i++) {
		clocks << cpu_frequencies_array[i] << " ";
	}
	std::stringstream fastest;
	fastest << cpu_frequencies_array[0];

	archlab_add_multi_option<std::vector<int> >("MHz",
						    mhz_s,
						    default_mhz,
						    fastest.str(),
						    "Which clock rate to run.  Possibilities on this machine are: " + clocks.str());

	std::vector<std::string> functions;
	std::vector<std::string> default_functions;

	default_functions.push_back("baseline_int");
	archlab_add_multi_option<std::vector<std::string>>("function",
							   functions,
							   default_functions,
							   "baseline_int",
							   "Which versions to run");

	std::vector<unsigned long int> sizes;
	std::vector<unsigned long int> default_sizes;
	default_sizes.push_back(1024*1024);
	archlab_add_multi_option<std::vector<unsigned long int> >("size",
					      sizes,
					      default_sizes,
					      "1024*1024",
					      "Size.  Pass multiple values to run with multiple sizes.");

	unsigned long int reps;
	archlab_add_option<unsigned long int >("reps",
					       reps,
					       1,
					       "1",
					       "How many times to repeat the experiment.");

	unsigned long int power_virus_threads;
	archlab_add_option<unsigned long int >("power-viruses",
					       power_virus_threads,
					       0,
					       "0",
					       "How many power virus threads to start");

	archlab_parse_cmd_line(&argc, argv);


	std::map<const std::string, uint64_t*(*)(uint64_t *, unsigned long int)>
		function_map =
		{
#define FUNCTION(n) {#n, n}
			FUNCTION(baseline_int),
			FUNCTION(baseline_int_O4),
			FUNCTION(baseline_double),
			FUNCTION(baseline_float),
			FUNCTION(baseline_char),
			FUNCTION(baseline_double_O4),
			FUNCTION(matrix_row_major),
			FUNCTION(matrix_column_major),
			FUNCTION(everything),
			FUNCTION(option_1),
			FUNCTION(option_2),
			FUNCTION(everything_opt)
		};

	power_virus = function_map[functions[0]];
	array_size  = *std::max_element(sizes.begin(), sizes.end())+256;
	uint64_t * array = new uint64_t[array_size]; // and 256 because we matrix_* needs a little extra space
	for(uint i = 0; i < array_size; i++) {
		array[i] = 0;
	}

	if (power_virus_threads > 0) {
		// power_virus_array = new uint64_t[l]; // and 256 because we matrix_* needs a little extra space
		// for(uint i = 0; i < l; i++) {
		// 	power_virus_array[i] = 0;
		// }
		power_virus_reps = reps;

		for (unsigned long int v = 0; v < power_virus_threads; v++) {
			pthread_t thread;
			pthread_create(&thread, NULL, go_power_virus, (void*)(v+3));
		}
	}

	theDataCollector->disable_prefetcher();			
	for(auto &mhz: mhz_s) {
		set_cpu_clock_frequency(mhz);
		for(auto & size: sizes ) {
			power_virus_size = size;
			for(uint r = 0; r < reps; r++) {
				for(auto & function : functions) {
					//pristine_machine();					
					theDataCollector->register_tag("size",size);
					START_TRACE();
					{								
						ArchLabTimer timer;					
						flush_caches();
						//enable_prefetcher();
						//set_cpu_clock_frequency(cpu_frequencies[0]);
						NEW_TRACE(function.c_str());					
						timer.attr("function", function.c_str());				
						timer.attr("rep", r);
						timer.attr("cmdlineMHz", mhz);
						timer.attr("viruses",running_viruses);
						timer.go();						
						DUMP_START_ALL(function.c_str(), false);
						if (function_map.find(function) == function_map.end()) {
							std::cerr << "Unknown function: " << function <<"\n";
							exit(1);
						}
//						uint * t =
						function_map[function](array, size);					
						DUMP_STOP(function.c_str());					
						//if (t)
							//delete t;						
					}								
				}
			}
		}
	}
	delete [] array;
	archlab_write_stats();
	std::cout << "Execution complete\n";
	return 0;
}
