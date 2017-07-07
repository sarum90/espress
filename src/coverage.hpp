#pragma once

#ifdef COVERAGE
extern "C" {
extern void __gcov_flush(void);
}
inline void coverage_before_exit(){ __gcov_flush(); }
#else 
inline void coverage_before_exit(){}
#endif

