/*
 *  Copyright 2001 Adrian Thurston <thurston@cs.queensu.ca>
 */

/*  This file is part of Aapl.
 *
 *  Aapl is free software; you can redistribute it and/or modify it under the
 *  terms of the GNU Lesser General Public License as published by the Free
 *  Software Foundation; either version 2.1 of the License, or (at your option)
 *  any later version.
 *
 *  Aapl is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Aapl; if not, write to the Free Software Foundation, Inc., 59
 *  Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <iostream>
#include "tavltree.h"
#include "avltree.h"

using namespace std;

#define TAVLTREE

#ifdef TAVLTREE

/* Test element 1. */
struct TreeEl1 : public TAvlTreeEl, public CmpOrd<int>
{
	TreeEl1() { }
	TreeEl1(const int key) : key(key) { }

	int getKey() { return key; }
	int key;
};

/* Test element 2. */
struct TreeEl2 : public TAvlTreeEl, public CmpOrd<int>
{
	TreeEl2() { }
	TreeEl2(const int key) : key(key) { }

	int getKey() { return key; }
	int key;
};

/* Test element 3. */
struct TreeEl3 : public TAvlTreeEl, public CmpOrd<int>
{
	TreeEl3() { }
	TreeEl3(const int key) : key(key) { }

	int getKey() { return key; }
	int key;
};

/* Test element 4. */
struct TreeEl4 : public TAvlTreeEl, public CmpOrd<int>
{
	TreeEl4() { }
	TreeEl4(const int key) : key(key) { }

	int getKey() { return key; }
	int key;
};

/* Test element 5. */
struct TreeEl5 : public TAvlTreeEl, public CmpOrd<int>
{
	TreeEl5() { }
	TreeEl5(const int key) : key(key) { }

	int getKey() { return key; }
	int key;
};

/* Test element 6. */
struct TreeEl6 : public TAvlTreeEl, public CmpOrd<int>
{
	TreeEl6() { }
	TreeEl6(const int key) : key(key) { }

	int getKey() { return key; }
	int key;
};

typedef TAvlTree< TreeEl1, int > Tree1;
typedef TAvlTree< TreeEl2, int > Tree2;
typedef TAvlTree< TreeEl3, int > Tree3;
typedef TAvlTree< TreeEl4, int > Tree4;
typedef TAvlTree< TreeEl5, int > Tree5;
typedef TAvlTree< TreeEl6, int > Tree6;

/* Instantiate the entire tree. */
//template class TAvlTree< TreeEl1, int >;
//template class TAvlTree< TreeEl2, int >;
//template class TAvlTree< TreeEl3, int >;
//template class TAvlTree< TreeEl4, int >;
//template class TAvlTree< TreeEl5, int >;
//template class TAvlTree< TreeEl6, int >;

#else

/* Test element 1. */
struct TreeEl1 : public AvlTreeEl<TreeEl1>, public CmpOrd<int>
{
	TreeEl1() { }
	TreeEl1(const int key) : key(key) { }

	int getKey() { return key; }
	int key;
};

/* Test element 2. */
struct TreeEl2 : public AvlTreeEl<TreeEl2>, public CmpOrd<int>
{
	TreeEl2() { }
	TreeEl2(const int key) : key(key) { }

	int getKey() { return key; }
	int key;
};

/* Test element 3. */
struct TreeEl3 : public AvlTreeEl<TreeEl3>, public CmpOrd<int>
{
	TreeEl3() { }
	TreeEl3(const int key) : key(key) { }

	int getKey() { return key; }
	int key;
};

/* Test element 4. */
struct TreeEl4 : public AvlTreeEl<TreeEl4>, public CmpOrd<int>
{
	TreeEl4() { }
	TreeEl4(const int key) : key(key) { }

	int getKey() { return key; }
	int key;
};

/* Test element 5. */
struct TreeEl5 : public AvlTreeEl<TreeEl5>, public CmpOrd<int>
{
	TreeEl5() { }
	TreeEl5(const int key) : key(key) { }

	int getKey() { return key; }
	int key;
};

/* Test element 6. */
struct TreeEl6 : public AvlTreeEl<TreeEl6>, public CmpOrd<int>
{
	TreeEl6() { }
	TreeEl6(const int key) : key(key) { }

	int getKey() { return key; }
	int key;
};

typedef AvlTree< TreeEl1, int > Tree1;
typedef AvlTree< TreeEl2, int > Tree2;
typedef AvlTree< TreeEl3, int > Tree3;
typedef AvlTree< TreeEl4, int > Tree4;
typedef AvlTree< TreeEl5, int > Tree5;
typedef AvlTree< TreeEl6, int > Tree6;

/* Instantiate the entire tree. */
//template class TAvlTree< TreeEl1, int >;
//template class TAvlTree< TreeEl2, int >;
//template class TAvlTree< TreeEl3, int >;
//template class TAvlTree< TreeEl4, int >;
//template class TAvlTree< TreeEl5, int >;
//template class TAvlTree< TreeEl6, int >;

#endif

void testTAvlTree1()
{
	Tree1 tree;
	TreeEl1 element1(1), element2(2);
	tree.insert( &element1 );
	tree.insert( &element2 );
	Tree1::Iter it = tree.first();
	for ( ; it.lte(); it++ )
		cout << it->key << endl;
}

void testTAvlTree2()
{
	Tree2 tree;
	TreeEl2 element1(1), element2(2);
	tree.insert( &element1 );
	tree.insert( &element2 );
	Tree2::Iter it = tree.first();
	for ( ; it.lte(); it++ )
		cout << it->key << endl;
}

void testTAvlTree3()
{
	Tree3 tree;
	TreeEl3 element1(1), element2(2);
	tree.insert( &element1 );
	tree.insert( &element2 );
	Tree3::Iter it = tree.first();
	for ( ; it.lte(); it++ )
		cout << it->key << endl;
}

void testTAvlTree4()
{
	Tree4 tree;
	TreeEl4 element1(1), element2(2);
	tree.insert( &element1 );
	tree.insert( &element2 );
	Tree4::Iter it = tree.first();
	for ( ; it.lte(); it++ )
		cout << it->key << endl;
}

void testTAvlTree5()
{
	Tree5 tree;
	TreeEl5 element1(1), element2(2);
	tree.insert( &element1 );
	tree.insert( &element2 );
	Tree5::Iter it = tree.first();
	for ( ; it.lte(); it++ )
		cout << it->key << endl;
}

void testTAvlTree6()
{
	Tree6 tree;
	TreeEl6 element1(1), element2(2);
	tree.insert( &element1 );
	tree.insert( &element2 );
	Tree6::Iter it = tree.first();
	for ( ; it.lte(); it++ )
		cout << it->key << endl;
}

int main()
{
	testTAvlTree1();
	testTAvlTree2();
	testTAvlTree3();
	testTAvlTree4();
	testTAvlTree5();
	testTAvlTree6();
	return 0;
}
