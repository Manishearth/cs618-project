
void func ()
{
	int * p, q;
	p = &q;
}

int main ()
{
	int ** x, * y, z;
	y = &z;
	x = &y;

	func ();

	return **x;
}
