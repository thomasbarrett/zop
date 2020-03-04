#!/usr/bin/env python3

import clang.cindex
import glob
import json

def fully_qualified(c):
    if c is None:
        return ''
    elif c.kind == clang.cindex.CursorKind.TRANSLATION_UNIT:
        return ''
    else:
        res = fully_qualified(c.semantic_parent)
        if res != '':
            return res + '::' + c.spelling
    return c.spelling

classes = {}

def find_methods(node, filename):
    if node.location.file:
        if node.location.file.name == filename:
            if node.kind == clang.cindex.CursorKind.CXX_METHOD:
                classes[fully_qualified(node.semantic_parent)]['methods'].append({
                    'displayname': node.type.get_result().spelling + " " + node.displayname,
                    'brief_comment': node.brief_comment,
                })
            elif node.kind == clang.cindex.CursorKind.CLASS_DECL:
                classes[fully_qualified(node)] = {
                    'displayname': node.displayname,
                    'brief_comment': node.brief_comment,
                    'methods': []
                }
            elif node.kind == clang.cindex.CursorKind.STRUCT_DECL:
                classes[fully_qualified(node)] = {
                    'displayname': node.displayname,
                    'brief_comment': node.brief_comment,
                    'methods': []
                }
            elif node.kind == clang.cindex.CursorKind.FUNCTION_DECL:
                return_type = node.type.get_result().spelling
                name = node.displayname
                comment = node.brief_comment
            elif node.kind == clang.cindex.CursorKind.CONSTRUCTOR:
                classes[fully_qualified(node.semantic_parent)]['methods'].append({
                    'displayname': node.type.get_result().spelling + " " + node.displayname,
                    'brief_comment': node.brief_comment,
                })

    # Recurse for children of this node`
    for child in node.get_children():
        find_methods(child, filename)
    

if __name__ == "__main__":
    index = clang.cindex.Index.create()
    for file in glob.glob('./include/*'):
        classes = {}
        print(file)
        translation_unit = index.parse(file, args=['-std=c++17', '-Iinclude', '-I/usr/lib/clang/9.0.1/include', '-x', 'c++'])
        if (len(list(translation_unit.diagnostics)) > 0):
            for error in list(translation_unit.diagnostics):
                print(error)
        else:
            filename = translation_unit.spelling
            find_methods(translation_unit.cursor, filename)
            print(json.dumps(classes))