CXX=g++-7

FILES= \
  src/json.cpp \
  src/main.cpp

CXXFLAGS=-std=c++17 -O2 -g -I src/
CXXCOVFLAGS=-std=c++17 -O0 --coverage -g -DCOVERAGE -I ../../src/

TESTFLAGS=-lmettle

TEST_EXECUTABLES = \
	test_buffered_writer \
	util/test_assert \
	util/test_write

TEST_EXECUTABLES_LOC = $(addprefix out/test/, $(TEST_EXECUTABLES))

# N.B.: The way coverage works currently, you can't have two test targets with
# the same name, even if they are in different directories.
TEST_EXECUTABLES_BASENAME = $(notdir $(TEST_EXECUTABLES))
TEST_COVERAGE_EXECUTABLES = $(addsuffix _cov, $(TEST_EXECUTABLES))
TEST_COVERAGE_EXECUTABLES_DEP = $(addprefix out/cov/, $(TEST_COVERAGE_EXECUTABLES))
TEST_COVERAGE_EXECUTABLES_CWD = $(addprefix ./, $(TEST_COVERAGE_EXECUTABLES))

default: test

.PHONY: all
all: test
	$(CXX) $(CXXFLAGS) $(FILES) -o main
	./main

.PHONY: test
test: $(TEST_EXECUTABLES_LOC)
	mettle $(TEST_EXECUTABLES_LOC)

.PHONY: coverage
coverage: out/cov/gcov.stdout
	python tools/check_coverage.py out/cov/gcov.stdout

out/cov/gcov.stdout: $(TEST_COVERAGE_EXECUTABLES_DEP)
	cd out/cov && rm -f *.gcda
	cd out/cov && mettle $(TEST_COVERAGE_EXECUTABLES_CWD)
	mkdir -p out/cov && cd out/cov && gcov-7 -l -r -s ../../src/ $(TEST_EXECUTABLES_BASENAME) | tee gcov.stdout

out/test/%: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -o $@ $(TESTFLAGS)

out/cov/%_cov: src/%.cpp
	@mkdir -p $(dir $@)
	cd out/cov && $(CXX) $(CXXCOVFLAGS) ../../$< -o ../../$@ $(TESTFLAGS)

clean:
	rm -f main
	rm -rf out

