"""Needed by YouCompleteMe
"""
# Copyright (C) 2014 Google Inc.
#
# This file is part of ycmd.
#
# ycmd is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# ycmd is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with ycmd.  If not, see <http://www.gnu.org/licenses/>.
import os
import ycm_core

# These are the compilation flags that will be used in case there's no
# compilation database set (by default, one is not set).
# CHANGE THIS LIST OF FLAGS. YES, THIS IS THE DROID YOU HAVE BEEN LOOKING FOR.
flags = [
    '-I' , '/Users/matthew.hoggan/Devel/gitlab/mehoggan/dag_scheduler/stages_lib/include',
    '-I', '/Users/matthew.hoggan/Devel/gitlab/mehoggan/dag_scheduler/test/utils',
    '-I', '/Users/matthew.hoggan/Devel/gitlab/mehoggan/dag_scheduler/include',
    '-I', '/Library/Developer/CommandLineTools/SDKs/MacOSX15.2.sdk/usr/include/c++/v1',
    '-I', '/usr/local/Cellar/boost@1.85/1.85.0_2/include',
    '-I', '/Library/Developer/CommandLineTools/SDKs/MacOSX15.2.sdk/usr/include',
    '-isysroot', '/Library/Developer/CommandLineTools/SDKs/MacOSX15.2.sdk/usr/include/c++/v1',
    '-isystem', '/Library/Developer/CommandLineTools/SDKs/MacOSX15.2.sdk/usr/include/c++/v1',
    '-isysroot', '/Library/Developer/CommandLineTools/SDKs/MacOSX15.2.sdk/usr/include',
    '-isystem', '/Library/Developer/CommandLineTools/SDKs/MacOSX15.2.sdk/usr/include',
    '-x',
    'c++',
    '-Wall',
    '-std=c++17',
    '-std=gnu++17'
]


def DirectoryOfThisScript():
    """Helper method used by `Settings`
    """
    return os.path.dirname(os.path.abspath(__file__))


def Settings(** kwargs):
    """YCM API
    """
    return {
        'flags': flags,
        'include_paths_relative_to_dir': DirectoryOfThisScript()
    }
