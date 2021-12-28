# coding=utf-8
from sklearn.model_selection import train_test_split
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.metrics import confusion_matrix, classification_report
from sklearn.feature_selection import chi2
import numpy as np
import matplotlib.pyplot as plt
import pandas

root_dir = "D:\\Workspace\\tmp\\filetype"
data_csv = root_dir + "\\Classification-of-Programming-Languages\\data (1).csv"
data_csv2 = root_dir + "\\datasets\\RosettaCodeData.csv"

class ProgramLangClassifier(object):
    def __init__(self):
       pass

import os
def run(datapath):
    datas = pandas.read_csv(datapath)
    print(datas.language.value_counts())

    output_dir = "D:\\Workspace\\tmp\\filetype\\datasets\\splits_2"
    for index, d in datas.iterrows():
        lname = d["language"]
        lcode = d["code"]

        try:
            cur_dirpath = os.path.join(output_dir, lname)
            if not os.path.exists(cur_dirpath):
                os.mkdir(cur_dirpath)
            cur_filename = os.path.join(cur_dirpath, str(index))

            lcode = lcode.replace('!@#$%^&&^%$#@!', ',')
            lcode = lcode.replace(' br ', ' \n ')

            with open(cur_filename, "w", encoding="utf-8") as f:
                f.write(lcode)
        except Exception as e:
            print(str(e))


    datas = datas.drop(["proj_id", "file_id"], axis=1)
    datas = datas.dropna()
    datas["lang_id"] = datas['language'].factorize()[0]

    lang_id_df = datas[['language', 'lang_id']].drop_duplicates().sort_values('lang_id')
    lang_to_id = dict(lang_id_df.values)
    id_to_lang = dict(lang_id_df[['lang_id', 'language']].values)

    plt.figure(figsize=(8,6))
    datas.groupby('language').file_body.count().plot.bar(ylim=0)
    plt.show()

    tfidf = TfidfVectorizer(sublinear_tf=True, min_df=5, norm='l2', encoding='latin-1', ngram_range=(1, 2), stop_words='english')
    features = tfidf.fit_transform(datas.file_body.values.astype('U')).toarray()
    labels = datas.lang_id
    features.shape

    N = 10

    for language, lang_id in sorted(lang_to_id.items()):
        features_chi2 = chi2(features, labels == lang_id)
        indices = np.argsort(features_chi2[0])
        feature_names = np.array(tfidf.get_feature_names())[indices]
        unigrams = [v for v in feature_names if len(v.split(' ')) == 1]
        bigrams = [v for v in feature_names if len(v.split(' ')) == 2]
        print("# '{}':".format(language))
        print("  . Most correlated unigrams:\n. {}".format('\n. '.join(unigrams[-N:])))
        print("  . Most correlated bigrams:\n. {}".format('\n. '.join(bigrams[-N:])))


if __name__ == "__main__":
    run(data_csv2)