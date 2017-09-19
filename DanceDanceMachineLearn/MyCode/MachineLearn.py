from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import os
import urllib
import urllib.request

import numpy as np
import tensorflow as tf

#defining the csv columns into this variable but im unsure how to use it yet
CSV_COLUMNS = ["date", "time", "username", "wrist", "activity",
 "acceleration_x", "acceleration_y", "acceleration_z",
 "gyro_x", "gyro_y", "gyro_z"
]

# Data sets
RUNWALK_TRAINING = "runwalk_training.csv" #IRIS_TRAINING = "iris_training.csv"
IRIS_TRAINING_URL = "http://download.tensorflow.org/data/iris_training.csv"
RUNWALK_TRAINING = "runWalk.csv"
RUNWALK_TRAINING_URL = "https://www.kaggle.com/vmalyi/run-or-walk/downloads/dataset.csv"

RUNWALK_TEST = "runwalk_test.csv" #IRIS_TEST = "iris_test.csv"
IRIS_TEST_URL = "http://download.tensorflow.org/data/iris_test.csv"

def main():
  # If the training and test sets aren't stored locally, download them.
  # if not os.path.exists(IRIS_TRAINING):
  #   raw = urllib.request.urlopen(IRIS_TRAINING_URL).read().decode("utf-8")
  #   with open(IRIS_TRAINING, "w") as f:
  #     f.write(raw)

  # if not os.path.exists(IRIS_TEST):
  #   raw = urllib.request.urlopen(IRIS_TEST_URL).read().decode("utf-8")
  #   with open(IRIS_TEST, "w") as f:
  #     f.write(raw)
	
  # If the training and test sets aren't stored locally, report error.
  if not os.path.exists(RUNWALK_TRAINING):
  	print("Training file does not exist. \n")

  if not os.path.exists(RUNWALK_TEST):
  	print("Test file does not exist. \n")

  # Load datasets.
  training_set = tf.contrib.learn.datasets.base.load_csv_with_header(
      filename=RUNWALK_TRAINING,	# this is the filename, defined above
      target_dtype=np.int,			# I think "target" refers to the output
      features_dtype=np.float32)	# This should be refering to the data

  test_set = tf.contrib.learn.datasets.base.load_csv_with_header(
      filename=RUNWALK_TEST,
      target_dtype=np.int,
      features_dtype=np.float32)

  # Specify that all features have real-value data
  feature_columns = [tf.feature_column.numeric_column("x", shape=[4])]

  # Build 3 layer DNN with 10, 20, 10 units respectively.
  classifier = tf.estimator.DNNClassifier(feature_columns=feature_columns,
                                          hidden_units=[10, 20, 10],
                                          n_classes=2,						# this refers to the number of outputs. in IRIS its 3
                                          model_dir="/tmp/runwalk_model")
  # Define the training inputs
  train_input_fn = tf.estimator.inputs.numpy_input_fn(
      x={"x": np.array(training_set.data)},
      y=np.array(training_set.target),
      num_epochs=None,
      shuffle=True)

  # Train model.
  classifier.train(input_fn=train_input_fn, steps=2000)

  # Define the test inputs
  test_input_fn = tf.estimator.inputs.numpy_input_fn(
      x={"x": np.array(test_set.data)},
      y=np.array(test_set.target),
      num_epochs=1,
      shuffle=False)

  # Evaluate accuracy.
  accuracy_score = classifier.evaluate(input_fn=test_input_fn)["accuracy"]

  print("\nTest Accuracy: {0:f}\n".format(accuracy_score))

  # Classify two new flower samples.
  new_samples = np.array(
      [[6.4, 3.2, 4.5, 1.5],
       [5.8, 3.1, 5.0, 1.7]], dtype=np.float32)
  predict_input_fn = tf.estimator.inputs.numpy_input_fn(
      x={"x": new_samples},
      num_epochs=1,
      shuffle=False)

  predictions = list(classifier.predict(input_fn=predict_input_fn))
  predicted_classes = [p["classes"] for p in predictions]

  print(
      "New Samples, Class Predictions:    {}\n"
      .format(predicted_classes))

if __name__ == "__main__":
    main()