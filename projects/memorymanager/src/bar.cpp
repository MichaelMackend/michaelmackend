#include "bar.h"

Bar::Bar()
{

}

ThingOne* Bar::CallOne() {
    return new ThingOne;
}

ThingTwo* Bar::CallTwo() {
    return new ThingTwo;
}
