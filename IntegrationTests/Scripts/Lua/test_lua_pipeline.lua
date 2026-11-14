local clearRes = cadaver({type = "ClearProject", version = 1})
assert(clearRes.type == "ClearProject", "ClearProject failed in Lua")

local genRes = cadaver({
    type = "GenerateLandscape",
    version = 1,
    assetName = "LuaLandscape",
    dimension = 16,
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
assert(genRes.type == "GenerateLandscape", "GenerateLandscape failed in Lua")
assert(genRes.assetGuid ~= nil, "Expected assetGuid in Lua result")

local countRes = cadaver({type = "GetAssetCount", version = 1})
assert(countRes.type == "GetAssetCount", "GetAssetCount failed in Lua")
assert(countRes.count >= 1, "Expected at least 1 asset in Lua")

local namesRes = cadaver({type = "GetAssetNames", version = 1})
assert(namesRes.type == "GetAssetNames", "GetAssetNames failed in Lua")

local infoRes = cadaver({type = "GetAssetInfo", version = 1, assetName = "LuaLandscape"})
assert(infoRes.type == "GetAssetInfo", "GetAssetInfo by name failed in Lua")
assert(infoRes.assetGuid == genRes.assetGuid, "GUID mismatch in Lua")
