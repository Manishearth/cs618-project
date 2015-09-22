
/*******************************************************************************
  * This code has been extracted from tree-ssa-structalias.c of GCC. It was 
  * refactored by Prashant Singh Rawat. It was further improved by Prachi Yogi,
  * Sudakshina Das, Swati Rathi, Avantika Gupta, Pritam Gharat, and of course
  * me (Vini) ;-)
*******************************************************************************/


#include "common.hh"
#include "params.h"

#ifndef PARSER
#define PARSER

#include "block_information.hh"

// This file populates the list of constraints and variables for the whole program.

/* -----------------------------------------------------------------------------------
   Data structures picked up (and possibly modified) from tree-ssa-structalias.h file.
   -----------------------------------------------------------------------------------*/

struct csvariable_info
{
  /* ID of this variable  */
  unsigned int id;

  /* True for variables whose size is not known or variable.  */
  unsigned int is_unknown_size_var : 1;

  /* True for (sub-)fields that represent a whole variable.  */
  unsigned int is_full_var : 1;

  /* True if this field may contain pointers.  */
  unsigned int may_have_pointers : 1;

  /* True if it is a heap var. */
  unsigned int is_heap_var : 1;

  /* True if this represents a global variable.  */
  unsigned int is_global_var : 1;

  /* A link to the variable for the next field in this structure.  */
  struct csvariable_info *next;

  /* Offset of this variable, in bits, from the base variable  */
  unsigned HOST_WIDE_INT offset;

  /* Size of the variable, in bits.  */
  unsigned HOST_WIDE_INT size;

  /* Full size of the base variable, in bits.  */
  unsigned HOST_WIDE_INT fullsize;

  /* Name of this variable */
  const char *name;

  /* Tree that this variable is associated with.  */
  tree decl;

  /* The list of all the constraints {aliases} that have this 
     variable as their LHS */
  //df_list constraints_with_vi_as_lhs;

  /* The function in whose scope this variable is defined. */
  struct cgraph_node *scoping_function;
};
typedef struct csvariable_info *csvarinfo_t;

typedef enum {SCALAR, DEREF, ADDRESSOF} constraint_expr_type;

/* Use 0x8000... as special unknown offset.  */
#define UNKNOWN_OFFSET ((HOST_WIDE_INT)-1 << (HOST_BITS_PER_WIDE_INT-1))

/* An expression that appears in a constraint.  */

// // VEC or sequence of fields
// typedef HOST_WIDE_INT field;
// DEF_VEC_O(field);
// DEF_VEC_ALLOC_O(field, heap);

struct constraint_expr
{
  /* Constraint type.  */
  constraint_expr_type type;

  /* Variable we are referring to in the constraint.  */
  unsigned int var;

  // Added by Pritam
  bool pointer_arithmetic;

  // list<> cannot be used with VEC_safe_push()
  // list<HOST_WIDE_INT> offset_sequence;
  // VEC (field, heap) offset_sequence;

  // Added by Vini
  // Pointer to array (sequence) of field offsets.
  list<unsigned int> * offset_sequence;

  /* Offset, in bits, of this constraint from the beginning of
     variables it ends up referring to.

     IOW, in a deref constraint, we would deref, get the result set,
     then add OFFSET to each member.   */
  HOST_WIDE_INT offset;

  // Added by Vini
  constraint_expr ()
  {
	pointer_arithmetic = false;
	// Do not allocate new list<> here since some constraint_expr are
	// created by copying and the pointer to list<> gets pointed by more
	// than one constraint_expr.  Therefore, initialize at the place where
	// list<> is populated with offsets.
	offset_sequence = NULL;
  }

  // Do not delete offset_sequence here in the destructor because while copying
  // constraint_expr, the pointer to offset_sequence gets copied and is used
  // somewhere else. It shoule be deleted in the end in delete_parsed_data().
};

typedef struct constraint_expr ce_s;
DEF_VEC_O(ce_s);
DEF_VEC_ALLOC_O(ce_s, heap);

struct constraint
{
  struct constraint_expr lhs;
  struct constraint_expr rhs;

