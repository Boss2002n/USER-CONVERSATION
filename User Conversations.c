#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "log.h"

int logindex = 0;
int* logi = &logindex;

pthread_mutex_t tlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t dlock = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t person_counter;



void* conversation(void*); // this is signature for the 
                           // threads starting routine

typedef struct person{
  pthread_t tid;
  int counter;
  int index;
  int completed;
  char name[12];
}Person;

Person person[8];
int completed = 0;




void* conversation(void* arg){

void* ret = NULL;
char line[100];
Person *p;

pthread_mutex_lock(&dlock);
p = (Person*) arg;

if(p->counter < 1){
    completed++;
}

msg("[pid=%u,tid=%u,index=%u,name=%s] Enter Message:", getpid,pthread_self(), p -> index, p -> name);
fgets(line,100,stdin);
pthread_mutex_unlock(&dlock);
if(strncmp(line, "quit",4) == 0){

  pthread_mutex_lock(&dlock);
  completed-= 1;
  pthread_mutex_unlock(&dlock);
  p -> completed = 1;
  Msg("[pid=%u,tid=%u,index=%u,name=%s] Enter Message:\n user quit",getpid,pthread_self(), p -> index, p->name, line);
  pthread_exit(&ret);
  }

else {

  p -> counter++;
  Msg("[pid=%u,tid=%u,index=%u,name=%s] Enter Message:\n %s",getpid,pthread_self(), p -> index, p -> name,line);
  pthread_exit(&ret);
  }

}

int main(int argc, char argv[]) {

  int* ret;
  if (_level > 0 || _level < 9) {

     msg("There are %d personalities", _level);
     int i;
     for (i = 0; i < _level; i++) {
        msg("Person%d", i);
     }
     
     create_log("assgn2.log");

     int j;
     for (j = 0; j < _level; j++) {
       person[j].index = j;
       char name[10] = "Person";
       person[j].tid = -1;
       snprintf(person[j].name, 8, "Person%d",j);
       person[j].completed = -1;
       person[j].counter = 0;
     }

     do {
       int i;
       for(i = 0; i < _level; i++) {
         if(person[i].completed != 1) {
           if (pthread_create(&(person[i].tid), NULL, conversation,&person[i])) {
             msg("Error1");
             exit(1);
           }
         }
       }

       int j;
       for(j = 0; j < _level; j++) {
         if(person[j].completed != 1) {
           if(pthread_join(person[j].tid, NULL)) {
             msg ("Error2");
             exit(1);
           }
         }
       }

     } while (completed != 0);

     int k;
     for (k = 0; k < _level; k++) {
       msg("Person %s processed %d lines",person[k].name,person[k].counter);
     }

  }
  else msg_exit("permitted levels are 1 .. 8");

  return 0;
}




