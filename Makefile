CXX=g++-7

COMMON_FILES= \
	operators/to_number.cpp \
	operators/to_string.cpp \
	json.cpp \
	jsvalue.cpp \
	pipe.cpp \
	subprocess.cpp

FILES= \
  $(COMMON_FILES) \
  main.cpp

CXXFLAGS=-std=c++17 -O2 -g -I src/ -Wall -Werror
CXXCOVFLAGS=-std=c++17 -O0 --coverage -g -DCOVERAGE -I ../../src/ -Wall -Werror

TESTFLAGS=-lmettle

TEST_EXECUTABLES = \
	operators/test_plus \
	operators/test_to_number \
	operators/test_to_string \
	test/test_log_test \
	test_buffered_writer \
	test_checked_syscalls \
	test_json \
	test_jsvalue \
	test_pipe \
	test_reader \
	test_writer \
	test_subprocess \
	util/test_assert \
	util/test_read \
	util/test_write

COMMON_OBJ=$(patsubst %.cpp, %.o, $(COMMON_FILES))
TEST_COMMON_OBJ=$(addprefix out/test/obj/, $(COMMON_OBJ))
TEST_COVERAGE_COMMON_OBJ=$(addprefix out/cov/obj/, $(COMMON_OBJ))

TEST_EXECUTABLES_LOC = $(addprefix out/test/, $(TEST_EXECUTABLES))

TEST_COVERAGE_EXECUTABLES = $(addsuffix _cov, $(TEST_EXECUTABLES))
TEST_COVERAGE_EXECUTABLES_DEP = $(addprefix out/cov/, $(TEST_COVERAGE_EXECUTABLES))
TEST_COVERAGE_EXECUTABLES_CWD = $(addprefix ./, $(TEST_COVERAGE_EXECUTABLES))

TEST_OBJ_BASE = $(TEST_EXECUTABLES) $(basename $(COMMON_FILES))

default: test

.PHONY: all
all: test
	$(CXX) $(CXXFLAGS) $(addprefix src/, $(FILES)) -o main
	./main

.PHONY: travis
travis: test coverage check-format

.PHONY: test
test: $(TEST_EXECUTABLES_LOC)
	mettle --timeout 5000 --show-terminal $(TEST_EXECUTABLES_LOC)

# Slower than make test, but should leave you in a gdb session, helps for debugging segfaults.
.PHONY: gdb
gdb: $(TEST_EXECUTABLES_LOC)
	bash -c 'for a in $(TEST_EXECUTABLES_LOC); do gdb -ex run -ex quit --args $$a --no-subproc || exit -1; done'

.PHONY: coverage
coverage: out/cov/gcov.stdout
	python tools/check_coverage.py out/cov/gcov.stdout

.PHONY: format
format:
	python tools/clang_format.py -i src/

.PHONY: check-format
check-format:
	python tools/clang_format.py src/

out/cov/gcov.stdout: $(TEST_COVERAGE_EXECUTABLES_DEP)
	cd out/cov && rm -f *.gcda
	cd out/cov && mettle $(TEST_COVERAGE_EXECUTABLES_CWD)
	mkdir -p out/cov && cd out/cov && gcov-7 -l -r -s ../../src/ $(addprefix obj/, $(TEST_EXECUTABLES)) $(patsubst %.cpp, obj/%, $(COMMON_FILES)) | tee gcov.stdout

out/test/%: out/test/obj/%.o $(TEST_COMMON_OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -o $@ $(TESTFLAGS) $(TEST_COMMON_OBJ)

out/test/obj/%.o: src/%.cpp out/test/deps/%.d
	@mkdir -p $(dir $@)
	@mkdir -p $(dir out/test/deps/$*)
	$(CXX) -c $(CXXFLAGS) $< -o $@ -MT $@ -MMD -MP -MF out/test/deps/$*.Td
	@mv out/test/deps/$*.Td out/test/deps/$*.d

out/cov/%_cov: out/cov/obj/%.o $(TEST_COVERAGE_COMMON_OBJ)
	@mkdir -p $(dir $@)
	cd out/cov && $(CXX) $(CXXCOVFLAGS) ../../$< -o ../../$@ $(TESTFLAGS) $(addprefix ../../, $(TEST_COVERAGE_COMMON_OBJ))

out/cov/obj/%.o: src/%.cpp out/cov/deps/%.d
	@mkdir -p $(dir $@)
	@mkdir -p $(dir out/cov/deps/$*)
	cd out/cov && $(CXX) -c $(CXXCOVFLAGS) ../../$< -o ../../$@ -MT ../../$@ -MMD -MP -MF ../../out/cov/deps/$*.Td
	@sed 's_\.\./\.\./_./_g' out/cov/deps/$*.Td > out/cov/deps/$*.d
	@touch $@

clean:
	rm -f main
	rm -rf out

out/test/deps/%.d: ;
.PRECIOUS: out/test/deps/%.d

out/cov/deps/%.d: ;
.PRECIOUS: out/cov/deps/%.d

.SECONDARY: $(patsubst %, out/test/obj/%.o, $(TEST_EXECUTABLES)) $(TEST_COMMON_OBJ)
.SECONDARY: $(patsubst %, out/cov/obj/%.o, $(TEST_EXECUTABLES)) $(TEST_COVERAGE_COMMON_OBJ)


include $(wildcard $(patsubst %,out/test/deps/%.d,$(TEST_OBJ_BASE)))
include $(wildcard $(patsubst %,out/cov/deps/%.d,$(TEST_OBJ_BASE)))
