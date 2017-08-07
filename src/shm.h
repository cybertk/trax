/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */

#ifndef SHM_H
#define SHM_H

typedef struct {
    char name[12];
    int length;
    char* ptr;
} shm_region;

int shm_decodelen(const char *bufcoded);

int shm_decode(shm_region *bufplain, const char *bufcoded);

int shm_encodelen(int len);

int shm_encode(shm_region *encoded, const unsigned char *string, int len);

#endif 
