#ifndef DV_THRUSTER_H
#define DV_THRUSTER_H

#include <stdlib.h>

/// Rotational (tilting in `n` direction) and lateral thruster configurations
typedef enum
{
	forward,
	backward,
	sink,
	rise,
	left,
	right
} DV_thruster_config_t;

typedef struct
{
	DV_thruster_config_t lateral;
	DV_thruster_config_t rotational;
} DV_thruster_t;

DV_thruster_t *DV_thruster_create(DV_thruster_config_t lateral, DV_thruster_config_t rotational)
{
	DV_thruster_t *this = (DV_thruster_t *)malloc(sizeof(DV_thruster_t));
	this->lateral = lateral;
	this->rotational = rotational;

	return this;
}

void DV_thruster_destroy(DV_thruster_t *this)
{
	free(this);
	this = NULL;
}

#endif /* DV_THRUSTER_H */