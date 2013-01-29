/*
TERATHON SOFTWARE: 3D GRAPHICS LIBRARY
http://www.terathon.com/code/vector2d.html

Copyright � 2001�2008, Terathon Software LLC
*/

#include <math.h>

class Vector2D
{
    public:
    
        float   x;
        float   y;
        
        Vector2D() {}
        
        Vector2D(float x1, float y1){
            x = x1;
            y = y1;
        }

        Vector2D& Set(float x1, float y1){
            x = x1;
            y = y1;
            return (*this);
        }
        
        float& operator [](long k){
            return ((&x)[k]);
        }
        
        const float& operator [](long k) const{
            return ((&x)[k]);
        }
        
        Vector2D& operator +=(const Vector2D& v)
        {
            x += v.x;
            y += v.y;
            return (*this);
        }
        
        Vector2D& operator -=(const Vector2D& v)
        {
            x -= v.x;
            y -= v.y;
            return (*this);
        }
        
        Vector2D& operator *=(float t)
        {
            x *= t;
            y *= t;
            return (*this);
        }
        
        Vector2D& operator /=(float t)
        {
            float f = 1.0F / t;
            x *= f;
            y *= f;
            return (*this);
        }
        
        Vector2D& operator &=(const Vector2D& v)
        {
            x *= v.x;
            y *= v.y;
            return (*this);
        }
        
        Vector2D operator -(void) const
        {
            return (Vector2D(-x, -y));
        }
        
        Vector2D operator +(const Vector2D& v) const
        {
            return (Vector2D(x + v.x, y + v.y));
        }
        
        Vector2D operator -(const Vector2D& v) const
        {
            return (Vector2D(x - v.x, y - v.y));
        }
        
        Vector2D operator *(float t) const
        {
            return (Vector2D(x * t, y * t));
        }
        
        Vector2D operator /(float t) const
        {
            float f = 1.0F / t;
            return (Vector2D(x * f, y * f));
        }
        
        float operator *(const Vector2D& v) const
        {
            return (x * v.x + y * v.y);
        }
        
        Vector2D operator &(const Vector2D& v) const
        {
            return (Vector2D(x * v.x, y * v.y));
        }
        
        bool operator ==(const Vector2D& v) const
        {
            return ((x == v.x) && (y == v.y));
        }
        
        bool operator !=(const Vector2D& v) const
        {
            return ((x != v.x) || (y != v.y));
        }
        
        Vector2D& Normalize(void)
        {
            return (*this /= sqrtf(x * x + y * y));
        }
        
        Vector2D& Rotate(float angle);
};