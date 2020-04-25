#pragma once

const float PI = 3.14159265359;
const float PI_2 = PI / 2.0f;

template <typename T> inline constexpr T slerpi(T x, T b) {
  return (3 * x * x * b - 2 * x * x * x) / (b * b);
}

template <typename T> inline constexpr T fakesini(T x, T b) {
  const T nx = x & (b - 1);
  const T nb = b / 2;
  return (nx < nb) ? (slerpi<T>(nx, nb)) : (nb - 1 - slerpi<T>(nx - nb, nb));
}

template <class T> inline constexpr T abs(T v) { return v > T() ? v : -v; }
template <class T> inline constexpr T max(T a, T b) { return a > b ? a : b; }
template <class T> inline constexpr T min(T a, T b) { return a < b ? a : b; }
template <class T> inline constexpr T square(T x) { return x * x; }
template <class T> inline constexpr T lengthSq(T dx, T dy) {
  return square(dx) + square(dy);
}

float Q_rsqrt(float number) {
  long i;
  float x2, y;
  const float threehalfs = 1.5F;
  x2 = number * 0.5F;
  y = number;
  i = *(long *)&y;
  i = 0x5f3759df - (i >> 1);
  y = *(float *)&i;
  y = y * (threehalfs - (x2 * y * y));
  return y;
}

float ApproxAtan2(float y, float x) {
  const float n1 = 0.97239411f;
  const float n2 = -0.19194795f;
  float result = 0.0f;
  if (x != 0.0f) {
    const union {
      float flVal;
      unsigned int nVal;
    } tYSign = {y};
    const union {
      float flVal;
      unsigned int nVal;
    } tXSign = {x};
    if (abs(x) >= abs(y)) {
      union {
        float flVal;
        unsigned int nVal;
      } tOffset = {PI};
      tOffset.nVal |= tYSign.nVal & 0x80000000u;
      tOffset.nVal *= tXSign.nVal >> 31;
      result = tOffset.flVal;
      const float z = y / x;
      result += (n1 + n2 * z * z) * z;
    } else {
      union {
        float flVal;
        unsigned int nVal;
      } tOffset = {PI_2};
      tOffset.nVal |= tYSign.nVal & 0x80000000u;
      result = tOffset.flVal;
      const float z = x / y;
      result -= (n1 + n2 * z * z) * z;
    }
  } else if (y > 0.0f) {
    result = PI_2;
  } else if (y < 0.0f) {
    result = -PI_2;
  }
  return result;
}

template <class T> struct vec3 {
  T x{};
  T y{};
  T z{};
  vec3 &operator+=(const vec3<T> &o) {
    x += o.x;
    y += o.y;
    z += o.z;
    return *this;
  }
};

template <class VECT>
vec3<VECT> operator+(const vec3<VECT> &a, const vec3<VECT> &b) {
  return {a.x + b.x, a.y + b.y, a.z + b.z};
}
template <class VECT>
vec3<VECT> operator-(const vec3<VECT> &a, const vec3<VECT> &b) {
  return {a.x - b.x, a.y - b.y, a.z - b.z};
}
template <class VECT> vec3<VECT> operator*(VECT a, const vec3<VECT> &b) {
  return {a * b.x, a * b.y, a * b.z};
}
template <class VECT> vec3<VECT> operator/(const vec3<VECT> &a, VECT b) {
  return {a.x / b, a.y / b, a.z / b};
}
template <class VECT> VECT lengthsq(const vec3<VECT> &v) {
  return v.x * v.x + v.y * v.y + v.z * v.z;
}
vec3<float> unitizef(const vec3<float> &v) { return Q_rsqrt(lengthsq(v)) * v; }

template <class T> T cross3(const T &a, const T &b) {
  return {
      a.y * b.z - a.z * b.y,
      a.z * b.x - a.x * b.z,
      a.x * b.y - a.y * b.x,
  };
}
