###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: EnhancedPositionService
#
# Author: Thomas Bader
#
# Copyright (C) 2012, BMW Car IT GmbH
# 
# License:
# This Source Code Form is subject to the terms of the
# Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
# this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# @licence end@
###########################################################################

if (WITH_TESTS)

add_subdirectory(gtest-1.6.0)
#include_directories(${gtest_SOURCE_DIR}/include)

endif(WITH_TESTS)
