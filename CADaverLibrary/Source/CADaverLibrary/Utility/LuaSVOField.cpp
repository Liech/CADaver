#include "Utility/LuaSVOField.h"

#include "CADacomb/Volume/MergeStrategy/MostPrevalentExceptMergeStrategy.h"
#include "CADacomb/Volume/SVO/Generator/SVOLandscapeGenerator.h"

#include <cmath>
#include <lua.hpp>
#include <vector>

#if defined(_OPENMP)
#include <omp.h>
#endif

LuaSVOField::LuaSVOField() {}

LuaSVOField::~LuaSVOField() {}

int LuaSVOField::luaPerlin3D(lua_State* state)
{
    double   x    = luaL_checknumber(state, 1);
    double   y    = luaL_checknumber(state, 2);
    double   z    = luaL_checknumber(state, 3);
    uint32_t seed = static_cast<uint32_t>(luaL_optinteger(state, 4, 0));

    double result = CADacomb::SVOLandscapeGenerator::perlinNoise3D(glm::dvec3(x, y, z), seed);
    lua_pushnumber(state, result);
    return 1;
}

int LuaSVOField::luaFbm3D(lua_State* state)
{
    double   x           = luaL_checknumber(state, 1);
    double   y           = luaL_checknumber(state, 2);
    double   z           = luaL_checknumber(state, 3);
    int      octaves     = static_cast<int>(luaL_optinteger(state, 4, 4));
    double   frequency   = luaL_optnumber(state, 5, 1.0);
    double   persistence = luaL_optnumber(state, 6, 0.5);
    double   lacunarity  = luaL_optnumber(state, 7, 2.0);
    uint32_t seed        = static_cast<uint32_t>(luaL_optinteger(state, 8, 0));

    double result = CADacomb::SVOLandscapeGenerator::fbm3D(glm::dvec3(x, y, z), octaves, frequency, persistence, lacunarity, seed);
    lua_pushnumber(state, result);
    return 1;
}

void LuaSVOField::registerNoiseHelpers(lua_State* state)
{
    lua_register(state, "perlin3d", &LuaSVOField::luaPerlin3D);
    lua_register(state, "fbm3d", &LuaSVOField::luaFbm3D);
}

