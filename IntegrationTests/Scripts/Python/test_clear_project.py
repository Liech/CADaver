result = cadaver({"type": "ClearProject", "version": 1})
assert result.get("type") == "ClearProject", f"ClearProject failed: {result}"

count_res = cadaver({"type": "GetAssetCount", "version": 1})
assert count_res.get("type") == "GetAssetCount", f"GetAssetCount failed: {count_res}"
assert count_res.get("count") == 0, f"Expected 0 assets, got: {count_res}"

names_res = cadaver({"type": "GetAssetNames", "version": 1})
assert names_res.get("type") == "GetAssetNames", f"GetAssetNames failed: {names_res}"
assert len(names_res.get("assetNames", [])) == 0, f"Expected empty assetNames, got: {names_res}"
