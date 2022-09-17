#include "cbuffer.h"
#include <iostream>
#include <cassert>
#include <string>
/**
 * @brief Struct person che rappresenta una persona.
 * 
 *  Struct person che rappresenta una persona.
 */
struct person{
    std::string name;///<  nome della persona
    std::string surname;///< cognome della persona

    /**
    * @brief Costruttore di default
    */
    person(): name(""), surname(""){}
    /**
     * @brief Costruttore secondario
     * 
     * @param name nome della persona
     * @param surname cognome della persona
     */
    person(const std::string name, const std::string surname)
        :name(name), surname(surname){}
    
    /**
     * @brief Operatore assegnamento
     * Necessario per la classe sparsematrix
     * 
     * @param other persona
     * @return reference dell'oggetto persona this
     */
    person& operator=(const person &other){
        name=other.name;
        surname=other.surname;
        return *this;
    } 
     /**
     * @brief Ridefinizione dell'operatore di stream << per un point.
     * Necessario per l'operatore di stream della classe sparsematrix.
     * 
     * */
    friend std::ostream& operator<<(std::ostream &os, const person &p){
        return os<<"["<<p.name<<","<<p.surname<<"]"; 
    }
    /**
     * @brief Ridefinizione dell'operatore assegnamento
     * 
     * @param other reference di un oggetto persona
     * @return true se i dati membri dell'oggetto this sono uguali a
     * quelli dell'oggetto other
     * @return false altrimenti
     */
    bool operator==(const person &other) const{
      return name == other.name && surname == other.surname;
    }

    /**
     * @brief Funzione che verifica se il nome della persona
     * inizia con il carattere passato come parametro
     * 
     * @param c carattere iniziale
     * @return true se il nome inizia con il carattere passato in input
     * @return false altrimenti
     */
    bool name_stars_with(const char &c) const{
      return name.at(0) == c;
    }
};

/**
 * @brief Funzione che carica i dati nel cbuffer
 * 
 * @param b riferimento del cbuffer 
 */
void load_data(cbuffer<int> &b){
  for(unsigned int i = 0; i < b.size()/2; ++i)
      b.enqueue(i-10);
}
/**
 * @brief Funzione che carica i dati nel cbuffer
 * 
 * @param b riferimento del cbuffer 
 */
void load_data(cbuffer<std::string> &b){
  b.enqueue("Java");
  b.enqueue("C++");
  b.enqueue("PHP");
  b.enqueue("Go lang");
  b.enqueue("SQL");
}
/**
 * @brief Funzione che carica i dati nel cbuffer
 * 
 * @param b riferimento del cbuffer 
 */
void load_data(cbuffer<person> &b){
  person vettore[5] = {
    person("Giulio", "Cesare"), person("Marco", "Aurelio"), person("Ottaviano", "Augusto"),
    person("Tito", "Flavio"), person("Lucio", "Vero")};
    for(unsigned int i = 0; i < 5; ++i)
      b.enqueue(vettore[i]);
}
/**
 * @brief Test sui metodi fondamentali con cbuffer di tipo int
 * 
 */
