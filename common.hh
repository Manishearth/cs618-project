#include <cstdlib>		// If this is not the first header file, we get POISONED error on using -std=c++0x
//#include <algorithm>		// Should be used before gcc-plugin.h
#include "gcc-plugin.h"		// Dynamic plugin
#include "config.h"
#include <stdio.h>
//#include "stdlib.h"
#include "system.h"
#include "cgraph.h"
#include "coretypes.h"
#include "tm.h"
#include "diagnostic.h"
#include "gimple-pretty-print.h"
#include "tree.h"
#include "tree-flow.h"
#include "tree-pass.h"
#include "toplev.h"
#include "gimple.h"
#include "cfgloop.h"

#include "vec.h"
#include "ggc.h"
#include "alloc-pool.h"
#include "params.h"
#include "string.h"
#include <string>
#include <sstream>
#include <iostream>
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

#define RESULT(...) fprintf (dump_file, __VA_ARGS__)
//#define RESULT(...) 

