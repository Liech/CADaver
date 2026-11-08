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

local stl_path = find_repo_file("bo.stl")
local temp_out_path = "temp_lua_stl_test.stl"

-- 1. Clear project
local res = cadaver({type = "ClearProject", version = 1})
assert(res.status == "Success", "ClearProject failed in Lua")

-- 2. Load STL with filepath fallback
local load_res = cadaver({
    type = "LoadStlAsset",
    version = 1,
    name = "LuaFileStl",
    source = stl_path
})
assert(load_res.status == "Success", "LoadStlAsset with filepath failed in Lua")
local asset_guid = load_res.assetGuid
assert(asset_guid ~= nil, "Missing assetGuid")

-- 3. Save STL using destination callback
local saved_buf = nil
local function save_dest(buf)
    saved_buf = buf
end

local save_res = cadaver({
    type = "SaveStlAsset",
    version = 1,
    assetGuid = asset_guid,
    destination = save_dest
})
assert(save_res.status == "Success", "SaveStlAsset with callback failed in Lua")
assert(saved_buf ~= nil, "Callback did not receive buffer")
assert(#saved_buf > 0, "Received buffer is empty")

-- 4. Clear project
res = cadaver({type = "ClearProject", version = 1})
assert(res.status == "Success")

-- 5. Load STL using source callback returning buffer
local function load_source()
    return saved_buf
end

local cb_load_res = cadaver({
    type = "LoadStlAsset",
    version = 1,
    name = "LuaCallbackStl",
    source = load_source
})
assert(cb_load_res.status == "Success", "LoadStlAsset with callback failed in Lua")
local cb_asset_guid = cb_load_res.assetGuid
assert(cb_asset_guid ~= nil, "Missing cb_asset_guid")

-- 6. Save STL with filepath fallback in destination
local save_file_res = cadaver({
    type = "SaveStlAsset",
    version = 1,
    assetGuid = cb_asset_guid,
    destination = temp_out_path
})
assert(save_file_res.status == "Success", "SaveStlAsset with filepath failed in Lua")

local check_file = io.open(temp_out_path, "rb")
assert(check_file ~= nil, "Destination file was not created")
check_file:close()

-- 7. Clean up
os.remove(temp_out_path)