void test_metodi_fondamentali_int(){
  cbuffer<int> b;
  cbuffer<int> c(10);
  //test costruttore default
  assert(b.size() == 0 && b.stored_elements() == 0);
  assert(!b.is_full());
  assert(b.is_empty());
  try{
    int head = b.head();
  }catch(const empty_queue_exception &e){
    std::cout<<e.what()<<std::endl;
    try{
      int tail = b.tail();
    }catch(const empty_queue_exception &e){
      std::cout<<e.what()<<std::endl;
    }
  }
  try{
      b[0];
    }catch(const std::out_of_range &e){
      std::cout<<e.what()<<std::endl;
  }
  cbuffer<int> a(b);
  assert(a.size() == 0 && a.stored_elements() == 0);
  assert(!a.is_full());
  assert(a.is_empty());
  for(unsigned int i = 0; i < c.size()-2; ++i)
    c.enqueue(i+10);
  assert(c.size() == 10 && c.stored_elements() == 8);
  c.enqueue(42);
  c.enqueue(43);
  assert(c.stored_elements() == 10);
  assert(c.is_full());
  assert(!c.is_empty());
  std::cout<< c <<std::endl;
  //test operatore assegnamento
  a = c;
  std::cout<< a <<std::endl;
  assert(a.size() == c.size() && a.stored_elements() == c.stored_elements());
  assert(a.tail() == c.tail() && a.head() == c.head());
  assert(a[2] == c[2]);

  //test costruttore templato
  double vettore[5] = {10.5,30.3,60.9,40.999,-20.333};
  try{
    cbuffer<int> eq(0, vettore, vettore + 5); // size = 0
  }catch(const empty_queue_exception &e){
    std::cout<< e.what() <<std::endl;
  }
  try{
    cbuffer<int> x(-314, vettore, vettore + 5); // size < 0
  }catch(const negative_queue_size_exception &e){
    std::cout<< e.what() <<std::endl;
  }
  cbuffer<int> empty_queue(100, vettore, vettore); // coda vuota
  assert(empty_queue.size() == 100 && empty_queue.stored_elements() == 0);
  assert(!empty_queue.is_full());
  assert(empty_queue.is_empty());
  try{
    empty_queue[101];
  }catch(const std::out_of_range &e){
    std::cout<< e.what() <<std::endl;
  }

  cbuffer<int> d(5, vettore, vettore + 5); //coda piena
  std::cout<< d <<std::endl;
  assert(d.size() == 5 && d.stored_elements() == d.size());
  assert(d.head() == 10 && d.tail() == -20);
  for(unsigned int j = 0; j < d.size(); ++j)
      assert((int)vettore[j] == d[j]);

  try{
    cbuffer<int> queue(-100, vettore, vettore + 5);
  }catch(const negative_queue_size_exception &e){
    std::cout<<e.what()<<std::endl;
  }

  cbuffer<int> x(2, vettore, vettore + 5); //size < vettore size 
  std::cout<< x <<std::endl;
  assert(x.size()==2);
  assert(x.head() == 40 && x.tail() == -20);
  //test clear usata dal distruttore
  d.clear();
  assert(d.stored_elements() == 0);
  assert(!d.is_full());
  assert(d.is_empty());
  std::cout<<"Coda vuota: "<<d<<std::endl;

}

/**
 * @brief Test sui metodi fondamentali con cbuffer di tipo person
 * 
 */
