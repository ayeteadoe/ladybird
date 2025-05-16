/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <QTableWidget>

namespace Ladybird::WebGPUTestBed {

class WebGPUTabWidget : public QTabWidget {
    Q_OBJECT
public:
    explicit WebGPUTabWidget(QWidget* parent = nullptr);
};

}
