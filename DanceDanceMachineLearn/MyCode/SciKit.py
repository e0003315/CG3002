# Load libraries
from six.moves import input
import datetime
import pandas
import numpy
from numpy import mean, absolute
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

def mad(data, axis=None):
    return mean(absolute(data-mean(data,axis)), axis)
# Load dataset
# url = "C:/Users/CheeYeo/Desktop/CG3002/Code/DanceDanceMachineLearn/MyCode/data2.csv" #CY's computer file path
url = "C:/Users/User/Documents/SEM5/CG3002/Project3002/Test case/TenMoves.csv"  # Kelvin's computer file path
# names = ['accel_x', 'accel_y', 'accel_z', 'gyro_x', 'gyro_y', 'gyro_z', 'activity']
# dataset = pandas.read_csv(url, names=names)
dataset = pandas.read_csv(url)
window_size = 80

# Split-out validation dataset
array = dataset.values
X = array[:, 0:12]
Y = array[:, 12]

# label encode
le = preprocessing.LabelEncoder()
le.fit(['NoMove', 'WaveHand', 'BusDrive', 'FrontBack', 'SideStep', 'Jumping', 'jumpingJack', 'turnClap', 'squatTurnClap', 'window', 'window360'])
Y_encoded = le.transform(Y)

# print(datetime.datetime.now().time())

N = dataset.shape[0]
dim_X = X.shape[1]
K = N // window_size
segments_X = numpy.empty((K, window_size, dim_X))
segments_Y = numpy.empty((K, window_size))
for i in range(K):
    segment_X = X[i * window_size : (i * window_size) + window_size , :]
    segment_X = preprocessing.normalize(segment_X)
    segment_Y = Y_encoded[i * window_size : (i * window_size) + window_size]
    segments_X[i] = segment_X
    segments_Y[i] = segment_Y

features = numpy.empty((K, 36))
outputs = numpy.empty((K))

for i in range(K):
    for j in range(0, features.shape[1] - 1, 3):
        features[i, j] = segments_X[i, : , j // 3].mean()
        features[i, j + 1] = segments_X[i, : , j // 3].std()
        features[i, j + 2] = mad(segments_X[i, : , j // 3])
    outputs[i] = stats.mode(segments_Y[i])[0]

    
# print(datetime.datetime.now().time())

validation_size = 0.20
seed = 9
X_train, X_validation, Y_train, Y_validation = model_selection.train_test_split(features, outputs, test_size=validation_size, random_state=seed)

# Test options and evaluation metric
scoring = 'accuracy'

# Spot Check Algorithms
models = []
# models.append(('LR', LogisticRegression()))
# models.append(('LDA', LinearDiscriminantAnalysis()))
models.append(('KNN', KNeighborsClassifier(n_neighbors=3)))
# models.append(('CART', DecisionTreeClassifier()))
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
 
# Make predictions on validation dataset
knn = KNeighborsClassifier(n_neighbors=3)
knn.fit(X_train, Y_train)
predictions = knn.predict(X_validation)
print(accuracy_score(Y_validation, predictions))
print(confusion_matrix(Y_validation, predictions))
print(classification_report(Y_validation, predictions))

a = input()
# rawData = numpy.empty(80, 12)
# featureData = numpy.empty(24)
while (a != "0"):
#     counter = 0
#     while (counter < 80):
    my_list = a.split()
    matrix = [my_list]
#         rawData[counter] = matrix
#         counter = counter + 1
    predictions = knn.predict(matrix)
    
#     for j in range(0, featureData.shape[1] - 1, 2):
#         featureData[j] = rawData[: , j//2].mean()
#         featureData[j+1] = rawData[: , j//2].std()

#     predictions = knn.predict(featureData)
    print(predictions)
    a = input()
