# Operating Systems Assignment #2: KURock

## Overview

This assignment focuses on implementing various reader-writer lock mechanisms and analyzing their performance and scalability.

## Reader-Writer Locks

### Read-preferring RW locks

- Allow for maximum concurrency but can lead to write starvation if contention is high
- What we have already learned in the class

### Write-preferring RW locks

- Avoid the problem of writer starvation
- Prevent any new readers from acquiring the lock if there is a writer queued and waiting for the lock

## Sequence Lock (seqlock)

- A reader-writer consistent mechanism that avoids the problem of writer starvation
- Consists of a counter and a lock
  - The counter is for indicating consistency in readers
  - The lock is to support synchronization between writers
- Writer
  - Increments the counter, both after acquiring the lock and before releasing the lock
- Reader
  - Reads the counter before and after the critical section
  - Simply retry if the counters are different or if the counter is odd

## Interfaces

### Read-preferring RW locks

```c
typedef struct _rwrlock_t {
    ...
} rwrlock_t;

void rwrlock_init(rwrlock_t *rwr);
void rwrlock_acquire_writelock(rwrlock_t *rwr);
void rwrlock_release_writelock(rwrlock_t *rwr);
void rwrlock_acquire_readlock(rwrlock_t *rwr);
void rwrlock_release_readlock(rwrlock_t *rwr);
```

### Write-preferring RW locks

```c
typedef struct _rwwlock_t {
    ...
} rwwlock_t;

void rwwlock_init(rwwlock_t *rww);
void rwwlock_acquire_writelock(rwwlock_t *rww);
void rwwlock_release_writelock(rwwlock_t *rww);
void rwwlock_acquire_readlock(rwwlock_t *rww);
void rwwlock_release_readlock(rwwlock_t *rww);
```

### Sequence Lock

```c
typedef struct _seqlock_t {
    ...
} seqlock_t;

void seqlock_init(seqlock_t *seq);
void seqlock_write_lock(seqlock_t *seq);
void seqlock_write_unlock(seqlock_t *seq);
unsigned seqlock_read_begin(seqlock_t *seq);
unsigned seqlock_read_retry(seqlock_t *seq, unsigned cnt);
```

## Use Case

- Write path:

```c
void seqlock_write_lock(&seq);
/* write-side critical section */
void seqlock_write_unlock(&seq);
```

- Read path:

```c
do {
    cnt = seqlock_read_begin(&seq);
    /* read-side critical section */
} while (seqlock_read_retry(&seq, cnt));
```

## Provided File

- `kurock.c`

```bash
> kurock <lock> <readers> <rhz> <writers> <whz> <s>
```

- `<lock>`: rwr, rww, or seq
- `<readers>`: Number of read threads
- `<rhz>`: Read frequency (1~100000)
- `<writers>`: Number of writer threads
- `<whz>`: Write frequency (1~100000)
- `<s>`: Total execution time (seconds)
- Will be uploaded by 5/25 (Fri.)

## Test Levels

1. Read-preferring reader-writer lock
2. Sequence lock
3. Write-preferring reader-writer lock

## Submission

- Source codes and documents
  - Source files: `rwrlock.h`, `rwwlock.h`, and `seqlock.h`
    - Will be compiled and tested on a Linux machine
    - Don't use a special library that requires an additional compilation option
  - Document (.pdf, .hwp, or .doc)
    - Basic design (2~3 pages)
    - Experiment results (3~4 pages): System specification, measurement setup, graphs, and analyses
    - Description of important functions:


      | Function Name | Functionality | Parameters | Return Value |
      | ------------- | ------------- | ---------- | ------------ |
      |               |               |            |              |

## Overhead/Scalability Analysis

- You can measure detailed overheads in `kurock.c` by generating timestamps as needed
  - `clock_gettime()`
  - `gettimeofday()`
- Refer to graphs in Chapter 29
  - Figure 29.5
  - Figure 29.6
  - Figure 29.11

## Submission Details

- Submit your homework through eCampus
  - Deadline: 6/7 Friday Midnight (11:59 pm)
- Cheating, plagiarism, and other anti-intellectual behavior will be dealt with severely
