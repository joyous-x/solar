# coding=utf-8

"""
http://jst.tsinghuajournals.com/CN/rhhtml/20200504.htm

https://cloud.tencent.com/developer/article/1771586

https://wooyun.js.org/drops/%E5%88%A9%E7%94%A8%E6%9C%BA%E5%99%A8%E5%AD%A6%E4%B9%A0%E8%BF%9B%E8%A1%8C%E6%81%B6%E6%84%8F%E4%BB%A3%E7%A0%81%E5%88%86%E7%B1%BB.html

https://security-informatics.springeropen.com/track/pdf/10.1186/2190-8532-1-1.pdf
"""

"""
模型又不适用了？ --论安全应用的概念漂移样本检测
https://cloud.tencent.com/developer/article/1767313

在安全场景下部署机器学习算法的一个重大阻碍就是“概念漂移”现象，只要是在“封闭世界”中学习到的模型，
应用到“开放世界”中都会遇到类似的问题，该现象在安全中尤其明显，如恶意软件检测，在封闭训练集上有良好性能的分类器，
当输入一个新的家族样本或者变种后，会被分类器强行分到一个已知家族分类中，而显然该样本不属于任何一个分类，
类似的情况在入侵检测、WebShell检测、网站指纹等领域同样会出现，出现这种问题的核心是训练数据与测试数据的分布不一致，
而所有的机器学习模型都是基于样本的“独立同分布”假设的，这种分布的不一致性往往也是导致机器学习模型不适用的根本原因。

概念漂移一般有以下两种情况：
1 出现了新的分类。在模型线上部署时，由于线下测试不会覆盖所有样本，往往会出现新的分类。
2 已有分类进化。当模型更换环境时，已有分类的数据分布可能会发生变化，如正常流量在各个业务场景中都是不一样的；同一个家族内的恶意软件会出现新的变种。

在测试集中，与训练集样本中分布不一致的样本为概念漂移样本，一般做法是通过置信度来判定是否为概念漂移样本。
但是这只是理想情况，很多情况下，新的分类样本在已知分类中被赋予很高的概率，导致无法检测。

本文介绍一种最新的概念漂移样本检测技术CADE，CADE在检测出概念漂移样本的同时对结果进行解释，即哪些特征是做出该判断的重要特征。该方法已开源（https://github.com/whyisyoung/CADE）
"""
class FeatureItem(object):
    """ Base class from which each feature type may inherit """

    name = ''
    dim = 0

    def __repr__(self):
        return '{}({})'.format(self.name, self.dim)

    def raw_features(self, bytez, lief_binary):
        """ Generate a JSON-able representation of the file """
        raise NotImplementedError

    def feature_addition(self, bytez, lief_library):
        """ Additions """
        raise NotImplementedError

    def feature_vector(self, bytez, lief_binary):
        """ Directly calculate the feature vector from the sample itself. This should only be implemented differently
        if there are significant speedups to be gained from combining the two functions. """
        raise NotImplementedError


class AntiVirusFeatures(object):
    def __init__(self):
        self.__features_data = map()
        self.__features_getter = map()
        self._file = None

    @staticmethod
    def feature_func_table():
        features_getter = {
            "dos_header": None,
            "pe_header": None,
            "optional_header": None,
            "optional_header": None,
        }
        return features_getter

    def register_feature_func(self, name, func_feature_getter):
        self.__features_getter[name] = func_feature_getter

    def extract_features(self):
        for name, feature_getter in self.__features_getter.items():
            self.__features_data[name] = feature_getter(self._file)

    def extract_feature(self, name):
        if name not in self.__features_getter:
            return None
        return self.__features_getter[name](self._file)


import sys
import argparse
from anti_virus_ember import *


def extract_ember_features(filepath, feature_version=2, raw_features=False):
    file_bytes = None
    with open(filepath, 'rb') as f:
        file_bytes = f.read()
    f.close()

    ember_extractor = PEFeatureExtractor(feature_version)
    if raw_features:
        pe_features = ember_extractor.raw_features(file_bytes)
    else:
        pe_features = np.array(ember_extractor.feature_vector(file_bytes), dtype=np.float32)
        pe_features = pe_features[0]
    print(pe_features)
    return pe_features


if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser(prog=sys.argv[0], description="ai features for anti-virus")
    arg_parser.add_argument("-v", "--version", type=int, default=2, help="ember feature version")
    arg_parser.add_argument("-f", "--filepath", type=str, default=None, help="filepath which need to be parsed")
    args = arg_parser.parse_args()

    args.filepath = "D:\\Workspace\\tmp\\pe-sample-5w\\0000bed33b6f3233503a4481be7e456a"
    if args.filepath is None:
        arg_parser.print_help()
    else:
        extract_ember_features(args.filepath)
