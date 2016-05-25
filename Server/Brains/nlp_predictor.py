from nltk.tokenize import RegexpTokenizer
from nltk.probability import FreqDist
from nltk.util import Trie
import numpy as np
import pandas as pd

tokenizer = RegexpTokenizer('[a-zA-Z]+')

words = []
with open("resources/big.txt") as f:
    for l in f:
        words += [x.lower() for x in tokenizer.tokenize(l)]

fdist = FreqDist(words)
wordfreq_dic = dict((w, f) for w, f in fdist.most_common())

trie = Trie([w for w, c in fdist.most_common()])

def letterProbs(query):
    global trie, fdist, wordfreq_dic
    subtrie = trie
    for c in query:
        subtrie = subtrie.get(c)
    possib_words = []
    dir(subtrie)

    def traverse(prefix, subtrie):
        words = []
        for k in subtrie.keys():
            if k == True:
                words += [prefix]
            if k != True:
                words += traverse(prefix + k, subtrie[k])
        return words
    letword_dic = {}
    for k in subtrie.keys():
        if k == True:
            letword_dic[-1] = [query]
        else:
            letword_dic[k] = traverse(query + k, subtrie[k])
    letfreq_dic = {}
    for k, v in letword_dic.iteritems():
        letfreq_dic[k] = sum([wordfreq_dic[w] for w in v])
    letfreq_dic
    for k in letfreq_dic:
        letfreq_dic[k] = 1.0 * letfreq_dic[k] / sum(letfreq_dic.values())
    return letfreq_dic

def predict(prefix):
    probs = letterProbs(prefix)
    letters = ['a', 'b', 'c', 'd', 'e']
    ls = []
    for i in range(5):
        ls.append(probs.get(letters[i], 0.0))
    return ls
