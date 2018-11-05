#include "backtrace.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#define UNW_LOCAL_ONLY
#include <libunwind.h>

#include <cxxabi.h>

#include <stdlib.h>
#include <stdio.h>


BackTrace BackTrace::CreateBackTrace() {
    unw_cursor_t cursor;
    unw_context_t context;

    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    BackTrace stack;
    stack.context = context;

    while ( unw_step(&cursor) ) {
        stack.PushCursor(cursor);
    }

    return stack;
}

BackTrace::BackTrace() : stackheight(0) {}

void BackTrace::PushCursor(const unw_cursor_t& cursor) {
    if(stackheight == MAX_STACK) {
        return;
    }
    cursorstack[stackheight++] = cursor;
}

void BackTrace::Print() const {
    //int n=0;
    //while(stackheight > 0) {
    for(int n = 0; n < stackheight; ++n) {

        auto cursor = cursorstack[n];

        unw_word_t ip, sp, off;

        unw_get_reg(&cursor, UNW_REG_IP, &ip);
        unw_get_reg(&cursor, UNW_REG_SP, &sp);

        char symbol[256] = {"<unknown>"};
        char *name = symbol;

        if ( !unw_get_proc_name(&cursor, symbol, sizeof(symbol), &off) ) {
            int status;
            if ( (name = abi::__cxa_demangle(symbol, NULL, NULL, &status)) == 0 )
                name = symbol;
        }

        printf("#%-2d 0x%016" PRIxPTR " sp=0x%016" PRIxPTR " %s + 0x%" PRIxPTR "\n",
               n,
               static_cast<uintptr_t>(ip),
               static_cast<uintptr_t>(sp),
               name,
               static_cast<uintptr_t>(off));

        if ( name != symbol )
            free(name);

    }
}
