#pragma once

#include "CADacomb/Util/GUID.h"
#include "Polyglot/Node.h"

#include <future>
#include <memory>

class Defer;
class GatedDatabase;

// Chains a default ClusterByNormal and ConvertClusteringToCad run on one triangulation asset. Must be owned by a shared_ptr; keeps itself alive until finished.
class DefaultCadConversion : public std::enable_shared_from_this<DefaultCadConversion>
{
  public:
    DefaultCadConversion(GatedDatabase& gatedDatabase, Defer& defer, const CADacomb::GUID& assetGuid);
    virtual ~DefaultCadConversion();

    static void start(GatedDatabase& gatedDatabase, Defer& defer, const CADacomb::GUID& assetGuid);

  private:
    void launch();
    void schedulePoll();
    void poll();

    GatedDatabase*              gatedDatabase;
    Defer*                      defer;
    CADacomb::GUID              assetGuid;
    std::future<Polyglot::Node> clusterFuture;
};
