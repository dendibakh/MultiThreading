#pragma once

#include "MutexWithAtomics.h"

class Lock_Guard
{
public:
       Lock_Guard(Mutex& m) : m(m) 
       {
       	m.lock();
       }

       ~Lock_Guard() 
       {
       	m.unlock();
       }
private:
	Mutex& m;
};