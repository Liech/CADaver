local clearRes = cadaver({type = "ClearProject"})
assert(clearRes.status == "Success", "ClearProject failed in Lua")

local genRes = cadaver({
    type = "GenerateLandscape",
    assetName = "LuaLandscape",
    dimension = 16
})
assert(genRes.status == "Success", "GenerateLandscape failed in Lua")
assert(genRes.assetGuid ~= nil, "Expected assetGuid in Lua result")
