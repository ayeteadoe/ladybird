/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibTest/TestCase.h>
#include <LibWebGPUNative/Instance.h>

// FIXME: Complete enough of the implementation to test a "clear value" render pass into to a Gfx::Bitmap for headless/offscreen verification
TEST_CASE(create_instance)
{
    EXPECT_NO_DEATH("Create Instance", [&] {
        WebGPUNative::Instance instance;
        MUST(instance.initialize());
    }());
}
