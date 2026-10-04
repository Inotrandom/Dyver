#ifndef DV_PIGPIO_T200_H
#define DV_PIGPIO_T200_H

#include <stdlib.h>
#include <string.h>

#include "DV_defs.h"

typedef struct
{
	char *id;
} DV_pigpio_t200_t;

DV_pigpio_t200_t *DV_pigpio_t200_create(char *id)
{
	DV_pigpio_t200_t *this = (DV_pigpio_t200_t *)malloc(sizeof(DV_pigpio_t200_t));
	this->id = strdup(id);

	return 0;
}

void DV_pigpio_t200_destroy(DV_pigpio_t200_t *this)
{
	free(this->id);
	this->id = NULL;

	free(this);
	this = NULL;
}

#endif /* DV_PIGPIO_T200_H */
