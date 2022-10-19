.SUFFIXES: 

default: 

PRIVATE_FILES=Lab.key.ipynb admin .git Moneta.md Lab-equinix.key.ipynb

STUDENT_EDITABLE_FILES=hello_world2.cpp

.PHONY: create-labs
create-labs:
	cse142 lab delete -f intro-bench
	cse142 lab create --name "Lab 1: The Performance Equation (Benchmark)" --short-name "intro-bench" --docker-image stevenjswanson/cse142l-swanson-runner:latest --starter-repo https://github.com/NVSL/CSE141pp-Lab-The-PE.git --starter-branch fa21-starter --execution-time-limit 0:05:00 --total-time-limit 1:00:00 --due-date 2021-10-12T23:59:59

	cse142 lab delete -f intro
	cse142 lab create --name "Lab 1: The Performance Equation" --short-name "intro" --docker-image stevenjswanson/cse142l-swanson-runner:latest --execution-time-limit 0:05:00 --total-time-limit 1:00:00 --due-date 2021-10-12T23:59:59

COMPILER=gcc-8
include $(ARCHLAB_ROOT)/cse141.make

$(BUILD)microbench.o: OPTIMIZE=$(MICROBENCH_OPTIMIZE)
$(BUILD)microbench.s: OPTIMIZE=$(MICROBENCH_OPTIMIZE)

microbench.exe: $(BUILD)microbench.o
hello_world.exe: $(BUILD)hello_world.o
hello_world2.exe: $(BUILD)hello_world2.o
.PHONY: autograde

autograde: hello_world2.exe
	./hello_world2.exe

bitcount.exe: $(BUILD)bitcount.o

#test
#test2
