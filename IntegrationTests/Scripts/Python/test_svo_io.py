import os
import sys

temp_out_path = os.path.abspath("temp_python_svo_test.svo")

# 1. Clear project
res = cadaver({"type": "ClearProject", "version": 1})
assert res.get("status") == "Success", f"ClearProject failed: {res}"

# 2. Generate a small landscape to produce an SVO voxel asset
gen_res = cadaver({
    "type": "GenerateLandscape",
    "version": 1,
    "assetName": "SourceLandscape",
    "dimension": 8,
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
asset_guid = gen_res.get("assetGuid")
assert asset_guid, "Missing assetGuid"

# 3. Save Voxel asset using destination callback
saved_data = None
def save_dest(data):
    global saved_data
    saved_data = data

save_res = cadaver({
    "type": "SaveVoxelAsset",
    "version": 1,
    "assetGuid": asset_guid,
    "destination": save_dest
})
assert save_res.get("status") == "Success", f"SaveVoxelAsset with callback failed: {save_res}"
assert saved_data is not None and len(saved_data) > 0, "Callback did not receive binary data"

# 4. Clear project
res = cadaver({"type": "ClearProject", "version": 1})
assert res.get("status") == "Success"

# 5. Load SVO using source callback returning binary data
def load_source(*args):
    return saved_data

cb_load_res = cadaver({
    "type": "LoadSvoAsset",
    "version": 1,
    "name": "CallbackSvo",
    "source": load_source
})
assert cb_load_res.get("status") == "Success", f"LoadSvoAsset with callback failed: {cb_load_res}"
cb_asset_guid = cb_load_res.get("assetGuid")
assert cb_asset_guid, "Missing assetGuid from callback load"

# 6. Save SVO with filepath fallback in destination
save_file_res = cadaver({
    "type": "SaveVoxelAsset",
    "version": 1,
    "assetGuid": cb_asset_guid,
    "destination": temp_out_path
})
assert save_file_res.get("status") == "Success", f"SaveVoxelAsset with filepath failed: {save_file_res}"
assert os.path.exists(temp_out_path), "Destination file was not created"
assert os.path.getsize(temp_out_path) > 0, "Destination file is empty"

# 7. Clear project and load SVO with filepath fallback
res = cadaver({"type": "ClearProject", "version": 1})
assert res.get("status") == "Success"

file_load_res = cadaver({
    "type": "LoadSvoAsset",
    "version": 1,
    "name": "FileSvo",
    "source": temp_out_path
})
assert file_load_res.get("status") == "Success", f"LoadSvoAsset with filepath failed: {file_load_res}"
assert file_load_res.get("assetGuid"), "Missing assetGuid from file load"

# 8. Clean up
if os.path.exists(temp_out_path):
    os.remove(temp_out_path)
