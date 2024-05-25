typedef struct _rwrlock_t {
} rwrlock_t;
void rwrlock_init(rwrlock_t *rwr);
void rwrlock_acquire_writelock(rwrlock_t *rwr);
void rwrlock_release_writelock(rwrlock_t *rwr);
void rwrlock_acquire_readlock(rwrlock_t *rwr);
void rwrlock_release_readlock(rwrlock_t *rwr);

void rwrlock_init(rwrlock_t *rwr){
}
void rwrlock_acquire_writelock(rwrlock_t *rwr){
}
void rwrlock_release_writelock(rwrlock_t *rwr){
}
void rwrlock_acquire_readlock(rwrlock_t *rwr){
}
void rwrlock_release_readlock(rwrlock_t *rwr){
}