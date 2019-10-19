"""
Author      : Yi-Chieh Wu, Sriram Sankararman
Description : Twitter
"""

from string import punctuation

import numpy as np

# !!! MAKE SURE TO USE SVC.decision_function(X), NOT SVC.predict(X) !!!
# (this makes ``continuous-valued'' predictions)
from sklearn.svm import SVC
from sklearn.cross_validation import StratifiedKFold
from sklearn import metrics
import sklearn
import sklearn

######################################################################
# functions -- input/output
######################################################################

def read_vector_file(fname):
    """
    Reads and returns a vector from a file.
    
    Parameters
    --------------------
        fname  -- string, filename
        
    Returns
    --------------------
        labels -- numpy array of shape (n,)
                    n is the number of non-blank lines in the text file
    """
    return np.genfromtxt(fname)


def write_label_answer(vec, outfile):
    """
    Writes your label vector to the given file.
    
    Parameters
    --------------------
        vec     -- numpy array of shape (n,) or (n,1), predicted scores
        outfile -- string, output filename
    """
    
    # for this project, you should predict 70 labels
    if(vec.shape[0] != 70):
        print("Error - output vector should have 70 rows.")
        print("Aborting write.")
        return
    
    np.savetxt(outfile, vec)    


######################################################################
# functions -- feature extraction
######################################################################

def extract_words(input_string):
    """
    Processes the input_string, separating it into "words" based on the presence
    of spaces, and separating punctuation marks into their own words.
    
    Parameters
    --------------------
        input_string -- string of characters
    
    Returns
    --------------------
        words        -- list of lowercase "words"
    """
    
    for c in punctuation :
        input_string = input_string.replace(c, ' ' + c + ' ')
    return input_string.lower().split()


def extract_dictionary(infile):
    """
    Given a filename, reads the text file and builds a dictionary of unique
    words/punctuations.
    
    Parameters
    --------------------
        infile    -- string, filename
    
    Returns
    --------------------
        word_list -- dictionary, (key, value) pairs are (word, index)
    """
    
    word_list = {}
    count = 0
    with open(infile, 'rU') as fid :
        ### ========== TODO : START ========== ###
        # part 1a: process each line to populate word_list
        for line in fid:
            line = line.strip('\n')
            for word in extract_words(line):
                if word not in word_list:
                    word_list[word] = count
                    count += 1
        ### ========== TODO : END ========== ###

    return word_list


def extract_feature_vectors(infile, word_list):
    """
    Produces a bag-of-words representation of a text file specified by the
    filename infile based on the dictionary word_list.
    
    Parameters
    --------------------
        infile         -- string, filename
        word_list      -- dictionary, (key, value) pairs are (word, index)
    
    Returns
    --------------------
        feature_matrix -- numpy array of shape (n,d)
                          boolean (0,1) array indicating word presence in a string
                            n is the number of non-blank lines in the text file
                            d is the number of unique words in the text file
    """
    
    num_lines = sum(1 for line in open(infile,'rU'))
    num_words = len(word_list)
    feature_matrix = np.zeros((num_lines, num_words))
    
    with open(infile, 'rU') as fid :
        ### ========== TODO : START ========== ###
        # part 1b: process each line to populate feature_matrix
        
        line_num = 0
        for line in fid:
            words = extract_words(line.strip('\n'))
            for word in words:
                feature_matrix[line_num][word_list[word]] = 1
            line_num += 1
        ### ========== TODO : END ========== ###
        
    return feature_matrix


######################################################################
# functions -- evaluation
######################################################################

