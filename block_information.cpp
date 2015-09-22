
/************************
 * @author Vini Kanvar
************************/

#include "block_information.hh"

#define DEBUG_CONTAINER 0
//#define DEBUG(...) fprintf (dump_file, __VA_ARGS__)
#define DEBUG(...)

block_information::
block_information ()
{
	block_type = 0;
	is_loop_join = false;
}

unsigned int block_information::
get_block_type ()
{
	return block_type;
}

void block_information::
set_block_type (unsigned int bt)
{
	DEBUG ("\nBlock old type %d, new type %d", block_type, block_type | bt);
	block_type |= bt;
}

bool block_information::
get_loop_join ()
{
	return is_loop_join;
}

void block_information::
set_loop_join ()
{
	is_loop_join = true;
}


unsigned int block_information::
get_block_order ()
{
	return order;
}

void block_information::
set_block_order (unsigned int order)
{
	this->order = order;
}

void block_information::
push (unsigned int i, bool b)
{
	parsed_data_indices.push_back (make_pair (i, b));
}

void block_information::
add_to_parsed_data_indices (unsigned int id, bool b, basic_block bb)
{
	DEBUG ("\nadd_to_parsed_data_indices");

	if (!bb->aux)
		RESULT ("\nError: bb(%d)->aux is NULL", bb->index);
	((block_information *)(bb->aux))->push (id, b);
}

list<pair<unsigned int, bool> > block_information::
get_parsed_data_indices ()
{
	return parsed_data_indices;
}

void block_information::
erase_assignment_indices ()
{
	list<pair<unsigned int, bool> >::iterator pdi;
	for (pdi = parsed_data_indices.begin (); pdi != parsed_data_indices.end (); )
	{
		bool is_assignment = pdi->second;
		if (is_assignment)
			parsed_data_indices.erase (pdi++);
		else
			pdi++;
	}
}
