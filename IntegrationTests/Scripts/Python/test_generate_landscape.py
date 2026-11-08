clear_res = cadaver({"type": "ClearProject", "version": 1})
assert clear_res.get("status") == "Success", f"ClearProject failed: {clear_res}"

gen_res = cadaver({
    "type": "GenerateLandscape",
    "version": 1,
    "assetName": "IntegrationLandscape",
    "dimension": 16,
    "seed": 1337,
    "octaves": 4,
    "frequency": 0.05,
    "persistence": 0.5,
    "lacunarity": 2.0,
    "baseHeightRatio": 0.4,
    "heightScale": 1.2,
    "enableCaves": False,
    "caveFrequency": 0.04,
    "caveThreshold": 0.12,
    "solidThreshold": 0.0
})
assert gen_res.get("status") == "Success", f"GenerateLandscape failed: {gen_res}"
assert "assetGuid" in gen_res, "Expected assetGuid in result"

count_res = cadaver({"type": "GetAssetCount", "version": 1})
assert count_res.get("status") == "Success", f"GetAssetCount failed: {count_res}"
assert count_res.get("count") >= 1, f"Expected at least 1 asset, got: {count_res}"

names_res = cadaver({"type": "GetAssetNames", "version": 1})
assert names_res.get("status") == "Success", f"GetAssetNames failed: {names_res}"
assert "IntegrationLandscape" in names_res.get("assetNames", []), f"Expected IntegrationLandscape in names, got: {names_res}"

info_res_by_name = cadaver({"type": "GetAssetInfo", "version": 1, "assetName": "IntegrationLandscape"})
assert info_res_by_name.get("status") == "Success", f"GetAssetInfo by name failed: {info_res_by_name}"
assert info_res_by_name.get("assetGuid") == gen_res.get("assetGuid"), f"GUID mismatch: {info_res_by_name}"

info_res_by_guid = cadaver({"type": "GetAssetInfo", "version": 1, "assetGuid": gen_res.get("assetGuid")})
assert info_res_by_guid.get("status") == "Success", f"GetAssetInfo by guid failed: {info_res_by_guid}"
assert info_res_by_guid.get("assetName") == "IntegrationLandscape", f"Name mismatch: {info_res_by_guid}"