void test_metodi_fondamentali_person(){
  cbuffer<person> b;
  cbuffer<person> c(10);
  //test cbuffer vuoto
  assert(b.size() == 0 && b.stored_elements() == 0);
  assert(!b.is_full());
  assert(b.is_empty());
  try{
    person head = b.head();
  }catch(const empty_queue_exception &e){
    std::cout<<e.what()<<std::endl;
    try{
      person tail = b.tail();
    }catch(const empty_queue_exception &e){
      std::cout<<e.what()<<std::endl;
    }
  }
  try{
      b[0];
    }catch(const std::out_of_range &e){
      std::cout<<e.what()<<std::endl;
  }
  cbuffer<person> a(b);
  assert(a.size() == 0 && a.stored_elements() == 0);
  assert(!a.is_full());
  assert(a.is_empty());
  for(unsigned int i = 0; i < c.size()-2; ++i)
    c.enqueue(person("Mario"+1, "Rossi"+i));
  assert(c.size() == 10 && c.stored_elements() == 8);
  c.enqueue(person("Mario "+99, "Rossi "+99));
  c.enqueue(person("Mario "+98, "Rossi "+98));
  assert(c.stored_elements() == 10);
  assert(c.is_full());
  assert(!c.is_empty());
  //test operatore assegnamento
  a = c;
  assert(a.size() == c.size() && a.stored_elements() == c.stored_elements());
  assert(a.tail() == c.tail() && a.head() == c.head());
  assert(a[2] == c[2]);

  //test costruttore templato
  person vettore[5] = {
    person("Giulio", "Cesare"), person("Marco", "Aurelio"), person("Ottaviano", "Augusto"),
    person("Tito", "Flavio"), person("Lucio", "Vero")};
  try{
    cbuffer<person> eq(0, vettore, vettore + 5); // size = 0
  }catch(const empty_queue_exception &e){
    std::cout<< e.what() <<std::endl;
  }
  try{
    cbuffer<person> x(-314, vettore, vettore + 5); // size < 0
  }catch(const negative_queue_size_exception &e){
    std::cout<< e.what() <<std::endl;
  }
  cbuffer<person> empty_queue(100, vettore, vettore); // coda vuota
  assert(empty_queue.size() == 100 && empty_queue.stored_elements() == 0);
  assert(!empty_queue.is_full());
  assert(empty_queue.is_empty());
  try{
    empty_queue[101];
  }catch(const std::out_of_range &e){
    std::cout<< e.what() <<std::endl;
  }

  cbuffer<person> d(5, vettore, vettore + 5); //coda piena
  std::cout<< d <<std::endl;
  assert(d.size() == 5 && d.stored_elements() == d.size());
  assert(d.head() == person("Giulio", "Cesare") && d.tail() == person("Lucio", "Vero"));
  for(unsigned int j = 0; j < d.size(); ++j)
      assert(vettore[j] == d[j]);

  try{
    cbuffer<person> queue(-100, vettore, vettore + 5);
  }catch(const negative_queue_size_exception &e){
    std::cout<<e.what()<<std::endl;
  }
  //test clear usata dal distruttore
  d.clear();
  assert(d.stored_elements() == 0);
  assert(!d.is_full());
  assert(d.is_empty());
  std::cout<<"Coda vuota: "<<d<<std::endl;

  cbuffer<person> x(2, vettore, vettore + 5); //size < vettore size 
  std::cout<< x <<std::endl;
  assert(x.size()==2);
  assert(x.head() == person("Tito", "Flavio") && x.tail() ==  person("Lucio", "Vero")); 

}

/**
 * @brief Test dei metodi di cbuffer su un cbuffer const di tipo int
 * 
 * @param b riferimento al cbuffer 
 */
void test_cbuffer_const_int(const cbuffer<int> &b){ 
  std::cout<< b << std::endl;
  assert(b.head() == -10 && b.tail() == -1);
  assert(!b.is_full());
  assert(!b.is_empty());
  assert(b.stored_elements() < b.size());
  assert(b.stored_elements() == 10 && b.size() == 20);
  assert(b[0] == -10);//testa
  assert(b[1] == -9);
  assert(b[b.stored_elements()-1] == -1);
  cbuffer<int>::const_iterator be, e;
  be = b.begin();
  e = b.end();
  std::cout<<"Queue: ";
  for(; be != e; ++be)
    std::cout<< *be <<" ";
  std::cout<<std::endl;
}

/**
 * @brief Test dei metodi di cbuffer su un cbuffer const di tipo person
 * 
 * @param b riferimento al cbuffer 
 */
void test_cbuffer_const_person(const cbuffer<person> &b){
  std::cout<< b << std::endl;
  assert(b.head() == person("Giulio", "Cesare") && b.tail() == person("Lucio", "Vero"));
  assert(b.is_full());
  assert(!b.is_empty());
  assert(b.stored_elements() == b.size());
  assert(b.stored_elements() == 5 && b.size() == 5);
  assert(b[0] == person("Giulio", "Cesare"));//testa
  assert(b[1] == person("Marco", "Aurelio"));
  assert(b[2] == person("Ottaviano", "Augusto"));
  cbuffer<person>::const_iterator be, e;
  be = b.begin();
  e = b.end();
  std::cout<<"Queue: ";
  for(; be != e; ++be)
    std::cout<< *be <<" ";
  std::cout<<std::endl;
}

/**
 * @brief Test dei metodi di cbuffer su un cbuffer const di tipo string
 * 
 * @param b riferimento al cbuffer 
 */
