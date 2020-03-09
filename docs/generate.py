#!/usr/bin/env python3

import clang.cindex
import glob
import json
from jinja2 import Template
from jinja2 import Environment, FileSystemLoader
import os
import html

def md(text, *args, **kwargs): 
    from markdown import markdown
    return markdown(text, *args, **kwargs)


def ensure_dir(file_path):
    directory = os.path.dirname(file_path)
    if not os.path.exists(directory):
        os.makedirs(directory)

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

def process_raw_comment(raw_comment):
    if raw_comment == None: return None
    result = []
    lines = raw_comment.split("\n")
    for line_num, line in enumerate(lines):
        if line_num == 0 or line_num == len(lines) - 1:
            line = line.strip(' /')
        result.append(line.strip().strip('*').strip())
    return "\n".join(result)

def find_base_classes(node):
    lst = []
    def find_base_classes_(node):
        if node.kind == clang.cindex.CursorKind.CXX_BASE_SPECIFIER:
            lst.append(node.type.get_declaration())
        for child in node.get_children():
            find_base_classes_(child)
        return lst
    find_base_classes_(node)
    return lst

def find_methods(node, filename):
    if node.location.file:
        if node.location.file.name == filename:
            if node.kind == clang.cindex.CursorKind.CXX_METHOD:
                if node.access_specifier != clang.cindex.AccessSpecifier.PRIVATE:
                    if node.is_static_method():
                        classes[fully_qualified(node.semantic_parent)]['static_methods'].append({
                            'displayname': node.type.get_result().spelling + " " + node.displayname,
                            'comment': process_raw_comment(node.raw_comment),
                        })
                    elif node.spelling.startswith('operator'):
                        classes[fully_qualified(node.semantic_parent)]['operators'].append({
                            'displayname': node.type.get_result().spelling + " " + node.displayname,
                            'comment': process_raw_comment(node.raw_comment),
                        })
                    else:
                        classes[fully_qualified(node.semantic_parent)]['methods'].append({
                            'displayname': node.type.get_result().spelling + " " + node.displayname,
                            'comment': process_raw_comment(node.raw_comment),
                        })
            elif node.kind == clang.cindex.CursorKind.CLASS_DECL:
                if node.access_specifier != clang.cindex.AccessSpecifier.PRIVATE:
                    classes[fully_qualified(node)] = {
                        'displayname': node.displayname,
                        'comment': process_raw_comment(node.raw_comment),
                        'base_classes': list(map(fully_qualified, find_base_classes(node))),
                        'constructors': [],
                        'static_methods': [],
                        'methods': [],
                        'operators': []
                    }
            elif node.kind == clang.cindex.CursorKind.CLASS_TEMPLATE:
                if node.access_specifier != clang.cindex.AccessSpecifier.PRIVATE:
                    classes[fully_qualified(node)] = {
                        'displayname': node.displayname,
                        'comment': process_raw_comment(node.raw_comment),
                        'base_classes': list(map(fully_qualified, find_base_classes(node))),
                        'constructors': [],
                        'static_methods': [],
                        'methods': [],
                        'operators': []
                    }
            elif node.kind == clang.cindex.CursorKind.STRUCT_DECL:
                if node.access_specifier != clang.cindex.AccessSpecifier.PRIVATE:
                    classes[fully_qualified(node)] = {
                        'displayname': node.displayname,
                        'comment': process_raw_comment(node.raw_comment),
                        'base_classes': list(map(fully_qualified, find_base_classes(node))),
                        'constructors': [],
                        'static_methods': [],
                        'methods': [],
                        'operators': []
                    }
            elif node.kind == clang.cindex.CursorKind.FUNCTION_DECL:
                return_type = node.type.get_result().spelling
                name = node.displayname
                comment = node.raw_comment
            elif node.kind == clang.cindex.CursorKind.CONSTRUCTOR:
                if node.access_specifier != clang.cindex.AccessSpecifier.PRIVATE:
                    classes[fully_qualified(node.semantic_parent)]['constructors'].append({
                        'displayname': node.type.get_result().spelling + " " + node.displayname,
                        'comment': process_raw_comment(node.raw_comment),
                    })

    # Recurse for children of this node`
    for child in node.get_children():
        find_methods(child, filename)
    

if __name__ == "__main__":
    #clang.cindex.Config.set_library_file('/usr/local/Cellar/llvm/9.0.1/lib/libclang.dylib')
    index = clang.cindex.Index.create()
    for file in glob.glob('./include/*'):
        print(file)
        
        include_path_macos = [
            '-Iinclude',
            '-I/Library/Developer/CommandLineTools/usr/include/',
            '-I/Library/Developer/CommandLineTools/usr/include/c++/v1/',
            '-I/usr/local/Cellar/llvm/9.0.1/lib/clang/9.0.1/include/'
        ]

        include_path_linux = [
            '-Iinclude',
            '-I/usr/lib/clang/9.0.1/include',
        ]

        include_path = include_path_linux
        translation_unit = index.parse(file, args=['-std=c++17', *include_path, '-x', 'c++'], options=clang.cindex.TranslationUnit.PARSE_SKIP_FUNCTION_BODIES)

        if (len(list(translation_unit.diagnostics)) > 0):
            for error in list(translation_unit.diagnostics):
                print(error)
        else:
            filename = translation_unit.spelling
            find_methods(translation_unit.cursor, filename)


    file_loader = FileSystemLoader('docs/templates')
    env = Environment(loader=file_loader)
    env.filters['markdown'] = md

    links = list(map(lambda class_name: {
        'spelling': "::".join(class_name.split("::")[1:]),
        'href': f'/{"/".join(class_name.split("::"))}.html'
    }, classes.keys()))

    for class_name in classes.keys():
        html_name = f'docs/{"/".join(class_name.split("::")[1:])}.html'
        ensure_dir(html_name)
        with open(html_name,'w') as file:
            template = env.get_template('class.html')
            output = template.render(current_class=classes[class_name], links=links)
            file.write(output)