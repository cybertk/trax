/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */

#ifndef SHM_H
#define SHM_H

#define SHM_NAME_LENGTH sizeof("/trax-12345678")

typedef struct {
    char name[SHM_NAME_LENGTH+1];
    int fd;
    int size;
    char* ptr;
} shm_region;

shm_region* shm_alloc(int size);

int shm_decodelen(const char *bufcoded);

int shm_decode(shm_region *shm, const char *bufcoded);

int shm_encodelen(int len);

int shm_encode(char *encoded, const shm_region *shm);

#endif 
