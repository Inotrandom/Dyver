#ifndef DV_ROV_H
#define DV_ROV_H

#include "DV_thruster.h"

#define DV_ROV_THRUSTER_MAX 24

typedef struct
{
	DV_thruster_t **thrusters;
} DV_rov_t;

DV_rov_t *DV_rov_create()
{
	DV_rov_t *this = (DV_rov_t *)malloc(sizeof(DV_rov_t));
	this->thrusters = (DV_thruster_t **)malloc(sizeof(DV_thruster_t *) * DV_ROV_THRUSTER_MAX);

	return this;
}

void DV_rov_destroy(DV_rov_t *this)
{
	for (DV_thruster_t **iter = this->thrusters; iter < (this->thrusters + DV_ROV_THRUSTER_MAX); ++iter)
	{
		DV_thruster_destroy(*iter);
	}
	free(this->thrusters);
	this->thrusters = NULL;

	free(this);
	this = NULL;
}

#endif /* DV_ROV_H */