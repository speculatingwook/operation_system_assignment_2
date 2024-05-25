typedef struct _seqlock_t {
} seqlock_t;
void seqlock_init(seqlock_t *seq);
void seqlock_write_lock(seqlock_t *seq);
void seqlock_write_unlock(seqlock_t *seq);
unsigned seqlock_read_begin(seqlock_t *seq);
unsigned seqlock_read_retry(seqlock_t *seq, unsigned cnt);

void seqlock_init(seqlock_t *seq){

}
void seqlock_write_lock(seqlock_t *seq){

}
void seqlock_write_unlock(seqlock_t *seq){

}
unsigned seqlock_read_begin(seqlock_t *seq){

}
unsigned seqlock_read_retry(seqlock_t *seq, unsigned cnt){

}