  struct constraint * previous_phi;
  constraint ()
  {
     previous_phi = NULL;
  }
};
typedef struct constraint *constraint_t;

/* This structure is used during pushing fields onto the fieldstack
   to track the offset of the field, since bitpos_of_field gives it
   relative to its immediate containing type, and we want it relative
   to the ultimate containing object.  */

struct fieldoff		/* Look into */
{
  /* Offset from the base of the base containing object to this field.  */
  HOST_WIDE_INT offset;

  /* Size, in bits, of the field.  */
  unsigned HOST_WIDE_INT size;

  unsigned has_unknown_size : 1;

  unsigned must_have_pointers : 1;

  unsigned may_have_pointers : 1;

  unsigned only_restrict_pointers : 1;
};
typedef struct fieldoff fieldoff_s;

DEF_VEC_O(fieldoff_s);
DEF_VEC_ALLOC_O(fieldoff_s,heap);


/*----------------------
  Variable Declarations.
  ---------------------*/
#define SSAVAR(x) (TREE_CODE (x) == SSA_NAME ? SSA_NAME_VAR (x) : x)
#define alloc_mem ggc_alloc_cleared_atomic 
#define total_bbs n_basic_blocks-NUM_FIXED_BLOCKS

// escaped_id is not being used in points_to_analysis_simple. Therefore, we
// assume that variable id = 0 does not exist.
enum {escaped_id = 0, undef_id = 1, null_id = 2, readonly_id = 3, universal_id = 4};

/*--------------------------------
  VEC data structure declarations.
  -------------------------------*/
DEF_VEC_P(csvarinfo_t);
DEF_VEC_ALLOC_P(csvarinfo_t, heap);
DEF_VEC_P(constraint_t);
DEF_VEC_ALLOC_P(constraint_t,heap);

#define variable_data csvarmap
#define assignment_data aliases

class parser
{
public:
	struct cgraph_node * main_cnode;
	struct cgraph_node * global_cnode;
	VEC (csvarinfo_t, heap) * csvarmap;
	VEC (constraint_t, heap) * aliases;
	map<unsigned int, expanded_location> heap_location;

private:
	bool multi_rhs;
	bool compute_only_pinfo;
	bool compute_alias_and_pinfo;
	bool all_contexts_together;
	bool check_deref;
	bool deref_stmt;
	alloc_pool constraint_pool;
	alloc_pool csvarinfo_pool;
	VEC (constraint_t, heap) * constraints;
	struct pointer_map_t * vi_for_tree;

private:	
	HOST_WIDE_INT bitpos_of_field (const tree fdecl);
	constraint_t new_constraint (const struct constraint_expr lhs,const struct constraint_expr rhs);
	bool constraint_expr_equal (struct constraint_expr a, struct constraint_expr b);
	bool constraint_equal (struct constraint a, struct constraint b);
	const char * alias_get_name (tree decl);
	inline bool var_can_have_subvars (const_tree v);
	bool type_must_have_pointers (tree t);
	bool push_fields_onto_fieldstack (tree type, VEC(fieldoff_s,heap) **fieldstack,HOST_WIDE_INT offset);
	tree get_decl (csvarinfo_t var);
	tree get_decl_private (tree var_decl, HOST_WIDE_INT var_offset, HOST_WIDE_INT search_offset);
	unsigned int count_num_arguments (tree decl, bool *is_varargs);
	bool check_for_overlaps (VEC (fieldoff_s,heap) *fieldstack);
	void sort_fieldstack (VEC(fieldoff_s,heap) *fieldstack);
	void cs_insert_vi_for_tree (tree t, csvarinfo_t vi);
	
