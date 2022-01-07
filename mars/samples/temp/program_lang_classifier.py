# coding = utf-8

import os
import chardet
from sklearn import linear_model, naive_bayes, svm
from sklearn.datasets import load_files
from sklearn.model_selection import train_test_split, cross_val_score
from sklearn.feature_selection import chi2
from sklearn.feature_extraction.text import CountVectorizer, TfidfTransformer, TfidfVectorizer
from sklearn.ensemble import RandomForestClassifier
import sklearn.metrics
import numpy as np
import matplotlib.pyplot as plt
import pylab
import pandas as pd
import seaborn as sns
import shutil
import hashlib
import random
import re

pylab.mpl.rcParams['font.sans-serif'] = ['SimHei'] # for chinese
pylab.mpl.rcParams['axes.unicode_minus'] = False


class ProHelper(object):
    types_table = { "c": ["h", "c"], "cc": ["h", "cpp", "cxx", "hpp", "cc", "gcc"], "java":["java"], "c_sharp": ["cs", "csharp"], "go": ["go"]
                    , "php": ["php"], "javascript": ["js"], "swift": ["swift"], "ms_bat": ["bat", "cmd"], "shell": ["sh"], "python": ["py"]
                    , "objective-c": ["h", "m"], "ruby": ["rb"], "rust": ["rs"], "scala": ["scala"], "lisp": ["lisp", "cl"]
                    , "haskell": ["hs"], "clojure": ["clj"], "ocaml": ["ocaml", "ml"], "perl": ["pm","pl","perl"]
                    , "scheme": ["rkt"], "tcl": ["tcl"], "clojure": ["clojure"], "asm": ["s"], "powershell": ["ps1"]}

    def __init__(self):
        pass

    @staticmethod
    def enum_dir(dir_path, ignores = []):
        dirs = [dir_path]
        files = []
        while True:
            if len(dirs) == 0:
                break
            paths = dirs
            dirs = []
            for p in paths:
                if os.path.isdir(p):
                    dirs.extend([os.path.join(p, t) for t in os.listdir(p) if ignores is None or t not in ignores] )
                    continue
                files.append(p)
        return files

    @staticmethod
    def preprocess_file(filepath):
        unicode_littlendian = [0xFF, 0xFE]
        unicode_bigendian = [0xFE, 0xFF]
        utf8 = [0xEF, 0xBB, 0xBF]

        with open(filepath, 'rb') as f:
            file_bytes = f.read()
        if len(file_bytes) < 0x20:
            return

        if file_bytes[0] == unicode_littlendian[0] and file_bytes[1] == unicode_littlendian[1]:
            print("unicode_littlendian")
        elif file_bytes[0] == unicode_bigendian[0] and file_bytes[1] == unicode_bigendian[1]:
            print("unicode_bigendian")
        elif file_bytes[0] == utf8[0] and file_bytes[1] == utf8[1] and file_bytes[2] == utf8[2]:
            print("utf8")
        else:
            print("ansi")

        source_encoding = chardet.detect(file_bytes)['encoding']
        if source_encoding != 'utf-8' and source_encoding != 'UTF-8-SIG':
            file_bytes = file_bytes.decode(source_encoding, 'ignore')
        else:
            file_bytes = str(file_bytes)
        return file_bytes

    @staticmethod
    def token_pattern():
        return r"(?u)((?:\s[^\w\-\s]?[a-zA-Z\-_]{2,}\s)|(?:[^\w\s]{2,}))"  #    r"(?u)((?:\b[a-zA-Z\-_]{2,20}\b)|(?:\b[^0-9]{2,20}\b))"

    reg_a = re.compile(r"^[^\w\-\s]?[a-zA-Z_\-]{2,20}[^\w\-\s]?$", re.I)
    reg_b = re.compile(r"[^\w\-\s]{2,}", re.I)

    @staticmethod
    def split_text(s):
        def is_str_ok(x):
            for c in x:
                if ord(c) > 127 or ord(c) < 32 or c in "\n\r\t":
                    return False
                if not c.isprintable():
                    return False
            return True
        out = []
        items = re.split(r'[\\/\(\)\{\},;:\s]\s*', s) # '(){}[]<>,;:'
        for item in items:
            if not is_str_ok(item):
                continue
            if ProHelper.reg_a.match(item):
                out.append(item)
                continue
            subs = ProHelper.reg_b.findall(item)
            out += [sub for sub in subs if len(sub) < 8]
        return out


