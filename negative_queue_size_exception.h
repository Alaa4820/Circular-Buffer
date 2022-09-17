#ifndef NEGATIVE_QUEUE_SIZE_EXCEPTION_H
#define NEGATIVE_QUEUE_SIZE_EXCEPTION_H
#include <stdexcept>
/**
 * @brief Classe Eccezione
 * 
 * La classe implementa un'eccezione a run time in
 * caso di dimensione negativa 
 * 
 */
class negative_queue_size_exception : public std::runtime_error {
	
	public:
		/**
		 * @brief Costruttore 
		 * 
		 * @param message stringa contenente il messaggio
		 */
		negative_queue_size_exception(const std::string &message);

};

#endif