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
