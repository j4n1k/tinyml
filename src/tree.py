import numpy as np
import pandas as pd
from sklearn.tree import DecisionTreeClassifier
from sklearn.model_selection import train_test_split
import json

data = pd.read_csv("../data/input/data.csv")
data = data.dropna()

y = data["Crop_Damage"]
X  = data.drop(["ID", "Crop_Damage"], axis=1)

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.33, random_state=42)

clf_tree = DecisionTreeClassifier( random_state=42)
clf_tree.fit(X_train, y_train)


flat = []
tree = clf_tree.tree_

for left, right, feature, th, value in zip(tree.children_left, tree.children_right, tree.feature, tree.threshold, tree.value):
    if left == -1 and right == -1:
        idx_max = value[0].tolist().index(max(value[0]))
        n = [-1, idx_max, -1, -1]  # leaf

    else:
        n = [int(feature), float(th), int(left), int(right)]

    flat.append(n)

print(len(flat))
output = {}
for n in range(len(flat)):
    output[n] = flat[n]

json_object = json.dumps(output)

with open("flat_tree.json", "w") as outfile:
    outfile.write(json_object)
