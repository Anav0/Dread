#ifndef Animator_H
#define Animator_H

#pragma once

#include <chrono>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <glm/ext/vector_float4.hpp>

#include "GLM.h" 
#include "Misc/Constants.h" 

class WindowManager;

using namespace std::chrono;

enum class RepetitionMode {
    Once,
    Infinite,
    nTimes,
};

enum class RepetitionDirection {
    StartOver,
    LoopBack,
};

template <typename T>
struct KeyFrame {
    T target_value;
    milliseconds duration;
		//not set by user
    steady_clock::time_point startpoint;
    steady_clock::time_point endpoint;
};

template <typename T>
using Keyframes = std::vector<KeyFrame<T>>;

class Animator {
    std::map<std::string, steady_clock::time_point> start_points;

    std::map<std::string, f32> base_float;
    std::map<std::string, v2>  base_vec2;
    std::map<std::string, v4>  base_vec4;

    std::map<std::string, bool> remembered_bool;
    std::map<std::string, v3>   remembered_vec3;

    std::map<std::string, int> prev_index;
    std::map<std::string, int> animation_counter;

		std::set<std::string>      running_animations;

    i32 repeat;
    RepetitionMode mode;
    RepetitionDirection direction;

    template <typename T>
    void RecalculateKeyframesStartAndEnd(std::string key, std::vector<KeyFrame<T>>& keyframes, int from_index);

    v3* RememberV3(std::string key, v3 from);
    steady_clock::time_point* RememberStartTime(WindowManager* window, std::string key, bool replace = false);

    template <typename T>
    bool SetupForRepeat(std::string& key, std::vector<KeyFrame<T>>& keyframes, int& current_keyframe_index, KeyFrame<T>*& current_keyframe);

    void AdjustRepeatCounter(std::string& key);

    template <typename T>
    f64 GetElapsedTime(steady_clock::time_point now, KeyFrame<T>* current_keyframe);

    template <typename T>
    KeyFrame<T>* GetCurrentKeyframe(int* current_keyframe_index, std::string key, std::vector<KeyFrame<T>>& keyframes, steady_clock::time_point* frame_start_point);

public:
    steady_clock::time_point GetAnimationStartTime(std::string key);

    void AnimateFloat(std::string key, WindowManager* mgr, f32* base, std::vector<KeyFrame<f32>>& keyframes);
    void AnimateVec2(std::string key, WindowManager* mgr, v2* base, std::vector<KeyFrame<v2>>& keyframes);
    bool AnimateVec3(WindowManager* window, std::string key, v3* from, v3 to, milliseconds duration_ms, bool forward = true);
    v4 AnimateVec4(std::string key, WindowManager* mgr, std::vector<KeyFrame<v4>>& keyframes);

    void Repeat(RepetitionMode mode, int n = 0);
    void Direction(RepetitionDirection direction);
};

extern Animator A;

#endif
