SRIPATH ?=/Users/sabrina/Desktop/srilm-1.5.10
MACHINE_TYPE ?=macosx

CXX=/usr/bin/g++
CXXFLAGS=-O3 -I$(SRIPATH)/include -w --std=c++11

vpath lib%.a $(SRIPATH)/lib/$(MACHINE_TYPE)

PITCH_FILE = pitch.txt
BIG_MAP = big_map.txt

TARGET = zhuyin_decode
SRC = zhuyin_decode.cpp
OBJ = $(SRC:.cpp=.o)

TARGET_PER = calculate_perplexity
SRC_PER = calculate_perplexity.cpp
OBJ_PER = $(SRC_PER:.cpp=.o)

.PHONY: all clean map run run_compare run_perplexity

all: $(TARGET)

$(TARGET): $(OBJ) -loolm -ldstruct -lmisc
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<


run:
	@
	@for i in $(shell seq 1 1) ; do \
		echo "Running test.txt";\
		./zhuyin_decode finaldata/test_zhuyin.txt $(BIG_MAP) trigram_train.lm result_final/run/run_test.txt;\
	    done;

run_compare:
	@for i in $(shell seq 1 1) ; do \
		echo "Running test.txt";\
		./zhuyin_decode finaldata/test_zhuyin.txt $(BIG_MAP) trigram_ref.lm result_final/run_compare/run_compare_test.txt;\
	    done;


run_perplexity: $(OBJ_PER) -loolm -ldstruct -lmisc
	$(CXX) $(LDFLAGS) -o $(TARGET_PER) $^
	./calculate_perplexity big_corpus_test_big5.txt trigram_train.lm	


clean:
	$(RM) $(OBJ) $(TARGET)
	$(RM) $(OBJ_PER) $(TARGET_PER)
