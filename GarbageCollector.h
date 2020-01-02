#ifndef GARBAGE_COLLECTOR
#define GARBAGE_COLLECTOR

#include <iostream>
#include <list>
#include <typeinfo>
#include <cstdlib>


using namespace std;

//#define DISPLAY

class OutOfRangeException {
	OutOfRangeException() {
		setlocale(LC_ALL, "");
		cout << "Adres wykroczy³ poza skalê!!!" << endl;
	}
};

template <class T> class Iter {
	T* ptr;
	T* end;
	T* begin;
	unsigned length;

public:
	//konstruktory
	Iter() {
		ptr = end = begin = NULL; ///NULL
		length = 0;
	}

	Iter(T* p, T* first, T* last) {
		ptr = p;
		end = last;
		begin = first;
		length = last - first;
	}
	//
	unsigned size() { return length; }


	//zwraca wartoœæ wskaŸnika
	T& operator*() {
		if ((ptr >= end) || (ptr < begin))
			throw OutOfRangeException();
		return *ptr;
	}
	//zwraca adres wskaŸnika
	T* operator->() {
		if ((ptr >= end) || (ptr < begin))
			throw OutOfRangeException();
		return ptr;
	}

	//prefixy
	Iter operator++() {
		ptr++;
		return *this;
	}

	Iter operator--() {
		ptr--;
		return *this;
	}

	//postfixy
	Iter operator++(int notused) {
		T* tmp = ptr;
		ptr++;
		return Iter<T>(tmp, begin, end);
	}

	Iter operator--(int notused) {
		T* tmp = ptr;
		ptr--;
		return Iter<T>(tmp, begin, end);
	}






	////zwracanie wartoœci w punkcie tabeli
	T& operator[](int i) {
		if ((i < 0) || (i >= (end - begin)))
			throw OutOfRangeException();
		return ptr[i];
	}



	////operatory relacji
	bool operator==(Iter op2) {
		return ptr == op2.ptr;
	}

	bool operator!=(Iter op2) {
		return ptr != op2.ptr;
	}

	bool operator<(Iter op2) {
		return ptr < op2.ptr;
	}

	bool operator<=(Iter op2) {
		return ptr <= op2.ptr;
	}

	bool operator>(Iter op2) {
		return ptr > op2.ptr;
	}

	bool operator>=(Iter op2) {
		return ptr >= op2.ptr;
	}


	//Matematyczne
	Iter operator-(int n) {
		ptr -= n;
		return *this;
	}

	Iter operator+(int n) {
		ptr += n;
		return *this;
	}

	int operator-(Iter<T>& itr2) {
		return ptr - itr2.ptr;
	}
};




//////Klasa definiuj¹ca elementy przechowywane w garbage kolektorze
template <class T> class GCInfo {
public:
	unsigned refcount;
	T* memPtr; //wsaŸnik do zaalokowanej pamiêci
	bool isArray;  //true gdy wskazuje na tablicê
	unsigned arraySize; //wielkoœæ zaalokowanej tablicy

	GCInfo(T* mPtr, unsigned size = 0) {
		refcount = 1;
		memPtr = mPtr;
		if (size != 0)
			isArray = true;
		else
			isArray = false;

		arraySize = size;
	}
};

template <class T>
bool operator==(const GCInfo<T>& ob1, const GCInfo<T>& ob2) {
	return (ob1.memPtr == ob2.memPtr);
}

template <class T, int size = 0> class GCPtr {
	static list<GCInfo<T>> gclist; //lista œmieci

	T* addr; //wskaŸnik do zaalokowanej pamiêci
	bool isArray;
	unsigned arraySize;

	unsigned Length;  //beta

	static bool first;

	typename list<GCInfo<T>>::iterator findPtrInfo(T* ptr);


public:
	typedef Iter<T> GCiterator;

	GCPtr(T* t = NULL) { ///NULL
		if (first) atexit(shutdown);
		first = false;

		typename list<GCInfo<T>>::iterator p;

		p = findPtrInfo(t);

		//je¿eli t jest w liœcie to zwiêkszyæ liczbê referencji
		if (p != gclist.end())
			p->refcount++;
		else {
			GCInfo<T> gcObj(t, size);
			gclist.push_front(gcObj);
		}

		addr = t;
		arraySize = size;
		if (size > 0) isArray = true;
		else isArray = false;

		Length = size;  //beta

#ifdef  DISPLAY
		cout << "Constructing GCPtr. ";
		if (isArray)
			cout << " Size [" << arraySize << "]" << endl;
		else
			cout << endl;
#endif //  DISPLAY
	}

	//Konstruktor kopiuj¹cy
	GCPtr(const GCPtr& ob) {
		typename list<GCInfo<T>>::iterator p;

		p = findPtrInfo(ob.addr);
		p->refcount++;

		addr = ob.addr;
		arraySize = ob.arraySize;
		if (arraySize > 0) isArray = true;
		else isArray = false;

#ifdef DISPLAY
		cout << "Constructing copy. ";
		if (isArray)
			cout << " Size [" << arraySize << "]" << endl;
		else
			cout << endl;
#endif // DISPLAY
	}

	//Destruktor
	~GCPtr();

	//Zbieranie œmieciów
	static bool collect(bool show = false);

	T* operator=(T* t);

	GCPtr& operator=(GCPtr& rv);
	void operator=(const GCPtr& rv); //beta

	T& operator*() {
		return *addr;
	}

	T* operator->() {
		return addr;
	}

	T& operator[](int i) {
		return addr[i];
	}

	operator T* () {
		return addr;
	}

	Iter<T> begin() {
		int Size;
		if (isArray) Size = arraySize;
		else Size = 1;

		return Iter<T>(addr, addr, addr + Size);
	}

	Iter<T> end() {
		int Size;
		if (isArray) Size = arraySize;
		else Size = 1;

		return Iter<T>(addr + Size, addr, addr + Size);
	}

	static int gclistSize() {
		return gclist.size();
	}

	static void showlist();

	static void shutdown();

	unsigned length() { //beta
		return Length;
	}

private:
	void set_length() { //beta
		typename list<GCInfo<T>>::iterator p;
		p = findPtrInfo(addr);
		this->Length = *p->memPtr;
	}
};

