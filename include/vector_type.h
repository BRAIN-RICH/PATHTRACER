#ifndef RAYTRACER_VECTORS_H
#define RAYTRACER_VECTORS_H

#include "math_utils.h"

#include <cmath>
#include <iostream>
#include <utility>
//后续函数为匹配数据类型的各种运算符重写


namespace scg
{

template<int DIM, typename T>
class Vector
{
public:
    union
    {
        // TODO: check order
        struct//坐标
        {
            T x;
            T y;
            T z;
            T w;
        };
        struct//颜色
        {
            T r;
            T g;
            T b;
            T a;
        };
        struct
        {
            T u;
            T v;
        };
        T data[DIM];//维度
    };

    Vector()//默认构造
    {
        for (int i = 0; i < DIM; ++i)
        {
            this->data[i] = 0;
        }
    };

    Vector(Vector const& arg) = default;

    template<typename V>
    Vector(V arg)//有参构造模板
    {
        for (int i = 0; i < DIM; ++i)
        {
            this->data[i] = static_cast<T>(arg);//强制转化T至data[i]的数据类型
        }
    }

	//参数包typename... Args，
    template <typename... Args>
    /*explicit*/ Vector(Args... args) : data{ T(args)... } 
	{
        static_assert(sizeof...(Args) == DIM, "Wrong number of arguments.");//确保arguments和维度尺寸一样
    }

	//内联函数
    inline T length() const
    {
        T length = 0;
        for (int i = 0; i < DIM; ++i)
        {
            length += this->data[i] * this->data[i];
        }
        return std::sqrt(length);//返回数据的均方根
    }

	//重写<<，输出数据
    friend std::ostream& operator<<(std::ostream& out, Vector const& arg)
    {
        out << "Vector{ ";
        for (int i = 0; i < DIM; ++i)
        {
            out << arg.data[i] << " ";
        }
        out << "}";
        return out;
    }

    //inline T operator [](int i) const { return this->data[i]; }
    //inline T& operator [](int i) { return this->data[i]; }



    // Unary operators
	//单元运算符，后加const表示函数为只读，重写-。
    inline Vector operator -() const
    {
        Vector result;
        for (int i = 0; i < DIM; ++i)
        {
            result.data[i] = -this->data[i];
        }
        return result;//输出数据负值
    }

    // Binary operators
	//二进制运算符，重写+
    template<typename V>
    inline Vector operator +(V const& arg) const
    {
        Vector result;
        for (int i = 0; i < DIM; ++i)
        {
            result.data[i] = (this->data[i] + static_cast<T>(arg));
        }
        return result;
    }

	//
    template<typename V>
    inline Vector operator +(Vector<DIM, V> const& arg) const
    {
        Vector result;
        for (int i = 0; i < DIM; ++i)
        {
            result.data[i] = (this->data[i] + static_cast<T>(arg.data[i]));
        }
        return result;
    }

    template<typename V>
    inline Vector operator -(V const& arg) const
    {
        Vector result;
        for (int i = 0; i < DIM; ++i)
        {
            result.data[i] = (this->data[i] - static_cast<T>(arg));
        }
        return result;
    }

    template<typename V>
    inline Vector operator -(Vector<DIM, V> const& arg) const
    {
        Vector result;
        for (int i = 0; i < DIM; ++i)
        {
            result.data[i] = (this->data[i] - static_cast<T>(arg.data[i]));
        }
        return result;
    }

    template<typename V>
    inline Vector operator *(V const& arg) const
    {
        Vector result;
        for (int i = 0; i < DIM; ++i)
        {
            result.data[i] = (this->data[i] * static_cast<T>(arg));
        }
        return result;
    }

    template<typename V>
    inline Vector operator *(Vector<DIM, V> const& arg) const
    {
        Vector result;
        for (int i = 0; i < DIM; ++i)
        {
            result.data[i] = (this->data[i] * static_cast<T>(arg.data[i]));
        }
        return result;
    }

    template<typename V>
    inline Vector operator /(V const& arg) const
    {
        Vector result;
        for (int i = 0; i < DIM; ++i)
        {
            result.data[i] = (this->data[i] / static_cast<T>(arg));
        }
        return result;
    }

    template<typename V>
    inline Vector operator /(Vector<DIM, V> const& arg) const
    {
        Vector result;
        for (int i = 0; i < DIM; ++i)
        {
            result.data[i] = (this->data[i] / static_cast<T>(arg.data[i]));
        }
        return result;
    }

    template<typename V>
    inline Vector& operator =(V const& arg)
    {
        for (int i = 0; i < DIM; ++i)
        {
            this->data[i] = static_cast<T>(arg.data[i]);
        }
        return *this;
    }

    template<typename V>
    inline Vector& operator +=(V const& arg)
    {
        for (int i = 0; i < DIM; ++i)
        {
            this->data[i] += static_cast<T>(arg);
        }
        return *this;
    }