def performance(y_true, y_pred, metric="accuracy"):
    """
    Calculates the performance metric based on the agreement between the 
    true labels and the predicted labels.
    
    Parameters
    --------------------
        y_true -- numpy array of shape (n,), known labels
        y_pred -- numpy array of shape (n,), (continuous-valued) predictions
        metric -- string, option used to select the performance measure
                  options: 'accuracy', 'f1-score', 'auroc', 'precision',
                           'sensitivity', 'specificity'        
    
    Returns
    --------------------
        score  -- float, performance score
    """
    # map continuous-valued predictions to binary labels
    y_label = np.sign(y_pred)
    y_label[y_label==0] = 1
    
    ### ========== TODO : START ========== ###
    # part 2a: compute classifier performance
    tn, fp, fn, tp = sklearn.metrics.confusion_matrix(y_true, y_label).ravel()
    #print sklearn.metrics.confusion_matrix(y_true, y_label)
    metrics = {
        'accuracy': sklearn.metrics.accuracy_score(y_true, y_label),
        'f1_score': sklearn.metrics.f1_score(y_true, y_label),
        'auroc': sklearn.metrics.roc_auc_score(y_true, y_label),
        'precision': sklearn.metrics.precision_score(y_true, y_label),
        'sensitivity': float(tp)/float(tp+fn),
        'specificity': float(tn)/float(tn+fp),
    }

    return metrics[metric]
    ### ========== TODO : END ========== ###


def cv_performance(clf, X, y, kf, metric="accuracy"):
    """
    Splits the data, X and y, into k-folds and runs k-fold cross-validation.
    Trains classifier on k-1 folds and tests on the remaining fold.
    Calculates the k-fold cross-validation performance metric for classifier
    by averaging the performance across folds.
    
    Parameters
    --------------------
        clf    -- classifier (instance of SVC)
        X      -- numpy array of shape (n,d), feature vectors
                    n = number of examples
                    d = number of features
        y      -- numpy array of shape (n,), binary labels {1,-1}
        kf     -- cross_validation.KFold or cross_validation.StratifiedKFold
        metric -- string, option used to select performance measure
    
    Returns
    --------------------
        score   -- float, average cross-validation performance across k folds
    """
    
    ### ========== TODO : START ========== ###
    score = []
    # part 2b: compute average cross-validation performance    
    for train_idx, test_idx in kf.split(X, y):
        clf.fit(X[train_idx], y[train_idx])
        s = performance(y[test_idx], clf.decision_function(X[test_idx]), metric=metric)
        score.append(s)
    return np.mean(score)
    ### ========== TODO : END ========== ###

def select_param_linear(X, y, kf, metric="accuracy"):
    """
    Sweeps different settings for the hyperparameter of a linear-kernel SVM,
    calculating the k-fold CV performance for each setting, then selecting the
    hyperparameter that 'maximize' the average k-fold CV performance.
    
    Parameters
    --------------------
        X      -- numpy array of shape (n,d), feature vectors
                    n = number of examples
                    d = number of features
        y      -- numpy array of shape (n,), binary labels {1,-1}
        kf     -- cross_validation.KFold or cross_validation.StratifiedKFold
        metric -- string, option used to select performance measure
    
    Returns
    --------------------
        C -- float, optimal parameter value for linear-kernel SVM
    """
    
    print 'Linear SVM Hyperparameter Selection based on ' + str(metric) + ':'
    C_range = 10.0 ** np.arange(-3, 3)
    
    ### ========== TODO : START ========== ###
    # part 2c: select optimal hyperparameter using cross-validation
    scores = []
    for c in C_range:
        clf = sklearn.svm.SVC(kernel='linear', C=c)
        score = cv_performance(clf, X, y, kf, metric)
        scores.append(score)
        print c, score
    
    return C_range[scores.index(max(scores))]
    ### ========== TODO : END ========== ###


