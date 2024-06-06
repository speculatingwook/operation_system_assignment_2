#include <semaphore.h>
typedef struct _seqlock_t {
    sem_t lock;
    unsigned counter;
} seqlock_t;

void seqlock_init(seqlock_t *seq);
void seqlock_write_lock(seqlock_t *seq);
void seqlock_write_unlock(seqlock_t *seq);
unsigned seqlock_read_begin(seqlock_t *seq);
unsigned seqlock_read_retry(seqlock_t *seq, unsigned cnt);

void seqlock_init(seqlock_t *seq){
    sem_init(&seq->lock, 0, 1);
    seq->counter =0;
}

void seqlock_write_lock(seqlock_t *seq){
    sem_wait(&seq->lock);
    seq->counter++;
}

void seqlock_write_unlock(seqlock_t *seq){
    seq->counter++;
    sem_post(&seq->lock);
}

unsigned seqlock_read_begin(seqlock_t *seq){
    return seq->counter;
}

unsigned seqlock_read_retry(seqlock_t *seq, unsigned cnt){
    return seq->counter % 2 == 1 || cnt != seq->counter;
}
