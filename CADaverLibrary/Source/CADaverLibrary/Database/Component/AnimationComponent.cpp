#include "Database/Component/AnimationComponent.h"

#include "CADacomb/Animation/KeyframeAnimation/InterpolationMode.h"

AnimationComponent::AnimationComponent() {}

AnimationComponent::~AnimationComponent() {}

std::string AnimationComponent::getType() const
{
    return "Animation";
}

std::string AnimationComponent::getDescription() const
{
    return "Component: Animation\nAttaches a keyframe animation clip and playback settings to a Triangulation asset.";
}

size_t AnimationComponent::getSizeInBytes() const
{
    size_t total = sizeof(AnimationComponent);
    for (const auto& track : clip.getTracks())
    {
        total += sizeof(CADacomb::Animation::JointTrack);
        total += track.getTranslationTrack().getKeyframes().size() * sizeof(CADacomb::Animation::Vec3Keyframe);
        total += track.getRotationTrack().getKeyframes().size() * sizeof(CADacomb::Animation::QuatKeyframe);
        total += track.getScaleTrack().getKeyframes().size() * sizeof(CADacomb::Animation::Vec3Keyframe);
    }
    return total;
}

bool AnimationComponent::isCompatible(const std::string& assetType) const
{
    return assetType == "Triangulation";
}

nlohmann::json AnimationComponent::toDataJson() const
{
    nlohmann::json tracksArray = nlohmann::json::array();
    for (const auto& track : clip.getTracks())
    {
        nlohmann::json translationKeys = nlohmann::json::array();
        for (const auto& key : track.getTranslationTrack().getKeyframes())
        {
            const auto& v = key.getValue();
            translationKeys.push_back(nlohmann::json{ key.getTime(), v.x, v.y, v.z });
        }

        nlohmann::json rotationKeys = nlohmann::json::array();
        for (const auto& key : track.getRotationTrack().getKeyframes())
        {
            const auto& q = key.getValue();
            rotationKeys.push_back(nlohmann::json{ key.getTime(), q.w, q.x, q.y, q.z });
        }

        nlohmann::json scaleKeys = nlohmann::json::array();
        for (const auto& key : track.getScaleTrack().getKeyframes())
        {
            const auto& v = key.getValue();
            scaleKeys.push_back(nlohmann::json{ key.getTime(), v.x, v.y, v.z });
        }

        tracksArray.push_back(nlohmann::json{
          {                "jointName",                                                 track.getJointName() },
          { "translationInterpolation", static_cast<int>(track.getTranslationTrack().getInterpolationMode()) },
          {          "translationKeys",                                                      translationKeys },
          {    "rotationInterpolation",    static_cast<int>(track.getRotationTrack().getInterpolationMode()) },
          {             "rotationKeys",                                                         rotationKeys },
          {       "scaleInterpolation",       static_cast<int>(track.getScaleTrack().getInterpolationMode()) },
          {                "scaleKeys",                                                            scaleKeys }
        });
    }

    return nlohmann::json{
        {  "Version",                  1 },
        {  "looping",            looping },
        {     "name",     clip.getName() },
        { "duration", clip.getDuration() },
        {   "tracks",        tracksArray }
    };
}

void AnimationComponent::fromDataJson(const nlohmann::json& jsonObject)
{
    int version = jsonObject.at("Version").get<int>();
    (void)version;

    looping = jsonObject.at("looping").get<bool>();

    CADacomb::Animation::AnimationClip loadedClip(jsonObject.at("name").get<std::string>());
    loadedClip.setDuration(jsonObject.at("duration").get<double>());

    const auto& tracksArray = jsonObject.at("tracks");
    for (const auto& trackJson : tracksArray)
    {
        CADacomb::Animation::JointTrack track(trackJson.at("jointName").get<std::string>());
        track.getTranslationTrack().setInterpolationMode(static_cast<CADacomb::Animation::InterpolationMode>(trackJson.at("translationInterpolation").get<int>()));
        track.getRotationTrack().setInterpolationMode(static_cast<CADacomb::Animation::InterpolationMode>(trackJson.at("rotationInterpolation").get<int>()));
        track.getScaleTrack().setInterpolationMode(static_cast<CADacomb::Animation::InterpolationMode>(trackJson.at("scaleInterpolation").get<int>()));

        for (const auto& keyJson : trackJson.at("translationKeys"))
        {
            track.getTranslationTrack().addKeyframe(keyJson[0].get<double>(), glm::dvec3(keyJson[1].get<double>(), keyJson[2].get<double>(), keyJson[3].get<double>()));
        }
        for (const auto& keyJson : trackJson.at("rotationKeys"))
        {
            track.getRotationTrack().addKeyframe(keyJson[0].get<double>(), glm::dquat(keyJson[1].get<double>(), keyJson[2].get<double>(), keyJson[3].get<double>(), keyJson[4].get<double>()));
        }
        for (const auto& keyJson : trackJson.at("scaleKeys"))
        {
            track.getScaleTrack().addKeyframe(keyJson[0].get<double>(), glm::dvec3(keyJson[1].get<double>(), keyJson[2].get<double>(), keyJson[3].get<double>()));
        }

        loadedClip.addTrack(track);
    }

    clip = std::move(loadedClip);
    markModified();
}

const CADacomb::Animation::AnimationClip& AnimationComponent::getClip() const
{
    return clip;
}

CADacomb::Animation::AnimationClip& AnimationComponent::getClip()
{
    return clip;
}

void AnimationComponent::setClip(const CADacomb::Animation::AnimationClip& newClip)
{
    clip = newClip;
    markModified();
}

bool AnimationComponent::isLooping() const
{
    return looping;
}

void AnimationComponent::setLooping(bool newLooping)
{
    looping = newLooping;
    markModified();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("AnimationComponent/JsonRoundTrip")
{
    AnimationComponent component;
    CHECK(component.getType() == "Animation");
    CHECK(component.isCompatible("Triangulation"));
    CHECK_FALSE(component.isCompatible("Voxel"));

    CADacomb::Animation::AnimationClip srcClip("Walk");
    CADacomb::Animation::JointTrack    track("Hips");
    track.getTranslationTrack().addKeyframe(0.0, glm::dvec3(0.0, 1.0, 0.0));
    track.getTranslationTrack().addKeyframe(1.0, glm::dvec3(0.0, 2.0, 0.0));
    track.getRotationTrack().addKeyframe(0.0, glm::dquat(1.0, 0.0, 0.0, 0.0));
    track.getScaleTrack().addKeyframe(0.0, glm::dvec3(1.0, 1.0, 1.0));
    srcClip.addTrack(track);
    srcClip.recomputeDuration();

    component.setClip(srcClip);
    component.setLooping(false);

    nlohmann::json     serialized = component.toDataJson();
    AnimationComponent restored;
    restored.fromDataJson(serialized);

    CHECK_FALSE(restored.isLooping());
    CHECK(restored.getClip().getName() == "Walk");
    CHECK(restored.getClip().getDuration() == Catch::Approx(1.0));
    REQUIRE(restored.getClip().getTracks().size() == 1);
    CHECK(restored.getClip().getTracks()[0].getJointName() == "Hips");
    REQUIRE(restored.getClip().getTracks()[0].getTranslationTrack().getKeyframes().size() == 2);
    CHECK(restored.getClip().getTracks()[0].getTranslationTrack().getKeyframes()[1].getValue().y == Catch::Approx(2.0));
}
#endif
