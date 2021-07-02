#pragma once

#include <memory>
#include <glad/glad.h>  // must put before glfw
#include <GLFW/glfw3.h>

#include <eigen3/Eigen/eigen>
#include <random>
#include <iostream>
#include <unordered_map>

namespace psi {

template <typename T>
using Ptr = std::shared_ptr<T>;

template <typename T, typename... Args>
inline Ptr<T> New(Args&&... args){
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T, typename U>
inline Ptr<T> Cast(const Ptr<U>& arg) {
    return std::reinterpret_pointer_cast<T>(arg);
}

template <typename T, typename U>
using Hashmap = std::unordered_map<T, U>;

using Vec2i = Eigen::Vector2i;
using Vec3i = Eigen::Vector3i;
using Vec2f = Eigen::Vector2f;
using Vec3f = Eigen::Vector3f;
using Vec4f = Eigen::Vector4f;
using Mat3f = Eigen::Matrix3f;
using Mat4f = Eigen::Matrix4f;

constexpr float kEpsilon = 0.001f;
constexpr float kInf = std::numeric_limits<float>::max();
constexpr float kPi = 3.1415926535898f;

inline float GetRandomFloat() {
    static std::random_device dev;
    static std::default_random_engine rng(dev());
    static std::uniform_real_distribution<float> dist(0.f, 1.f);

    return dist(rng);
}

inline float deg2rad(float deg) {
    return deg / 180 * kPi;
}

}