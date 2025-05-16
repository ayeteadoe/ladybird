/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <UI/Qt/WebGPUTestBed/WebGPUTabWidget.h>

#include <UI/Qt/WebGPUTestBed/WebGPUWindow.h>

namespace Ladybird::WebGPUTestBed {

WebGPUTabWidget::WebGPUTabWidget(QWidget* parent)
    : QTabWidget(parent)
{
    auto triangle = new WebGPUWindow();
    addTab(QWidget::createWindowContainer(triangle, this),
        "Triangle");
}

}
