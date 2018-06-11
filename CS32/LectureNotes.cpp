//Circle.h

#include "Circle.h"
#include <iostream>
#include <math>
using namespace std;

Circle::Circle(double x, double y, double r)
	: m_x(x), m_y(y), m_r(r) // member initialzation list
{
	if(r <= 0)
	{
		cout << "Cannot create a circle with radius " << r;
		exit(1);
	}
	m_x = x;
	m_y = y;
	m_r = r;
}

bool Circle:;scale(double factor)
{
 	if(factor <= 0)
		return false;
	m_r *= factor;
	return true;
}

void Circle::draw() const
{
	.. draw circle centered at (m_x, m_y)
	.. with radius m_r
}

double Circle::radius() const
{
	return m_r;
}

// Lowkey v inefficient without '&'
double area(const Circle& x) 
{
	return PI * x.radius() * x.radius();
}


//StickFigure.h
#include some shit lol idk


//StickFigure.cpp
StickFigure::StickFigure(double b1, double headDiameterd, string nm, double hx, double hy)
: m_bodyLength(b1), m_name(nm), m_head(hx, hy, headDiameter/2)
{
	if (b1 <= 0)
		...
}


How an object is constructed:
1. —————————— (we’ll see this one later) ———————————
2. Construct the data members, consulting the member initialization list.
If a member is not listed if it is of a:
	1. Built in type, it is left uninitialized
	2. class type, it is default-constructed (If no default ctor, error!)

3. Execute the body 


