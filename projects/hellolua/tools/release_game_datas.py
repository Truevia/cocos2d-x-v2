#!/usr/bin/env python3
# coding=utf-8

import os
import plistlib
import hashlib
import base64

# self
from rrcore import *
from safe_file_id_base62 import short_id_from_path

CUR_DIR = normalpath(os.path.dirname(os.path.abspath(__file__)))
RESOURCES_DIR = normalpath(os.path.join(CUR_DIR, '..', 'Resources'))
GDATAS_DIR = normalpath(os.path.join(RESOURCES_DIR, 'gdatas'))
LOOKUP_FILE_NAME = 'lookup'

def release_one_dir(from_folder, to_folder):
    files = scan(from_folder, excludes=['.DS_Store'])
    metadata = {}
    for file in files:
        base_path = file.replace(from_folder + '/', '')
        md5 = md5str(base_path)
        md5 = short_id_from_path(base_path, bits=64)
        folder_name = md5[2].lower() + md5[5].lower() # 特别注意这里, 必须小写或者大写, 不能混合
        new_folder = os.path.join(to_folder, folder_name)
        new_path = os.path.join(new_folder, md5)
        mkdirs(new_folder)
        # new_path = os.path.join(to_folder, md5)
        shutil.copy(file, new_path)
        k = base_path
        v = folder_name + '/' + md5
        # v = md5
        metadata[k] = v
        print(k,v)
    return metadata

def release_game():
    print('release_game')
    print(RESOURCES_DIR)
    rmdir(GDATAS_DIR)
    mkdirs(GDATAS_DIR)

    src_dir = unixpath(os.path.join(RESOURCES_DIR, 'src'))
    res_dir = unixpath(os.path.join(RESOURCES_DIR, 'res'))
    src_metadata = release_one_dir(src_dir, GDATAS_DIR)
    res_metadata = release_one_dir(res_dir, GDATAS_DIR)
    files = merge_dicts(src_metadata, res_metadata)

    plist = {}
    plist['metadata'] = {'version':1}
    plist['filenames'] = files
    output = os.path.join(GDATAS_DIR, LOOKUP_FILE_NAME)
    print(f'>output: {output}')
    with open(output, 'wb') as fs:
        plistlib.dump(plist, fs, sort_keys=True)

if __name__ == '__main__':
    release_game()
