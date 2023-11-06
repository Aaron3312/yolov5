import firebase_admin 
from firebase_admin import db, credentials
cred = credentials.Certificate("credentials.json")
firebase_admin.initialize_app(cred, {"databaseURL": "https://iot-robotarm-default-rtdb.firebaseio.com/"})
ref = db.reference('/')

ref.get()

db.reference('/name').set(3)
ref.get()

db.reference('/video').set(4)
ref.get()

db.reference('/').update({'lenuage': "python"})
ref.get()

db.reference('/titles').push().set("moder ui in python")
print(ref.get())