std::unique_ptr<CADacomb::SVO<bool>> LuaSVOField::generateSVO(uint32_t dimension, const std::string& script, std::string& outError)
{
    if (dimension == 0 || (dimension & (dimension - 1)) != 0)
    {
        outError = "Dimension must be a positive power of two.";
        return nullptr;
    }

    int threadCount = 1;
#if defined(_OPENMP)
    threadCount = std::max(1, omp_get_max_threads());
#endif

    std::vector<lua_State*> states(static_cast<size_t>(threadCount), nullptr);
    std::vector<int>        evalRefs(static_cast<size_t>(threadCount), LUA_NOREF);
    std::vector<int>        sdfRefs(static_cast<size_t>(threadCount), LUA_NOREF);
    std::vector<int>        classifyRefs(static_cast<size_t>(threadCount), LUA_NOREF);

    auto cleanupStates = [&]()
    {
        for (size_t i = 0; i < states.size(); i++)
        {
            if (states[i] != nullptr)
            {
                lua_close(states[i]);
                states[i] = nullptr;
            }
        }
    };

    for (size_t i = 0; i < states.size(); i++)
    {
        lua_State* state = luaL_newstate();
        if (state == nullptr)
        {
            outError = "Failed to allocate Lua state.";
            cleanupStates();
            return nullptr;
        }
        states[i] = state;
        luaL_openlibs(state);
        registerNoiseHelpers(state);

        if (luaL_dostring(state, script.c_str()) != LUA_OK)
        {
            const char* errMsg = lua_tostring(state, -1);
            outError           = (errMsg != nullptr) ? std::string(errMsg) : "Lua script error.";
            cleanupStates();
            return nullptr;
        }

        lua_getglobal(state, "eval");
        if (lua_isfunction(state, -1))
        {
            evalRefs[i] = luaL_ref(state, LUA_REGISTRYINDEX);
        }
        else
        {
            lua_pop(state, 1);
        }

        lua_getglobal(state, "sdf");
        if (lua_isfunction(state, -1))
        {
            sdfRefs[i] = luaL_ref(state, LUA_REGISTRYINDEX);
        }
        else
        {
            lua_pop(state, 1);
        }

        lua_getglobal(state, "classifyBox");
        if (lua_isfunction(state, -1))
        {
            classifyRefs[i] = luaL_ref(state, LUA_REGISTRYINDEX);
        }
        else
        {
            lua_pop(state, 1);
        }

        if (evalRefs[i] == LUA_NOREF && sdfRefs[i] == LUA_NOREF)
        {
            outError = "Lua script must define either function sdf(x, y, z) or function eval(x, y, z).";
            cleanupStates();
            return nullptr;
        }
    }

    const double invDim       = 2.0 / static_cast<double>(dimension);
    const double sqrt3Half    = 0.86602540378443864676;
    const bool   hasSdf       = (sdfRefs[0] != LUA_NOREF);
    const bool   hasClassify  = (classifyRefs[0] != LUA_NOREF);

    auto getThreadIndex = []() -> size_t
    {
#if defined(_OPENMP)
        return static_cast<size_t>(omp_get_thread_num());
#else
        return 0;
#endif
    };

    auto evalPoint = [&](size_t tid, double nx, double ny, double nz) -> bool
    {
        lua_State* state = states[tid];
        if (hasSdf)
        {
            lua_rawgeti(state, LUA_REGISTRYINDEX, sdfRefs[tid]);
            lua_pushnumber(state, nx);
            lua_pushnumber(state, ny);
            lua_pushnumber(state, nz);
            if (lua_pcall(state, 3, 1, 0) != LUA_OK)
            {
                lua_pop(state, 1);
                return false;
            }
            double dist = lua_tonumber(state, -1);
            lua_pop(state, 1);
            return dist < 0.0;
        }

        lua_rawgeti(state, LUA_REGISTRYINDEX, evalRefs[tid]);
        lua_pushnumber(state, nx);
        lua_pushnumber(state, ny);
        lua_pushnumber(state, nz);
        if (lua_pcall(state, 3, 1, 0) != LUA_OK)
        {
            lua_pop(state, 1);
            return false;
        }

        bool solid = false;
        if (lua_isboolean(state, -1))
        {
            solid = (lua_toboolean(state, -1) != 0);
        }
        else if (lua_isnumber(state, -1))
        {
            solid = (lua_tonumber(state, -1) > 0.0);
        }
        lua_pop(state, 1);
        return solid;
    };

    auto classifyBox = [&](const glm::ivec3& boxMin, uint32_t boxSize, bool& outUniformVal) -> bool
    {
        size_t     tid   = getThreadIndex();
        lua_State* state = states[tid];

        double worldSize = static_cast<double>(boxSize) * invDim;
        double minX      = static_cast<double>(boxMin.x) * invDim - 1.0;
        double minY      = static_cast<double>(boxMin.y) * invDim - 1.0;
        double minZ      = static_cast<double>(boxMin.z) * invDim - 1.0;

        if (hasClassify)
        {
            lua_rawgeti(state, LUA_REGISTRYINDEX, classifyRefs[tid]);
            lua_pushnumber(state, minX);
            lua_pushnumber(state, minY);
            lua_pushnumber(state, minZ);
            lua_pushnumber(state, worldSize);
            if (lua_pcall(state, 4, 1, 0) != LUA_OK)
            {
                lua_pop(state, 1);
                return false;
            }

            if (lua_isboolean(state, -1))
            {
                outUniformVal = (lua_toboolean(state, -1) != 0);
                lua_pop(state, 1);
                return true;
            }
            lua_pop(state, 1);
            return false;
        }

        if (hasSdf)
        {
            double cx     = minX + 0.5 * worldSize;
            double cy     = minY + 0.5 * worldSize;
            double cz     = minZ + 0.5 * worldSize;
            double radius = worldSize * sqrt3Half;

            lua_rawgeti(state, LUA_REGISTRYINDEX, sdfRefs[tid]);
            lua_pushnumber(state, cx);
            lua_pushnumber(state, cy);
            lua_pushnumber(state, cz);
            if (lua_pcall(state, 3, 1, 0) != LUA_OK)
            {
                lua_pop(state, 1);
                return false;
            }

            double dist = lua_tonumber(state, -1);
            lua_pop(state, 1);

            if (dist > radius)
            {
                outUniformVal = false;
                return true;
            }
            if (dist < -radius)
            {
                outUniformVal = true;
                return true;
            }
        }

        return false;
    };

    auto evalBlock = [&](const glm::ivec3& blockMin, uint32_t blockSize, bool* outBlock) -> void
    {
        size_t tid    = getThreadIndex();
        size_t outIdx = 0;

        for (uint32_t lz = 0; lz < blockSize; lz++)
        {
            double nz = (static_cast<double>(blockMin.z + static_cast<int>(lz)) + 0.5) * invDim - 1.0;
            for (uint32_t ly = 0; ly < blockSize; ly++)
            {
                double ny = (static_cast<double>(blockMin.y + static_cast<int>(ly)) + 0.5) * invDim - 1.0;
                for (uint32_t lx = 0; lx < blockSize; lx++)
                {
                    double nx        = (static_cast<double>(blockMin.x + static_cast<int>(lx)) + 0.5) * invDim - 1.0;
                    outBlock[outIdx] = evalPoint(tid, nx, ny, nz);
                    outIdx++;
                }
            }
        }
    };

    auto svo = CADacomb::SVO<bool>::buildTopDown(dimension, false, CADacomb::MostPrevalentExceptMergeStrategy<bool, false>::instance(), classifyBox, evalBlock, "Generating function SVO");

    cleanupStates();
    return svo;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LuaSVOField/GenerateSphereSdf")
{
    std::string script = "function sdf(x, y, z)\n"
                         "    return math.sqrt(x * x + y * y + z * z) - 0.6\n"
                         "end\n";
    std::string error;
    auto        svo = LuaSVOField::generateSVO(32, script, error);

    REQUIRE(svo != nullptr);
    CHECK(error.empty());
    CHECK(svo->getVoxel({ 16, 16, 16 }) == true);
    CHECK(svo->getVoxel({ 0, 0, 0 }) == false);
}

TEST_CASE("LuaSVOField/GenerateEvalWithNoiseHelper")
{
    std::string script = "function eval(x, y, z)\n"
                         "    return y < 0.0 + 0.1 * perlin3d(x * 2, y * 2, z * 2, 42)\n"
                         "end\n";
    std::string error;
    auto        svo = LuaSVOField::generateSVO(16, script, error);

    REQUIRE(svo != nullptr);
    CHECK(error.empty());
    CHECK(svo->getVoxel({ 8, 1, 8 }) == true);
    CHECK(svo->getVoxel({ 8, 14, 8 }) == false);
}

TEST_CASE("LuaSVOField/InvalidScriptReturnsError")
{
    std::string error;
    auto        svo = LuaSVOField::generateSVO(16, "this is not valid lua(((", error);
    CHECK(svo == nullptr);
    CHECK_FALSE(error.empty());
}
#endif
