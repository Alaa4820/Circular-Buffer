# Circular-Buffer
Senza utilizzare strutture dati a lista, il progetto richiede la realizzazione di una
classe cbuffer generica che implementa un buffer circolare di elementi di tipo
T (vedi figura seguente).

![image](https://user-images.githubusercontent.com/101665044/190851377-97295176-66f1-4a23-b51f-42cd641f03c1.png)

Il buffer ha una capacità fissa, decisa a costruzione. L'inserimento aggiunge gli
elementi nel primo posto libero finché il buffer non raggiunge la capacità
massima. Una volta riempito, i nuovi dati vengono scritti partendo dall'inizio
del buffer e sovrascrivendo i vecchi.
La classe, oltre ai metodi di uso comune (metodi fondamentali, capacità,
numero elementi inseriti, etc), deve permettere:
- la costruzione di un cbuffer anche a partire da una sequenza di dati
generici Q identificata da una coppia di iteratori generici. Questo
costruttore prende in input: la dimensione del buffer, l’iteratore di inizio
sequenza, l’iteratore di fine sequenza. Lasciate al compilatore la gestione
della conversione di dati tra Q e T.
- l’inserimento di un nuovo elemento in coda al buffer
- la cancellazione dell’elemento attualmente in testa al buffer
- l’accesso in lettura e scrittura dell'elemento i-esimo tramite operatore []
- il supporto agli iteratori in lettura e scrittura
