
/************************
 * @author Vini Kanvar
************************/

#include "analysis_plugin.hh"

// class parser's object PROGRAM is made global
parser program;

static unsigned int
heap_analysis ()
{
	// Run this analysis only in LTO mode
	if (!in_lto_p)
		return 0;

	program.initialization ();
	program.preprocess_control_flow_graph ();
	program.print_parsed_data ();
	program.delete_block_aux ();

	RESULT ("\n\n");

	return 0;
}


