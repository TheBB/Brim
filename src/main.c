#include <string.h>
#include <stdio.h>

#include "namespace.h"
#include "opcodes.h"
#include "compiler.h"
#include "runtime.h"
#include "vm.h"


int main(int argc, char** argv)
{
    brim_new_frame();

    if (argc > 1) {
        // Create a string from the argument and push it to the stack
        brim_push(brim_make_string(argv[1], strlen(argv[1])));

        // Call read, top of stack should now be the parsed lisp object, or undefined in case of error
        brim_read();

        // Print parsed result
        brim_duplicate();
        brim_display();
        printf("\n");
        brim_pop();

        // Analyze
        transform(brim_peek(0));

        // brim_print_stack();

        // // Compile to bytecode object
        // brim_compile();

        // // Print bytecode object
        // brim_duplicate();
        // brim_display();
        // printf("\n");
        // brim_pop();

        // // Evaluate bytecode object
        // brim_eval();

        // // Stack should have just one object: the result
        // brim_print_stack();
    }

    brim_free_frame();
    return 0;
}
