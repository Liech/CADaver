local function find_repo_file(rel_path)
    local candidates = {
        rel_path,
        "../../../../" .. rel_path,
        "../../../" .. rel_path
    }
    for _, path in ipairs(candidates) do
        local f = io.open(path, "rb")
        if f then
            f:close()
            return path
        end
    end
    error("Could not find test file: " .. rel_path)
end

local step_path = find_repo_file("cadexample/test.step")
local temp_out_path = "temp_lua_step_test.step"

-- 1. Clear project
local res = cadaver({type = "ClearProject", version = 1})
assert(res.status == "Success", "ClearProject failed in Lua")

-- 2. Load STEP with filepath fallback in source
local load_res = cadaver({
    type = "LoadStepAsset",
    version = 1,
    name = "LuaFileStep",
    source = step_path
})
assert(load_res.status == "Success", "LoadStepAsset with filepath failed in Lua")
local asset_guid = load_res.assetGuid
assert(asset_guid ~= nil, "Missing assetGuid")

-- 3. Save STEP using destination callback
local saved_buf = nil
local function save_dest(buf)
    saved_buf = buf
end

local save_res = cadaver({
    type = "SaveStepAsset",
    version = 1,
    assetGuid = asset_guid,
    destination = save_dest
})
assert(save_res.status == "Success", "SaveStepAsset with callback failed in Lua")
assert(saved_buf ~= nil, "Callback did not receive buffer")
assert(#saved_buf > 0, "Received buffer is empty")

-- 4. Clear project
res = cadaver({type = "ClearProject", version = 1})
assert(res.status == "Success")

-- 5. Load STEP using source callback returning buffer
local function load_source()
    return saved_buf
end

local cb_load_res = cadaver({
    type = "LoadStepAsset",
    version = 1,
    name = "LuaCallbackStep",
    source = load_source
})
assert(cb_load_res.status == "Success", "LoadStepAsset with callback failed in Lua")
local cb_asset_guid = cb_load_res.assetGuid
assert(cb_asset_guid ~= nil, "Missing cb_asset_guid")

-- 6. Save STEP with filepath fallback in destination
local save_file_res = cadaver({
    type = "SaveStepAsset",
    version = 1,
    assetGuid = cb_asset_guid,
    destination = temp_out_path
})
assert(save_file_res.status == "Success", "SaveStepAsset with filepath failed in Lua")

-- 7. Clear project and load STEP with filepath fallback
res = cadaver({type = "ClearProject", version = 1})
assert(res.status == "Success")

local file_load_res = cadaver({
    type = "LoadStepAsset",
    version = 1,
    name = "LuaSavedFileStep",
    source = temp_out_path
})
assert(file_load_res.status == "Success", "LoadStepAsset with saved file failed in Lua")

-- 8. Clean up
os.remove(temp_out_path)
