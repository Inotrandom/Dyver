#ifndef THRUSTER_DRIVER_H
#define THRUSTER_DRIVER_H

class thruster_driver_t
{
public:
	explicit thruster_driver_t() {}
	~thruster_driver_t() {}

	virtual auto set_throttle(double t) -> int = 0;

private:
};

#endif // THRUSTER_DRIVER_H