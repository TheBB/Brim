#include "types.h"


#ifndef VM_H
#define VM_H


void brim_initialize(void);
void brim_reset(void);
void brim_inhibit_collect(void);
void brim_allow_collect(void);
void brim_force_collect(void);
void brim_count(size_t* ntotal, size_t* nlive, size_t* ndead);

brimobj brim_make_intern(const char* name, size_t size);
brimobj brim_make_string(const char* data, size_t size);
brimobj brim_make_string_nullterm(const char* data);
brimobj brim_make_pair(brimobj car, brimobj cdr);
brimobj brim_make_vector(size_t size);
brimobj brim_make_bytecode();

void brim_new_frame(void);
void brim_free_frame(void);
brimobj* brim_save_stack(void);
size_t brim_stack_size(void);
void brim_restore_stack(brimobj* ptr);
void brim_push(brimobj obj);
void brim_pop_many(size_t nobjects);
brimobj brim_pop(void);
brimobj brim_pop_at(size_t index);
brimobj brim_peek(size_t index);
void brim_duplicate(void);

void brim_eval(void);


#endif /* VM_H */
