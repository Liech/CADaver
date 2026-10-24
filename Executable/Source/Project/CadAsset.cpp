#include "CadAsset.h"

CadAsset::CadAsset(const std::string& nameInput)
  : Asset(nameInput, AssetType::Cad)
{
}

CadAsset::~CadAsset() {}