	struct cgraph_node * scoping_fn (unsigned int varid);
	bool var_defined_in_cfun (unsigned int varid, struct cgraph_node * cnode);
	void cs_get_constraint_for_rhs (tree t, VEC (ce_s, heap) **results, basic_block bb, struct cgraph_node * cnode);
	csvarinfo_t cs_new_var_info (tree t, const char *name, struct cgraph_node * cnode);
	csvarinfo_t cs_create_variable_info_for_1 (tree decl, const char *name, struct cgraph_node * cnode);
	unsigned int cs_create_variable_info_for (tree decl, const char *name, basic_block bb, struct cgraph_node * cnode);
	struct constraint_expr cs_new_scalar_tmp_constraint_exp (const char *name, struct cgraph_node * cnode);
	tree build_fake_var_decl (tree type);
	tree get_memref_casted_inside_type (tree ptr_use, tree ptr);
	tree get_inside_type (tree ptr);
	tree get_casted_inside_type (tree ptr);
	tree get_heap_decl (tree lhs, basic_block bb, struct cgraph_node * cnode);
	csvarinfo_t cs_make_heapvar_for (tree lhs, const char *name, basic_block bb, struct cgraph_node * cnode);
	void cs_make_constraint_from (csvarinfo_t vi, int from, basic_block bb);
	csvarinfo_t cs_make_constraint_from_heapvar (tree lhs, const char *name, basic_block bb, struct cgraph_node * cnode);
	csvarinfo_t cs_first_or_preceding_vi_for_offset (csvarinfo_t start,unsigned HOST_WIDE_INT offset);
	void cs_do_deref (VEC (ce_s, heap) **constraints, basic_block bb, struct cgraph_node * cnode);
	void cs_get_constraint_for_ptr_offset (tree ptr, tree offset,VEC (ce_s, heap) **results, basic_block bb, struct cgraph_node * cnode);
	void cs_get_constraint_for_component_ref (tree t, VEC(ce_s, heap) **results,bool address_p, bool lhs_p, basic_block bb, struct cgraph_node * cnode);
	void cs_get_constraint_for_ssa_var (tree t, VEC(ce_s, heap) **results, bool address_p, basic_block bb, struct cgraph_node * cnode);
	void cs_get_constraint_for_1 (tree t, VEC (ce_s, heap) **results, bool address_p, bool lhs_p, basic_block bb, struct cgraph_node * cnode);
	void cs_process_all_all_constraints (VEC (ce_s, heap) *lhsc, VEC (ce_s, heap) *rhsc, basic_block bb);
	void cs_get_constraint_for_address_of (tree t, VEC (ce_s, heap) **results, basic_block bb, struct cgraph_node * cnode);
	void cs_get_constraint_for (tree t, VEC (ce_s, heap) **results, basic_block bb, struct cgraph_node * cnode);
	csvarinfo_t cs_create_func_info_for (tree decl, const char *name, struct cgraph_node * cnode);
	void cs_do_structure_copy (tree lhsop, tree rhsop, basic_block bb, struct cgraph_node * cnode);
	void cs_init_base_vars (struct cgraph_node * cnode);
	void cs_init_alias_vars (struct cgraph_node * cnode);
	tree cs_get_var (tree t);
	void insert_alias_in_pool (constraint_t t, csvarinfo_t vi, basic_block bb);
	void cs_process_constraint (constraint_t t, basic_block bb);
	bool possibly_deref (gimple stmt);
	bool associate_varinfo_to_alias (struct cgraph_node *node, void *data);
	void process_library_call (gimple stmt, basic_block bb, struct cgraph_node * cnode);
	void map_arguments_at_call (gimple stmt, tree decl, bool generate_liveness, basic_block bb, struct cgraph_node * cnode);
	
	void process_gimple_assign_stmt (gimple, basic_block bb, struct cgraph_node * cnode);
	void process_gimple_condition(gimple, basic_block bb, struct cgraph_node * cnode);
	void process_gimple_phi_stmt (gimple, basic_block bb, struct cgraph_node * cnode);
	
	bool is_gimple_endblock (gimple t);
	void generate_retval_liveness (gimple stmt, basic_block bb, struct cgraph_node * cnode);
	bool process_phi_pointers (basic_block bb, struct cgraph_node * cnode);
	tree get_called_fn_decl (gimple stmt);

	// Basic block is passed by reference, since we need to update the basic block 
	// to the current program split, which will be in a new block after block splitting.
	gimple_stmt_iterator split_bb_at_stmt (basic_block & bb, gimple stmt);
	void initialize_block_aux (basic_block block);
	gimple_stmt_iterator split_block_at_statement (gimple statement, basic_block block);
	unsigned int get_tree_index (tree v);
	bool is_loop_join (basic_block block);

