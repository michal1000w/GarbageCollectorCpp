#include <iostream>
#include "GarbageCollector.h"


using namespace std;


int main() {
	GCPtr<int> a;

	for (int i = 0; i < 9000; i++) {
		//a = new int(i);
		a = new int[1000000];
	}
	/*
	GCPtr<double, 10000> b;
	for (int i = 0; i < 100000; i++) {
		b = new double[10000];
		if (i % 10000 == 0) b.collect();
	}

	GCPtr<double, 10000> z;
	for (int i = 0; i < 100000; i++) {
		z = new double[10000];
		if (i % 10000 == 0) z.collect();
	}


	GCPtr<double, 10000000> c;
	for (int i = 0; i < 100; i++) {
		c = new double[10000000];
	}

	GCPtr<double, 10000000> c2;
	for (int i = 0; i < 100; i++) {
		c2 = new double[10000000];
	}


	system("pause");
	a.collect();
	b.collect();
	c.collect();
	z.collect();
	c2.collect();
	cout << "Gotowe" << endl;
	
	a.shutdown();
	b.shutdown();
	c.shutdown();
	z.shutdown();
	c2.shutdown();
	*/
	system("pause");
	a.collect();

	system("pause");
}