void test_cbuffer_const_string(const cbuffer<std::string> &b){
  std::cout<< b << std::endl;
  assert(b.head() == "Java" && b.tail() == "SQL");
  assert(b.is_full());
  assert(!b.is_empty());
  assert(b.stored_elements() == b.size());
  assert(b.stored_elements() == 5 && b.size() == 5);
  assert(b[0] == "Java");//testa
  assert(b[1] == "C++");
  assert(b[b.stored_elements()-1] == "SQL");
  cbuffer<std::string>::const_iterator be, e;
  be = b.begin();
  e = b.end();
  std::cout<<"Queue: ";
  for(; be != e; ++be)
    std::cout<< *be <<" ";
  std::cout<<std::endl;
}

/**
 * @brief Test dei metodi di cbuffer su un cbuffer di tipo int
 *  
 */
void test_cbuffer_int(){
  //Test enqueue/dequeue
  cbuffer<int> b(20);
  b.clear();
  for(unsigned int i = 0; i < b.size()/2; ++i)
      b.enqueue(i+5);
  std::cout<< b <<std::endl;
  assert(b.size() == 20 && b.stored_elements() == 10);
  int x = 0;
  for(unsigned int i = 0; i < 5; ++i){ //tolgo 5 
    x = b.dequeue();
    std::cout<<"Value deleted: "<< x <<std::endl;
  }
  std::cout<< b <<std::endl;
  assert(!b.is_full());
  assert(b.stored_elements() == 5);
  assert(b.head() == 10 && b.tail() == 14);
  b.head() = 1000;
  b.tail() = 9999;
  assert(b.head() == 1000 && b.tail()==9999);
  b[1] = 42;
  assert(b[1] == 42);
  std::cout<< b <<std::endl;
  x = b.dequeue();
  x=100000;
  std::cout<< b <<std::endl;
  for(unsigned int i = 0; i < 16; ++i)
      b.enqueue(-1-i);
  std::cout<< b <<std::endl;
  b.enqueue(88); 
  std::cout<< b <<std::endl;

  for(unsigned int i = 0; i < 100; ++i){
      b.enqueue(i+4);
  }
  std::cout<< b <<std::endl;
  cbuffer<int> empty(0);
  try{
    empty.enqueue(10);
  }catch(const empty_queue_exception &e){
    std::cout<< e.what() <<std::endl;
  }
  
  b.clear();
  try{
    b.dequeue();
  }catch(const empty_queue_exception &e){
    std::cout<< e.what() <<std::endl;
  }
  b.enqueue(1);
  assert(b.size() == 20 && b.stored_elements() == 1);
  std::cout<< b <<std::endl;
  for(unsigned int i = 0; i < b.size(); ++i)
      b.enqueue(i+5);
  std::cout<< b <<std::endl;
  b.enqueue(9999);
  b.enqueue(9999);
  b.enqueue(9999);
  std::cout<< b <<std::endl;

}

/**
 * @brief Test dei metodi di cbuffer su un cbuffer di tipo string
 *  
 */
