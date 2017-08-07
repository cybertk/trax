/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
#include <assert.h>
#include <stdlib.h>

int shm_decodelen(const char *bufcoded) {
    return sizeof(shm_region);
}

// trax-NNNNNNNN-SIZE
int shm_decode(shm_region *shm, const char *bufcoded) {

    strncpy(shm->name, bufcoded, sizeof("/trax-NNNNNNNN"));
    shm->name[sizeof(shm->name)] = 0;
    // /trax-NNNNNNNN-
    shm->length = atoi(bufcoded + sizeof(shm->name) + 1);

    int fd = shm_open(shm->name, O_RDWR, S_IRUSR | S_IWUSR);
}

int shm_encodelen(int len);

int shm_encode(shm_region *encoded, const unsigned char *string, int len);
