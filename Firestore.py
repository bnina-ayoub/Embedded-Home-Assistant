import firebase_admin
# Application Default credentials are automatically created.
from firebase_admin import credentials
from firebase_admin import firestore

def init():
    cred = credentials.Certificate("firestorecredentials.json")
    firebase_admin.initialize_app(cred)
    db = firestore.client()
    return db