void test_cbuffer_string(){
  //Test enqueue/dequeue
  cbuffer<std::string> b(20);
  for(unsigned int i = 0; i < b.size()/2; ++i)
      b.enqueue("test"+std::to_string(i+5));
  

  std::cout<< b <<std::endl;
  assert(b.size() == 20 && b.stored_elements() == 10);
  std::string x = "";
  for(unsigned int i = 0; i < 5; ++i){ //tolgo 5 
    x = b.dequeue();
    std::cout<<"Value deleted: "<< x <<std::endl;
  }
  std::cout<< b <<std::endl;
  assert(!b.is_full());
  assert(b.stored_elements() == 5);
  assert(b.head() == "test10" && b.tail() == "test14");
  b.head() = "test1000";
  b.tail() = "test9999";
  assert(b.head() == "test1000" && b.tail()=="test9999");
  b[1] = "test42";
  assert(b[1] == "test42");
  std::cout<< b <<std::endl;
  x = b.dequeue();
  x="test100000";
  std::cout<< b <<std::endl;
  for(unsigned int i = 0; i < 16; ++i)
      b.enqueue("test"+std::to_string(i));
  std::cout<< b <<std::endl;
  b.enqueue("test88"); 
  std::cout<< b <<std::endl;

  for(unsigned int i = 0; i < 100; ++i){
      b.enqueue("test"+std::to_string(i+4));
  }
  std::cout<< b <<std::endl;
  cbuffer<int> empty(0);
  try{
    empty.enqueue(10);
  }catch(const empty_queue_exception &e){
    std::cout<< e.what() <<std::endl;
  }
  
  b.clear();
  try{
    b.dequeue();
  }catch(const empty_queue_exception &e){
    std::cout<< e.what() <<std::endl;
  }
  b.enqueue("test");
  assert(b.size() == 20 && b.stored_elements() == 1);
  std::cout<< b <<std::endl;
  for(unsigned int i = 0; i < b.size(); ++i)
      b.enqueue("test"+std::to_string(i+5));
  std::cout<< b <<std::endl;
  b.enqueue("test9999");
  b.enqueue("test9999");
  b.enqueue("test99999");
  std::cout<< b <<std::endl;
}

/**
 * @brief Test dei metodi di cbuffer su un cbuffer di tipo person
 *  
 */
void test_cbuffer_person(){
  //Test enqueue/dequeue
  cbuffer<person> b(20);
  b.clear();
  for(unsigned int i = 0; i < b.size()/2; ++i)
      b.enqueue(person("nome"+std::to_string(i+5),"cognome"+std::to_string(i+5)));
  assert(b.size() == 20 && b.stored_elements() == 10);
  person x;
  for(unsigned int i = 0; i < 5; ++i){ //tolgo 5 
    x = b.dequeue();
    std::cout<<"Value deleted: "<< x <<std::endl;
  }
  
  assert(!b.is_full());
  assert(b.stored_elements() == 5);
  assert(b.head() == person("nome"+std::to_string(10),"cognome"+std::to_string(10)));
  assert(b.tail() == person("nome14","cognome14"));
  b.head() = person("nome1000","cognome1000");
  b.tail() = person("nome9999","cognome9999");
  assert(b.head() == person("nome1000","cognome1000") && b.tail()==person("nome9999","cognome9999"));
  b[1] = person("nome55","cognome55");
  assert(b[1] == person("nome55","cognome55"));
  x = b.dequeue();

  for(unsigned int i = 0; i < 16; ++i)
      b.enqueue(person("nome"+std::to_string(-1),"cognome"+std::to_string(-1)));

  b.enqueue(person("nome88","cognome88")); 

  for(unsigned int i = 0; i < 100; ++i){
      b.enqueue(person("nome"+std::to_string(i+i),"cognome"+std::to_string(i+4)));
  }
  cbuffer<int> empty(0);
  try{
    empty.enqueue(10);
  }catch(const empty_queue_exception &e){
    std::cout<< e.what() <<std::endl;
  }
  
  b.clear();
  try{
    b.dequeue();
  }catch(const empty_queue_exception &e){
    std::cout<< e.what() <<std::endl;
  }
  b.enqueue(person());
  assert(b.size() == 20 && b.stored_elements() == 1);

  for(unsigned int i = 0; i < b.size(); ++i)
      b.enqueue(person("nome"+std::to_string(i+5),"cognome"+std::to_string(i+5)));

  b.enqueue(person("nome88","cognome88"));
  b.enqueue(person("nome88","cognome88"));
  b.enqueue(person("nome88","cognome88"));
  for(unsigned int i = 0; i<5; ++i)
      std::cout<< b[i] <<" ";
  std::cout<<std::endl;
}

/**
 * @brief Test classe iteraror su cbuffer di tipo person
 *  
 */