class SampleStatistics(object):
    @staticmethod
    def parse_dataset(target_ids, target_names):
        lang_count = {}
        for i in range(len(target_ids)):
            key = target_names[target_ids[i]]
            lang_count[key] = lang_count[key] + 1 if key in lang_count else 1

        X = range(len(lang_count))
        plt.figure(figsize=(8, 6))
        plt.title("样本分布")
        plt.ylabel("Count")
        plt.xlabel("language")
        plt.xticks(X, lang_count.keys(), rotation=60)
        plt.bar(x=X, height=lang_count.values(), width=0.4, alpha=0.8, label="Count")
        plt.show()

    @staticmethod
    def heatmap(ground_truth, predicted, y_labels):
        conf_mat = sklearn.metrics.confusion_matrix(ground_truth, predicted)
        fig, ax = plt.subplots(figsize=(10, 10))
        sns.heatmap(conf_mat, annot=True, fmt='d', xticklabels=y_labels, yticklabels=y_labels)
        plt.ylabel('Actual')
        plt.xlabel('Predicted')
        plt.show()
        print(sklearn.metrics.classification_report(ground_truth, predicted, target_names=y_labels))


def preprocess(dataset, ngram_range=(1,1)):
    count_vectorizer = CountVectorizer(token_pattern=ProHelper.token_pattern(), lowercase=True, ngram_range=ngram_range, analyzer=ProHelper.split_text)
    train_counts = count_vectorizer.fit_transform(dataset.data)
    train_tfidf = TfidfTransformer().fit_transform(train_counts)
    # x : (doc_id, feature_id, tfidf), y : target_id
    x, y, feature_names, target_names = train_tfidf, dataset.target, count_vectorizer.get_feature_names(), dataset.target_names
    # statistics
    count_statistics = None
    docs_features_statistics = True
    if count_statistics:
        features = [[f, 0, set()] for f in count_vectorizer.get_feature_names()]
        doc_cnt, feature_cnt = train_counts.shape[0], train_counts.shape[1]
        for i in range(feature_cnt):
            docs_info = train_counts[:, i].toarray()
            features[i][2] = np.count_nonzero(docs_info, 0)[0]
            if features[i][2] > 5:
                features[i][1] = np.sum(docs_info, axis=0)[0]
        features = sorted(features, key=lambda x: x[2], reverse=True)
    if docs_features_statistics:
        max_print = 10
        cur_print = {}
        for i in range(x.shape[0]):
            cur_print[y[i]] = 1 if y[i] not in cur_print else cur_print[y[i]] + 1
            if cur_print[y[i]] > max_print:
                continue
            target_name = target_names[y[i]]
            doc_feature = x.getrow(i).toarray()[0]
            doc_feature = [(doc_feature[i], feature_names[i]) for i in range(doc_feature.shape[0])]
            doc_feature = sorted(doc_feature, key=lambda x: x[0], reverse=True)
            feature_list = " ".join([str((round(j[0],5), j[1])) for j in doc_feature[:50]])
            print("docid=%d %s %s" % (i, target_name, feature_list))
    return x, y, feature_names, target_names


