#ifndef BOOKS_H
#define BOOKS_H
#define MAXBUFFER 1024
#define CUSTOMERLIMIT 512

#include "queue.h"

/**
 * A structure holding the information for book orders.
 */
 struct orderT {
    char *bookTitle;
    float price;
    int id;
    char *category;
}; 
 typedef struct orderT order;

/**
 * Creates a new book order structure.
 */
order *order_create(char *, float, int, char *);

/**
 * Destroys a book order structure, freeing all associated memory.
 */
void order_destroy(order *);

/**
 * A structure that carries data for the customer after every purchace.
 * successful orders use all three structure fields; unsuccessful book orders
 * only use the first two.
 */
 struct receiptT {
    char *bookTitle;
    float price;
    float creditLeft;
};
 typedef struct receiptT receipt;

/**
 * Creates a new order receipt.
 */
receipt *receipt_create(char *, float, float);

/**
 * Destroys a receipt structure.
 */
void receipt_destroy(void *);

/**
 * Structure holding all customer information.
 */
 struct customerT {
    char *name;
    int id;
    float credLimit;
    queue *success;
    queue *fail;
};
 typedef struct customerT customer;

/**
 * Creates a new customer for the database.
 */
customer *customer_create(char *, int, float);

/**
 * Destroys the customer, freeing all data.
 */
void customer_destroy(customer *);

/**
 * A hash map containing all customers and their data.
 */
 struct databaseT {
    customer *Customer[MAXBUFFER];
}; 
typedef struct databaseT database;

/**
 * Creates a new empty database.
 */
database *dbCreate(void);

/**
 * Destroys the given database, freeing all associated memory.
 */
void dbDestroy(database *);

/**
 * Adds a new customer to the database.
 */
void dbAddCustomer(database *, customer *);

/**
 * Retrieves a customer from the database.
 */
customer *dbRetrieveCustomer(database *, int);

#endif
