# pymars
python libraries, with which we can build all kinds of buildings on the deserted Mars.

## 1. Introduction
common libs for web server, image process and so on

### 1.1 安装:
```
    a. 将自己构建的包安装到 SDK/Lib/sit-packages 中 python setup.py install 
    b. pip install common_mars
```

#### 1.1.1 依赖
- conda install matplotlib 
- conda install -c anaconda scikit-learn
- pip install onnxruntime (pip install onnxruntime-gpu)
- pip install onnx-simplifier
- pip install netron

- pdf:
    + conda install --channel https://conda.anaconda.org/conda-forge camelot-py 
    +  conda install --channel https://conda.anaconda.org/conda-forge pdfplumber
        - 有时安装会出现："Installing collected", 切换到 pip 安装可以看到出错的依赖
 - office
    + conda install -c conda-forge python-docx

### 1.2 构建 
```
    a. 构建 .tar.gz/.zip 格式的第三方包： python setup.py sdist
    b. 构建 *.egg 格式的第三方包：python setup.py bdist_egg
    c. 构建 *.wheel 格式的第三方包：python setup.py bdist_wheel
    d. 构建多种分发包: python setup.py sdist bdist_egg bdist_wheel
```

## 2. Basic Features

## 3. Advanced Features
