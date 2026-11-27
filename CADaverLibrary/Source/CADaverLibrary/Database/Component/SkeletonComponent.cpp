#include "Database/Component/SkeletonComponent.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

SkeletonComponent::SkeletonComponent() {}

SkeletonComponent::~SkeletonComponent() {}

std::string SkeletonComponent::getType() const
{
    return "Skeleton";
}

std::string SkeletonComponent::getDescription() const
{
    return "Component: Skeleton\nAttaches a skeletal joint hierarchy with rest transforms and inverse bind matrices to a Triangulation asset.";
}

size_t SkeletonComponent::getSizeInBytes() const
{
    return sizeof(SkeletonComponent) + skeleton.getJointCount() * sizeof(CADacomb::Animation::Joint);
}

bool SkeletonComponent::isCompatible(const std::string& assetType) const
{
    return assetType == "Triangulation";
}

nlohmann::json SkeletonComponent::toDataJson() const
{
    nlohmann::json jointsArray = nlohmann::json::array();
    for (const auto& joint : skeleton.getJoints())
    {
        const auto&    t        = joint.getRestLocalTransform();
        const auto&    pos      = t.getTranslation();
        const auto&    rot      = t.getRotation();
        const auto&    scl      = t.getScale();
        const auto&    invBind  = joint.getInverseBindMatrix();
        nlohmann::json matArray = nlohmann::json::array();
        for (int col = 0; col < 4; col++)
        {
            for (int row = 0; row < 4; row++)
            {
                matArray.push_back(invBind[col][row]);
            }
        }

        jointsArray.push_back(nlohmann::json{
          {              "name",                joint.getName() },
          {       "parentIndex",         joint.getParentIndex() },
          {       "translation",        { pos.x, pos.y, pos.z } },
          {          "rotation", { rot.w, rot.x, rot.y, rot.z } },
          {             "scale",        { scl.x, scl.y, scl.z } },
          { "inverseBindMatrix",                       matArray }
        });
    }

    return nlohmann::json{
        { "Version",           1 },
        {  "joints", jointsArray }
    };
}

void SkeletonComponent::fromDataJson(const nlohmann::json& jsonObject)
{
    int version = jsonObject.at("Version").get<int>();
    (void)version;

    CADacomb::Animation::Skeleton loadedSkeleton;
    const auto&                   jointsArray = jsonObject.at("joints");
    for (const auto& jointJson : jointsArray)
    {
        std::string name        = jointJson.at("name").get<std::string>();
        int         parentIndex = jointJson.at("parentIndex").get<int>();
        const auto& posJson     = jointJson.at("translation");
        const auto& rotJson     = jointJson.at("rotation");
        const auto& sclJson     = jointJson.at("scale");
        const auto& matJson     = jointJson.at("inverseBindMatrix");

        glm::dvec3 pos(posJson[0].get<double>(), posJson[1].get<double>(), posJson[2].get<double>());
        glm::dquat rot(rotJson[0].get<double>(), rotJson[1].get<double>(), rotJson[2].get<double>(), rotJson[3].get<double>());
        glm::dvec3 scl(sclJson[0].get<double>(), sclJson[1].get<double>(), sclJson[2].get<double>());

        CADacomb::Animation::Joint joint(name, parentIndex, CADacomb::Animation::Transform(pos, rot, scl));

        glm::dmat4 invBind(1.0);
        for (int col = 0; col < 4; col++)
        {
            for (int row = 0; row < 4; row++)
            {
                invBind[col][row] = matJson[static_cast<size_t>(col * 4 + row)].get<double>();
            }
        }
        joint.setInverseBindMatrix(invBind);
        loadedSkeleton.addJoint(joint);
    }

    skeleton = std::move(loadedSkeleton);
    markModified();
}

const CADacomb::Animation::Skeleton& SkeletonComponent::getSkeleton() const
{
    return skeleton;
}

CADacomb::Animation::Skeleton& SkeletonComponent::getSkeleton()
{
    return skeleton;
}

void SkeletonComponent::setSkeleton(const CADacomb::Animation::Skeleton& newSkeleton)
{
    skeleton = newSkeleton;
    markModified();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SkeletonComponent/JsonRoundTrip")
{
    SkeletonComponent component;
    CHECK(component.getType() == "Skeleton");
    CHECK(component.isCompatible("Triangulation"));
    CHECK_FALSE(component.isCompatible("Voxel"));

    CADacomb::Animation::Skeleton  skel;
    int                            rootIdx = skel.addJoint(CADacomb::Animation::Joint("Root", -1, CADacomb::Animation::Transform::identity()));
    CADacomb::Animation::Transform childTransform(glm::dvec3(0.0, 1.5, 0.0), glm::dquat(1.0, 0.0, 0.0, 0.0), glm::dvec3(1.0, 1.0, 1.0));
    skel.addJoint(CADacomb::Animation::Joint("Spine", rootIdx, childTransform));
    skel.computeInverseBindMatrices();
    component.setSkeleton(skel);

    nlohmann::json    serialized = component.toDataJson();
    SkeletonComponent restored;
    restored.fromDataJson(serialized);

    REQUIRE(restored.getSkeleton().getJointCount() == 2);
    CHECK(restored.getSkeleton().getJoint(0).getName() == "Root");
    CHECK(restored.getSkeleton().getJoint(1).getName() == "Spine");
    CHECK(restored.getSkeleton().getJoint(1).getParentIndex() == 0);
    CHECK(restored.getSkeleton().getJoint(1).getRestLocalTransform().getTranslation().y == Catch::Approx(1.5));
    CHECK(restored.getSkeleton().getJoint(1).getInverseBindMatrix()[3][1] == Catch::Approx(-1.5));
}
#endif
