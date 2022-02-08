
/*
 *  vpow~
 *
 *  vpow~ is a vectorised version of pow~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_binary.hpp"
#include "Base/nans.hpp"
#include <SIMDExtended.hpp>
#include <algorithm>

#ifdef __APPLE__
#include <alloca.h>
#else
#include <malloc.h>
#endif 

struct pow_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a, const SIMDType<float, 1> b)
    {
        return nan_fixer()(powf(b.mVal, a.mVal));
    }
    
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a, const SIMDType<double, 1> b)
    {
        return nan_fixer()(pow(b.mVal, a.mVal));
    }
 
    template <class T>
    void operator()(T *o, T *i1, T *i2, long size, double val, inputs type)
    {
        switch (type)
        {
            case inputs::scalar1:
            {
                T *t = reinterpret_cast<T *>(alloca(sizeof(T) * size));
                std::fill_n(t, size, static_cast<T>(val));
                pow_array(o, i1, t, size);
                break;
            }
                
            case inputs::scalar2:
            {
                T *t = reinterpret_cast<T *>(alloca(sizeof(T) * size));
                std::fill_n(t, size, static_cast<T>(val));
                pow_array(o, t, i2, size);
                break;
            }
                
            case inputs::binary:
                pow_array(o, i1, i2, size);
                break;
        }
        nan_fixer()(o, size);
    }
};

typedef v_binary<pow_functor, calculation_type::vector_array, calculation_type::vector_array> vpow;

int C74_EXPORT main()
{
    vpow::setup<vpow>("vpow~");
}
