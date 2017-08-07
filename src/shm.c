/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#if defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#endif

#include "shm.h"

static int shm_region_next_id = 0;

int shm_decodelen(const char *bufcoded) {
    return sizeof(shm_region);
}

// trax-NNNNNNNN-SIZE
int shm_decode(shm_region *shm, const char *bufcoded) {

    strncpy(shm->name, bufcoded, SHM_NAME_LENGTH);
    shm->name[SHM_NAME_LENGTH] = 0;
    // /trax-NNNNNNNN-
    shm->size = atoi(bufcoded + sizeof(shm->name) + 1);

    shm->fd = shm_open(shm->name, O_RDWR, S_IRUSR | S_IWUSR);
    assert(shm->fd > 0);

    shm->ptr = mmap(NULL, shm->size, PROT_READ | PROT_WRITE, MAP_SHARED, shm->fd, 0);
    assert(shm->ptr != MAP_FAILED);
}

int shm_encodelen(int len) {
    return snprintf(NULL, 0, "%d", len) + SHM_NAME_LENGTH;
}

int shm_encode(char *encoded, const shm_region *shm) {

    sprintf(encoded, "%s-%d", shm->name, shm->size);

    return 0;
}

shm_region* shm_alloc(int size) {
    shm_region *shm;

    shm = (shm_region*) malloc(sizeof(shm_region));
    shm->size = size;

    while(1) {
    sprintf(shm->name, "/trax-%08x", shm_region_next_id);
    shm_region_next_id++;
    shm->fd = shm_open(shm->name, O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    if (shm->fd > 0)  break;
    }

    if (shm->fd < 0 ) goto failure;

    assert(ftruncate(shm->fd, size) != -1);

    shm->ptr = mmap(NULL, shm->size, PROT_READ | PROT_WRITE, MAP_SHARED, shm->fd, 0);
    if (shm->ptr == MAP_FAILED) goto failure;

    return shm;

failure:
    if (shm->fd > 0); close(shm->fd);
    free(shm);
}