	bool is_function_pointer (basic_block call_site);

public:

	parser ();
	void initialization (void);
	void add_global_addressof_initializations ();
	bool is_proper_var (unsigned int varid);
	bool heap_var (unsigned int varid);
	bool global_var (unsigned int varid);
	bool field_must_have_pointers (tree t);
	gimple bb_call_stmt (basic_block bb);
	basic_block get_start_block_of_function (struct cgraph_node *node);
	basic_block get_end_block_of_function (struct cgraph_node *node);
	bool is_pred_of_end_block (basic_block block);
	bool parm_decl (unsigned int varid);
	csvarinfo_t cs_lookup_vi_for_tree (tree t);
	csvarinfo_t cs_make_heapvar_offset_for (csvarinfo_t lhs, tree heap_pointer_type, unsigned int offset, const char * heap_name, basic_block bb, struct cgraph_node * cnode);
	csvarinfo_t cs_get_vi_for_tree (tree stmt, basic_block bb, struct cgraph_node * cnode);
	void map_function_pointer_arguments (struct cgraph_node * src_function, basic_block call_site, struct cgraph_node * called_function);
	void map_return_value (basic_block call_block, struct cgraph_node * src_function, basic_block end_block, struct cgraph_node * called_function);

	bool unexpandable_var (unsigned int var, HOST_WIDE_INT offset);
	bool function_var (csvarinfo_t var);
	bool parameter_var (csvarinfo_t var);
	csvarinfo_t cs_first_vi_for_offset (csvarinfo_t start, unsigned HOST_WIDE_INT offset);
	void handle_unknown_offset (constraint_expr & e);
	csvarinfo_t cs_get_varinfo (unsigned int n);
	VEC (fieldoff_s, heap) * get_fieldstack (unsigned int pointer_record);
	set<unsigned int> get_global_variables ();
	set<unsigned int> get_global_named_pointers ();
	set<unsigned int> get_local_variables (struct cgraph_node * function);
	set<unsigned int> get_local_non_temp_pointers (struct cgraph_node * function);
	set<unsigned int> get_function_arguments (basic_block current_block, struct cgraph_node * src_function);
	set<unsigned int> get_function_parameters (struct cgraph_node * function);
	void delete_block_aux ();
	void delete_parsed_data (basic_block block);

	set<unsigned int> get_reachable_fields (csvarinfo_t var);
	void get_reachable_fields_private (csvarinfo_t root, tree var_decl, HOST_WIDE_INT var_offset, set<unsigned int> & reachable_fields);
	set<unsigned int> get_field_var_ids (tree decl);
	void get_field_var_ids (tree decl, set<unsigned int> & field_ids);
	void get_field_var_ids (unsigned int var_id, set<unsigned int> & field_ids);
	void get_field_var_ids (set<unsigned int> & var_ids, set<unsigned int> & field_ids);
	void get_offset_sequence (tree ref, list<unsigned int> & offset_sequence);
	void copy_offset_sequence (list<unsigned int> ** dest, list<unsigned int> & src);
	void append_offset_sequence (list<unsigned int> ** dest, list<unsigned int> & src);
	void print_offset_sequence (list<unsigned int> * offset_sequence);
	unsigned int get_next_field (unsigned int var_id);
	void connect_with_previous_phi (basic_block block);
	void initialize_prev_of_first_phi (basic_block block);
	void save_heap_location (gimple stmt, unsigned int heapvar);
	void print_heap_location (csvarinfo_t variable);

	void preprocess_control_flow_graph ();
	void restore_control_flow_graph ();
	void print_block_statements (basic_block block);
	bool print_source_location (basic_block block);
	void print_parsed_data (basic_block current_block);
	void print_parsed_data ();
	void print_original_cfg ();
	void print_variable_data (int index);
	void print_assignment_data (int index);
	void print_variable_data ();
	void print_assignment_data ();


};

extern parser program;

#endif
