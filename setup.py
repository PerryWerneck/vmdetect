#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright 2016 The Meson development team

import sys

if sys.version_info < (3, 7):
    raise SystemExit('ERROR: Tried to install Meson with an unsupported Python version: \n{}'
                     '\nMeson requires Python 3.7.0 or greater'.format(sys.version))

from setuptools import setup

setup()