```
void h()
{
	String s("Hello");
	...
}

int main()
{
	String t;
	for(...)
		h();
	...
}


//String.h
class String
{
	public:
		String(char* value);
		String();
	private:
		// Classs invariant:
		// EITHER:
		// m_text points to a dynaimically allocated array of m_len+1 chars
		// m_len >= 0
		// m_text[m_len] == '\0'
		// OR: 
		// m_text == nullptr
		// m_len == 0
		char* m_text;
		int m_len;
};

//String.cpp
String::String(const char* value)
{
	m_len = strlen(value);
	m_text = new char[m_len+1];
	strcpy(m_text, value);
}
int main()
{
	String x("Wow!");
	... verify that x is "Wow!" ...
	
	char buffer[1000];
	cin.getline(buffer, 1000); //Suppose the user types Wow!
	String s(buffer);
	... Verify that s is indeed "Wow!"
	cin.getline(buffer, 1000); // Suppose the user types Ouch!
	... Verify that s is "Wow!" ...
}





==================================================================
Code (Lecture 1.22)												 =
==================================================================


If a member if not listed, if it is of a
	- built in  type, it is left uninitialized
	- class type, it is default-constructed ( if not default ctor, error!)

3. Execute the body of the constructor

How an object is destroyed:
1. Execute the body of the destructor
2. Deestroy the data members
	-built in type, do nothing (ptr are built in types)
	- class type, call its destructor


class String{
public:
	String (const char* value = "");
	String(const String& other);
	~String();
	String operator=(const String& rhs);
private:
	// Class invariant:
	// m_text points to a dynamically allocated array of m_len+1 chars
	// m_len >= 0
	// m_text[m_len] == '\0'
	char* m_text;
	int m_len;
};

String& String::operator=(const String& rhs){
	if(this != &rhs){
		delete [] m_text;
		m_len = rhs.m_len;
		m_text = new char[m_len+1];
		strcpy(m_text, rhs.m_text);
		return *this;
	}
}

String::String(const char* value){
	if(value == nullptr)
		value = "";
	m_len = strlen(value);
	m_text = new char[m_len+1];
	strcpy(m_text, value);
}

String::String(const String& other)
{
	m_len = other.m_len;
	m_text = new char[m_len+1];
	strcpy(m_text, other.m_text);
}

String::~String(){
	delete [] m_text;
}


==================================================================
Code (Lecture 1.24)												 =
==================================================================
// Account has 'debit' and 'credit' functions indicating the
// location to move the money
void transfer(Account& from , Account& to, double amt){
	if(&from != &to){
		if(from.balance() >= amt){
			from.debit(amt);
			to.credit(amt);
		}
	}
}

int mul(int i, int j);

void fire(Ship& from, Ship& to);


struct Node{
	int data;
	Node* next;
}

Node* head;


================================================================
Notes (Lecture 1.24):										   =
================================================================
Fixed-size array
Dynamically-allocated fixed size array

//Looks like he going into linked lists n shit

Resizeable array



void LinkedList::print(){
	Node* cur = head;
	while(cur != nullptr){
		cout << cur->data;
		cur = cur->next;
	}
}

================================================================
Notes (Lecture 1.29):										   =
================================================================


Draw pictues of the linked list;

Draw the empty container with 1 object, one with 2 objects, one with several objects


Stacks: 

Stacks use a LIFO (Last in First out) datat structure

#include <stack>
using namespace std:

int main(){
	stack<int> s; // why do you put a '<int>' here what is <int>??
	s.push(10); // put 10 on the stack
	s.push(20); // put 20 on the stack
	int n = s.top(); // n = the value of element at the top of the stack
	s.pop(); // removes the top element (pop off)
	if(!s.empty()) // is the stack empty? has no elements
		cout << s.size() << endl; // print the size (should be 1)
}

OH FUCK! SMALLBERG JUST BUSTED OUT THE OLD ENGLISH
Use a queue like a line at Domnios, and use a stack like a pile of cookies

Issa Stack: Pop off to remove, Push to add
x x x x x <- 
		  ->	


-------------------------------------------------------------------
Queues:

Queues use a FIFO (First in first out)

int main(){
	int main(){
	stack<int> s; // why do you put a '<int>' here what is <int>??
	s.push(10); // put 10 on the queue
	s.push(20); // put 20 on the queue
	int n = s.top(); // n = the value of element at the top of the queue (10)
	s.pop(); // removes the last element (pop off, removes 10)
	if(!s.empty()) // is the queue empty? has no elements
		cout << s.size() << endl; // print the size (should be 1)
}


Issa Queue: Dequeue to remove (call pop in code), Enqueue to add (call push in code)
-------------------------------------------------------------------

name = "Smith" and (salary >= 70000 or dept = "IT")


- stack for the integers (operand stack)
- stack for the operations (operator tack)

prefix:
f(x,y,z)
add(sub(8, div(6,2)),1)
+ - 8 / 6 2 1

operatro stack: + 
operand stack: 8 2

1. Push the operator to the operator stack. Keep adding until a number is encountered.
2. Push the operands to the operand stack. If you encounter 2 or more operands in a row, pop the operator
   stack (if not empty)
3. Perform the operation and push the result onto the operand stack.


infix:
8-6/2*1*6/2

1.Push the operand onto the stack until you reach an operator
2.Push the operator until you reach a higher operator and then perform the higher operator
on the top two operands (if you reach an operation that is the same precedence. perfom that operator on
	the top of the stack)
3.Push result onto the operand stack

With parenthasis
3*(2*(4-1)+3)
ator stack : * ( * ( - ... open encountered, pop and begin to do the maths ...
oand stack : 3 2 4

1. Once a close par is encountered, pop until a close is encountered
2. 

postfix:
8 6 2 / - 1 +

1.once you hit an operation and you have >=2 elemetns perform the operation on the top
two elements of the number stack
2.Add result to the top of the number stack
3. Repeat until the end



}


A(int sz){
	n = sz;
}

A(const A& other){
	b = other.b;
	
	n = other.n;
	int *a = arr;
	for(int i = 0; i < n; i++){
		*a = *arr;
		arr++;
	}
}

A& operator=(const A& other){

}



==================================================================
Lecuture Notes (2.7.18)
==================================================================


class Shape
{
	virtual void move(double xnew, double ynew);
	virtual void draw() const = 0; // "pure virtual function" // virtual says that each sub class has a draw function
	double m_x;
	double m_y;
};

Shape::Shape(double x, double y) : m_x(x), m_y(y)
{}

class WarningSymbol : public Shape
{
	virtual void move(double xnew, double ynew);
	... 
};

class Circle : public Shape
{
	virtual void draw() const; 
	double m_r;
};

Circle::Circle(double r, double x, double y)
	: Shape(x,y), m_r(r) // Wrong! (apparently)
{

}

class Rectangle : public Shape
{
	virtual void draw() const; // virtual bc you want to call this instead of shape
	virtual double diag() const; // virtual bc you may make a square in the future w this class
	double m_dx;
	double m_dy;
};

Shape* pic[100];
// These will all get cast to a shape
pic[0] = new Circle;
pic[1] = new Rectangle;
pic[2] = new Circle;
for(int k = 0; k < 100; k++)
	pic[k] -> draw();

void f(Shape& s){
	s.move(10,20);
	s.draw();
}

Circle c;
f(c);
c.move(30,40);
c.draw();


"heterogeneous collection"
"strongly-typed languges"

inheritance

compile-time binding 	static binding
runtime binding			dynamic binding

Derived* ==> Base*
Derived& ==> Base&

void Shape::move(double xnew, double ynew)
{
	m_x = xnew;
	m_y = ynew;
}

void WarningSymbol::move(double xnew, double ynew)
{
	Shape::move(xnew, ynew);
	... flash 3 times ...
}

WarningSymbol ws;
ws.move(10, 20); // this calls WarningSymbol's move function
f(ws); // this calls shape's move function

void Shape::draw() const
{
	... draw a cloud centered at (m_x, m_y) ...
}

void Circle::draw() const
{
	... draw a circle of specified radius m_r centered at m_x, m_y
}

void Rectangle::draw() const
{
	... draw the rectangle at the points ... 
}

 double Rectangle::diag() const
 {
 	return sqrt(m_dx * m_dx + m_dy * m_dy);
 }


Constructor:
1. Construct the base part
2. Construct the data members
3. Execute the body of the constructor


Destructor:
1. Execute the body of the destructor
2. Destroy the data memebers
3. Destroy the base part


==================================================================
Lecuture Notes (2.9.18)
==================================================================


class Shape
{
public:
	Shape(double x, double y);
	Shape();
	virtual ~Shape(){} 
	...
private:
	string m_name;
	double m_x;
	double m_y;
};

Shape::Shape(double x, double y)
	: m_x(x), m_y(y)
{}

Shape::Shape()
	: Shape(0,0)
{}

class Circle : public Shape{
public:
	Circle(double r, double x, double y);
	Circle(double r);
private:
	double m_r;
};


Circle::Circle(double r, double x, double y)
	: Shape(x,y), m_r(r)
{
	if(r<=0)
		...
}

Circle::Circle(double r)
	: Circle(r, 0, 0)
{}

class Polygon : public Shape
{
	~Polygon();
}

Polygon::~Polygon()
{
	... delete the nodes ...
}


// If a class is designed to be a base class you
// have to declare its constructor to be virtual
void f()
{
	Shape* pic[100];
	...
	... add various types of shapes to the pic array ...
	... 		pic[i[= new Polygon(......)]]
	for(int k = 0; k < ...; k++){
		delete pic[k];
	}
}



Compiler-generated destructor:
	Circle 


===========
Recurssion:
===========

sort(an unsorted pile of papers)
{
	if(the number of items in the pile >= 2)
	{
		split the pil evenly into two unsorted subpiles
		get the left pile sorted
		get the right pile sorted
		merge the two sorted sub piles into one sorted pile
	}
}


To show that it terminates:
	There must be at least one "base case" (i.e. on ethat is solved non recursivly)

	Every recursive call must be to solve a strictly "smaller" problem (i.e. one that
		is closer to the base case)


void sort(int a[], int b, int e){
	if(e - b >=2)
	{
		int mid = (b+e)/2;
		sort(a,b,mid); // sort left half
		A sort(a,mid,e); // sort right half
		B merge(a,b,mid,e); // merge two halves
	C}
}

environment of
main: [0] [1] [2] [3] [4]
  arr: 40  30  20  50  10

sort1: b:0 e:5 mid:2 return to D
sort2: b:0 e:2 mid:1 return to A
sort3: b:0 e:1 		 return to A
sort4: b:1 e:2 		 return to B
merge1: b:0 mid:1 e:2 return to C