//Rezerwowanie pamiêci
template <class T, int size>
list<GCInfo<T>>GCPtr<T, size>::gclist;

template <class T, int size>
bool GCPtr<T, size>::first = true;


//destruktor
template <class T, int size>
GCPtr<T, size>::~GCPtr() {
	typename list<GCInfo<T>>::iterator p;

	p = findPtrInfo(addr);
	if (p->refcount) p->refcount--;

#ifdef DISPLAY
	cout << "GCPtr going out of scope.\n";
#endif

	//Zbieranie œmieci gdy wskaŸnik wychodzi poza zakres

	collect();

	//niewykluczone, ¿e to siê zmieni na usuwanie dopiero
	//gdy kilka wskaŸników wyjdzie poza zakres
	//do sprawdzenia przy dzia³aniu programu
}

//Collect
template <class T, int size>
bool GCPtr<T, size>::collect(bool showCount) {
	bool memfreed = false;

#ifdef DISPLAY
	cout << "Before garbage collection: ";
	showlist();
#endif

	typename list<GCInfo<T>>::iterator p;
	do {
		//skanowanie listy w poszukiwaniu wskaŸników bez referencji
		for (p = gclist.begin(); p != gclist.end(); p++) {
			if (p->refcount > 0) continue;

			memfreed = true;

			//free memory
			if (p->memPtr) {
				if (p->isArray) {
#ifdef DISPLAY
					cout << "Deleting array of size: " << p->arraySize << endl;
#endif
					if (showCount) cout << "Deleting array of size: " << p->arraySize << endl;
					delete[] p->memPtr;
				}
				else {
#ifdef DISPLAY
					cout << "Deleting: " << *(T*)p->memPtr << "\n";
#endif
					delete p->memPtr;
				}
			}
			//remove from list
			gclist.remove(*p);

			break;
		}
	} while (p != gclist.end());

#ifdef DISPLAY
	cout << "After garbage collector: ";
	showlist();
#endif // DISPLAY

	return memfreed;
}

template <class T, int size>
T* GCPtr<T, size>::operator=(T* t) {  //new T
	typename list<GCInfo<T>>::iterator p;

	p = findPtrInfo(addr);
	p->refcount--;

	p = findPtrInfo(t);
	if (p != gclist.end())
		p->refcount++;
	else {
		GCInfo<T> gcObj(t, size);
		gclist.push_front(gcObj);
	}

	addr = t;
	this->set_length();  //beta
	return t;
}


template <class T, int size>
GCPtr<T, size>& GCPtr<T, size>::operator=(GCPtr& rv) {  //set object
	typename list<GCInfo<T>>::iterator p;

	p = findPtrInfo(addr);
	p->refcount--;

	p = findPtrInfo(rv.addr);
	p->refcount++;

	addr = rv.addr;
	return rv;
}

template <class T, int size>
void GCPtr<T, size>::operator=(const GCPtr& rv) {  //set const object //beta
	typename list<GCInfo<T>>::iterator p;

	p = findPtrInfo(addr);
	p->refcount--;

	this->addr = rv.addr;
	this->arraySize = rv.arraySize;
	this->first = rv.first;
	this->gclist = rv.gclist;
	this->isArray = rv.isArray;
	this->Length = rv.Length;

	p = findPtrInfo(addr);
	p->refcount++;

	//return *this;
}


template <class T, int size>
void GCPtr<T, size>::showlist() {
	typename list<GCInfo<T>>::iterator p;

	cout << "gclist<" << typeid(T).name() << ", "
		<< size << ">:\n";
	cout << "memPtr        refcount      value\n";

	if (gclist.begin() == gclist.end()) {
		cout << "                      -- Empty --\n\n";
		return;
	}

	for (p = gclist.begin(); p != gclist.end(); p++) {
		cout << "[" << (void*)p->memPtr << "]"
			<< "        " << p->refcount << "      ";
		if (p->memPtr) cout << "    " << *p->memPtr;
		else cout << "   ---";
		cout << endl;
	}
	cout << endl;
}

//Find pointer in gclist
template <class T, int size>
typename list<GCInfo<T>>::iterator
GCPtr<T, size>::findPtrInfo(T* ptr) {
	typename list<GCInfo<T>>::iterator p;

	for (p = gclist.begin(); p != gclist.end(); p++)
		if (p->memPtr == ptr)
			return p;

	return p;
}

//Czyszczenie pamiêci gdy wychodzimy z programu
template <class T, int size>
void GCPtr<T, size>::shutdown() {
	if (gclistSize() == 0) return; //gdy lista pusta

	typename list<GCInfo<T>>::iterator p;

	for (p = gclist.begin(); p != gclist.end(); p++) {
		p->refcount = 0;
	}

#ifdef DISPLAY
	cout << "Before collecting for shutdown() for "
		<< typeid(T).name() << endl;
#endif // DISPLAY

	collect();

#ifdef DISPLAY
	cout << "After collection for shutdown() for "
		<< typeid(T).name() << endl;
#endif // DISPLAY
}

#endif