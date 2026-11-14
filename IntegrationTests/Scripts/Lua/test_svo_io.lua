local temp_out_path = "temp_lua_svo_test.svo"

-- 1. Clear project
local res = cadaver({type = "ClearProject", version = 1})
assert(res.type == "ClearProject", "ClearProject failed in Lua")

-- 2. Generate Landscape
local gen_res = cadaver({
    type = "GenerateLandscape",
    version = 1,
    assetName = "LuaLandscape",
    dimension = 8,
    seed = 1337,
    octaves = 4,
    frequency = 0.05,
    persistence = 0.5,
    lacunarity = 2.0,
    baseHeightRatio = 0.4,
    heightScale = 1.2,
    enableCaves = false,
    caveFrequency = 0.04,
    caveThreshold = 0.12,
    solidThreshold = 0.0
})
assert(gen_res.type == "GenerateLandscape", "GenerateLandscape failed in Lua")
local asset_guid = gen_res.assetGuid
assert(asset_guid ~= nil, "Missing assetGuid")

-- 3. Save Voxel asset using destination callback
local saved_buf = nil
local function save_dest(buf)
    saved_buf = buf
end

local save_res = cadaver({
    type = "SaveAsset",
    version = 1,
    assetGuid = asset_guid,
    destination = save_dest
})
assert(save_res.type == "SaveAsset", "SaveAsset with callback failed in Lua")
assert(saved_buf ~= nil, "Callback did not receive buffer")
assert(#saved_buf > 0, "Received buffer is empty")

-- 4. Clear project
res = cadaver({type = "ClearProject", version = 1})
assert(res.type == "ClearProject")

-- 5. Load SVO using source callback returning buffer
local function load_source()
    return saved_buf
end

local cb_load_res = cadaver({
    type = "LoadAsset",
    version = 1,
    name = "LuaCallbackSvo",
    assetType = "Voxel",
    source = load_source
})
assert(cb_load_res.type == "LoadAsset", "LoadAsset with callback failed in Lua")
local cb_asset_guid = cb_load_res.assetGuid
assert(cb_asset_guid ~= nil, "Missing cb_asset_guid")

-- 6. Save SVO with filepath fallback in destination
local save_file_res = cadaver({
    type = "SaveAsset",
    version = 1,
    assetGuid = cb_asset_guid,
    destination = temp_out_path
})
assert(save_file_res.type == "SaveAsset", "SaveAsset with filepath failed in Lua")

-- 7. Clear project and load SVO with filepath fallback
res = cadaver({type = "ClearProject", version = 1})
assert(res.type == "ClearProject")

local file_load_res = cadaver({
    type = "LoadAsset",
    version = 1,
    name = "LuaFileSvo",
    source = temp_out_path
})
assert(file_load_res.type == "LoadAsset", "LoadAsset with filepath failed in Lua")
assert(file_load_res.assetGuid ~= nil, "Missing assetGuid from file load")

-- 8. Clean up
os.remove(temp_out_path)
