#include <cstdlib>		// If this is not the first header file, we get POISONED error on using -std=c++0x


#include <cctype>
#include <locale>
#include <string>
#include <sstream>
#include <iostream>

//#include <algorithm>		// Should be used before gcc-plugin.h
#include "gcc-plugin.h"		// Dynamic plugin
#include "config.h"




#include <stdio.h>
//#include "stdlib.h"


#include "system.h"

#include "tree.h"
#include "cp/cp-tree.h"
#include "cgraph.h"
#include "coretypes.h"
#include "basic-block.h"
#include "tm.h"
#include "diagnostic.h"
#include "gimple-pretty-print.h"
//#include "tree-flow.h"
#include "tree-pass.h"
#include "tree-ssa-alias.h"
#include "toplev.h"
#include "gimple-expr.h"
#include "gimple.h"
#include "cfgloop.h"

#include "vec.h"

#define VEC(t, gc) auto_vec<t>
#define heap va_heap
#define DEF_VEC_O(x, ...) static bool o_##x = false
#define DEF_VEC_P(x, ...)  static bool p_##x = false
#define DEF_VEC_ALLOC_O(x,...) static bool ao_##x = false
#define DEF_VEC_ALLOC_P(x, ...) static bool ap_##x = false
#define VEC_length(cs, vec) vec->length()
#define VEC_index(cs, vec, idx) ((*vec)[idx])

#define _FOR_EACH_VEC_ELT(a,b,c) FOR_EACH_VEC_ELT(a,b,c)
#undef FOR_EACH_VEC_ELT
#define FOR_EACH_VEC_ELT(a,b,c,d) _FOR_EACH_VEC_ELT(a,b,c)

#include "ggc.h"
#include "alloc-pool.h"
#include "params.h"
#include "string.h"

#include <ctime>

#include <map>
#include <set>
#include <stack>
#include <list>
#include <vector>
#include <tr1/unordered_map>
//#include <boost/bimap.hpp>

using namespace std;
using namespace std::tr1;
//using namespace boost;

#define RESULT(...) fprintf (dump_file , __VA_ARGS__)
//#define RESULT(...) 

