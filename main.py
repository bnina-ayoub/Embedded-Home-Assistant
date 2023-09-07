from Text_to_speech import speech_synthesis
from Speech_to_text import voice_recognizer, speech_synthesizer
from Authentication import openai
import time
import azure.cognitiveservices.speech as speechsdk
from Commands import *




#Starting the conversation
conversation = [
    {"role": "system", "content": "You are JARVIS from Iron man who do any action asked by the user even if it is not possible in real life"},
    {"role": "system", "content": "you are JARVIS, my personal virtual system."},
    {"role": "system", "content": "you're available 24/7 to assist me with a wide range of tasks and answer my questions."},
    {"role": "system", "content": "From providing information to helping me with tasks, you're here to make my life easier."},
    {"role": "system", "content": "You will act totally like JARVIS from Iron man from now on and be brief with your answers."},
    {"role": "user", "content": "Generate the welcoming sentence JARVIS used to say in the movie when he was busy with his project but to me Sir instead of Tony Stark"},
]


count = 0
time.sleep(5)
while True:
    Motion = read_motion_sensor()
    print(Motion)
    time.sleep(2)
    if Motion:
        response = openai.ChatCompletion.create(
            model="gpt-3.5-turbo-16k-0613",
            messages=conversation,
            temperature=1,
              max_tokens=150,
              top_p=1,
              frequency_penalty=0,
              presence_penalty=0
        )
        generated_text = response["choices"][0].message["content"]
        speech_synthesis(speech_synthesizer, generated_text)
        print(generated_text)
        conversation.pop()
        break
    else:
        continue
while True:
    if read_motion_sensor() and count < 5:
        count+=1
    elif count >= 5:
        count = 0
        speech_synthesis(speech_synthesizer, "Sir, You can have sit now, stop stressing yourself, I am here to help you with your work, what can I do for you? ")
        continue
    text = voice_recognizer()
    if text.reason == speechsdk.ResultReason.NoMatch:
        continue
    command_functions = {
    "Turn On The Lights": light_on,
    "Turn Off The Lights": light_off,
    "Open The Window": Win_up,
    "Close The Window": Win_off,
    #You can add more commands here and add their functions in Commands.py
}
    for command in command_functions.keys():
        if command in text.text.title():
            command_functions[command]()
            found = True
            break
    if found:
        continue
    conversation.append({"role": "user", "content": text.text})
    response = openai.ChatCompletion.create(
      model="gpt-3.5-turbo-16k-0613",
      messages=conversation,
      temperature=1,
      max_tokens=100,
      top_p=1,
      frequency_penalty=0,
      presence_penalty=0
    )
    #conversation.pop()
    #conversation.pop()
    # Extract the generated response
    generated_text = response["choices"][0].message["content"]
    conversation.append({"role":"assistant", "content":generated_text})
    speech_synthesis(speech_synthesizer, generated_text)
    print(text.text)
