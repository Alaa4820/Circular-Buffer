#ifndef CBUFFER_H
#define CBUFFER_H
#include <algorithm>
#include <ostream>
#include <cassert>
#include <math.h> 
#include <iostream>
#include <iterator> // std::random_iterator_tag
#include <cstddef> // std::ptrdiff_t
#include "negative_queue_size_exception.h"
#include "empty_queue_exception.h"
/**
 * @brief Classe cbuffer
 * 
 * La classe implementa una generica coda circolare
 * 
 * @tparam T Tipo degli elementi contenuti nella coda
 */
template<typename T> class cbuffer{

    int _head; ///< indice della testa
    int _tail; ///< indice della coda
    unsigned int _size; ///< dimensione massima della coda
    unsigned int _stored_elements; ///< numero di elementi salvati
    T *_queue; ///< puntatore all'array in cui sono salvati i dati
   

    /**
     * @brief Funzione di supporto per eliminare dalla
     * memoria tutti i dati
     */
    void erase(){
        delete[] _queue;
        _queue = nullptr;
        _head = _tail = -1;
        _size = _stored_elements = 0;
    }

    public:
        /**
         * @brief Costruttore di dafault
         * 
         * @post _head == -1
         * @post _tail == -1
         * @post _size == 0
         * @post _stored_elements == 0
         * @post _queue == nullptr
         * 
         */
        cbuffer(): _head(-1), _tail(-1), _size(0), _stored_elements(0), _queue(nullptr){}

        /**
         * @brief Costruttore secondario
         * 
         * @param size dimensione massima della coda
         * @post _head == -1
         * @post _tail == -1
         * @post _size == size
         * @post _stored_elements == 0
         * @post _queue != nullptr
         * @throw negative_queue_size_exception eccezione lanciata in caso di dimensione strettamente negativa
         * @throw std::bad_alloc eccezione lanciata in caso di allocazione dell'array non riuscita
         */
        explicit cbuffer(int size):_head(-1), _tail(-1), _size(0), _stored_elements(0), _queue(nullptr){
            if(size < 0)
                throw negative_queue_size_exception("Cannot create a cbuffer with a negative size");
            try{
                _size = size;
                _queue = new T[_size];
            }catch(...){
                erase();
                throw;
            }
        }
        
        /**
         * @brief Copy constructor
         * 
         * @param other coda da copiare
         * 
         * @post _head == other._head
         * @post _tail == other._tail
         * @post _size == other._size
         * @post _stored_elements == other._stored_elements
         * @post _queue != nullptr
         * @throw std::bad_alloc eccezione lanciata in caso di allocazione dell'array non riuscita
         */
        cbuffer(const cbuffer &other):_head(-1), _tail(-1), _size(0), _stored_elements(0), _queue(nullptr){
            try{
                _size = other._size;
                _queue = new T[other._size];
                int head = other._head;
                int tail = other._tail;
                if(!other.is_empty()){
                    if(head <= tail){//coda lineare
                        while(head <= tail){
                            enqueue(other._queue[head]);
                            head++;
                        }
                    }else { //coda circolare
                        while (head < other._size ) { // seconda metà
                            enqueue(other._queue[head]);
                            head++;
                        }
                        head = 0;
                        while (head <= tail) { //prima metà
                            enqueue(other._queue[head]);
                            head++;
                        }
                    }
                }
            }catch(...){
                erase();
                throw;
            }
        }

        /**
         * @brief Costruttore templato sul tipo Q
         * 
         * @tparam Q tipo dell'iteratore passato in input
         * @param size dimensione con cui costruire la coda
         * @param b iteratore di inizio
         * @param e iteratore di fine
         * @throw negative_queue_size_exception eccezione lanciata in caso di dimenzione negativa (< 0)
         * @throw empty_queue_exception eccezione lanciata in caso di dimenzione nulla
         */
        template<typename Q> cbuffer(int size, Q b, Q e): _head(-1), _tail(-1), _size(0), _stored_elements(0), _queue(nullptr){
            if(size < 0)
                throw negative_queue_size_exception("Cannot create a cbuffer with a negative size");
            if(size == 0)
                throw empty_queue_exception("Cannot create a cbuffer from the iterators due to: size = 0");
            try{
                _size = size;
                _queue = new T[_size];
                for(; b != e; ++b)
                    enqueue(static_cast<T>(*b));

            }catch(...){
                erase();
                throw;
            }   
        }

        /**
         * @brief Operatore assegnamento
         * 
         * @param other coda da copiare
         * @return cbuffer& riferimento al cbuffer this
         * 
         * @post _head == other._head
         * @post _tail == other._tail
         * @post _size == other._size
         * @post _stored_elements == other._stored_elements
         * @post _queue != nullptr
         */
        cbuffer& operator=(const cbuffer &other){
            if(this != &other){
                cbuffer tmp(other);
                std::swap(_head, tmp._head);
                std::swap(_tail, tmp._tail);
                std::swap(_size, tmp._size);
                std::swap(_stored_elements, tmp._stored_elements);
                std::swap(_queue, tmp._queue);
            }
            return *this;
        }

        /**
         * @brief Distruttore
         * 
         */
        ~cbuffer(){
            erase();
        }

        /**
         * @brief Funzione che imposta gli indici di testa e coda a -1
         * e il numero di elementi salvati a 0
         * 
         * @post _head == -1
         * @post _tail == -1
         * @post _stored_elements == 0
         */
        void clear(){
            delete[] _queue;
            _queue = new T[_size];
            _head = _tail = -1;
            _stored_elements = 0;
        }


        /**
         * @brief Funzione che accoda il valore passato in input secondo
         * la logica FIFO
         * 
         * @param value valore da inserire in test
         * 
         * @post _stored_elements = _stored_elements + 1
         * @throw empty_queue_exception eccezione lanciata in caso di aggiunta su una coda con size pari a 0
         */
        void enqueue(const T& value){
            if(_size <= 0)
                throw empty_queue_exception("Cannot add an element in an empty queue");

            if ((_tail + 1) % _size == _head){ //coda piena: tail si trova indietro di una posizione (mod) a head
                _tail = (_tail + 1) % _size; //sposto la coda di una posizione 
                _queue[_tail] = value; //_queue non sarà mai nullptr
                _head = (_head + 1) % _size;
                return; //stored elements non varia
            }
            if (_head == -1 && _tail == -1) // inserimento primo elemento
                _head = _tail = 0;
            else 
                _tail = (_tail + 1) % _size; //se la coda contiene dei dati (non piena) sposto tail
            
            _queue[_tail] = value;
            _stored_elements++;
        }

        /**
         * @brief Funzione che rimuove l'elemento in testa
         * 
         * @return T& riferimento dell'elemento rimosso
         * @post _stored_elements = _stored_elements - 1
         * @throw empty_queue_exception eccezione lanciata in caso di rimozione di un elemento da una coda vuota
         */
        T& dequeue(){
            if(is_empty())
                throw empty_queue_exception("Cannot remove an element from an empty queue");
            
            T* value = &_queue[_head]; // elemento in testa
            _stored_elements--;
            if(_head == _tail)// caso: rimozione dell'ultimo elemento 
                _head = _tail = -1;
            else
                _head = (_head + 1) % _size; 

            return *value; 
        }

        /**
         * @brief Funzione che ritorna la testa della coda
         * 
         * @return T& riferimento della testa
         * @throw empty_queue_exception eccezione lanciata nel caso la coda avesse dimensione nulla
         */
        T& head() const{ // la testa può essere modificata
            if(is_empty())
                throw empty_queue_exception("Cannot get the head from an empty queue");
            return _queue[_head];
        }
        /**
         * @brief Funzione che ritorna la coda della coda
         * 
         * @return T& T& riferimento della coda
         * @throw empty_queue_exception eccezione lanciata nel caso la coda avesse dimensione nulla
         */
        T& tail() const{
            if(is_empty())
                throw empty_queue_exception("Cannot get the tail from an empty queue");
            return _queue[_tail];
        }

        /**
         * @brief Funzione che ritorna true o false nel caso
         * in cui la coda è piena
         * 
         * @return true se la coda è piena
         * @return false se la coda non è piena
         */
        bool is_full() const{
            if (_head == 0 && _tail == _size - 1) //caso: enqueue fino a rimpiere il buffer
                return true;
            
            if (_head == _tail + 1) // caso: _tail parte da 0 a causa dell'incremento circolare
                return true;
            
            return false;
        }
        /**
         * @brief Funzione che ritorna true o false nel caso
         * in cui la coda è vuota
         * 
         * @return true se la coda è vuota
         * @return false se la coda non è vuota
         */
        bool is_empty() const{
            if (_head == -1 && _tail == -1)
                return true;
            else
                return false;
        }
        /**
         * @brief Funzione che ritorna la dimensione massima della coda
         * 
         * @return unsigned int dimensione della coda
         */
        unsigned int size() const{
            return _size;
        }
        /**
         * @brief Funzione che ritorna il numero di elementi salvati
         * nella coda
         * 
         * @return unsigned int 
         */
        unsigned int stored_elements() const{
            return _stored_elements;
        }

        /**
         * @brief Operatore di stream
         * 
         * @param os stream di output
         * @param b cbuffer da spedire sullo stream
         * @return std::ostream& reference dello stream di output
         */
        friend std::ostream& operator<<(std::ostream &os, const cbuffer &b){
            if(!b.is_empty()){
                os<<"Head: " <<b._head <<" - value: ["<< b._queue[b._head] <<"] "<<std::endl;
                os<<"Tail: "<<b._tail<<" - value: ["<< b._queue[b._tail] <<"] "<<std::endl;
                os<<"Size: "<<b.size()<<std::endl;
                os<<"Stored elements: "<<b.stored_elements()<<std::endl;
                int head = b._head;
                int tail = b._tail;
                os<<"[ ";
                if(head <= tail){
                    while(head <= tail){
                        os << b._queue[head] << " ";
                        head++;
                    }
                }else {
                    while (head < b.size() ) { //stampo la seconda metà
                        os<<b._queue[head]<<" ";
                        head++;
                    }
                    head = 0;
                    while (head <= tail) { //stampo la prima metà
                        os<<b._queue[head]<<" ";
                        head++;
                    }
                }
                unsigned int i = b.stored_elements(); 
                while(i < b.size()){
                    os<<"# ";
                    i++;
                }
                os<<"]";
            }else
                os<<"[]";
            
            return os;
        }

        /**
         * @brief Operator[]
         * 
         * @param index indice
         * @return T& riferimento dell'elemento nella posizione index
         * @throw std::out_of_range eccezione lanciata in caso di indice fuori range
         */
        T& operator[](int index){
            if (index < 0 || index >= _size) 
				throw std::out_of_range("Cannot call the operator[] due to an index out of bound");

			return _queue[(_head + index ) % _size];
		}

        /**
         * @brief Operator[] const
         * 
         * @param index indice
         * @return T& riferimento dell'elemento nella posizione index
         * @throw std::out_of_range eccezione lanciata in caso di indice fuori range
         */
        const T& operator[](int index) const {
			if (index < 0 || index >= _size) 
				throw std::out_of_range("Cannot call the operator[] due to an index out of bound");

			return _queue[(_head + index) % _size];
		}

        /**
         * @brief Classe const_iterator
         * Gli iteratori iterano sui dati contenuti nella coda
         */
        class const_iterator;

        class iterator {	
            public:
                typedef std::forward_iterator_tag iterator_category;
                typedef T                         value_type;
                typedef ptrdiff_t                 difference_type;
                typedef T*                        pointer;
                typedef T&                        reference;

                /**
                 * @brief Costruttore di default
                 * 
                 */
                iterator() : _ptr(nullptr), _cbuffer(nullptr) , _index(-1){}

                /**
                 * @brief Copy constructor
                 * 
                 * @param other iteratore da cui copiare i dati
                 */
                iterator(const iterator &other) : _ptr(other._ptr), _cbuffer(other._cbuffer), 
                    _index(other._index), _distance(other._distance){}

                /**
                 * @brief Operatore assegnamento
                 * 
                 * @param other iteratore da cui copiare i dati
                 * @return reference all'iteratore this 
                 */
                iterator& operator=(const iterator &other) {
                    _ptr = other._ptr;
                    _cbuffer = other._cbuffer;
                    _index = other._index;
                    _distance = other._distance;
                    return *this;
                }

                /**
                 * @brief Distruttore
                 * 
                 */
                ~iterator() {}

                /**
                 * @brief Operatore*
                 * @return reference al dato riferito dall'iteratore (dereferenziamento) 
                 */
                reference operator*() const {
                    return *_ptr;  
                }

                /**
                 * @brief Operatore-> 
                 * 
                 * @return puntatore al dato riferito dall'iteratore 
                 */
                pointer operator->() const {
                    return _ptr;
                }

                /**
                 * @brief Operatore di accesso random
                 * 
                 * @param index indice
                 * @return reference riferimento del valore in posizione index
                 */
		        reference operator[](int index) {
                    if(_cbuffer != nullptr && !_cbuffer->is_empty()){
                        if(index >= 0){
                            index = index % _cbuffer->_stored_elements;  
                        }else{ 
                            int q = static_cast<int>(floor(static_cast<double>(index) / _cbuffer->_stored_elements));
                            index =  index - _cbuffer->_stored_elements * q;
                        }

                        return _cbuffer->_queue[(_index + index) % _cbuffer->_size];  
                    }
                    return *_ptr;  
		        }
                /**
                * @brief Operatore++ di post-incremento
                * @return copia dell'iteratore che punta al valore precedente
                */
                iterator operator++(int) {
                    iterator tmp(*this);
                    if(_cbuffer != nullptr)
                        _ptr = next(_ptr);
                    return tmp;
                }

                /**
                 * @brief Operatore++ pre-incremento
                 * @return reference all'teratore this
                 */
                iterator& operator++() {
                    if(_cbuffer != nullptr)
                        _ptr = next(_ptr);
                    return *this;
                }

                /**
                 * @brief Operatore di iterazione post-decremento
                 * 
                 * @return const_iterator& reference iteratore corrente
                 */ 
                iterator operator--(int) {
                    iterator tmp(*this);
                    if(_cbuffer != nullptr)
                        _ptr = previous(_ptr);
                    return tmp;
                }

                /**
                 * @brief Operatore di iterazione pre-decremento
                 * 
                 * @return const_iterator& reference iteratore corrente
                 */
                iterator &operator--() {
                    if(_cbuffer != nullptr)
                        _ptr = previous(_ptr);
                    return *this;
                }

                /**
                 * @brief Spostamento in avanti
                 * 
                 * @param offset scostamento
                 * @return iterator iteratore che punta al nuovo dato
                 */
                iterator operator+(int offset) {
                    int index = offset % _cbuffer->_stored_elements;  
                    index = (_index + index) % _cbuffer->_size;
                    iterator tmp = *this;
                    if(_cbuffer != nullptr && !_cbuffer->is_empty())
                        return iterator(&tmp[offset], _cbuffer, index, index);
                    
                    return *this;
                      
                }

                /**
                 * @brief Spostamento all'indietro
                 * 
                 * @param offset scostamento
                 * @return iterator iteratore che punta al nuovo dato
                 */
                iterator operator-(int offset) {
                    iterator tmp = *this;
                    return tmp + (- offset); 
                }
		
                /**
                 * @brief Spostamento in avanti
                 * 
                 * @param offset scostamento
                 * @return iterator& iteratore corrente che punta al nuovo dato
                 */
                iterator& operator+=(int offset) {
                    iterator tmp = *this;
                    tmp = tmp + offset;
                    _ptr = tmp._ptr;
                    _index = tmp._index;
                    _distance = tmp._distance;
                    return *this;

                }

		        /**
                 * @brief Spostamento all'indietro
                 * 
                 * @param offset scostamento
                 * @return iterator& iteratore corrente che punta al nuovo dato
                 */
                iterator& operator-=(int offset) {
                    iterator tmp = *this;
                    tmp = tmp - offset;
                    _ptr = tmp._ptr;
                    _index = tmp._index;
                    _distance = tmp._distance;
                    return *this;
                }

                /**
                 * @brief Numero di elementi tra due iteratori
                 * 
                 * @param other iteratore other
                 * @return difference_type 
                 */
                difference_type operator-(const iterator &other) {
                    if(_cbuffer != other._cbuffer)
                        return difference_type(0);
                       
                    if(_cbuffer->_head <= _cbuffer->_tail)// coda lineare
                        return (_ptr - other._ptr);
                
                    return difference_type(_cbuffer->_size) + _ptr - other._ptr;
                }

                /**
                 * @brief Operatore==
                 * 
                 * @param other iteratore con cui fare il confronto
                 * @return true se l'iteratore this e other puntano allo stesso dato
                 * @return false se l'iteratore this e other non puntano allo stesso dato
                 */
                bool operator==(const iterator &other) const {
                    if(_cbuffer){
                        pointer tail = &_cbuffer->_queue[_cbuffer->_tail];
                        pointer head = &_cbuffer->_queue[_cbuffer->_head];
                        if(_ptr == head && other._ptr == head && _distance > other._distance)
                            return true;
                    }
                    
                    return (_ptr == other._ptr);
                }

                /**
                 * @brief Operatore!=
                 * 
                 * @param other iteratore con cui fare il confronto
                 * @return true se l'iteratore this e other non puntano allo stesso dato
                 * @return false se l'iteratore this e other puntano allo stesso dato
                 */
                bool operator!=(const iterator &other) const {
                    if(_cbuffer){
                        std::cout<<_distance << "<=" << other._distance<< std::endl;
                        pointer tail = &_cbuffer->_queue[_cbuffer->_tail];
                        pointer head = &_cbuffer->_queue[_cbuffer->_head];
                        if(_ptr == head && other._ptr == head && _distance <= other._distance)
                            return true;
                    }
                    return (_ptr != other._ptr);
                }

                /**
                 * @brief Operatore di confronto
                 * 
                 * @param other iterator
                 * @return true se l'iteratore corrente è > dell'iteratore other
                 * @return false altrimenti
                 */
                bool operator>(const iterator &other) const {
                    if(_cbuffer != other._cbuffer) //puntano a cbuffer diversi
                        return (_ptr > other._ptr); 

                    if(_cbuffer == nullptr)
                        return false;

                    if(_cbuffer->_head < _cbuffer->_tail) //coda lineare
                        return (_ptr > other._ptr);

                    return _ptr < other._ptr;//coda circolare
                }
		
                /**
                 * @brief Operatore di confronto
                 * 
                 * @param other iterator
                 * @return true se l'iteratore corrente è >= dell'iteratore other
                 * @return false altrimenti
                 */
                bool operator>=(const iterator &other) const {
                    if(_cbuffer != other._cbuffer) //puntano a cbuffer diversi
                        return (_ptr >= other._ptr); // return false ?

                    if(_cbuffer == nullptr)
                        return false;

                    if(_cbuffer->_head <= _cbuffer->_tail) //coda lineare
                        return (_ptr >= other._ptr);

                    return _ptr <= other._ptr;//coda circolare
                }

                /**
                 * @brief Operatore di confronto
                 * 
                 * @param other iterator
                 * @return true se l'iteratore corrente è < dell'iteratore other
                 * @return false altrimenti
                 */
                bool operator<(const iterator &other) const {
                    if(_cbuffer != other._cbuffer) //puntano a cbuffer diversi
                        return (_ptr < other._ptr); // return false ?

                    if(_cbuffer == nullptr)
                        return false;

                    if(_cbuffer->_head < _cbuffer->_tail) //coda lineare
                        return (_ptr < other._ptr);

                    return _ptr > other._ptr;//coda circolare
                }
		
		
                /**
                 * @brief Operatore di confronto
                 * 
                 * @param other iterator
                 * @return true se l'iteratore corrente è <= dell'iteratore other
                 * @return false altrimenti
                 */
                bool operator<=(const iterator &other) const {
                    if(_cbuffer != other._cbuffer) //puntano a cbuffer diversi
                        return (_ptr <= other._ptr); // return false ?

                    if(_cbuffer == nullptr)
                        return false;

                    if(_cbuffer->_head <= _cbuffer->_tail) //coda lineare
                        return (_ptr < other._ptr);

                    return _ptr >= other._ptr;//coda circolare
                }
                
                friend class const_iterator;///< friend della classe const_iterator

                /**
                 * @brief Operatore==
                 * 
                 * @param other iteratore costante con cui fare il confronto
                 * @return true se l'iteratore this e other puntano allo stesso dato
                 * @return false se l'iteratore this e other non puntano allo stesso dato
                 */
		        bool operator==(const const_iterator &other) const {
			        if(_cbuffer){
                        pointer tail = &_cbuffer->_queue[_cbuffer->_tail];
                        pointer head = &_cbuffer->_queue[_cbuffer->_head];
                        if(_ptr == head && other._ptr == head && _distance > other._distance)
                            return true;
                    }
                    
                    return (_ptr == other._ptr);

		        }

                /**
                 * @brief Operatore!=
                 * 
                 * @param other iteratore costante con cui fare il confronto
                 * @return true se l'iteratore this e other non puntano allo stesso dato
                 * @return false se l'iteratore this e other puntano allo stesso dato
                 */
                bool operator!=(const const_iterator &other) const {
                    pointer tail = &_cbuffer->_queue[_cbuffer->_tail];
                    pointer head = &_cbuffer->_queue[_cbuffer->_head];
                    if(_ptr == head && other._ptr == head && _distance <= other._distance )
                        return true;

                    return (_ptr != other._ptr);
                }
        
                /**
                 * @brief Operatore di confronto
                 * 
                 * @param other const_iterator
                 * @return true se l'iteratore corrente è > dell'iteratore other
                 * @return false altrimenti
                 */
                bool operator>(const const_iterator &other) const {
                    if(_cbuffer != other._cbuffer) 
                        return _ptr > other._ptr; 

                    if(_cbuffer == nullptr)
                        return false;

                    if(_cbuffer->_head < _cbuffer->_tail) 
                        return _ptr > other._ptr;

                    return _ptr < other._ptr;
                }
		

                /**
                 * @brief Operatore di confronto
                 * 
                 * @param other const_iterator
                 * @return true se l'iteratore corrente è >= dell'iteratore other
                 * @return false altrimenti
                 */
                bool operator>=(const const_iterator &other) const {
                    if(_cbuffer != other._cbuffer) 
                        return _ptr >= other._ptr; 

                    if(_cbuffer == nullptr)
                        return false;

                    if(_cbuffer->_head <= _cbuffer->_tail) 
                        return _ptr >= other._ptr;

                    return _ptr <= other._ptr;
                }

                /**
                 * @brief Operatore di confronto
                 * 
                 * @param other const_iterator
                 * @return true se l'iteratore corrente è < dell'iteratore other
                 * @return false altrimenti
                 */
                bool operator<(const const_iterator &other) const {
                    if(_cbuffer != other._cbuffer) 
                        return _ptr < other._ptr; 

                    if(_cbuffer == nullptr)
                        return false;

                    if(_cbuffer->_head < _cbuffer->_tail) 
                        return _ptr < other._ptr;

                    return _ptr > other._ptr;
                }

                /**
                 * @brief Operatore di confronto
                 * 
                 * @param other const_iterator
                 * @return true se l'iteratore corrente è <= dell'iteratore other
                 * @return false altrimenti
                 */
                bool operator<=(const const_iterator &other) const {
                    if(_cbuffer != other._cbuffer)
                        return _ptr <= other._ptr; 

                    if(_cbuffer == nullptr)
                        return false;

                    if(_cbuffer->_head <= _cbuffer->_tail)
                        return _ptr <= other._ptr;

                    return _ptr >= other._ptr;
                }

           // private:
                friend class cbuffer;///< friend della classe cbuffer
                cbuffer *_cbuffer;///< puntatore al cbuffer corrente
                T *_ptr;///< puntatore al valore
                int _index;///< posizione nell'array del valore puntato da _ptr
                unsigned int _distance;///< distanza del puntatore dalla testa
                
                
                /**
                 * @brief Costruttore privato
                 * 
                 * @param q puntatore di tipo cbuffer
                 * @param ptr puntatore al valore
                 * @param index indice del puntatore ptr
                 */
                iterator(T *ptr, cbuffer *q, int index, unsigned int distance)
                    : _cbuffer(q), _ptr(ptr), _index(index), _distance(distance){}

                /**
                 * @brief Funzione che sposta il puntatore di una posizione in avanti
                 * 
                 * @param ptr puntatore da spostare
                 * @return T* puntatore spostato
                 */
                T* next(T* ptr){
                    if(ptr == &_cbuffer->_queue[_cbuffer->_tail]){
                        if(_distance == _cbuffer->_size - 1)
                            _distance = _index;

                        ptr = &_cbuffer->_queue[_cbuffer->_head];
                        _index = _cbuffer->_head;
                        _distance = (_distance + 1) % (_cbuffer->_size + 1);
                        
                    }else{
                        ptr = &_cbuffer->_queue[(_index + 1) % _cbuffer->_size];
                        _index = (_index + 1) % _cbuffer->_size;
                       _distance = (_distance + 1) % _cbuffer->_size;
                        
                    }
                    return ptr;
                }
                /**
                 * @brief Funzione che sposta il puntatore di una posizione in dietro
                 * 
                 * @param ptr puntatore da spostare
                 * @return const T* const puntatore spostato
                 */
                T* previous(T* ptr){
                    int prev  = (_index - 1) % _cbuffer->_stored_elements; 
                    int q = static_cast<int>(floor(static_cast<double>(prev) / _cbuffer->_stored_elements));
                    prev = prev - _cbuffer->_stored_elements * q;
                    if(ptr == &_cbuffer->_queue[_cbuffer->_head]){
                        ptr = &_cbuffer->_queue[_cbuffer->_tail];
                        _index = _cbuffer->_tail;
                        _distance = _cbuffer->_tail;
                    }else{
                        ptr = &_cbuffer->_queue[(_index - 1) % _cbuffer->_size];
                        _index = (_index - 1) % _cbuffer->_size;
                        _distance --;
                        
                    }
                    
                    return ptr;  
                }
        
            
        }; // classe iterator
        
        /**
         * @brief Classe const_iterator
         * Gli iteratori iterano sui dati contenuti nella coda
         * 
         */
        class const_iterator {
            
            public:
                typedef std::forward_iterator_tag iterator_category;
                typedef T                         value_type;
                typedef ptrdiff_t                 difference_type;
                typedef const T*                  pointer;
                typedef const T&                  reference;
                
                /**
                 * @brief Costruttore di default 
                 */
                const_iterator() : _ptr(nullptr), _cbuffer(nullptr), _index(-1){}

                /**
                 * @brief Copy constructor
                 * @param other iteratore da cui copiare i dati
                 */
                const_iterator(const const_iterator &other) : _ptr(other._ptr), _cbuffer(other._cbuffer),
                    _index(other._index), _distance(other._distance){}

                /**
                 * @brief Operatore assegnamento
                 * 
                 * @param other iteratore da cui copiare i dati
                 * @return reference all'iteratore this 
                 */
                const_iterator& operator=(const const_iterator &other) {
                    _ptr = other._ptr;
                    _cbuffer = other._cbuffer;
                    _index = other._index;
                    _distance = other._distance;
                    return *this;
                }

                /**
                 * @brief Costruttore di conversione iterator -> const_iterator
                 * 
                 * @param other iteratore
                 */
                const_iterator(const iterator &other) {
                    _ptr = other._ptr;
                    _cbuffer = other._cbuffer;
                    _index = other._index;
                    _distance = other._distance;
                }

                /**
                 * @brief Distruttore
                 * 
                 */
                ~const_iterator() {}

                /**
                 * @brief Operatore*
                 * 
                 * @return reference al dato riferito dall'iteratore (dereferenziamento) 
                 */
                reference operator*() const {
                    return *_ptr;
                }

                /**
                 * @brief Operatore-> 
                 * 
                 * @return puntatore al dato riferito dall'iteratore 
                 */
                pointer operator->() const {
                    return _ptr;
                }
                
                /**
                 * @brief Operatore di accesso random
                 * 
                 * @param index indice
                 * @return reference riferimento del valore in posizione index
                 */
                reference operator[](int index) {
                    if(_cbuffer != nullptr && !_cbuffer->is_empty()){
                        if(index >= 0){
                            index = index % _cbuffer->_stored_elements;  
                        }else{ 
                            int q = static_cast<int>(floor(static_cast<double>(index) / _cbuffer->_stored_elements));
                            index =  index - _cbuffer->_stored_elements * q;
                        }  
                        return _cbuffer->_queue[(_index + index) % _cbuffer->_size];  
                    }
                    return *_ptr; 
                }
                
                /**
                 * @brief Operatore++ di post-incremento
                 * @return copia dell'iteratore che punta al valore precedente
                 */
                const_iterator operator++(int) {
                    const_iterator tmp(*this);
                    if(_cbuffer != nullptr)
                        _ptr = next(_ptr);
                    return tmp;
                }

                /**
                 * @brief Operatore++ pre-incremento
                 * @return reference all'teratore this
                 */
                const_iterator& operator++() {
                    if(_cbuffer != nullptr)
                        _ptr = next(_ptr);
                    
                    return *this;
                }

                /**
                 * @brief Operatore di iterazione post-decremento
                 * 
                 * @return const_iterator& reference iteratore corrente
                 */
                const_iterator operator--(int) {
                    const_iterator tmp(*this);
                    if(_cbuffer != nullptr)
                        _ptr = previous(_ptr);
                    return tmp;
                }

                /**
                 * @brief Operatore di iterazione pre-decremento
                 * 
                 * @return const_iterator& reference iteratore corrente
                 */
                const_iterator &operator--() {
                    if(_cbuffer != nullptr)
                        _ptr = previous(_ptr);
                    return *this;
                }

                /**
                 * @brief Spostamento in avanti
                 * 
                 * @param offset scostamento
                 * @return const_iterator iteratore che punta al nuovo dato
                 */
                const_iterator operator+(int offset) {
                    int index = offset % _cbuffer->_stored_elements;  
                    index = (_index + index) % _cbuffer->_size;
                    const_iterator tmp = *this;
                    if(_cbuffer != nullptr && !_cbuffer->is_empty())
                        return const_iterator(&tmp[offset], _cbuffer, index, index);
                    
                    return *this;
                }

               /**
                 * @brief Spostamento all'indietro
                 * 
                 * @param offset scostamento
                 * @return const_iterator iteratore che punta al nuovo dato
                 */
                const_iterator operator-(int offset) {
                    const_iterator tmp = *this;
                    return tmp + (- offset);
                }
                
                /**
                 * @brief Spostamento in avanti
                 * 
                 * @param offset scostamento
                 * @return const_iterator& iteratore corrente che punta al nuovo dato
                 */
                const_iterator& operator+=(int offset) {
                    const_iterator tmp = *this;
                    tmp = tmp + offset;
                    _ptr = tmp._ptr;
                    _index = tmp._index;
                    _distance = tmp._distance;
                    return *this;
                }

                /**
                 * @brief Spostamento all'indietro
                 * 
                 * @param offset scostamento
                 * @return const_iterator& iteratore corrente che punta al nuovo dato
                 */
                const_iterator& operator-=(int offset) {
                    const_iterator tmp = *this;
                    tmp = tmp - offset;
                    _ptr = tmp._ptr;
                    _index = tmp._index;
                    _distance = tmp._distance;
                    return *this;
                }

               /**
                 * @brief Numero di elementi tra due iteratori
                 * 
                 * @param other iteratore costante other
                 * @return difference_type 
                 */
                difference_type operator-(const const_iterator &other) {
                    if(_cbuffer != other._cbuffer)
                        return difference_type(0);
                       
                    if(_cbuffer->_head <= _cbuffer->_tail)// coda lineare
                        return (_ptr - other._ptr);
                
                    return difference_type(_cbuffer->_size) + _ptr - other._ptr;
                }

                /**
                 * @brief Operatore==
                 * 
                 * @param other iteratore costante con cui fare il confronto
                 * @return true se l'iteratore this e other puntano allo stesso dato
                 * @return false se l'iteratore this e other non puntano allo stesso dato
                 */
                bool operator==(const const_iterator &other) const {
                    if(_cbuffer){
                        pointer tail = &_cbuffer->_queue[_cbuffer->_tail];
                        pointer head = &_cbuffer->_queue[_cbuffer->_head];
                        if(_ptr == head && other._ptr == head && _distance > other._distance)
                            return true;
                    }
                    
                    return (_ptr == other._ptr);

                }
                
                /**
                 * @brief Operatore!=
                 * next
                 * @param other iteratore costante con cui fare il confronto
                 * @return true se l'iteratore this e other non puntano allo stesso dato
                 * @return false se l'iteratore this e other puntano allo stesso dato
                 */
                bool operator!=(const const_iterator &other) const {
                    if(_cbuffer){
                        pointer tail = &_cbuffer->_queue[_cbuffer->_tail];
                        pointer head = &_cbuffer->_queue[_cbuffer->_head];
                        if(_ptr == head && other._ptr == head && _distance <= other._distance)
                            return true;
                    }
                    return (_ptr != other._ptr);
                }


                /**
                 * @brief Operatore di confronto
                 * 
                 * @param other const_iterator
                 * @return true se l'iteratore corrente è > dell'iteratore other
                 * @return false altrimenti
                 */
                bool operator>(const const_iterator &other) const {
                    if(_cbuffer != other._cbuffer) //puntano a cbuffer diversi
                        return (_ptr > other._ptr); 

                    if(_cbuffer == nullptr)
                        return false;

                    if(_cbuffer->_head < _cbuffer->_tail) //coda lineare
                        return (_ptr > other._ptr);

                    return _ptr < other._ptr;//coda circolare
                }
                

                /**
                 * @brief Operatore di confronto
                 * 
                 * @param other const_iterator
                 * @return true se l'iteratore corrente è >= dell'iteratore other
                 * @return false altrimenti
                 */
                bool operator>=(const const_iterator &other) const {
                    if(_cbuffer != other._cbuffer) //puntano a cbuffer diversi
                        return (_ptr >= other._ptr); // return false ?

                    if(_cbuffer == nullptr)
                        return false;

                    if(_cbuffer->_head <= _cbuffer->_tail) //coda lineare
                        return (_ptr >= other._ptr);

                    return _ptr <= other._ptr;//coda circolare
                }

                /**
                 * @brief Operatore di confronto
                 * 
                 * @param other const_iterator
                 * @return true se l'iteratore corrente è < dell'iteratore other
                 * @return false altrimenti
                 */
                bool operator<(const const_iterator &other) const {
                    if(_cbuffer != other._cbuffer) //puntano a cbuffer diversi
                        return (_ptr < other._ptr); // return false ?

                    if(_cbuffer == nullptr)
                        return false;

                    if(_cbuffer->_head < _cbuffer->_tail) //coda lineare
                        return (_ptr < other._ptr);

                    return _ptr > other._ptr;//coda circolare
                }
                
                
                /**
                 * @brief Operatore di confronto
                 * 
                 * @param other const_iterator
                 * @return true se l'iteratore corrente è <= dell'iteratore other
                 * @return false altrimenti
                 */
                bool operator<=(const const_iterator &other) const {
                    if(_cbuffer != other._cbuffer) //puntano a cbuffer diversi
                        return (_ptr <= other._ptr); // return false ?

                    if(_cbuffer == nullptr)
                        return false;

                    if(_cbuffer->_head <= _cbuffer->_tail) //coda lineare
                        return (_ptr < other._ptr);

                    return _ptr >= other._ptr;//coda circolare
                }
                
                
                friend class iterator;///< friend della classe iterator

                /**
                 * @brief Operatore==
                 * 
                 * @param other iteratore con cui fare il confronto
                 * @return true se l'iteratore this e other puntano allo stesso dato
                 * @return false se l'iteratore this e other non puntano allo stesso dato
                 */
                bool operator==(const iterator &other) const {
                    if(_cbuffer){
                        pointer tail = &_cbuffer->_queue[_cbuffer->_tail];
                        pointer head = &_cbuffer->_queue[_cbuffer->_head];
                        if(_ptr == head && other._ptr == head && _distance > other._distance)
                            return true;
                    }
                    
                    return (_ptr == other._ptr);

                }

                /**
                 * @brief Operatore!=
                 * 
                 * @param other iteratore con cui fare il confronto
                 * @return true se l'iteratore this e other non puntano allo stesso dato
                 * @return false se l'iteratore this e other puntano allo stesso dato
                 */
                bool operator!=(const iterator &other) const {
                    if(_cbuffer){
                        pointer tail = &_cbuffer->_queue[_cbuffer->_tail];
                        pointer head = &_cbuffer->_queue[_cbuffer->_head];
                        if(_ptr == head && other._ptr == head && _distance <= other._distance)
                            return true;
                    }
                    return (_ptr != other._ptr);
                }

                /**
                 * @brief Operatore di confronto
                 * 
                 * @param other iterator
                 * @return true se l'iteratore corrente è > dell'iteratore other
                 * @return false altrimenti
                 */
                bool operator>(const iterator &other) const {
                    if(_cbuffer != other._cbuffer) 
                        return _ptr > other._ptr; 

                    if(_cbuffer == nullptr)
                        return false;

                    if(_cbuffer->_head < _cbuffer->_tail) 
                        return _ptr > other._ptr;

                    return _ptr < other._ptr;
                }
		

                /**
                 * @brief Operatore di confronto
                 * 
                 * @param other iterator
                 * @return true se l'iteratore corrente è >= dell'iteratore other
                 * @return false altrimenti
                 */
                bool operator>=(const iterator &other) const {
                    if(_cbuffer != other._cbuffer) 
                        return _ptr >= other._ptr; 

                    if(_cbuffer == nullptr)
                        return false;

                    if(_cbuffer->_head <= _cbuffer->_tail) 
                        return _ptr >= other._ptr;

                    return _ptr <= other._ptr;
                }

                /**
                 * @brief Operatore di confronto
                 * 
                 * @param other iterator
                 * @return true se l'iteratore corrente è < dell'iteratore other
                 * @return false altrimenti
                 */
                bool operator<(const iterator &other) const {
                    if(_cbuffer != other._cbuffer) 
                        return _ptr < other._ptr; 

                    if(_cbuffer == nullptr)
                        return false;

                    if(_cbuffer->_head < _cbuffer->_tail) 
                        return _ptr < other._ptr;

                    return _ptr > other._ptr;
                }
		
		
                /**
                 * @brief Operatore di confronto
                 * 
                 * @param other iterator
                 * @return true se l'iteratore corrente è <= dell'iteratore other
                 * @return false altrimenti
                 */
                bool operator<=(const iterator &other) const {
                    if(_cbuffer != other._cbuffer)
                        return _ptr <= other._ptr; 

                    if(_cbuffer == nullptr)
                        return false;

                    if(_cbuffer->_head <= _cbuffer->_tail)
                        return _ptr <= other._ptr;

                    return _ptr >= other._ptr;
                }


                /**
                 * @brief Assegnamento di un iterator ad un const_iterator
                 * 
                 * @param other oggetto di tipo iterator
                 * @return const_iterator& riferimento al const_iterator this
                 */
                const_iterator &operator=(const iterator &other) {
                    _ptr = other._ptr;
                    _cbuffer = other._cbuffer;
                    _index = other._index;
                    _distance = other._distance;
                    return *this;
                }

            private:
                friend class cbuffer;///< friend della classe cbuffer
                const cbuffer* _cbuffer;///< puntatore al cbuffer corrente
                const T *_ptr;///< puntatore al dato
                int _index;///< posizione nell'array del valore puntato da _ptr
                unsigned int _distance;///< distanza del puntatore dalla testa
               
                /**
                 * @brief Costruttore privato
                 * 
                 * @param q puntatore di tipo cbuffer
                 * @param ptr puntatore al valore
                 * @param index indice del puntatore ptr
                 */
                const_iterator(const T *ptr, const cbuffer* const q, int index, unsigned int distance)
                    : _cbuffer(q), _ptr(ptr), _index(index), _distance(distance){}

                /**
                 * @brief Funzione che sposta il puntatore di una posizione in avanti
                 * 
                 * @param ptr puntatore da spostare
                 * @return const T* const puntatore spostato
                 */
                const T* const next(const T* ptr){
                   if(ptr == &_cbuffer->_queue[_cbuffer->_tail]){
                        if(_distance == _cbuffer->_size - 1)
                            _distance = _index;

                        ptr = &_cbuffer->_queue[_cbuffer->_head];
                        _index = _cbuffer->_head;
                        _distance = (_distance + 1) % (_cbuffer->_size + 1);
                        
                    }else{
                        ptr = &_cbuffer->_queue[(_index + 1) % _cbuffer->_size];
                        _index = (_index + 1) % _cbuffer->_size;
                       _distance = (_distance + 1) % _cbuffer->_size;
                        
                    }
                    return ptr;
                }
                /**
                 * @brief Funzione che sposta il puntatore di una posizione in dietro
                 * 
                 * @param ptr puntatore da spostare
                 * @return const T* const puntatore spostato
                 */
                const T* const previous(const T* ptr){
                    int prev  = (_index - 1) % _cbuffer->_stored_elements; 
                    int q = static_cast<int>(floor(static_cast<double>(prev) / _cbuffer->_stored_elements));
                    prev = prev - _cbuffer->_stored_elements * q;
                    if(ptr == &_cbuffer->_queue[_cbuffer->_head]){
                        ptr = &_cbuffer->_queue[_cbuffer->_tail];
                        _index = _cbuffer->_tail;
                        _distance = _cbuffer->_tail;
                        
                    }else{
                        ptr = &_cbuffer->_queue[(_index - 1) % _cbuffer->_size];
                        _index = (_index - 1) % _cbuffer->_size;
                        _distance --;  
                    }
                }
                      
            
        }; // classe const_iterator
         
        
        /**
         * @brief Iteratore di inizio
         * 
         * @return iterator 
         */
        iterator begin() { 
            if(_head > -1)
                return iterator(_queue + _head, this, _head, _head);
            else
                return iterator(nullptr, this, _head, _head);
        }
        
        /**
         * @brief Iteratore fine
         * 
         * @return iterator
         */
        iterator end() {
            if(is_empty())
                return iterator(nullptr, this, _tail, _tail);
            else
                return iterator(_queue + _head , this, _head, _tail);
           
        }

        /**
         * @brief Iteratore di inzio
         * 
         * @return const_iterator
         */
        const_iterator begin() const {
            if(_head > -1)
                return const_iterator(_queue + _head, this, _head, _head);
            else
                return const_iterator(nullptr, this, _head, _head);
        }
        
        /**
         * @brief Iteratore fine
         * 
         * @return const_iterator
         */
        const_iterator end() const{
            if(is_empty())
                return const_iterator(nullptr, this, _tail, _tail);
            else
                return const_iterator(_queue + _head , this, _head, _tail);
        }
};

#endif