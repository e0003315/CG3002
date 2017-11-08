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
from cProfile import label

pandas.set_option('display.height', 1000)
pandas.set_option('display.max_rows', 500)
pandas.set_option('display.max_columns', 500)
pandas.set_option('display.width', 1000)
pandas.set_option('display.expand_frame_repr', False)
numpy.set_printoptions(threshold=numpy.inf)        

class learning:
    def _init_(self):
        super(learning, self)._init_()
        
    def mad(self, data, axis=None):
        return mean(absolute(data-mean(data,axis)), axis)
    
    def processData(self, data, model):
        segment_X = preprocessing.normalize(data)
        features = numpy.empty(36)
        for j in range(0, features.shape[0] - 1, 3):
            features[j] = segment_X[ : , j // 3].mean()
            features[j + 1] = segment_X[ : , j // 3].std()
            features[j + 2] = self.mad(segment_X[ : , j // 3])
#         return le.inverse_transform([3])
#         print (model.predict([features]))
#         return(model.predict([features]))
        string = (model.predict([features]))
        return le.inverse_transform([int(string[0])])
#         return le.inverse_transform(model.predict([features]))
    def machineTrain(self):
        # Load dataset
        # url = "C:/Users/CheeYeo/Desktop/CG3002/Code/Test case/TenMoves.csv" #CY's computer file path
        #url = "C:/Users/User/Documents/SEM5/CG3002/Project3002/Week11 Readings/6pplData.csv"
        #valiUrl = "C:/Users/User/Documents/SEM5/CG3002/Project3002/Week11 Readings/ValidationDataSetWeek11.csv"
        #url = "C:/Users/User/Documents/SEM5/CG3002/Project3002/New 2/6pplData.csv"  # Kelvin's computer file path
        url = "/home/pi/Desktop/6pplData.csv"
        # names = ['accel_x', 'accel_y', 'accel_z', 'gyro_x', 'gyro_y', 'gyro_z', 'activity']
        # dataset = pandas.read_csv(url, names=names)
        dataset = pandas.read_csv(url)
#         validation = pandas.read_csv(valiUrl)
        global window_size
        window_size = 120
        shift_size = 30
        models = []
        models.append(('KNN', KNeighborsClassifier(n_neighbors=7)))
        knn = KNeighborsClassifier(n_neighbors=3)
        
        # Split-out validation dataset
        array = dataset.values
        X = array[:, 0:12]
        Y = array[:, 12]
#         array = validation.values
#         VALI_X = array[:, 0:12]
#         VALI_Y = array[:, 12]
        # label encode
        global le 
        le = preprocessing.LabelEncoder()
        le.fit(['NoMove', 'wavehands', 'busdriver', 'frontback', 'sidestep', 'jumping', 'jumpingjack', 'turnclap', 'squatturnclap', 'window', 'window360'])
        Y_encoded = le.transform(Y)
#         print(le.inverse_transform([0]))
#         print(le.inverse_transform([1]))
#         print(le.inverse_transform([2]))
#         print(le.inverse_transform([3]))
#         print(le.inverse_transform([4]))
#         print(le.inverse_transform([5]))
#         VALI_Y = le.transform(VALI_Y)
        
        # print(datetime.datetime.now().time())
        
        N = dataset.shape[0]
        dim_X = X.shape[1]
        K = (N // shift_size) - 7
        segments_X = numpy.empty((K, window_size, dim_X))
        segments_Y = numpy.empty((K, window_size))
        
        for i in range(K):
            segment_X = X[i * shift_size : (i * shift_size) + window_size , :]
#             print(segment_X)
            segment_X = preprocessing.normalize(segment_X)
            segment_Y = Y_encoded[i * shift_size : (i * shift_size) + window_size]
            segments_X[i] = segment_X
            segments_Y[i] = segment_Y
        
#         N = validation.shape[0]
#         dim_X = VALI_X.shape[1]
#         K = (N // shift_size) - 1
#         VALI_segments_X = numpy.empty((K, window_size, dim_X))
#         VALI_segments_Y = numpy.empty((K, window_size))
#         for i in range(K):
#             segment_X = VALI_X[i * shift_size : (i * shift_size) + window_size , :]
#             segment_X = preprocessing.normalize(segment_X)
#             segment_Y = VALI_Y[i * shift_size : (i * shift_size) + window_size]
#             VALI_segments_X[i] = segment_X
#             VALI_segments_Y[i] = segment_Y
        
        features = numpy.empty((K, 36))
        outputs = numpy.empty((K))
#         VALI_features = numpy.empty((K, 36))
#         VALI_outputs = numpy.empty((K))
        
        for i in range(K):
            for j in range(0, features.shape[1] - 1, 3):
                features[i, j] = segments_X[i, : , j // 3].mean()
                features[i, j + 1] = segments_X[i, : , j // 3].std()
                features[i, j + 2] = self.mad(data = segments_X[i, : , j // 3])
#                 VALI_features[i, j] = VALI_segments_X[i, : , j // 3].mean()
#                 VALI_features[i, j + 1] = VALI_segments_X[i, : , j // 3].std()
#                 VALI_features[i, j + 2] = self.mad(data = VALI_segments_X[i, : , j // 3])
            outputs[i] = stats.mode(segments_Y[i])[0]
#             VALI_outputs[i] = stats.mode(VALI_segments_Y[i])[0]
#         with pandas.option_context('display.max_rows', -1, 'display.max_columns', 5):
#             print(features[197, :], file=open('feature.txt', 'a'))
#         print(outputs, file=open('output.txt', 'w'))
            
        # print(datetime.datetime.now().time())
        
        validation_size = 0.2
        seed = 7
        X_train, X_validation, Y_train, Y_validation = model_selection.train_test_split(features, outputs, test_size=validation_size, random_state=seed)
        
        # Test options and evaluation metric
        scoring = 'accuracy'
        
        # Spot Check Algorithms
        # models.append(('LR', LogisticRegression()))
        # models.append(('LDA', LinearDiscriminantAnalysis()))
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
        knn.fit(X_train, Y_train)
        print("exit training")
        predictions = knn.predict(X_validation)
#         predictions = knn.predict(VALI_features)
        print("Accuracy Score: ", accuracy_score(Y_validation, predictions), file=open('summary.txt', 'a'))
#         print("Accuracy Score: ", accuracy_score(VALI_outputs, predictions), file=open('summary.txt', 'a'))
        #print("")
        print("Confusion Matrix: \n", confusion_matrix(Y_validation, predictions, labels = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]), file=open('summary.txt', 'a'))
#         print("Confusion Matrix: \n", confusion_matrix(VALI_outputs, predictions, labels = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]), file=open('summary.txt', 'a'))
        #print("")
        print("Classification Report: \n", classification_report(Y_validation, predictions), file=open('summary.txt', 'a'))
#         print("Classification Report: \n", classification_report(VALI_outputs, predictions), file=open('summary.txt', 'a'))
        
#         print("TEST\n")
#         print(self.processData(segment_X, knn))
#         print(stats.mode(segment_Y)[0])
        
        return knn
#         a = input()
#         rawData = numpy.empty(80, 12)
#         featureData = numpy.empty(24)
#         print("preWhile")
#         while (True):
# #             counter = 0
# #             while (counter < 80):
#             my_list = a.split()
#             matrix = [my_list]
# #         #         rawData[counter] = matrix
# #         #         counter = counter + 1
#             predictions = knn.predict(matrix)
#             predictions = le.inverse_transform([int(predictions[0])])
# #             
# #         #     for j in range(0, featureData.shape[1] - 1, 2):
# #         #         featureData[j] = rawData[: , j//2].mean()
# #         #         featureData[j+1] = rawData[: , j//2].std()
# #         
# #         #     predictions = knn.predict(featureData)
#             print(predictions)
#             a = input()

#run = learning()
#run.machineTrain()
