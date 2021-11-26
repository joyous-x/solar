# coding = utf-8

import os
import chardet
from sklearn import linear_model, naive_bayes, svm
from sklearn.datasets import load_files
from sklearn.model_selection import train_test_split, cross_val_score
from sklearn.feature_selection import chi2
from sklearn.feature_extraction.text import CountVectorizer, TfidfTransformer, TfidfVectorizer
import sklearn.metrics
import numpy as np
import matplotlib.pyplot as plt
import pylab
import pandas as pd
import seaborn as sns
import shutil
import github

pylab.mpl.rcParams['font.sans-serif'] = ['SimHei'] # for chinese
pylab.mpl.rcParams['axes.unicode_minus'] = False


class ProHelper(object):
    types_table = { "c": ["h", "c"], "cc": ["h", "cpp", "cxx", "hpp", "cc"], "java":["java"], "c_sharp": ["cs", "csharp"], "go": ["go"]
                    , "php": ["php"], "javascript": ["js"], "swift": ["swift"], "ms_bat": ["bat"], "shell": ["sh"], "python": ["py"]
                    , "objective-c": ["h", "m"], "ruby": ["rb"], "rust": ["rs"], "scala": ["scala"], "lisp": ["lisp"], "fortran": []
                    , "haskell": ["hs"], "clojure": ["clj"] , "ocaml": ["ocaml", "ml"], "perl": ["pm","pl","perl"], "scheme": ["rkt"], "tcl": ["tcl"]}

    def __init__(self):
        pass

    @staticmethod
    def github_spider(user, token, organization, path):
        """
        pip install pygithub
        """
        git = github.Github(user, token)
        org = git.get_organization(organization)
        tmp = org.get_repos()
        repos = [repo.name for repo in tmp]
        return repos

    @staticmethod
    def enum_dir(dir_path):
        dirs = [dir_path]
        files = []
        while True:
            if len(dirs) == 0:
                break
            paths = dirs
            dirs = []
            for p in paths:
                if os.path.isdir(p):
                    dirs.extend([os.path.join(p, t) for t in os.listdir(p)])
                    continue
                files.append(p)
        return files

    @staticmethod
    def preprocess_data(content_str):
        content = ''
        for i in range(len(content_str)):
            c = content_str[i]
            if c not in '(){}[]<>,:;=&|^\\/%.' and c.isprintable():
                content = content + c
                continue
            content = content + ' '
        return content

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
        content = ProHelper.preprocess_data(file_bytes)


def parse_dataset(dataset):
    lang_count = {}
    for i in range(len(dataset.target)):
        key = dataset.target_names[dataset.target[i]]
        if key not in lang_count:
            lang_count[key] = 0
        lang_count[key] = lang_count[key] + 1

    X = range(len(lang_count))
    langs = lang_count.keys()
    counts = lang_count.values()
    plt.figure(figsize=(8, 6))
    plt.xticks(X, langs, rotation=60)
    plt.xlabel("language")
    plt.bar(x=X, height=counts, width=0.4, alpha=0.8, label="Count")
    plt.ylabel("Count")
    plt.title("样本分布")
    plt.show()


def parse_tokenizer(features, targets, feature_names, labels, N=10):
    # tfidf = TfidfVectorizer(sublinear_tf=True, min_df=5, norm='l2', encoding='latin-1', ngram_range=(1, 2), stop_words='english')
    for i in range(len(labels)):
        lang_id = i
        lang_name = labels[i]
        features_chi2 = chi2(features, targets == lang_id)
        indices = np.argsort(features_chi2[0])
        feature_names = np.array(feature_names)[indices]
        unigrams = [v for v in feature_names if len(v.split(' ')) == 1]
        bigrams = [v for v in feature_names if len(v.split(' ')) == 2]
        print("# '{}':".format(lang_name))
        print("  . Most correlated unigrams:\n. {}".format('\n. '.join(unigrams[-N:])))
        print("  . Most correlated bigrams:\n. {}".format('\n. '.join(bigrams[-N:])))


