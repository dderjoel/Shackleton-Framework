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
                 
OBJS := $(SRC_FILES:c=o)

all: osaka ensure_directories

osaka: shackleton
shackleton: $(OBJS)
	cc -o ${@} $(^) $(DIR)/main.c
	cp ${@} $(DIR)/bin/init

ensure_directories:
	mkdir -p ./src/files/llvm/junk_output

%.o: %.c
	cc -c $^ -o $@

clean :
	rm -rf $(OBJS) shackleton $(DIR)/bin/init
