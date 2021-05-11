# -*- coding: utf-8 -*-

import os


ROOT_DIR = os.path.abspath(os.path.dirname(__file__))
SOURCE_EXTENSIONS = ['.cpp', '.cc', '.c']

database = None
flags_general = [
    '-Wall',
    '-std=c++17',
]


def is_header_file(filename):
    ext = os.path.splitext(filename)[1]
    return ext in ['.h', '.hpp', '.hh']


def find_src_file(filename):
    if is_header_file(filename):
        basename = os.path.splitext(filename)[0]
        for ext in SOURCE_EXTENSIONS:
            target_file = basename + ext
            if os.path.exists(target):
                return target_file
    return filename 


def Settings(**kwargs):
    import ycm_core

    language = kwargs['language']
    if language == 'cfamily':
        filename = find_src_file(kwargs['filename'])

    return {
        'flags': flags_general,
        'include_paths_relative_to_dir': os.path.join(ROOT_DIR, 'src'),
        'do_cache': True
    }
