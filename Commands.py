from Firestore import init
from Speech_to_text import speech_synthesizer
from Text_to_speech import speech_synthesis

db = init()
def read_motion_sensor():
    doc_ref = db.collection("Window").document("WDirection")
    doc = doc_ref.get()
    return doc.to_dict().get("IsSomeoneHere")
def light_on():
    doc_ref = db.collection("ledStatus").document("ledDocument")
    doc_ref.set({"isLedOn": True})
    speech_synthesis(speech_synthesizer, "Lights are on sir, you can ask me to turn them off anytime you want.")

def light_off():
    doc_ref = db.collection("ledStatus").document("ledDocument")
    doc_ref.set({"isLedOn": False})
    speech_synthesis(speech_synthesizer, "I turned off the lights sir, what else can I do for you?")
def Win_up():
    doc_ref = db.collection("Window").document("WDirection")
    doc_ref.update({"Up": True})
    speech_synthesis(speech_synthesizer, "Opening Window, you can ask me to close it anytime you want.")

def Win_off():
    doc_ref = db.collection("Window").document("WDirection")
    doc_ref.update({"Down": True})
    speech_synthesis(speech_synthesizer, "Closing Window, you can ask me to open it anytime you want.")