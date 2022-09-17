#ifndef EMPTY_QUEUE_EXCEPTION_H
#define EMPTY_QUEUE_EXCEPTION_H
#include <stdexcept>
/**
 * @brief Classe Eccezione
 * 
 * La classe implementa un'eccezione a run time in
 * caso di operazioni non consentite su una coda vuota
 * 
 */
class empty_queue_exception : public std::runtime_error {
	
	public:
		/**
		 * @brief Costruttore 
		 * 
		 * @param message stringa contenente il messaggio
		 */
		empty_queue_exception(const std::string &message);

};

#endif