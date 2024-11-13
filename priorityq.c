#include <stdio.h>
#include <stdlib.h>
#include "main.h"



typedef struct node Node;

struct node
{
   Event * data;
   int priority;

   struct node* next;

};





Node* newNode(Event *d, int p)
{
   Node* temp = (Node*)malloc(sizeof(Node));
   temp->data = d;
   temp->priority = p;
   temp->next = NULL;

   return temp;
};





Event * top(Node** head)
{
   return (*head)->data;
};




void pop(Node** head)
{
   Node* temp = *head;
   (*head) = (*head)->next;
   free(temp);
}



void push(Node** head, Event *d)
{
  if (*head == NULL)
   {
       *head = newNode(d, 0);
       return;
   }else{
   Node* start = (*head);
   int p = d->p->y;
  
   Node* temp = newNode(d, p);
   if ((*head)->priority > p)

   {
       temp->next = *head;
       (*head) = temp;
   }
   else
   {
       while (start->next != NULL &&
           start->next->priority < p)
       {
           start = start->next;
       }
       temp->next = start->next;
       start->next = temp;
   }
}}
int isEmpty(Node** head)
{
   return (*head) == NULL;
}
