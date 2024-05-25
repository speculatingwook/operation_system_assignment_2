typedef struct _rwwlock_t {
} rwwlock_t;
void rwwlock_init(rwwlock_t *rww);
void rwwlock_acquire_writelock(rwwlock_t *rww);
void rwwlock_release_writelock(rwwlock_t *rww);
void rwwlock_acquire_readlock(rwwlock_t *rww);
void rwwlock_release_readlock(rwwlock_t *rww);

void rwwlock_init(rwwlock_t *rww){
}
void rwwlock_acquire_writelock(rwwlock_t *rww){
}
void rwwlock_release_writelock(rwwlock_t *rww){
}
void rwwlock_acquire_readlock(rwwlock_t *rww){
}
void rwwlock_release_readlock(rwwlock_t *rww){
}
