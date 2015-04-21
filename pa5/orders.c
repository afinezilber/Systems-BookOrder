#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include "books.h"


database *consumer;

queue *qBooks;

int finished;

char *topic;

int is_file(char *filepath) 
{
    FILE *file = fopen(filepath, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    else {
        return 0;
    }
}

void print_usage() 
{
    printf("./order <datab base file> <orders file> <category file> \n");
}


void *producer_thread(void *args) 
{
    FILE *file;
    char *bookTopic, *ptr, *book;
    const char *delim = "|\r\n";
    float cost;
    int id;
    order *bookOrder;
    size_t l;
    ssize_t r;


    l = 0;
    ptr = NULL;
    file = fopen((char *) args, "r");
    if (file == NULL) 
    {
        fprintf(stderr, "Error Openning File.\n");
        exit(EXIT_FAILURE);
    }


    while ((r = getline(&ptr, &l, file)) != -1) 
    {

        book = strtok(ptr, delim);
        cost = atof(strtok(NULL, delim));
        id = atoi(strtok(NULL, delim));
        bookTopic = strtok(NULL, delim);

        if (strstr(topic, bookTopic) == NULL) 
        {
            fprintf(stderr, "%s is not a valid category. Skipping.\n",
                    bookTopic);
            continue;
        }

        
        bookOrder = order_create(book, cost, id, bookTopic);
        pthread_mutex_lock(&qBooks->mut);
        enqueue(qBooks, (void *) bookOrder);
        pthread_mutex_unlock(&qBooks->mut);

        
        pthread_cond_signal(&qBooks->open);
    }

    
    finished= 1;
    pthread_cond_broadcast(&qBooks->open);
    free(ptr);
    fclose(file);
    return NULL;
}



void *consumer_thread(void *args) 
{
    char *bookType, *input;
    customer *buyer;
    order *bookOrder;
    receipt *bookReceipt;


    input = (char *) args;
    bookType = (char *) malloc(strlen(input) + 1);
    strcpy(bookType, input);

    while (!finished || !isempty(qBooks))
     {
        pthread_mutex_lock(&qBooks->mut);

        if (!finished && isempty(qBooks)) 
        {
            pthread_cond_wait(&qBooks->open, &qBooks->mut);
        }

        if (finished && isempty(qBooks))
         {
            
            pthread_mutex_unlock(&qBooks->mut);
            return NULL;
        }
        else if (isempty(qBooks))
         {
            
            pthread_mutex_unlock(&qBooks->mut);
            sched_yield();
            continue;
        }

        bookOrder = (order *) peek(qBooks);
        if (strcmp(bookOrder->category, bookType) != 0) 
        {
            pthread_mutex_unlock(&qBooks->mut);
            sched_yield();
        }
        else 
        {
         
            bookOrder = (order *) dequeue(qBooks);
            buyer = dbRetrieveCustomer(consumer, bookOrder->id);
            if (!buyer) 
            {
               
                fprintf(stderr, "ID %d Not Found in Database.\n", bookOrder->id);
            }
            else {
                bookReceipt = receipt_create(bookOrder->bookTitle,
                                         bookOrder->price,
                                         buyer->credLimit - bookOrder->price);
                if (buyer->credLimit < bookOrder->price) {
                   
                    printf("%s does not have sufficient funds for a purchase.\n"
                           "\tBook: %s\n\tRemaining credit: $%.2f\n\n",
                           buyer->name,
                           bookOrder->bookTitle,
                           buyer->credLimit);
                    enqueue(buyer->fail, bookReceipt);
                }
                else
                {
                 
                    buyer->credLimit -= bookOrder->price;
                    printf("Customer: %s Successfully Purchased!\n"
                           "\tBook: %s\n\tPrice: $%.2f\n"
                           "\tRemaining credit: $%.2f\n\n",
                           buyer->name,
                           bookOrder->bookTitle,
                           bookOrder->price,
                           buyer->credLimit);
                    enqueue(buyer->success, bookReceipt);
                }
            }
            order_destroy(bookOrder);
            pthread_mutex_unlock(&qBooks->mut);
        }
    }

    free(bookType);
    return NULL;
}





/**
 * Sets up the customer database
 */
database *intialize(char *filepath) 
{
    FILE *db;
    int id;
    float limit; 
    char *data, *ptr, *cust;
    ssize_t r;
    size_t l;

    database *custoDb = dbCreate();
    customer *newClient = NULL;


    
    
    if (is_file(filepath) == 0) {
        fprintf(stderr, "Error: %s Invalid Filepath. Please Try Again.\n", filepath);
        exit(EXIT_FAILURE);
    }

    db = fopen(filepath, "r");


    ptr = NULL;
    l = 0;

    while ((r = getline(&ptr, &l, db)) != -1) {
        newClient = NULL;
        if((data = strtok(ptr, "|")) != NULL) {
            cust = (char *) malloc(strlen(data) + 1);
            strcpy(cust, data);
        }
        if ((data = strtok(NULL, "|")) != NULL) {
            id = atoi(data);
        }
        if ((data = strtok(NULL, "|")) != NULL) {
            limit = atof(data);
        }
        newClient = customer_create(cust, id, limit);
        dbAddCustomer(custoDb, newClient);

    }
    free(ptr);
    fclose(db);
    return custoDb;
}



int main(int argc, char **argv)
 {
	int i, numTopics;
	float total;
	void *ignore;
    char *category, **allTopics;
    receipt *purchases;
    customer *buyer;

	//Assures right amount of arguments
    if (argc != 4) {
        fprintf(stderr, "Error: Invalid Number of Arguments. Please Try Again.\n");
        print_usage();
        exit(EXIT_FAILURE);
    }

    //Calculates number of categories
    topic = malloc(strlen(argv[3]) + 1);
    strcpy(topic, argv[3]);
    allTopics = (char **) calloc(1024, sizeof(char *));
    numTopics = 0;
    category = strtok(argv[3], " ");
    if (category == NULL) {
        fprintf(stderr, "Error - Minimum One Category Needed.\n");
        exit(EXIT_FAILURE);
    }
    allTopics[0] = malloc(strlen(category) + 1);
    strcpy(allTopics[0], category);
    numTopics++;

    while ((category = strtok(NULL, " ")) != NULL) {
        allTopics[numTopics] = malloc(strlen(category) + 1);
        strcpy(allTopics[numTopics], category);
        numTopics++;
    }

 
    pthread_t tid[numTopics + 1];
    consumer = intialize(argv[1]);
    qBooks = create_queue();

    pthread_create(&tid[0], NULL, producer_thread, (void *) argv[2]);

    for (i = 0; i < numTopics; i++) {
        pthread_create(&tid[i + 1], NULL, consumer_thread, allTopics[i]);
    }

    for (i = 0; i < numTopics + 1; i++) {
        pthread_join(tid[i], &ignore);
    }

    printf("\n\n");
    total = 0.0f;
    for (i = 0; i < CUSTOMERLIMIT; i++) 
    {
        buyer = consumer->Customer[i];
        if (buyer == NULL) 
        {
            continue;
        }

        // Printing of receits
        printf("====================\nStart Customer Info\n====================\n");
        printf("--------------\n");
        printf("~~ Balance ~~\n");
        printf("Customer Name: %s\n", buyer->name);
        printf("Customer ID: %d\n", buyer->id);
        printf("Credit Remaining: %.2f\n", buyer->credLimit);

  
        printf("\nSuccessful Orders\n");
        printf("-----------------\n");
        if (isempty(buyer->success)) 
        {
                printf("\tNone.\n");
        }
        else
        {
            while ((purchases = (receipt *) dequeue(buyer->success)) != NULL) 
            {
                printf("%s|%.2f|%.2f\n", purchases->bookTitle, purchases->price,
                                           purchases->creditLeft);
                total += purchases->price;
                receipt_destroy(purchases);
            }
        }

        printf("\nFailed Orders\n");
        printf("-------------\n");
        if (isempty(buyer->fail)) 
        {
            printf("None.\n");
        }
        else 
        {
            while ((purchases = (receipt *) dequeue(buyer->fail)) != NULL) 
            {
                printf("%s|%.2f\n", purchases->bookTitle, purchases->price);
                receipt_destroy(purchases);
            }
        }
        
        printf("====================\nEnd of Customer Info\n====================\n\n");
    }

    printf("Total Revenue: $%.2f\n", total);

    dbDestroy(consumer);
    destroy_queue(qBooks, NULL);
    return EXIT_SUCCESS;
}
