#include "alloc.h"
using namespace asr;
void main()
{
	MemoryPool<> *Mp = &MemoryPool<>::getInstance();
	float* p = Mp->alloc<float>(10);
	double* pa = Mp->alloc<double>(400);
	for(int i = 0; i < 10; ++i)
		p[i] = 0.1;
	Mp->dealloc<float>(p);
	double* paa = Mp->alloc<double>(20000);
	Mp->dealloc<double>(paa);
	char* pchar =Mp->alloc<char>(10000);
	Mp->dealloc<char>(pchar);
	
	return;
}
