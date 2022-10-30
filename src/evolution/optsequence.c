/*
 ============================================================================
 Name        : fitness.c
 Author      : Hannah M. Peeler
 Version     : 1.0
 Copyright   : 
 
    Copyright 2019 Arm Inc., Andrew Sloss, Hannah Peeler

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Please refer to 
    https://github.com/ARM-software/Shackleton-Framework/blob/master/LICENSE.TXT
    for a full overview of the license covering this work.
    
 Description : Main and helper functions for determining the fitness
               of an individual and using that fitness value within
               the evolution process
 ============================================================================
 */

/*
 * IMPORT
 */

#include "optsequence.h"

//char defaultO3[86][50] = {"-licm", "-jump-threading", "-memoryssa", "-early-cse-memssa", "-memdep", "-tbaa", "-bdce", "-instsimplify", "-loop-sink", "-strip-dead-prototypes", "-aa", "-argpromotion", "-callsite-splitting", "-aggressive-instcombine", "-scalar-evolution", "-transform-warning", "-phi-values", "-early-cse", "-block-freq", "-gvn", "-verify", "-globaldce", "-instcombine", "-correlated-propagation", "-prune-eh", "-postdomtree", "-mldst-motion", "-loop-unswitch", "-elim-avail-extern", "-ipsccp", "-slp-vectorizer", "-branch-prob", "-loop-unroll", "-basicaa", "-lower-expect", "-basiccg", "-loop-distribute", "-sccp", "-inline", "-functionattrs", "-lazy-branch-prob", "-adce", "-float2int", "-alignment-from-assumptions", "-inferattrs", "-profile-summary-info", "-lazy-value-info", "-rpo-functionattrs", "-globals-aa", "-constmerge", "-reassociate", "-libcalls-shrinkwrap", "-dse", "-loop-vectorize", "-domtree", "-mem2reg", "-opt-remark-emitter", "-loop-simplify", "-loop-idiom", "-pgo-memop-opt", "-lcssa-verification", "-called-value-propagation", "-demanded-bits", "-div-rem-pairs", "-forceattrs", "-globalopt", "-indvars", "-simplifycfg", "-tti", "-memcpyopt", "-loop-rotate", "-targetlibinfo", "-barrier", "-lcssa", "-loop-deletion", "-loop-load-elim", "-sroa", "-lazy-block-freq", "-ee-instrument", "-loops", "-assumption-cache-tracker", "-loop-accesses", "-deadargelim", "-scoped-noalias", "-speculative-execution", "-tailcallelim"};

optseq* seq_from_default(char** org_seq, osaka_object_typ ot, int default_len) {
    struct optseq* seq = (optseq*) malloc(sizeof(optseq));
    struct linkedListNode *head = NULL;
    int seq_len = default_len;
    int i;
    for (i = 0; i < seq_len; i++) {
        head = append(head, org_seq[i]);
    }
    seq->length = seq_len;
    seq->seqList = head;
    seq->ot = ot;
    return seq;
}

char** get_default(int num_levels, int target_default) {
    int* default_string_lengths = gi_llvm_default_level_string_lengths(num_levels);
    char*** default_strings = gi_llvm_set_default_level_strings(default_string_lengths, num_levels);
    return default_strings[target_default];
}

int get_default_length(int num_levels, int target_default) {
    int* default_string_lengths = gi_llvm_default_level_string_lengths(num_levels);
    return default_string_lengths[target_default];
}

char* seq_str_fitness(node_str* indiv) {
    int num_levels = 7;
    int target_default = 4;
    //char** defaultO3 = get_default(num_levels, target_default);
    //optseq* seq = seq_from_default(defaultO3, indiv->objtype);
    int len = 0;
    return gen_seq_str(indiv, &len);
}