def train(x_train, y_train):
    models = [
        linear_model.SGDClassifier(),
        linear_model.LogisticRegression(random_state=0),
        svm.LinearSVC(),
        naive_bayes.MultinomialNB(),
        RandomForestClassifier(n_estimators=200, max_depth=3, random_state=0),
    ]
    entries = []
    for model in models:
        model_name = model.__class__.__name__
        accuracies = cross_val_score(model, x_train, y_train, scoring='accuracy', cv=5)
        for fold_idx, accuracy in enumerate(accuracies):
            entries.append((model_name, fold_idx, accuracy))
    cv_df = pd.DataFrame(entries, columns=['model_name', 'fold_idx', 'accuracy'])
    sns.stripplot(x='model_name', y='accuracy', data=cv_df, size=8, jitter=True, edgecolor="gray", linewidth=2)
    plt.show()
    # get the best model
    rst = cv_df.groupby('model_name').accuracy.mean()
    model_name, model_accu = "", 0
    for item in rst.items():
        if item[1] > model_accu:
            model_name, model_accu = item[0], item[1]
    for model in models:
        if model_name == model.__class__.__name__:
            model_target = model
            model_target.fit(x_train, y_train)
            break
    return model_target


def run(data_path, show_sample_distribute=False):
    dataset = load_files(data_path, load_content=True, encoding='UTF-8', decode_error='replace')
    if show_sample_distribute:
        SampleStatistics.parse_dataset(dataset.target, dataset.target_names)
    x, y, feature_names, labels = preprocess(dataset, ngram_range=(1,1))
    x_train, x_test, y_train, y_test = train_test_split(x, y, test_size=0.20, random_state=3114795823)
    # train && test
    model = train(x_train, y_train)
    SampleStatistics.heatmap(y_test, model.predict(x_test), labels)
    # signatures
    if model.__class__.__name__ == "LinearSVC":
        signs = model.coef_[0]
        signs = [(feature_names[i], signs[i]) for i in range(signs.shape[0])]
        signs = sorted(signs, key=lambda x: x[1], reverse=True)
        for i in signs:
            print("%s %.6f" % (i[0], i[1]))
    print("end!\n")


def sample_classification(filepath, catalog, dst_dir):
    files = ProHelper.enum_dir(filepath, [".git"])
    for file in files:
        fpath, fname = os.path.split(file)
        name, ext = os.path.splitext(fname)
        if ext is not None and len(ext) > 0:
            ext = ext[1:].lower()
        if catalog is None:
            subdir = None
            for c, e in ProHelper.types_table.items():
                if ext in e:
                    subdir = c
                    break
            if subdir is None:
                print("%s is not expected" % (file))
                continue
        else:
            subdir = catalog
        hasher = hashlib.md5()
        hasher.update(fname.encode("utf-8"))
        cur_dir = os.path.join(dst_dir, subdir)
        cur_dst = os.path.join(cur_dir, name + "_" + hasher.hexdigest() + "." + ext)
        if not os.path.exists(cur_dir):
            os.mkdir(cur_dir)
        shutil.move(file, cur_dst)
        print("move %s to %s \n" % (file, cur_dst))


def sample_count_balance(filepath, dst_dir, max_files=5000):
    for catalog in os.listdir(filepath):
        files = os.listdir(os.path.join(filepath, catalog))
        if len(files) < max_files:
            continue
        random.shuffle(files)
        cur_src_dir = os.path.join(filepath, catalog)
        cur_dst_dir = os.path.join(dst_dir, catalog)
        if not os.path.isdir(cur_dst_dir):
            os.mkdir(cur_dst_dir)
        for f in files[max_files:]:
            s = os.path.join(cur_src_dir, f)
            d = os.path.join(cur_dst_dir, f)
            shutil.move(s, d)
            print("move %s to %s \n" % (s, d))


if __name__ == "__main__":
    dataset_a = "/Users/jiao/Workspace/ftype_classifier/programming-language-classifier/data_test"
    dataset_b = "/Users/jiao/Workspace/ftype_classifier/data_train"
    dataset_c = "d:\\Workspace\\tmp\\filetype\\"
    #sample_count_balance(dataset_c + "sample\\", dataset_c + "sample_back\\")
    #sample_classification(dataset_c + "gits\\", None, dataset_c + "sample\\")
    run(dataset_c + "sample_back\\")
