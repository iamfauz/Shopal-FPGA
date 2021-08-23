import pyrebase
import json

config = {
  "apiKey": "AIzaSyBMl_4uF7U7DuBEb_DbKwniVLmba_Rah1I",
  "authDomain": "shopal391.firebaseapp.com",
  "databaseURL": "https://shopal391.firebaseio.com/",
  "storageBucket": "shopal391.appspot.com"
}

firebase = pyrebase.initialize_app(config)

db = firebase.database()
itemName = "letsGetThisBread" 
itemQty = 123

# Construct item to be added, must have name and quantity fields
data = {
  "name": itemName,
  "quantity": itemQty
}

# Add item to database under shopal_list
db.child("shopal_list").child("items").push(data)

# For retrieving/removing items on the list
items = db.child("shopal_list").child("items").get()

for item in items.each():
  key = item.key()
  name = db.child("shopal_list").child("items").child(str(key)).get()
  lol = name.val()["name"]
  print(lol)
  if str(lol) == "apple": # Remove apple for now as a sample
    db.child("shopal_list").child("items").child(str(key)).remove()
    print("apple removed")