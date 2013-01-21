#include <iostream>

using namespace std;

class BaseOne
{
public:
	BaseOne()
	{
		cout << "New BaseOne made!" << endl;
	}

	~BaseOne()
	{

	}

	void sayHello()
	{
		cout << "BASE ONE SAYS HELLO!" << endl;
	}
};

class BaseTwo
{
public:
	BaseTwo()
	{
		cout << "New BaseTwo made!" << endl;
	}

	~BaseTwo()
	{

	}

	virtual void sayHello()
	{
		cout << "BASE TWO SAYS HELLO!" << endl;
	}
};

class ExtOne : public BaseOne
{
public:
	ExtOne():BaseOne() // Explicit call to the constructor, would happen implictly anyway...
	{
		cout << "New ExtOne made!" << endl;
	}

	~ExtOne()
	{

	}

	void sayHello()
	{
		cout << "This will never be printed to screen :(" << endl;
	}
};

class ExtTwo : public BaseTwo
{
public:
	ExtTwo():BaseTwo()
	{
		cout << "New ExtTwo made!" << endl;
	}

	~ExtTwo()
	{

	}

	void sayHello()
	{
		cout << "Hello from ExtTwo!" << endl;
	}
};

int main(int argc, char **argv)
{
	BaseOne *baseOnes[10] = { NULL }; // Initialise an array of 10 BaseOne pointers, all pointing to NULL :D
	BaseTwo *baseTwos[10] = { NULL };

	for(int i = 0; i<10;i++)
	{
		if(i%2)
		{
			baseOnes[i] = new BaseOne();
			baseTwos[i] = new BaseTwo();
		}
		else
		{
			baseOnes[i] = new ExtOne();
			baseTwos[i] = new ExtTwo();
		}
	}
	cout << endl;
	// Now that we've got a bunch of objects, let get them to say hello!
	// Start off by the first 10 BaseOne objects then move to base 2
	bool first = true;
	for(int i = 0; i<10; i++)
	{
		if(first)
		{
			baseOnes[i]->sayHello();
			if(i == 9)
			{
				first = false;
				i = 0;
				cout << endl;
			}
		}
		else
		{
			baseTwos[i]->sayHello();
		}
	}

	return 0;
}

/*

Output Should Be:

BASE ONE SAYS HELLO!
BASE ONE SAYS HELLO!
BASE ONE SAYS HELLO!
BASE ONE SAYS HELLO!
BASE ONE SAYS HELLO!
BASE ONE SAYS HELLO!
BASE ONE SAYS HELLO!
BASE ONE SAYS HELLO!
BASE ONE SAYS HELLO!
BASE ONE SAYS HELLO!

BASE TWO SAYS HELLO!
Hello from ExtTwo!
BASE TWO SAYS HELLO!
Hello from ExtTwo!
BASE TWO SAYS HELLO!
Hello from ExtTwo!
BASE TWO SAYS HELLO!
Hello from ExtTwo!
BASE TWO SAYS HELLO!

Why? 

Since BaseOne's sayHello function wasn't marked as 'virtual', at runtime
when the sayHello function is called, the compiler just sees a BaseOne object
therefore it calls BaseOne's version of that function. If we did:

	((ExtOne*)baseOnes[i])->sayHello(); 

Then it would of worked, provided that we knew that the baseOnes[i] was in fact
an instance of ExtOne class. 

This example is really simple with just one class, but imagine the common...

class Mammal
{
	...
	void move();
	...
};

class Monkey : public Mammal
{
	void move();
}

class Dog : public Mammal
{
	void move();
}

And so on- if we had a huge list (typically a vector) of Mammals, we'd
be having to store extra data to know which class the Mammal was.

The virtual keyword makes the compiler create a 'vtable' for objects,
so it can tell which instances are of which class, meaning if you
had a ton of Mammals of difference classes, calling their move() function
if it were virtual, would cause the program to check the vtable to 
see which class the object is. This causes a O(1) slow down I think, so
the slowdown is minimal. 

So this is why our ExtTwo class gets to use it's sayHello function
but the ExtOne class doesn't.
*/
