#include <algorithm>

#include "Animator.h"
#include "WindowManager.h"

steady_clock::time_point Animator::GetAnimationStartTime(std::string key)
{
    if (start_points.contains(key))
        return start_points[key];

    auto now = steady_clock::now();
    start_points.insert(std::pair(key, now));

    return now;
}

void Animator::Repeat(RepetitionMode mode, int n)
{
    repeat = n;
    this->mode = mode;
}

void Animator::Direction(RepetitionDirection direction)
{
    this->direction = direction;
}

template <typename T>
KeyFrame<T>* Animator::GetCurrentKeyframe(int* current_keyframe_index, std::string key, std::vector<KeyFrame<T>>& keyframes, steady_clock::time_point* frame_start_point)
{
    for (int i = 0; i < keyframes.size(); i++) {
        auto keyframe = &keyframes.at(i);

        if (*frame_start_point <= keyframe->endpoint) {
            *current_keyframe_index = i;
            return keyframe;
        }
    }

    return nullptr;
}

template <typename T>
void Animator::RecalculateKeyframesStartAndEnd(std::string key, std::vector<KeyFrame<T>>& keyframes, int from_index)
{
    auto anim_start_point = GetAnimationStartTime(key);
    auto this_keyframe_end_point = anim_start_point;

    if (from_index > 0) {
        keyframes[0].startpoint = anim_start_point - 2000ms;
        keyframes[0].endpoint = anim_start_point - 1000ms;
    }

    for (int i = from_index; i < keyframes.size(); i++) {
        auto keyframe = &keyframes.at(i);

        this_keyframe_end_point += keyframe->duration;

        keyframe->endpoint = this_keyframe_end_point;

        if (i > from_index)
            keyframes[i].startpoint = keyframes[i - 1].endpoint;
        else
            keyframes[i].startpoint = anim_start_point;
    }
}

template <typename T>
f64 Animator::GetElapsedTime(steady_clock::time_point now, KeyFrame<T>* current_keyframe)
{
    auto end_time = current_keyframe->startpoint + current_keyframe->duration;

    auto end_time_ms = end_time.time_since_epoch().count();
    auto now_time_ms = now.time_since_epoch().count();
    auto keyframe_start_time_ms = current_keyframe->startpoint.time_since_epoch().count();

    f64 end_time_ms_double = (f64)end_time_ms;
    f64 now_time_ms_double = (f64)now_time_ms;
    f64 keyframe_start_time_ms_double = (f64)keyframe_start_time_ms;

    auto elapsed = (now_time_ms_double - keyframe_start_time_ms_double) / (end_time_ms_double - keyframe_start_time_ms_double);

    return std::clamp(elapsed, 0.0, 1.0);
}

v3* Animator::RememberV3(std::string key, v3 from)
{
    if (!remembered_vec3.contains(key))
        remembered_vec3.insert(std::pair(key, from));

    return &remembered_vec3[key];
}

steady_clock::time_point* Animator::RememberStartTime(WindowManager* window, std::string key, bool replace)
{
    if (!start_points.contains(key))
        start_points.insert(std::pair(key, window->frame_start_time));

    if (replace)
        start_points[key] = window->frame_start_time;

    return &start_points.at(key);
}

// TODO: string to ID
bool Animator::AnimateVec3(WindowManager* window, std::string key, v3* from, v3 to, milliseconds duration_ms, bool forward)
{
    auto to_key = key + "_target_value";
    auto from_key = key + "_initial_value";

    auto saved_from_value = RememberV3(from_key, *from);
    auto saved_to_value = RememberV3(to_key, to);

    if (!remembered_bool.contains(from_key)) {
        remembered_bool.insert(std::pair(from_key, forward));
    }

    bool changed_direction = remembered_bool[from_key] != forward;
    remembered_bool[from_key] = forward;

    const steady_clock::time_point* started = RememberStartTime(window, key, changed_direction);

    auto now = window->frame_start_time;
    auto diff = duration_cast<milliseconds>(now - *started);

    f64 progress = (static_cast<f64>(diff.count()) / duration_ms.count());

    if (progress > 1.0 || progress < 0.0)
        return false;

    if (!forward) {
        progress = 1.0 - progress;
    }

    from->x = std::lerp(saved_from_value->x, saved_to_value->x, progress);
    from->y = std::lerp(saved_from_value->y, saved_to_value->y, progress);
    from->z = std::lerp(saved_from_value->z, saved_to_value->z, progress);

    return true;
}

