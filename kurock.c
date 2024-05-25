#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include "seqlock.h"
#include "rwwlock.h"
#include "rwrlock.h"



#define STRING_LEN  64

typedef enum locks{
    rwr,
    rww,
    seq,
}lock_t;

unsigned long glob_counter = 0;

long long w_counter = 0;
long long r_counter = 0;
long long glob_string[STRING_LEN] = {0,};

int is_done = 0;

void* ku_lock;

struct thread_args{
    int locktype;
    int whz;
    int rhz;
    int duration;
    FILE* fp;
};

struct reader_view {
    int counter;
    char string[STRING_LEN+1];
};

struct linked_list{
    struct reader_view value;
    struct linked_list* next;
};

struct results{
    unsigned long rcnt;
    unsigned long wcnt;
};

struct linked_list* insert(struct linked_list* node, struct reader_view value){
    struct linked_list* new = (struct linked_list*)malloc(sizeof(struct linked_list));
    node->next = new;
    new->value = value;
    new->next = NULL;
    return new;
}

void traverse(FILE* fp, struct linked_list* node){

    struct linked_list* current = node->next;
    struct linked_list* tmp;

    while(current != NULL){
        fprintf(fp,"%d %s\n",current->value.counter, current->value.string);
        tmp = current;
        current = current->next;
        free(tmp);
    }
}
    

static inline int
timespec_cmp (struct timespec a, struct timespec b){
  return (a.tv_sec < b.tv_sec ? -1
	  : a.tv_sec > b.tv_sec ? 1
	  : a.tv_nsec - b.tv_nsec);
}


void writer_ops(){
    char alph = glob_counter % 26 + 'a';
    glob_counter++;
    for(int i = 0; i < STRING_LEN; i++)
        glob_string[i] =  alph;
}
void w_rww_lock_routine(){
    rwwlock_acquire_writelock((rwwlock_t*)ku_lock);
    writer_ops();
    rwwlock_release_writelock((rwwlock_t*)ku_lock);
}
void w_rwr_lock_routine(){
    rwrlock_acquire_writelock((rwrlock_t*)ku_lock);
    writer_ops();
    rwrlock_release_writelock((rwrlock_t*)ku_lock);
}
void w_seq_lock_routine(){
    seqlock_write_lock((seqlock_t*)ku_lock);
    writer_ops();
    seqlock_write_unlock((seqlock_t*)ku_lock);
}

void* writer(void* args){

    struct thread_args *t_args = args;

    struct timespec current;
    struct timespec deadline;

    useconds_t period = 1000000/t_args->whz;
    unsigned long cnt = 0;

    clock_gettime(CLOCK_MONOTONIC, &current);

    deadline.tv_sec = t_args->duration + current.tv_sec;
    deadline.tv_nsec = current.tv_nsec;

    do
    {
        /* critical section in */
        if(t_args->locktype == rwr)
            w_rwr_lock_routine();
        else if(t_args->locktype == rww)
            w_rww_lock_routine();
        else if(t_args->locktype == seq)
            w_seq_lock_routine();

        clock_gettime(CLOCK_MONOTONIC, &current);
        cnt++;
        usleep(period);
    } while (timespec_cmp(deadline, current) > 0);
    
    
    return (void*) cnt;
}

struct reader_view reader_ops(){

    struct reader_view view;
    view.counter = glob_counter;
    for(int i = 0; i < STRING_LEN; i++)
        view.string[i] = glob_string[i];
    view.string[STRING_LEN] = '\0';

    return view;
}

struct linked_list* r_seq_lock_routine(struct linked_list* list){

    struct reader_view view;
    struct linked_list* current;
    unsigned cnt;
    do{
        cnt = seqlock_read_begin((seqlock_t *)ku_lock);
        view = reader_ops();
    }while(seqlock_read_retry((seqlock_t*)ku_lock, cnt));

    current = insert(list,view);
    return current;
}

struct linked_list* r_rww_lock_routine(struct linked_list* list){

    struct reader_view view;
    struct linked_list* current;

    rwwlock_acquire_readlock((rwwlock_t*)ku_lock);
    view = reader_ops();
    rwwlock_release_readlock((rwwlock_t*)ku_lock);

    current = insert(list,view); 
    return current;
}
struct linked_list* r_rwr_lock_routine(struct linked_list* list){

    struct reader_view view;
    struct linked_list* current;

    rwrlock_acquire_readlock((rwrlock_t*)ku_lock);
    view = reader_ops();
    rwrlock_release_readlock((rwrlock_t*)ku_lock);

    current = insert(list,view); 
    return current;
}

