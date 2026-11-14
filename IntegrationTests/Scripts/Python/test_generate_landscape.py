clear_res = cadaver({"type": "ClearProject", "version": 1})
assert clear_res.get("type") == "ClearProject", f"ClearProject failed: {clear_res}"

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
assert gen_res.get("type") == "GenerateLandscape", f"GenerateLandscape failed: {gen_res}"
assert "assetGuid" in gen_res, "Expected assetGuid in result"

count_res = cadaver({"type": "GetAssetCount", "version": 1})
assert count_res.get("type") == "GetAssetCount", f"GetAssetCount failed: {count_res}"
assert count_res.get("count") >= 1, f"Expected at least 1 asset, got: {count_res}"

names_res = cadaver({"type": "GetAssetNames", "version": 1})
assert names_res.get("type") == "GetAssetNames", f"GetAssetNames failed: {names_res}"
assert "IntegrationLandscape" in names_res.get("assetNames", []), f"Expected IntegrationLandscape in names, got: {names_res}"

info_res_by_name = cadaver({"type": "GetAssetInfo", "version": 1, "assetName": "IntegrationLandscape"})
assert info_res_by_name.get("type") == "GetAssetInfo", f"GetAssetInfo by name failed: {info_res_by_name}"
assert info_res_by_name.get("assetGuid") == gen_res.get("assetGuid"), f"GUID mismatch: {info_res_by_name}"

info_res_by_guid = cadaver({"type": "GetAssetInfo", "version": 1, "assetGuid": gen_res.get("assetGuid")})
assert info_res_by_guid.get("type") == "GetAssetInfo", f"GetAssetInfo by guid failed: {info_res_by_guid}"
assert info_res_by_guid.get("assetName") == "IntegrationLandscape", f"Name mismatch: {info_res_by_guid}"

asset_aabb = cadaver({"type": "GetAssetAABB", "version": 1, "assetName": "IntegrationLandscape"})
assert asset_aabb.get("type") == "GetAssetAABB", f"GetAssetAABB failed: {asset_aabb}"
assert len(asset_aabb.get("min", [])) == 3, f"Expected 3D min vector: {asset_aabb}"

tasks_res = cadaver({"type": "GetActiveTasks", "version": 1})
assert tasks_res.get("type") == "GetActiveTasks", f"GetActiveTasks failed: {tasks_res}"

cam_prefs = cadaver({"type": "GetCameraPreferences", "version": 1})
assert cam_prefs.get("type") == "GetCameraPreferences", f"GetCameraPreferences failed: {cam_prefs}"
assert "cameraSpeed" in cam_prefs.get("preferences", {}), f"Expected cameraSpeed: {cam_prefs}"

render_prefs = cadaver({"type": "GetRenderPreferences", "version": 1})
assert render_prefs.get("type") == "GetRenderPreferences", f"GetRenderPreferences failed: {render_prefs}"

svo_prefs = cadaver({"type": "GetSVOPreferences", "version": 1})
assert svo_prefs.get("type") == "GetSVOPreferences", f"GetSVOPreferences failed: {svo_prefs}"

ui_prefs = cadaver({"type": "GetUIPreferences", "version": 1})
assert ui_prefs.get("type") == "GetUIPreferences", f"GetUIPreferences failed: {ui_prefs}"
