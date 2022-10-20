 ################################################################################ \
 Name        : makefile															  \
 Author      : Hannah M. Peeler													  \
 Version     : 1.0																  \
 Copyright   : 																	  \
																				  \
    Copyright 2019 Arm Inc., Andrew Sloss, Hannah Peeler						  \
																				  \
    Licensed under the Apache License, Version 2.0 (the "License");				  \
    you may not use this file except in compliance with the License.			  \
    You may obtain a copy of the License at										  \
																				  \
       http://www.apache.org/licenses/LICENSE-2.0								  \
																				  \
    Unless required by applicable law or agreed to in writing, software			  \
    distributed under the License is distributed on an "AS IS" BASIS,			  \
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.	  \
    See the License for the specific language governing permissions and			  \
    limitations under the License.												  \
																				  \
    Please refer to 															  \
    https://github.com/ARM-software/Shackleton-Framework/blob/master/LICENSE.TXT  \
    for a full overview of the license covering this work.						  \
																				  \
 Description : Makefile for main of Shackleton Framwork							  \
 #################################################################################


.PHONY: ensure_directories clean osaka
.PRECIOUS: shackleton

DIR := .
SRCDIR := $(DIR)/src

SRC_FILES := \
	$(SRCDIR)/evolution/crossover.c \
	$(SRCDIR)/evolution/evolution.c \
	$(SRCDIR)/evolution/fitness.c \
	$(SRCDIR)/evolution/generation.c \
	$(SRCDIR)/evolution/indivdata.c \
	$(SRCDIR)/evolution/mutation.c \
	$(SRCDIR)/evolution/optsequence.c \
	$(SRCDIR)/evolution/selection.c \
	$(SRCDIR)/module/assembler.c \
	$(SRCDIR)/module/binary_up_to_512.c \
	$(SRCDIR)/module/gi_llvm_pass.c \
	$(SRCDIR)/module/llvm_pass.c \
	$(SRCDIR)/module/modules.c \
	$(SRCDIR)/module/osaka_string.c \
	$(SRCDIR)/module/simple.c \
	$(SRCDIR)/osaka/osaka.c \
	$(SRCDIR)/osaka/osaka_test.c \
	$(SRCDIR)/support/cJSON.c \
	$(SRCDIR)/support/cache.c \
	$(SRCDIR)/support/llvm.c \
	$(SRCDIR)/support/test.c \
	$(SRCDIR)/support/utility.c \
	$(SRCDIR)/support/visualization.c \
	$(SRCDIR)/support/measure.c \
	$(SRCDIR)/support/build_llvm.c \
	$(SRCDIR)/passes/llvm_pass_dflt.c
                 
OBJS     := $(SRC_FILES:c=o)
LIB_MS   := ./MeasureSuite/libmeasuresuite.a
INCLUDES := -I src -I ./MeasureSuite/src/include

#default function number
FUN_NUM  ?= 0

all: osaka ensure_directories

osaka: shackleton

shackleton: $(OBJS) $(LIB_MS)
	cc -g $(INCLUDES) -o ${@} $(^) $(DIR)/main.c -L ./MeasureSuite -l:libmeasuresuite.a -ldl -lm 

$(LIB_MS):
	make -C ./MeasureSuite/ NO_AL=1 libmeasuresuite.a -B

ensure_directories:
	mkdir -p ./src/files/llvm/junk_output

%.o: %.c makefile
	cc -g $(INCLUDES) -c $< -o $@

clang: LLVM_BUILD=./llvm-project/build
clang:
	mkdir -p $(LLVM_BUILD)
	cd $(LLVM_BUILD) && cmake -DLLVM_ENABLE_PROJECTS=clang -DCMAKE_BUILD_TYPE=Release -GNinja ../llvm
	cd $(LLVM_BUILD) && ninja

clean:
	rm -rf $(OBJS) shackleton $(DIR)/bin/init

./src/files/params/fiat$(FUN_NUM).txt:
	cd ./src/files/params && ./fiat_gen.sh $(FUN_NUM)

run: shackleton ./src/files/params/fiat$(FUN_NUM).txt
	echo n | PATH=$(LLVM_BUILD)/bin/:$${PATH} ./shackleton \
		-parameters_file=fiat$(FUN_NUM).txt \
		-test_file=mwe-fiat/main.c \
		-source_file=fiat.txt \
		-obj_type=6 \
		-cache \
		-id fiat$(FUN_NUM)