void test_iterator_person(){
  //test iterator person
  cbuffer<person> queue(10);
  load_data(queue);
  std::cout<< queue <<std::endl;
  cbuffer<person>::iterator j, k;
  
  j = queue.begin();
  k = j;
  cbuffer<person>::const_iterator t(k);
  assert(*j == *k && *j == queue[0] && *k == queue.head());
  ++k;
  assert(k != t);
  k++;
  assert(*k == person("Ottaviano", "Augusto"));
  assert(k != j);
  assert(k->name_stars_with('O'));
  assert(k->name == "Ottaviano");
  assert(*k == queue[2]);
  j = queue.begin();
  k = queue.end();
  std::cout<<"Imperatori romani: ";
  for(; j != k; ++j)
      std::cout<< *j <<" ";
  std::cout<<std::endl;

  //operatori
  j = queue.begin();
  k = queue.end();
  assert(j[0]==person("Giulio", "Cesare"));
  assert(j[10]==person("Giulio", "Cesare"));
  assert(j[-20]==person("Giulio", "Cesare"));
  j++;
  assert(j[0]==person("Marco", "Aurelio"));
  j--;
  assert(j[0]==person("Giulio", "Cesare"));
  j--;
  assert(j[0]==person("Lucio", "Vero")); //coda

  j = queue.begin();
  j = j + 1;
  assert(*j == person("Marco", "Aurelio"));



}

/**
 * @brief Test classe iteraror su cbuffer di tipo double
 *  
 */
void test_iterator_double(){
  //test iterator double
  cbuffer<double> b(10);
  cbuffer<double>::iterator be, e, empty_iter;
  for(unsigned int j = 0; j < b.size(); ++j) 
      b.enqueue((double)j/2);

  std::cout<< b <<std::endl;
  assert(be == e);
  cbuffer<double>::iterator copy(e);
  assert(copy == e);
  be = b.begin(); //punta alla testa
  e = b.end(); //punta nullptr perchè la coda è piena
  assert(be != e);
  std::cout<<"Queue: ";
  while(be != e){
    std::cout<< *be<< " ";
    be++;
  }
  std::cout<<std::endl;
  be = b.begin();
  e = b.end();
  assert(*be == b.head());
  cbuffer<double>::const_iterator clone(be);
  copy = be;
  assert(copy == be && clone == be); // iterator == const iterator
  assert(*copy == *be && *clone == *be);
  clone++;
  assert(clone != be); 
  assert(*clone != *be);
  ++be;
  assert(clone == be); 
  assert(*clone == *be);
  assert(*be == 0.5);
  ++(++(++clone));
  assert(*clone == 2);
  std::cout<<"Queue: ";
  assert(be != e);
  std::cout<< b<< std::endl;
  be = b.begin();
  e = b.end();
  
  //assert(*(be+=1) == 0.5);
  be+=2;
  std::cout<<*be<< std::endl;
  be = be + 2;
  std::cout<<*be<< std::endl;


  /*be = b.begin();
  e = b.end();
  while(be != e){ //sovrascrivo tutti i dati
      *be = 42;
      be++;
  }
  assert(b.head() == 42 && b.tail() == 42 && b[4] == 42);
  std::cout<< b<< std::endl;*/

 
}

/**
 * @brief Test classe iteraror su cbuffer di tipo string
 *  
 */
void test_iterator_string(){
  //test iterator string
  cbuffer<std::string> s(20);
  load_data(s);
  std::cout<< s <<std::endl;
  cbuffer<std::string>::iterator p, q, c;
  
  p = s.begin();
  q = p;

  assert(q == p);
  assert(*p == "Java" && *q == s[0]);
  c = p;
  cbuffer<std::string>::iterator h(c);
  assert(c == p && p == h);
  assert(*c == "Java");
  p++;
  assert(*p == "C++");
  q++;
  assert(p == q && *p == *q);
  *p = "C#";
  assert(*q == "C#");
  cbuffer<std::string>::const_iterator const_iter = p;
  std::cout<< s <<std::endl;
  assert(q == const_iter);
  assert(*q == *const_iter);
  ++p;
  assert(p != const_iter);
  q->append(" stringa inutile");
  assert(*q == "C# stringa inutile");
  assert(q->find("#") > 0);
  std::cout<< s <<std::endl;

}

