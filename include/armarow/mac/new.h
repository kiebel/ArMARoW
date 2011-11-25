#include <stdlib.h>
void* operator new (size_t , void* buffer) { return buffer; }
