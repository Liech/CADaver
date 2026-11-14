import os
import sys

def find_repo_file(rel_path):
    candidates = [
        rel_path,
        os.path.join("../../../..", rel_path),
        os.path.join(os.path.dirname(__file__), "../../../..", rel_path) if "__file__" in globals() else "",
    ]
    for c in candidates:
        if c and os.path.exists(c):
            return os.path.abspath(c)
    raise FileNotFoundError(f"Could not find test file: {rel_path}")

step_path = find_repo_file(os.path.join("cadexample", "test.step"))
temp_out_path = os.path.abspath("temp_python_step_test.step")

# 1. Clear project
res = cadaver({"type": "ClearProject", "version": 1})
assert res.get("type") == "ClearProject", f"ClearProject failed: {res}"

# 2. Load STEP with filepath fallback in source
load_res = cadaver({
    "type": "LoadAsset",
    "version": 1,
    "name": "FileStep",
    "source": step_path
})
assert load_res.get("type") == "LoadAsset", f"LoadAsset with filepath failed: {load_res}"
asset_guid = load_res.get("assetGuid")
assert asset_guid, "Missing assetGuid"

# 3. Save STEP using destination callback
saved_data = None
def save_dest(data):
    global saved_data
    saved_data = data

save_res = cadaver({
    "type": "SaveAsset",
    "version": 1,
    "assetGuid": asset_guid,
    "destination": save_dest
})
assert save_res.get("type") == "SaveAsset", f"SaveAsset with callback failed: {save_res}"
assert saved_data is not None and len(saved_data) > 0, "Callback did not receive binary data"

# 4. Clear project
res = cadaver({"type": "ClearProject", "version": 1})
assert res.get("type") == "ClearProject"

# 5. Load STEP using source callback returning binary data
def load_source(*args):
    return saved_data

cb_load_res = cadaver({
    "type": "LoadAsset",
    "version": 1,
    "name": "CallbackStep",
    "assetType": "Cad",
    "source": load_source
})
assert cb_load_res.get("type") == "LoadAsset", f"LoadAsset with callback failed: {cb_load_res}"
cb_asset_guid = cb_load_res.get("assetGuid")
assert cb_asset_guid, "Missing assetGuid from callback load"

# 6. Save STEP with filepath fallback in destination
save_file_res = cadaver({
    "type": "SaveAsset",
    "version": 1,
    "assetGuid": cb_asset_guid,
    "destination": temp_out_path
})
assert save_file_res.get("type") == "SaveAsset", f"SaveAsset with filepath failed: {save_file_res}"
assert os.path.exists(temp_out_path), "Destination file was not created"
assert os.path.getsize(temp_out_path) > 0, "Destination file is empty"

# 7. Clear project and load STEP with filepath fallback from saved file
res = cadaver({"type": "ClearProject", "version": 1})
assert res.get("type") == "ClearProject"

file_load_res = cadaver({
    "type": "LoadAsset",
    "version": 1,
    "name": "SavedFileStep",
    "source": temp_out_path
})
assert file_load_res.get("type") == "LoadAsset", f"LoadAsset with saved file failed: {file_load_res}"

# 8. Clean up
if os.path.exists(temp_out_path):
    os.remove(temp_out_path)
