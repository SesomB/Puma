#include "../configuration/configuration.hpp"
#include "../puma-data-plane/puma-data-plane.hpp"
#include "SystemUtils.h"
#include "spdlog/spdlog.h"
#include <rte_errno.h>
#include <rte_flow.h>
class PumaControlPlane
{
  private:
    const int MBUF_POOL_SIZE = 16 * 1024 - 1;
    Configuration m_Configuration;
    bool m_KeepRunning = true;
    int m_PumaDataPlanesCoreMask = 0;
    std::vector<pcpp::DpdkWorkerThread *> m_PumaDataPlanes;

  public:
    PumaControlPlane();
    ~PumaControlPlane(){};

    void Run();
    void Stop();
};