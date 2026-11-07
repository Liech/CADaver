local clearRes = cadaver({type = "ClearProject", version = 1})
assert(clearRes.status == "Success", "ClearProject failed in Lua")

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
assert(genRes.status == "Success", "GenerateLandscape failed in Lua")
assert(genRes.assetGuid ~= nil, "Expected assetGuid in Lua result")
