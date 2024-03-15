#ifndef Animator_H
#define Animator_H

#pragma once

#include <chrono>
#include <cmath>
#include <map>
#include <string>
#include <vector>

#include <glm/ext/vector_float4.hpp>

#include "Constants.h" 

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
    steady_clock::time_point startpoint;
    steady_clock::time_point endpoint;
};

class Animator {
    std::map<std::string, steady_clock::time_point> start_points;

    std::map<std::string, f32> base_float;
    std::map<std::string, v2>  base_vec2;
    std::map<std::string, v4>  base_vec4;

    std::map<std::string, bool> remembered_bool;
    std::map<std::string, v3>   remembered_vec3;

    std::map<std::string, int> prev_index;
    std::map<std::string, int> animation_counter;

    i32 repeat;
    RepetitionMode mode;
    RepetitionDirection direction;

public:
    steady_clock::time_point GetAnimationStartTime(std::string key);

    void AnimateFloat(std::string key, WindowManager* mgr, f32* base, std::vector<KeyFrame<f32>>& keyframes);
    void AnimateVec2(std::string key, WindowManager* mgr, v2* base, std::vector<KeyFrame<v2>>& keyframes);
    bool AnimateVec3(WindowManager* window, std::string key, v3* from, v3 to, milliseconds duration_ms, bool forward = true);
    void AnimateVec4(std::string key, WindowManager* mgr, v4* base, std::vector<KeyFrame<v4>>& keyframes);

    v3* RememberV3(std::string key, v3 from);
    steady_clock::time_point* RememberStartTime(WindowManager* window, std::string key, bool replace = false);

    template <typename T>
    bool SetupForRepeat(std::string& key, std::vector<KeyFrame<T>>& keyframes, int& current_keyframe_index, KeyFrame<T>*& current_keyframe);

    void AdjustRepeatCounter(std::string& key);

    template <typename T>
    f64 GetElapsedTime(steady_clock::time_point now, KeyFrame<T>* current_keyframe);

    void Repeat(RepetitionMode mode, int n = 0);
    void Direction(RepetitionDirection direction);

    template <typename T>
    void RecalculateKeyframesStartAndEnd(std::string key, std::vector<KeyFrame<T>>& keyframes, int from_index);

    template <typename T>
    KeyFrame<T>* GetCurrentKeyframe(int* current_keyframe_index, std::string key, std::vector<KeyFrame<T>>& keyframes, steady_clock::time_point* frame_start_point);
};

extern Animator A;

#endif