char* gen_seq_str(node_str* indiv, int* seq_length) {
    int num_levels = 7;      // O0, O1, O2, O3, Os, Oz
    int target_default = 3;  // O3
    //visualization_print_individual_concise_details(indiv);
    char** default_str = get_default(num_levels, target_default);
    int default_len = get_default_length(num_levels, target_default);
    optseq* seq = seq_from_default(default_str, indiv->objtype, default_len);

    while (NEXT(indiv) != NULL){
        seq = mutate(seq, indiv);
        indiv = NEXT(indiv);
    }

    *seq_length = seq->length;
    char* seq_str = malloc(20000);
    strcpy(seq_str, "");
    linkedListNode* head = seq->seqList;

    //for (int i = 0; i < seq->length; i++) {
    int total = 0;
    int valid = 0;
    while (head->next != NULL){
        char* pass_name = head->data;
        linkedListNode* temp = head;
        head = head->next;
        free(temp);
        //printf("check result: %d - %s\n", check_valid_pass(pass_name), pass_name);
        if (check_valid_pass(pass_name)) {
            strcat(seq_str, pass_name);
            strcat(seq_str, " ");
            //printf("apppended\n");
            valid++;
        }
        total++;
    }
    free(seq);
    //printf("AFTER sequence: %s\nseq->length: %d, total: %d, valid: %d\n\n", seq_str, seq->length, total, valid);
    return seq_str;
}

optseq* mutate(optseq* seq, node_str* indiv) {
    char* type = gi_llvm_pass_get_type(OBJECT(indiv));
    //char* type = "replacement";
    if (strcmp(type, "insertion") == 0) {
        //printf("insertion mutation\n");
        return insert(seq, indiv);
    }
    else if (strcmp(type, "deletion") == 0) {
        //printf("deletion mutation\n");
        return delete(seq, indiv);
    }
    else if (strcmp(type, "replacement") == 0) {
        //printf("replacement mutation\n");
        return replace(seq, indiv);
    }
    printf("\nWARNING: INVALID PASS TYPE, RETURNING NULL SEQUENCE\n\n");
    return NULL; 
}


optseq* insert(optseq* seq, node_str* patch) {
    double loc = gi_llvm_pass_get_loc(OBJECT(patch));
    char* pass = gi_llvm_pass_get_pass(OBJECT(patch));

    int pos = (int)(loc*seq->length);
    if (pos == 0) {
        linkedListNode* head = newLinkedListNode(pass);
        head->next = seq->seqList;
        seq->seqList = head;
        seq->length++;
        return seq;
    }
    
    linkedListNode* current = seq->seqList;
    int counter = 0;
    while (counter < pos-1) {
        current = current->next;
        counter++;
    }

    linkedListNode* new_node = newLinkedListNode(pass);
    linkedListNode* temp = current->next;
    current->next = new_node;
    new_node->next = temp;
    seq->length++;
    return seq;
}

optseq* replace(optseq* seq, node_str* patch) {
    double loc = gi_llvm_pass_get_loc(OBJECT(patch));
    char* pass = gi_llvm_pass_get_pass(OBJECT(patch));
    int pos = (int)(loc*seq->length);
    linkedListNode* current = seq->seqList;
    int counter = 0;
    while (counter < pos) {
        current = current->next;
        counter++;
    }
    current->data = pass;
    return seq;
}

optseq* delete(optseq* seq, node_str* patch) {
    double loc = gi_llvm_pass_get_loc(OBJECT(patch));
    int pos = (int)(loc*seq->length);
    if (pos == 0) {
        linkedListNode* head = seq->seqList;
        seq->seqList = head->next;
        //free(head);
        seq->length--;
        return seq;
    }
    linkedListNode* current = seq->seqList;
    int counter = 0;
    while (counter < pos-1) {
        current = current->next;
        counter++;
    }
    linkedListNode* todelete = current->next;
    current->next = todelete->next;
    //free(todelete);
    seq->length--;
    return seq;
}

int length(linkedListNode* head) {
    int length = 0;
    linkedListNode *current;
    for(current = head; current != NULL; current = current->next) {
        length++;
    }
    return length;
}

linkedListNode* append(linkedListNode* head, char* pass) {
    linkedListNode *current = head;
    if (current == NULL) {
        //printf("current is null\n");
        return newLinkedListNode(pass);
    }
    while(current->next != NULL) {
        //printf("current: %p, current->next: %p\n", current, current->next);
        current = current->next;
    }
    //printf("current: %p, current->next: %p\n", current, current->next);
    current->next = newLinkedListNode(pass);
    //printf("finish append, current->next: %p, current->next->next: %p\n\n", current->next, current->next->next);
    return head;
}

