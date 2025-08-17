/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Variant.h>
#include <LibWeb/Bindings/GPUAdapterInfoPrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/Forward.h>

namespace Web::WebGPU {

using GPUSize32 = u32;
using GPUSize32Out = GPUSize32;

using GPUIntegerCoordinate = u32;
using GPUIntegerCoordinateOut = GPUIntegerCoordinate;

struct GPUExtent3DDict {
    GPUIntegerCoordinate width;
    GPUIntegerCoordinate height = 1;
    GPUIntegerCoordinate depth_or_array_layers = 1;
};

// FIXME: using GPUExtent3D =  Variant<Vector<GPUIntegerCoordinate>, GPUExtent3DDict>;
using GPUExtent3D = GPUExtent3DDict;

struct GPUObjectDescriptorBase {
    String label;
};

class GPUObjectBaseMixin {
public:
    virtual ~GPUObjectBaseMixin();

    String const& label() const { return m_label; }

private:
    String m_label;
};

};
