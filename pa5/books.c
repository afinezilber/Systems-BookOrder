#include "books.h"
#include "queue.h"
#include "node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


order *order_create(char *bookTitle, float price, int id, char *category)
 {
    order *Order = (order *) malloc(sizeof(order));
    if (Order) 
    {
        Order->id = id;
        Order->price = price;
        Order->bookTitle = (char *) malloc(strlen(bookTitle) + 1);
        Order->category = (char *) malloc(strlen(category) + 1);
        strcpy(Order->bookTitle, bookTitle);
        strcpy(Order->category, category);
    }
    return Order;
}


void order_destroy(order *Order) 
{
    if (Order) {
        free(Order->bookTitle);
        free(Order->category);
        free(Order);
    }
}


receipt *receipt_create(char *title, float price, float credit) 
{
    receipt *Receipt = (receipt *) malloc(sizeof(receipt));
    if (Receipt) 
    {
        Receipt->price = price;
        Receipt->creditLeft = credit;
        Receipt->bookTitle = (char *) malloc(strlen(title) + 1);
        strcpy(Receipt->bookTitle, title);
    }
    return Receipt;
}


void receipt_destroy(void *data)
 {
    receipt *Receipt;
    if (data) 
    {
        Receipt = (receipt *) data;
        free(Receipt->bookTitle);
        free(Receipt);
    }
}


customer *customer_create(char *name, int id, float credit)
 {
    customer *Customer = (customer *) malloc(sizeof(customer));
    if (Customer) 
    {
        Customer->id = id;
        Customer->credLimit = credit;
        Customer->name = (char *) malloc(strlen(name) + 1);
        strcpy(Customer->name, name);
        Customer->success = create_queue();
        Customer->fail = create_queue();
    }
    return Customer;
}

void customer_destroy(customer *Customer) 
{
    if (Customer) 
    {
        free(Customer->name);
        destroy_queue(Customer->success, &receipt_destroy);
        destroy_queue(Customer->fail, &receipt_destroy);
        free(Customer);
    }
}


database *dbCreate(void) {
    return (database *) malloc(sizeof(database));
}


void dbDestroy(database *Database)
 {
    int i;
    if (Database) {
        for (i = 0; i < MAXBUFFER; i++) 
        {
            customer_destroy(Database->Customer[i]);
        }
        free(Database);
    }
}


void dbAddCustomer(database *Database, customer *Customer) 
{
    Database->Customer[Customer->id] = Customer;
}


customer *dbRetrieveCustomer(database *Database, int id)
 {
    if (id >= 0 || id < MAXBUFFER) 
    {
        return Database->Customer[id];
    }
    else {
        fprintf(stderr, "The ID %d is not in the database.\n", id);
        return NULL;
    }
}