/**
 * @brief Test classe const_iteraror su cbuffer di tipo double
 *  
 */
void test_const_iterator_double(const cbuffer<int> &a){
  cbuffer<double> b(10);
  cbuffer<double>::const_iterator be, e, empty_iter;
  for(unsigned int j = 0; j < b.size(); ++j) 
      b.enqueue((double)j/2);

  std::cout<< b <<std::endl;
  be = b.begin(); 
  e = b.end(); 
  assert(be != e);
  std::cout<<"Queue: [";
  while(be != e){
    std::cout<< *be<< " ";
    be++;
  
  }
  std::cout<<"]"<<std::endl;

  
  assert(be == e);
  cbuffer<double>::const_iterator copy(e);
  assert(copy == e);
  std::cout<<std::endl;
  be = b.begin();
  e = b.end();
  assert(*be == b.head());
  cbuffer<double>::const_iterator clone(be);
  copy = be;
  assert(copy == be && clone == be); // iterator == const iterator
  assert(*copy == *be && *clone == *be);
  clone++;
  assert(clone != be); 
  assert(*clone != *be);
  ++be;
  assert(clone == be); 
  assert(*clone == *be);
  assert(*be == 0.5);
  ++(++(++clone));
  assert(*clone == 2);
  assert(be != e);
  std::cout<< b<< std::endl;

  //conversioni
  cbuffer<double>::iterator iter;
  cbuffer<double>::const_iterator const_iter;
  cbuffer<double> test(10);
  for(unsigned int i = 0; i<test.size(); i++, test.enqueue(i));
  iter=test.begin();
  const_iter = iter;
  assert(const_iter == iter);
  const_iter++;
  assert(const_iter != iter);
  cbuffer<double>::const_iterator copy_iter(iter);
  assert(copy_iter == iter);
  copy_iter++;
}

/**
 * @brief Test classe const_iteraror su cbuffer di tipo string
 *  
 */
void test_const_iterator_string(const cbuffer<std::string> &s){
  std::cout<< s <<std::endl;
  cbuffer<std::string>::const_iterator p, q, c;
  assert(p == q);
  p = s.begin();
  q = p;
  assert(q == p);
  assert(*p == "Java" && *q == s[0]);
  c = p;
  cbuffer<std::string>::const_iterator h(c);
  assert(c == p && p == h);
  assert(*c == "Java");
  p++;
  assert(*p == "C++");
  q++;
  assert(p == q && *p == *q);
  cbuffer<std::string>::const_iterator const_iter = p;
  std::cout<< s <<std::endl;
  assert(q == const_iter);
  assert(*q == *const_iter);
  ++p;
  assert(p != const_iter);
  assert(q->find("#") > 0);
  std::cout<< s <<std::endl;

  p = s.begin(); 
  q = s.end(); 
  std::cout<<"Queue: [";
  while(p != q){
    std::cout<< *p<< " ";
    p++;
  
  }
  std::cout<<"]"<<std::endl;

  //conversioni
  cbuffer<std::string>::iterator iter;
  cbuffer<std::string> test(10);
  for(unsigned int i = 0; i<test.size(); i++, test.enqueue("test"+std::to_string(i)));
  iter=test.begin();
  const_iter = iter;
  assert(const_iter == iter);
  const_iter++;
  assert(const_iter != iter);
  cbuffer<std::string>::const_iterator copy_iter(iter);
  assert(copy_iter == iter);
  copy_iter++;
  assert(copy_iter != iter);


}

/**
 * @brief Test classe const_iteraror su cbuffer di tipo person
 *  
 */
