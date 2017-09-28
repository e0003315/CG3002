# Load libraries
from six.moves import input
import datetime
import pandas
import numpy
from pandas.tools.plotting import scatter_matrix
from scipy import stats
import matplotlib.pyplot as plt
from sklearn import model_selection
from sklearn import preprocessing
from sklearn.decomposition import PCA
from sklearn.metrics import classification_report
from sklearn.metrics import confusion_matrix
from sklearn.metrics import accuracy_score
from sklearn.linear_model import LogisticRegression
from sklearn.tree import DecisionTreeClassifier
from sklearn.neighbors import KNeighborsClassifier
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis
from sklearn.naive_bayes import GaussianNB
from sklearn.svm import SVC

# Load dataset
# url = "https://www.kaggle.com/vmalyi/run-or-walk/downloads/dataset.csv"
# url = "https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data"
# names = ['sepal-length', 'sepal-width', 'petal-length', 'petal-width', 'class']
# url = "C:/Users/CheeYeo/Desktop/CG3002/Code/DanceDanceMachineLearn/MyCode/data2.csv" #CY's computer file path
url = "C:/Users/User/Documents/SEM5/CG3002/Project3002/DanceDanceMachineLearn/MyCode/data2.csv" #Kelvin's computer file path
names = ['accel_x', 'accel_y', 'accel_z', 'gyro_x', 'gyro_y', 'gyro_z', 'activity']
dataset = pandas.read_csv(url, names=names)
window_size = 100
#shape
# print(dataset.shape)
#head
# print(dataset.head(20))
#description
# print(dataset.describe())
#class distribution
# print(dataset.groupby('activity').size())
#box and whisker plots
# dataset.plot(kind='box', subplots=True, layout =(2,4), sharex=False, sharey=False)
# plt.show()
#histogram
# dataset.hist()
# plt.show()

#scatter plot matrix (multivariate plot)
# scatter_matrix(dataset)
# plt.show()

# Split-out validation dataset
array = dataset.values
# array = normalised_data.values
X = array[:,0:6]
Y = array[:,6]

# normalization
# normalised_data = preprocessing.normalize(X)

# label encode
le = preprocessing.LabelEncoder()
le.fit(['sit', 'walk', 'stairsdown'])
# print(list(le.classes_))
Y_encoded = le.transform(Y)
# print(Y_encoded)

print(datetime.datetime.now().time())

N = dataset.shape[0]
dim_X = X.shape[1]
K = N//window_size
segments_X = numpy.empty((K, window_size, dim_X))
segments_Y = numpy.empty((K, window_size))
for i in range(K):
    segment_X = X[i*window_size : (i*window_size ) + window_size , :]
    segment_X = preprocessing.normalize(segment_X)
    segment_Y = Y_encoded[i*window_size : (i*window_size) + window_size]
    segments_X[i] = segment_X
    segments_Y[i] = segment_Y

features = numpy.empty((K, 12))
outputs = numpy.empty((K))
for i in range(K):
    features[i, 0] = segments_X[i, : , 0].mean()
    features[i, 1] = segments_X[i, : , 0].std()
    features[i, 2] = segments_X[i, : , 1].mean()
    features[i, 3] = segments_X[i, : , 1].std()
    features[i, 4] = segments_X[i, : , 2].mean()
    features[i, 5] = segments_X[i, : , 2].std()
    features[i, 6] = segments_X[i, : , 3].mean()
    features[i, 7] = segments_X[i, : , 3].std()
    features[i, 8] = segments_X[i, : , 4].mean()
    features[i, 9] = segments_X[i, : , 4].std()
    features[i, 10] = segments_X[i, : , 5].mean()
    features[i, 11] = segments_X[i, : , 5].std()
    outputs[i] = stats.mode(segments_Y[i])[0]
    
# print(outputs)
# print(segment_X.mean())
print(datetime.datetime.now().time())
# print (segment_X.shape)
# print (segment_Y.shape)

# print(array)
# print(X)
# print(Y)
validation_size = 0.20
seed = 7
X_train, X_validation, Y_train, Y_validation = model_selection.train_test_split(features, outputs, test_size=validation_size, random_state=seed)
# Test options and evaluation metric
seed = 7
scoring = 'accuracy'

# Spot Check Algorithms
models = []
# models.append(('LR', LogisticRegression()))
# models.append(('LDA', LinearDiscriminantAnalysis()))
models.append(('KNN', KNeighborsClassifier(n_neighbors = 8)))
models.append(('CART', DecisionTreeClassifier()))
# models.append(('NB', GaussianNB()))
# models.append(('SVM', SVC()))
# evaluate each model in turn
results = []
names = []
for name, model in models:
    kfold = model_selection.KFold(n_splits=10, random_state=seed)
    cv_results = model_selection.cross_val_score(model, X_train, Y_train, cv=kfold, scoring=scoring)
    results.append(cv_results)
    names.append(name)
    msg = "%s: %f (%f)" % (name, cv_results.mean(), cv_results.std())
    print(msg)
     
# Compare Algorithms
# fig = plt.figure()
# fig.suptitle('Algorithm Comparison')
# ax = fig.add_subplot(111)
# plt.boxplot(results)
# ax.set_xticklabels(names)
# plt.show()
 
# Make predictions on validation dataset
knn = KNeighborsClassifier()
knn.fit(X_train, Y_train)
predictions = knn.predict(X_validation)
# print(predictions)
print(accuracy_score(Y_validation, predictions))
print(confusion_matrix(Y_validation, predictions))
print(classification_report(Y_validation, predictions))
  
# svm = SVC()
# svm.fit(X_train, Y_train)
# predictSVM = svm.predict(X_validation)
# print(accuracy_score(Y_validation, predictSVM))
# print(confusion_matrix(Y_validation, predictSVM))
# print(classification_report(Y_validation, predictSVM))

a = input()
while (a != "0"):
    my_list = a.split()
    matrix = [my_list]
    predictions = knn.predict(matrix)
    print(predictions)
    a = input()