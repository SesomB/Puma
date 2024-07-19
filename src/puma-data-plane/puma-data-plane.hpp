#pragma once

#include <thread>
#include "DpdkDevice.h"
#include "DpdkDeviceList.h"
#include "../configuration/configuration.hpp"

class PumaDataPlane : public pcpp::DpdkWorkerThread
{
private:
	pcpp::DpdkDevice *m_Device;
	Configuration& m_Configuration;
	int m_RxQueueId;
	bool m_KeepRunning;
	uint32_t m_CoreId;

public:
	// c'tor
	PumaDataPlane(Configuration& configuration,pcpp::DpdkDevice *device, int rxQueueId);

	// d'tor (does nothing)
	~PumaDataPlane() {}

	// implement abstract method

	// start running the worker thread
	bool run(uint32_t coreId);

	// ask the worker thread to stop
	void stop();

	// get worker thread core ID
	uint32_t getCoreId() const;
};
