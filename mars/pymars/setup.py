# coding=utf-8
import os
from setuptools import setup, find_packages

""" reference: http://kuanghy.github.io/2018/04/29/setup-dot-py
name: 包名称
version: 包版本
author: 程序的作者
author_email: 程序的作者的邮箱地址
url: 程序的官网地址
license: 程序的授权信息
description: 程序的简单描述
long_description: 程序的详细描述
long_description_content_type: 详情描述的文本格式
platforms: 程序适用的软件平台列表
keywords: 程序的关键字列表
packages: 需要处理的包目录
python_requires: python依懒版本
include_package_data: 自动包含包内所有受版本控制(cvs/svn/git)的数据文件
install_requires: 安装时需要安装的依赖包
entry_points: 用来支持自动生成cli命令
"""

# 把 README.md 里面的信息放到 long_description, 注意把 long_description_content_type 改成 text/markdown
path = os.path.abspath(os.path.dirname(__file__))
try:
    with open(os.path.join(path, 'README.md')) as f:
        long_description = f.read()
except Exception as e:
    long_description = "common libs for web server, image process and so on"

setup(
    name="pymars",
    version='0.0.1.alpha1.1',
    keywords=["pip", "flask", "image process", "ai"],
    description="common libs for web server, image process and so on",
    long_description=long_description,
    long_description_content_type='text/markdown',

    package_dir={"": "src"},
    packages=find_packages(where='src', exclude=(), include=('*',)),
    # 添加静态文件的方法
    #   1. include_package_data = True 以及 MANIFEST.in
    #   2. package_data={ "": ["README.md", "*.json", "*.pth"], }
    # 注：include_package_data 会使 package_data 信息无效，所以二者不要同时使用
    include_package_data=True,
    install_requires=["requests", "click", "flask>=1.1.2", "flask-cors>=3.0.10"],
    python_requires=">=3.5.0",
    platforms="any",

    url="https://github.com/joyous-x/solar/mars",
    author="joyous-x",
    author_email="",
    license="MIT Licence",

    scripts=[],
    entry_points={
        'console_scripts': [
            'pymars=cmd:main_cli'
        ]
    }
)