linkedListNode* newLinkedListNode(char* pass) {
    linkedListNode *new_node = (linkedListNode*) malloc(sizeof(linkedListNode));
    new_node->data = pass;
    new_node->next = NULL;
    return new_node;
}

int get_num_valid() {
    return 82;
}

char** get_valid_passes() {
    int num_passes = get_num_valid();

    char** values = malloc(sizeof(char*) * num_passes);

    values[0] = "-aa";
    values[1] = "-adce";
    values[2] = "-aggressive-instcombine";
    values[3] = "-alignment-from-assumptions";
    values[4] = "-always-inline";
    values[5] = "-annotation2metadata";
    values[6] = "-annotation-remarks";
    values[7] = "-assumption-cache-tracker";
    values[8] = "-barrier";
    values[9] = "-basic-aa";
    values[10] = "-basiccg";
    values[11] = "-bdce";
    values[12] = "-block-freq";
    values[13] = "-branch-prob";
    values[14] = "-called-value-propagation";
    values[15] = "-callsite-splitting";
    values[16] = "-constmerge";
    values[17] = "-correlated-propagation";
    values[18] = "-deadargelim";
    values[19] = "-demanded-bits";
    values[20] = "-div-rem-pairs";
    values[21] = "-dse";
    values[22] = "-early-cse-memssa";
    values[23] = "-elim-avail-extern";
    values[24] = "-float2int";
    values[25] = "-forceattrs";
    values[26] = "-function-attrs";
    values[27] = "-globaldce";
    values[28] = "-globalopt";
    values[29] = "-globals-aa";
    values[30] = "-gvn";
    values[31] = "-indvars";
    values[32] = "-inferattrs";
    values[33] = "-inject-tli-mappings";
    values[34] = "-inline";
    values[35] = "-instcombine";
    values[36] = "-instsimplify";
    values[37] = "-ipsccp";
    values[38] = "-jump-threading";
    values[39] = "-lazy-block-freq";
    values[40] = "-lazy-branch-prob";
    values[41] = "-lcssa";
    values[42] = "-lcssa-verification";
    values[43] = "-libcalls-shrinkwrap";
    values[44] = "-licm";
    values[45] = "-loop-accesses";
    values[46] = "-loop-deletion";
    values[47] = "-loop-distribute";
    values[48] = "-loop-idiom";
    values[49] = "-loop-instsimplify";
    values[50] = "-loop-load-elim";
    values[51] = "-loop-rotate";
    values[52] = "-loop-simplify";
    values[53] = "-loop-simplifycfg";
    values[54] = "-loop-sink";
    values[55] = "-loop-unroll";
    values[56] = "-loop-vectorize";
    values[57] = "-lower-constant-intrinsics";
    values[58] = "-mem2reg";
    values[59] = "-memcpyopt";
    values[60] = "-memoryssa";
    values[61] = "-mldst-motion";
    values[62] = "-openmp-opt-cgscc";
    values[63] = "-opt-remark-emitter";
    values[64] = "-phi-values";
    values[65] = "-profile-summary-info";
    values[66] = "-reassociate";
    values[67] = "-rpo-function-attrs";
    values[68] = "-sccp";
    values[69] = "-scoped-noalias-aa";
    values[70] = "-simple-loop-unswitch";
    values[71] = "-simplifycfg";
    values[72] = "-slp-vectorizer";
    values[73] = "-speculative-execution";
    values[74] = "-sroa";
    values[75] = "-strip-dead-prototypes";
    values[76] = "-tailcallelim";
    values[77] = "-targetlibinfo";
    values[78] = "-tbaa";
    values[79] = "-transform-warning";
    values[80] = "-tti";
    values[81] = "-vector-combine";
    return values;
}

int check_valid_pass(char* data) {
    char** valid_passes = get_valid_passes();
    int num_valid = get_num_valid();
    for (int i = 0; i < num_valid; i++) {
        if (strcmp(data, valid_passes[i]) == 0) {
            //printf("data: %s, valid_passes[%d]: %s\n", data, i, valid_passes[i]);
            //printf("YES: %s\n", data);
            return 1;
        }
    }
    free(valid_passes);
    //printf("NO:  %s\n", data);
    return 0;
}