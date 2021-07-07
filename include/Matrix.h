#pragma once

#include "global.h"

namespace psi {

class Matrix {
public:
    static Mat4f Rotation(const Vec3f& axis, float angle) {

        float c = cos(angle * kPi / 180);  // cos theta
        float s = sin(angle * kPi / 180);  // sin theta
        Vec3f k = axis.normalized(); // normalized axis

        Mat3f mat1 = Mat3f::Identity() * c;
        Mat3f mat2;
        mat2 << k[0] * k[0], k[0] * k[1], k[0] * k[2],
            k[1] * k[0], k[1] * k[1], k[1] * k[2],
            k[2] * k[0], k[2] * k[1], k[2] * k[2];
        mat2 *= 1 - c;
        Mat3f mat3;
        mat3 << 0, -k[2], k[1],
            k[2], 0, -k[0],
            -k[1], k[0], 0;
        mat3 *= s;

        Mat4f rotation = Mat4f::Identity();
        rotation.block(0, 0, 3, 3) << mat1 + mat2 + mat3;

        return rotation;
    }

    static Mat4f Projection(float eye_fov, float aspect_ratio,
        float zNear, float zFar)
    {
        float h_n = tan(eye_fov * kPi / 360);
        float w_n = aspect_ratio * h_n;

        Mat4f projection = Mat4f::Zero();
        projection(0, 0) = 1.f / w_n;
        projection(1, 1) = 1.f / h_n;
        projection(2, 2) = (zFar + zNear) / (zNear - zFar);
        projection(2, 3) = 2 * zFar * zNear / (zNear - zFar);
        projection(3, 2) = -1.f;

        return projection;
    }

    static Mat4f Ortho(float l, float r, float b, float t, float n, float f) {
        Mat4f ortho = Mat4f::Identity();
        ortho << 2 / (r - l), 0, 0, (r + l) / (l - r),
            0, 2 / (t - b), 0, (b + t) / (b - t),
            0, 0, 2 / (n - f), (f + n) / (n - f),
            0, 0, 0, 1;
        return ortho;
    }

    static Mat4f Translation(float x, float y, float z) {
        Mat4f translation;
        translation << 1, 0, 0, x, 0, 1, 0, y, 0, 0, 1,
            z, 0, 0, 0, 1;

        return translation;
    }
    static Mat4f Translation(const Vec3f& dir){
        return Translation(dir.x(), dir.y(), dir.z());
    }

    static Mat4f Scale(float x, float y, float z) {
        Mat4f scale = Mat4f::Identity();
        scale(0, 0) = z;
        scale(1, 1) = y;
        scale(2, 2) = z;
        return scale;
    }
    static Mat4f Scale(float coef) {
        return Scale(coef, coef, coef);
    }
    static Mat4f Scale(const Vec3f& coef) {
        return Scale(coef.x(), coef.y(), coef.z());
    }

    static Mat4f LookAt(const Vec3f& location, const Vec3f& target, const Vec3f& up) {
        Vec3f w = (location-target).normalized();
        Vec3f u = up.cross(w).normalized();
        Vec3f v = w.cross(u).normalized();

        Mat4f lookAt = Mat4f::Identity();
        lookAt.block(0, 0, 3, 3) <<
            u.x(), u.y(), u.z(),
            v.x(), v.y(), v.z(),
            w.x(), w.y(), w.z();
        lookAt *= Translation(-location);

        return lookAt;
    }


};





}