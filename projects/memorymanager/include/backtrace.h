#ifndef BACKTRACE_H
#define BACKTRACE_H
#define UNW_LOCAL_ONLY
#include <libunwind.h>

class BackTrace
{
public:
    static BackTrace CreateBackTrace();
    BackTrace();
    void Print() const;

private:
    void PushCursor(const unw_cursor_t& cursor);


    static const unsigned MAX_STACK = 30;
    unw_context_t context;
    unw_cursor_t cursorstack[MAX_STACK];
    int stackheight;
};
#endif // BACKTRACE_H