def select_param_rbf(X, y, kf, metric="accuracy"):
    """
    Sweeps different settings for the hyperparameters of an RBF-kernel SVM,
    calculating the k-fold CV performance for each setting, then selecting the
    hyperparameters that 'maximize' the average k-fold CV performance.
    
    Parameters
    --------------------
        X       -- numpy array of shape (n,d), feature vectors
                     n = number of examples
                     d = number of features
        y       -- numpy array of shape (n,), binary labels {1,-1}
        kf     -- cross_validation.KFold or cross_validation.StratifiedKFold
        metric  -- string, option used to select performance measure
    
    Returns
    --------------------
        gamma, C -- tuple of floats, optimal parameter values for an RBF-kernel SVM
    """
    
    print 'RBF SVM Hyperparameter Selection based on ' + str(metric) + ':'
    
    ### ========== TODO : START ========== ###
    # part 3b: create grid, then select optimal hyperparameters using cross-validation
    gammas = 10.0 ** np.arange(-3, 3)
    C_range = 10.0 ** np.arange(-3, 3)

    scores = []
    for g in gammas:
        for c in C_range:
            clf = sklearn.svm.SVC(kernel='rbf', C=c, gamma=g)
            score = cv_performance(clf, X, y, kf, metric)
            scores.append(score)
    opt = scores.index(max(scores))
    opt_g = opt // len(gammas)
    opt_c = opt % len(gammas)
    print scores[opt]
    return gammas[opt_g], C_range[opt_c]
    ### ========== TODO : END ========== ###


def performance_test(clf, X, y, metric="accuracy"):
    """
    Estimates the performance of the classifier using the 95% CI.
    
    Parameters
    --------------------
        clf          -- classifier (instance of SVC)
                          [already fit to data]
        X            -- numpy array of shape (n,d), feature vectors of test set
                          n = number of examples
                          d = number of features
        y            -- numpy array of shape (n,), binary labels {1,-1} of test set
        metric       -- string, option used to select performance measure
    
    Returns
    --------------------
        score        -- float, classifier performance
    """

    ### ========== TODO : START ========== ###
    # part 4b: return performance on test data by first computing predictions and then calling performance

    y_pred = clf.predict(X)
    y_true = y

    score = performance(y_true, y_pred, metric=metric)

    return score
    ### ========== TODO : END ========== ###


######################################################################
# main
######################################################################
 
def main() :
    np.random.seed(1234)
    
    # read the tweets and its labels   
    dictionary = extract_dictionary('../data/tweets.txt')
    X = extract_feature_vectors('../data/tweets.txt', dictionary)
    y = read_vector_file('../data/labels.txt')
    
    metric_list = ["accuracy", "f1_score", "auroc", "precision", "sensitivity", "specificity"]
    
    ### ========== TODO : START ========== ###
    # part 1c: split data into training (training + cross-validation) and testing set
    X_train, X_test = X[:560], X[560:]
    y_train, y_test = y[:560], y[560:]
    
    # part 2b: create stratified folds (5-fold CV)
    skf = sklearn.model_selection.StratifiedKFold(n_splits=5)
    
    # part 2d: for each metric, select optimal hyperparameter for linear-kernel SVM using CV
    '''
    for metric in metric_list:
        print metric, select_param_linear(X_train, y_train, skf, metric=metric)
    '''
    # part 3c: for each metric, select optimal hyperparameter for RBF-SVM using CV
    '''
    for metric in metric_list:
        print metric, select_param_rbf(X_train, y_train, skf, metric=metric)
    '''
    # part 4a: train linear- and RBF-kernel SVMs with selected hyperparameters
    clf_linear = sklearn.svm.SVC(kernel='linear', C=10)
    clf_rbf = sklearn.svm.SVC(kernel='rbf', C=100, gamma=0.01)

    clf_linear.fit(X_train, y_train)
    clf_rbf.fit(X_train, y_train)
    
    # part 4c: report performance on test data
    for metric in metric_list:
        lin = performance_test(clf_linear, X_test, y_test, metric=metric)
        rbf = performance_test(clf_rbf, X_test, y_test, metric=metric)
        print metric, lin, rbf
    
    ### ========== TODO : END ========== ###
    
    
if __name__ == "__main__" :
    main()