void* reader(void* args){

    struct thread_args* r_args = args;

    struct timespec current;
    struct timespec deadline;

    struct linked_list* list = (struct linked_list*)malloc(sizeof(struct linked_list));
    struct linked_list* current_list = list;

    clock_gettime(CLOCK_MONOTONIC, &current);

    useconds_t period = 1000000/r_args->rhz;
    unsigned long cnt = 0;

    deadline.tv_sec = r_args->duration + current.tv_sec;
    deadline.tv_nsec = current.tv_nsec;

    do
    {
        if (r_args->locktype == seq){
            current_list = r_seq_lock_routine(current_list);
        } 
        else if(r_args->locktype == rwr){
            current_list = r_rwr_lock_routine(current_list);
        } 
        else if(r_args->locktype == rww){
            current_list = r_rww_lock_routine(current_list);
        }

        clock_gettime(CLOCK_MONOTONIC, &current);
        cnt++;
        usleep(period);

    } while (timespec_cmp(deadline, current) > 0);

    traverse(r_args->fp, list);

    return (void*)cnt;
}

void init_lock(lock_t lock){
    rwrlock_t *rwrlock;
    rwwlock_t *rwwlock;
    seqlock_t *seqlock;

    switch(lock){
        case rwr:
            rwrlock = (rwrlock_t*)malloc(sizeof(rwrlock_t));
            ku_lock = rwrlock;
            rwrlock_init(ku_lock);
            break;
        case rww:
            rwwlock = (rwwlock_t*)malloc(sizeof(rwwlock_t));
            ku_lock = rwwlock;
            rwwlock_init(ku_lock);
            break;
        case seq:
            seqlock = (seqlock_t*)malloc(sizeof(seqlock_t));
            ku_lock = seqlock;
            seqlock_init(ku_lock);
            break;
    }
}

struct results works(lock_t lock, int readers, int writers, int whz, int rhz, int duration){

    int status;

    struct results res;
    res.rcnt = 0;
    res.wcnt = 0;


    pthread_t *r_thrs = (pthread_t*)malloc(sizeof(pthread_t) * readers);
    pthread_t *w_thrs = (pthread_t*)malloc(sizeof(pthread_t) * writers);

    FILE* fp = fopen("reader_log.txt", "w");
    if(fp == NULL){
        printf("file open error\n");
        exit(1);
    }

    struct thread_args args = {
        .locktype = lock,
        .whz = whz,
        .rhz = rhz,
        .duration = duration,
        .fp = fp,
    };


    init_lock(lock);

    for(int i = 0; i < writers; i ++)
        pthread_create(&w_thrs[i], NULL, writer, (void*)&args);

    for(int i = 0; i < readers; i ++)
        pthread_create(&r_thrs[i], NULL, reader, (void*)&args);


    for(int i = 0; i < writers; i++){
        pthread_join(w_thrs[i], (void**)&status);
        res.wcnt+=status;
    }
    for(int i = 0; i < readers; i++){
        pthread_join(r_thrs[i], (void**)&status);
        res.rcnt+=status;
    }


    free(w_thrs);
    free(r_thrs);

    fclose(fp);

    return res;

}

void stats(int duration, struct results res){
    
    double w_th = (double) res.wcnt / duration;
    double r_th = (double) res.rcnt / duration;

    printf("Local writer counter total sum : %ld\n", res.wcnt);
    printf("Local reader counter total sum : %ld\n", res.rcnt);

    printf("\n");
    printf("Global counter : %ld\n", glob_counter);
    printf("\n");

    printf("Wrtier throughput: %f ops/sec\n", w_th);
    printf("Reader throughput: %f ops/sec\n", r_th);

}

void usage(){

    printf("kurock <lock> <readers> <rhz> <writers> <whz> <s>\n");
    printf("<lock>: rwr, rww, or seq \n");
    printf("<readers>: Number of read threads\n");
    printf("<rhz>: Read frequency (1~100000)\n");
    printf("<writers>: Number of writer threads\n");
    printf("<whz>: Write frequency (1~100000)\n");
    printf("<s>: Total execution time (seconds)\n");
}

int main(int argc, char* argv[]){

    lock_t locktype;
    int readers, writers;
    int rhz, whz;
    int duration;
    struct results res;

    if(argc != 7){
        usage();
        printf("invaild arg numbs!\n");
        exit(1);
    }

    if(strcmp("rwr", argv[1]) == 0)
        locktype = rwr;
    
    else if(strcmp("rww", argv[1]) == 0)
        locktype = rww;

    else if(strcmp("seq", argv[1]) == 0)
        locktype = seq;

    else{
        usage();
        printf("lock type invalid!\n");
        exit(1);
    }

    readers = atoi(argv[2]);
    if(readers < 1){
        usage();
        printf("reader count invalid!\n");
        exit(1);
    }

    rhz = atoi(argv[3]);
    if(rhz < 1 ||  rhz > 100000){
        usage();
        printf("rhz invalid!\n");
    }

    writers = atoi(argv[4]);
    if(writers < 1){
        usage();
        printf("writer count invalid!\n");
        exit(1);
    }

    whz = atoi(argv[5]);
    if(whz < 1 ||  whz > 100000){
        usage();
        printf("whz invalid!\n");
    }

    duration = atoi(argv[6]);

    res = works(locktype, readers, writers, whz, rhz, duration);

    stats(duration,res);

    return 0;

}