    template<typename V>
    inline Vector& operator +=(Vector<DIM, V> const& arg)
    {
        for (int i = 0; i < DIM; ++i)
        {
            this->data[i] += static_cast<T>(arg.data[i]);
        }
        return *this;
    }

    template<typename V>
    inline Vector operator -=(V const& arg)
    {
        for (int i = 0; i < DIM; ++i)
        {
            this->data[i] -= static_cast<T>(arg);
        }
        return *this;
    }

    template<typename V>
    inline Vector operator -=(Vector<DIM, V> const& arg)
    {
        for (int i = 0; i < DIM; ++i)
        {
            this->data[i] -= static_cast<T>(arg.data[i]);
        }
        return *this;
    }

    template<typename V>
    inline Vector operator *=(V const& arg)
    {
        for (int i = 0; i < DIM; ++i)
        {
            this->data[i] *= static_cast<T>(arg);
        }
        return *this;
    }

    template<typename V>
    inline Vector operator *=(Vector<DIM, V> const& arg)
    {
        for (int i = 0; i < DIM; ++i)
        {
            this->data[i] *= static_cast<T>(arg.data[i]);
        }
        return *this;
    }

    template<typename V>
    inline Vector operator /=(V const& arg)
    {
        for (int i = 0; i < DIM; ++i)
        {
            this->data[i] /= static_cast<T>(arg);
        }
        return *this;
    }

    template<typename V>
    inline Vector operator /=(Vector<DIM, V> const& arg)
    {
        for (int i = 0; i < DIM; ++i)
        {
            this->data[i] /= static_cast<T>(arg.data[i]);
        }
        return *this;
    }
};

//对数据进行各种运算及旋转

// Useful types
//给已知vector起个新名字
typedef Vector<2, int> Vec2i;
typedef Vector<2, float> Vec2f;
typedef Vector<3, float> Vec3f;//三维向量，用来实现关于三维向量的各种操作
typedef Vector<4, float> Vec4f;

// Vector methods
template<int DIM, typename T>
inline Vector<DIM, T> normalise(Vector<DIM, T> const& a)
{
    Vector<DIM, T> result;
    T length = a.length();
    for (int i = 0; i < DIM; ++i)
    {
        result.data[i] = a.data[i] / length;
    }
    return result;
}

template<typename T>
inline Vector<3, T> cross(Vector<3, T> const& a, Vector<3, T> const& b)
{
    return Vector<3, T>{
        a.y * b.z - b.y * a.z,
        a.z * b.x - b.z * a.x,
        a.x * b.y - b.x * a.y
    };
}

template<int DIM, typename T>
inline T dot(Vector<DIM, T> const& a, Vector<DIM, T> const& b)
{
    T result = 0;
    for (int i = 0; i < DIM; ++i)
    {
        result += a.data[i] * b.data[i];
    }
    return result;
}

template<int DIM, typename T>
inline Vector<DIM, T> multiply(Vector<DIM, T> const& a, Vector<DIM, T> const& b)
{
    Vector<DIM, T> result;
    for (int i = 0; i < DIM; ++i)
    {
        result.data[i] = a.data[i] * b.data[i];
    }
    return result;
}

template<typename T>
inline Vector<3, T> rotateX(Vector<3, T> const& a, float radians)
{
    float sin = std::sin(radians);
    float cos = std::cos(radians);

    return Vector<3, T>{
        a.x,
        a.y * cos - a.z * sin,
        a.y * sin + a.z * cos
    };
}

template<typename T>
inline Vector<3, T> rotateY(Vector<3, T> const& a, float radians)
{
    float sin = std::sin(radians);
    float cos = std::cos(radians);

    return Vector<3, T>{
        a.x * cos + a.z * sin,
        a.y,
        -a.x * sin + a.z * cos
    };
}

template<typename T>
inline Vector<3, T> rotateZ(Vector<3, T> const& a, float radians)
{
    float sin = std::sin(radians);
    float cos = std::cos(radians);

    return Vector<3, T>(
        a.x * cos - a.y * sin,
        a.x * sin + a.y * cos,
        a.z
    );
}

template<typename T>
inline Vector<3, T> rotate(Vector<3, T> const& a, Vector<3, T> const& rot)
{
    return rotateZ(rotateY(rotateX(a, toRadians(rot.x)), toRadians(rot.y)), toRadians(rot.z));
}

inline Vec3f minV(Vec3f const& v1, Vec3f const& v2)
{
    return Vec3f(
        fminf(v1.x, v2.x),
        fminf(v1.y, v2.y),
        fminf(v1.z, v2.z)
    );
}

inline Vec3f maxV(Vec3f const& v1, Vec3f const& v2)
{
    return Vec3f(
        fmaxf(v1.x, v2.x),
        fmaxf(v1.y, v2.y),
        fmaxf(v1.z, v2.z)
    );
}

}

#endif //RAYTRACER_VECTORS_H