void test_const_iterator_person(const cbuffer<person> &c){
  cbuffer<person> queue(10);
  load_data(queue);
  std::cout<< queue <<std::endl;
  cbuffer<person>::const_iterator j, k;
  assert(j == k);
  j = queue.begin();
  k = j;
  cbuffer<person>::const_iterator t(k);
  assert(j == k && *j == queue[0] && *k == queue.head());
  assert(k == t);
  ++k;
  assert(k != t);
  k++;
  assert(*k == person("Ottaviano", "Augusto"));
  assert(k != j);
  assert(k->name_stars_with('O'));
  assert(k->name == "Ottaviano");
  assert(*k == queue[2]);
  j = queue.begin();
  k = queue.end();
  std::cout<<"Imperatori romani: ";
  for(; j != k; ++j)
      std::cout<< *j <<" ";
  std::cout<<std::endl;

  //conversioni
  cbuffer<person>::iterator iter;
  cbuffer<person> test(10);
  for(unsigned int i = 0; i<test.size(); i++, test.enqueue(person("mario","rossi")));
  iter=test.begin();
  t = iter;
  assert(t == iter);
  t++;
  assert(t != iter);
  cbuffer<person>::const_iterator copy_iter(iter);
  assert(copy_iter == iter);
  copy_iter++;
  assert(copy_iter != iter);

  j = c.begin(); 
  k = c.end(); 
  std::cout<<"Queue: [";
  while(j != k){
    std::cout<< *j<< " ";
    j++;
  
  }
  std::cout<<"]"<<std::endl;
  //riparte da capo
  assert(*j == c.head());
  j++;
  assert(*j == j[0]);

}

/**
 * @brief Test su cbuffer di tipo cbuffer di double
 *  
 */
void test_cbuffer_of_cbuffer(){
  typedef cbuffer<cbuffer<double>> buffer;
  buffer b;
  assert(b.size() == 0 && b.stored_elements() == 0);
  buffer cb(5);
  assert(cb.size() == 5);
  cbuffer<double> element(5);
  for(unsigned int i = 0; i < element.size(); ++i)
      element.enqueue(i);
  for(unsigned int i = 0; i < cb.size(); ++i)
      cb.enqueue(element);
  
  buffer::const_iterator be, e;
  be = cb.begin();
  e = cb.end();
  std::cout<<"Queue made of queue: ";
  while(be != e){
    cbuffer<double>::const_iterator x,y;
    x = (*be).begin();
    y = (*be).end();
    std::cout<<"[ ";
    while(x != y){
      std::cout<<*x<<" ";
      x++;
    }
    std::cout<<"] ";
    ++be;
  }
  std::cout<<std::endl;
  try{
    cb[100];
  }catch(const std::out_of_range &e){
    std::cout<< e.what() <<std::endl;
  }
  cbuffer<double> ee(4);
  for(unsigned int i = 0; i < ee.size(); ++i)
      ee.enqueue(i+20);
  buffer another_copy, copy(cb);
  assert(copy.size() == cb.size());
  another_copy = cb;
  assert(another_copy.size() == cb.size());

  cbuffer<double> removed_buffer = cb.dequeue();
  assert(cb.stored_elements() == cb.size() - 1);
  assert(!cb.is_full());
  std::cout<< cb.tail() << std::endl;
  cb.clear();
  try{
      cb.dequeue();
  }catch(const empty_queue_exception &e){
    std::cout<< e.what() <<std::endl;
  }
  
}

int main(){
  cbuffer<int> buffer_int(20);
  cbuffer<person> buffer_person(5);
  cbuffer<std::string> buffer_string(5);
  load_data(buffer_int);
  load_data(buffer_person);
  load_data(buffer_string);

  test_cbuffer_const_int(buffer_int);
  test_cbuffer_const_person(buffer_person);
  test_cbuffer_const_string(buffer_string);
  test_metodi_fondamentali_int();
  test_metodi_fondamentali_person();
  test_cbuffer_int();
  test_cbuffer_string();
  test_cbuffer_person();
  test_iterator_person();
  test_iterator_string();
  test_iterator_double();
  test_const_iterator_double(buffer_int);
  test_const_iterator_string(buffer_string);
  test_const_iterator_person(buffer_person);
  test_cbuffer_of_cbuffer();

  return 0;
}