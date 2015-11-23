#include <cstdlib>  // If this is not the first header file, we get POISONED error on using -std=c++0x
#include <cctype>
#include <locale>
#include <string>
#include <sstream>
#include <iostream>
//#include <algorithm> // Should be used before gcc-plugin.h
#include "gcc-plugin.h" // Dynamic plugin
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
#include "tree-cfg.h"
#include "tree-cfgcleanup.h"
#include "toplev.h"
#include "gimple-expr.h"

#include "tree-ssa-alias.h"

#include "gimple.h"

#include "tree-dfa.h"
#include "tree-ssa-operands.h"
#include "tree-phinodes.h"

#include "cfgloop.h"
#include "vec.h"
#include "gimple-iterator.h"
#include "gimple-ssa.h"
#include "ssa-iterators.h"

#include "stor-layout.h"

#define WITH_GCC_5

//#define VEC(t, gc) vec<t,gc>
#define VEC_free(t,gc,o) 1+1// (&o)->~vec()

#define VEC_dec(t, gc, ident) vec<t,gc> ident // VEC(t, gc) *ident = NULL for 4
#define VEC_ret(t, gc) vec<t,gc>  // VEC(t, gc) *ident = NULL for 4
#define VEC_arg(t, gc, ident) vec<t,gc> ident // VEC(t, gc) *ident  for 4

#define heap va_heap
#define DEF_VEC_O(x, ...) static bool o_##x = false
#define DEF_VEC_P(x, ...) static bool p_##x = false
#define DEF_VEC_ALLOC_O(x,...) static bool ao_##x = false
#define DEF_VEC_ALLOC_P(x, ...) static bool ap_##x = false

//#define VEC_length(cs, vec) (vec)->length()
#define VEC_length(cs, vec) (vec).length()
#define VEC_empty(cs, vec) (vec).is_empty()
#define VEC_truncate(cs, vec, n) (vec).truncate(n)
#define VEC_qsort(cs, vec, fun) (vec).qsort(fun)
#define VEC_last(cs,vec) &((vec).last())
#define VEC_index(cs, vec, idx) (((vec))[idx])
#define VEC_index_2(cs, vec, idx) (&((vec))[idx])
#define VEC_pop(cs,vec) &((vec).pop())

#define VEC_replace(cs,vec, idx, new_) (vec)[idx] = *(new_)

// #define VEC_safe_push(cs,gc, v, d) vec_safe_push((v), (d))
#define VEC_safe_push(cs,gc, v, d) (v).safe_push((d))
#define VEC_safe_push_2(cs,gc, v, d) (v).safe_push((*d))
#define VEC_safe_push_3(cs,gc, v, d) (v).safe_push((*(cs*)d)) // careful, nullptrderef is happening here


#define FOR_EACH_VEC_ELT_NEW(a,b,c,d) FOR_EACH_VEC_ELT(b,c,d) 
//#define _FOR_EACH_VEC_ELT(a,b,c) FOR_EACH_VEC_ELT(a,b,c)
//#undef FOR_EACH_VEC_ELT
//#define FOR_EACH_VEC_ELT(a,b,c,d) _FOR_EACH_VEC_ELT(a,b,c)
#define FOR_EACH_LOOP_NEW(a,b,c) FOR_EACH_LOOP(b,c)

#define CGRAPH_NODE cgraph_node //  = struct cgraph_node for 4
#define cgraph_nodes cgraph_new_nodes[0] // is this correct?
#define CGRAPH_CAST (cgraph_node*)
#define VARPOOL_CAST (varpool_node*)

#define VEC_alloc_NEW(vec, cs, h, len) (vec).create(len) // vec = VEC_alloc(args...) in 4

#define FOR_EACH_BB(bb) FOR_EACH_BB_FN(bb, cfun)
 
#define cgraph_node_name(node) node->asm_name()

#define LOOP_ITERATOR(li)

#define IS_GIMPLE_RETURN(ret, stmt) greturn* (ret) = dyn_cast <greturn *> (stmt)
// (gimple_code (stmt) == GIMPLE_RETURN) on 4

#define VEC_iterate(ty, to_visit, idx, num) (to_visit).iterate ((idx), &(num))


#define pointer_map_create() new hash_map<tree, csvarinfo_t>
#define pointer_map_contains(map, key) (void**)((map)->get(key))
#define POINTER_MAP_INSERT(map, key, value) (void**)((map)->put(key, value)) // two step process for 4
#define pointer_map_t hash_map<tree, csvarinfo_t>
#define host_integerp(a,b) tree_fits_uhwi_p(a)

#define TYPE_SIZE_(ty, ...) TYPE_SIZE(ty)

#define double_int_to_tree_(a,b) wide_int_to_tree(a,b) // ident on 4

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

