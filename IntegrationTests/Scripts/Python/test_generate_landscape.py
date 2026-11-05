clear_res = cadaver({"type": "ClearProject", "version": 1})
assert clear_res.get("status") == "Success", f"ClearProject failed: {clear_res}"

gen_res = cadaver({
    "type": "GenerateLandscape",
    "version": 1,
    "assetName": "IntegrationLandscape",
    "dimension": 16
})
assert gen_res.get("status") == "Success", f"GenerateLandscape failed: {gen_res}"
assert "assetGuid" in gen_res, "Expected assetGuid in result"
