result = cadaver({"type": "ClearProject", "version": 1})
assert result.get("status") == "Success", f"ClearProject failed: {result}"

count_res = cadaver({"type": "GetAssetCount", "version": 1})
assert count_res.get("status") == "Success", f"GetAssetCount failed: {count_res}"
assert count_res.get("count") == 0, f"Expected 0 assets, got: {count_res}"

names_res = cadaver({"type": "GetAssetNames", "version": 1})
assert names_res.get("status") == "Success", f"GetAssetNames failed: {names_res}"
assert len(names_res.get("assetNames", [])) == 0, f"Expected empty assetNames, got: {names_res}"