void Animator::AnimateVec2(std::string key, WindowManager* mgr, v2* final, std::vector<KeyFrame<v2>>& keyframes)
{
    AdjustRepeatCounter(key);

    auto& base_map = base_vec2;

    if (!base_map.contains(key)) {
        base_map.insert(std::pair(key, *final));
        RecalculateKeyframesStartAndEnd(key, keyframes, 0);
    }

    auto now = mgr->frame_start_time;
    int current_keyframe_index;
    auto current_keyframe = GetCurrentKeyframe(&current_keyframe_index, key, keyframes, &now);

    if (!current_keyframe) {
        if (SetupForRepeat(key, keyframes, current_keyframe_index, current_keyframe)) {
            base_vec2[key] = v2(*final);
        }
        return;
    }

    auto keyframe_startpoint = current_keyframe->startpoint;
    auto keyframe_endpoint = current_keyframe->endpoint;

    if (current_keyframe_index > 0) {
        auto prevIndex = prev_index[key];

        if (prevIndex != current_keyframe_index) {
            auto copy = v2(*final);
            base_map[key] = copy;
        }
    }

    auto source = &base_map[key];

    auto elapsed = GetElapsedTime(now, current_keyframe);

    if (elapsed >= 1.0)
        return;

    final->x = std::lerp(source->x, current_keyframe->target_value.x, elapsed);
    final->y = std::lerp(source->y, current_keyframe->target_value.y, elapsed);

    prev_index[key] = current_keyframe_index;
}

void Animator::AnimateVec4(std::string key, WindowManager* mgr, v4* final, std::vector<KeyFrame<v4>>& keyframes)
{
    AdjustRepeatCounter(key);

    auto& base_map = base_vec4;

    if (!base_map.contains(key)) {
        base_map.insert(std::pair(key, *final));
        RecalculateKeyframesStartAndEnd(key, keyframes, 0);
    }

    auto now = mgr->frame_start_time;
    int current_keyframe_index;
    auto current_keyframe = GetCurrentKeyframe(&current_keyframe_index, key, keyframes, &now);

    if (!current_keyframe) {
        if (SetupForRepeat(key, keyframes, current_keyframe_index, current_keyframe)) {
            base_map[key] = v4(*final);
        }
        return;
    }

    auto keyframe_startpoint = current_keyframe->startpoint;
    auto keyframe_endpoint = current_keyframe->endpoint;

    if (current_keyframe_index > 0) {
        auto prevIndex = prev_index[key];

        if (prevIndex != current_keyframe_index) {
            auto copy = v4(*final);
            base_map[key] = copy;
        }
    }

    auto source = &base_map[key];

    auto elapsed = GetElapsedTime(now, current_keyframe);

    if (elapsed >= 1.0)
        return;

    final->x = std::lerp(source->x, current_keyframe->target_value.x, elapsed);
    final->y = std::lerp(source->y, current_keyframe->target_value.y, elapsed);
    final->z = std::lerp(source->z, current_keyframe->target_value.z, elapsed);
    final->w = std::lerp(source->w, current_keyframe->target_value.w, elapsed);

    // printf("(%f) %i s: (%f, %f, %f) | f: (%f, %f, %f)\n", elapsed, current_keyframe_index, source->x, source->y, source->z, final->x, final->y, final->z);

    prev_index[key] = current_keyframe_index;
}

void Animator::AnimateFloat(std::string key, WindowManager* mgr, f32* final, std::vector<KeyFrame<f32>>& keyframes)
{
    AdjustRepeatCounter(key);

    auto& base_map = base_float;

    if (!base_map.contains(key)) {
        base_map.insert(std::pair(key, *final));
        RecalculateKeyframesStartAndEnd(key, keyframes, 0);
    }

    auto now = mgr->frame_start_time;
    int current_keyframe_index;
    auto current_keyframe = GetCurrentKeyframe(&current_keyframe_index, key, keyframes, &now);

    if (!current_keyframe) {
        if (SetupForRepeat(key, keyframes, current_keyframe_index, current_keyframe)) {
            base_map[key] = *final;
        }
        return;
    }

    auto keyframe_startpoint = current_keyframe->startpoint;
    auto keyframe_endpoint = current_keyframe->endpoint;

    if (current_keyframe_index > 0) {
        auto prevIndex = prev_index[key];

        if (prevIndex != current_keyframe_index) {
            base_map[key] = *final;
        }
    }

    auto source = &base_map[key];

    auto elapsed = GetElapsedTime(now, current_keyframe);

    if (elapsed >= 1.0)
        return;

    *final = std::lerp(*source, current_keyframe->target_value, elapsed);

    prev_index[key] = current_keyframe_index;
}

template <typename T>
bool Animator::SetupForRepeat(std::string& key, std::vector<KeyFrame<T>>& keyframes, int& current_keyframe_index, KeyFrame<T>*& current_keyframe)
{
    if (mode == RepetitionMode::Once)
        return false;

    if (mode == RepetitionMode::nTimes) {
        if (animation_counter[key] == 0)
            return false;

        animation_counter[key]--;
        return true;
    }

    if (direction == RepetitionDirection::LoopBack) {
        printf("LOOOOOOP\n");
        std::reverse(keyframes.begin(), keyframes.end());
        start_points.erase(key);
        RecalculateKeyframesStartAndEnd(key, keyframes, 1);
        current_keyframe_index = 1;
        if (keyframes.size() < 2)
            current_keyframe_index = 0;
        current_keyframe = &keyframes.at(current_keyframe_index);
        prev_index[key] = current_keyframe_index;
        return true;
    }

    return false;
}

void Animator::AdjustRepeatCounter(std::string& key)
{
    if (mode == RepetitionMode::nTimes && !animation_counter.contains(key)) {
        if (mode == RepetitionMode::nTimes) {
            animation_counter[key] = repeat * 2;
        } else {
            animation_counter[key] = repeat;
        }
    }
}

Animator A;