def preprocess(dataset, ngram_range=(1,2)):
    for i in range(len(dataset.data)):
        dataset.data[i] = ProHelper.preprocess_data(dataset.data[i])
    token_pattern = r"(?u)\b[a-zA-Z\-_$@!]{2,15}\b"  # r"(?u)\b\w\w+\b"   r"(?u)\b[^0-9\.\*\s\'\"]{2,20}\b"
    count_vectorizer = CountVectorizer(token_pattern=token_pattern, lowercase=True, max_features=10240, ngram_range=ngram_range)
    tfidf_transformer = TfidfTransformer()
    train_counts = count_vectorizer.fit_transform(dataset.data)
    train_tfidf = tfidf_transformer.fit_transform(train_counts)
    x, y, feature_names, target_names = train_tfidf, dataset.target, count_vectorizer.get_feature_names(), dataset.target_names
    return x, y, feature_names, target_names


def train(x_train, y_train):
    models = [
        linear_model.SGDClassifier(),
        linear_model.LogisticRegression(random_state=0),
        svm.LinearSVC(),
        naive_bayes.MultinomialNB(),
    ]
    entries = []
    for model in models:
        model_name = model.__class__.__name__
        accuracies = cross_val_score(model, x_train, y_train, scoring='accuracy', cv=5)
        for fold_idx, accuracy in enumerate(accuracies):
            entries.append((model_name, fold_idx, accuracy))
    cv_df = pd.DataFrame(entries, columns=['model_name', 'fold_idx', 'accuracy'])
    #sns.boxplot(x='model_name', y='accuracy', data=cv_df)
    sns.stripplot(x='model_name', y='accuracy', data=cv_df, size=8, jitter=True, edgecolor="gray", linewidth=2)
    plt.show()
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


def test(model, x_test, y_test, target_list):
    y_pred = model.predict(x_test)
    conf_mat = sklearn.metrics.confusion_matrix(y_test, y_pred)
    fig, ax = plt.subplots(figsize=(10, 10))
    sns.heatmap(conf_mat, annot=True, fmt='d', xticklabels=target_list, yticklabels=target_list)
    plt.ylabel('Actual')
    plt.xlabel('Predicted')
    plt.show()
    print(sklearn.metrics.classification_report(y_test, y_pred, target_names=target_list))


def predict(model, x_to_predict):
    pass


def run(data_path):
    dataset = load_files(data_path, load_content=True, encoding='UTF-8', decode_error='replace')
    #parse_dataset(dataset)

    x, y, feature_names, labels = preprocess(dataset, ngram_range=(1,1))
    x_train, x_test, y_train, y_test = train_test_split(x, y, test_size=0.20, random_state=3114795823)
    #parse_tokenizer(x, y, feature_names, labels)

    model = train(x_train, y_train)
    test(model, x_test, y_test, labels)
    #predict(model, x_to_predict=None)


def sample_filter(filepath):
    suffixs = ProHelper.types_table
    for cat in os.listdir(filepath):
        if cat in suffixs:
            expected_suffixs = suffixs[cat]
        else:
            print("%s is not expected" % (cat))
            continue
        files = os.listdir(os.path.join(filepath, cat))
        for file in files:
            name, suffix = os.path.splitext(file)
            if suffix and len(suffix) > 0:
                suffix = suffix[1:]
                if suffix in expected_suffixs:
                    continue
            curfile = os.path.join(os.path.join(filepath, cat), file)
            print("cat %s --- suffix %s is not expected" % (cat, curfile))
            os.remove(curfile)


def download_samples():
    datas = ProHelper.github_spider(None, None, "IBM", "")
    print(datas)


if __name__ == "__main__":
    dataset_a = "/Users/jiao/Workspace/ftype_classifier/programming-language-classifier/data_test"
    dataset_b = "/Users/jiao/Workspace/ftype_classifier/data_train"
    #sample_filter(dataset_b)
    download_samples()
    run(dataset_b)
