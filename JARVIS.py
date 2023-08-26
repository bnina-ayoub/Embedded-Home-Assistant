from Text_to_speech import speech_synthesis
from Speech_to_text import voice_recognizer, speech_synthesizer
from Authentication import conversation, generated_welcome, openai
import time
# This example requires environment variables named "SPEECH_KEY" and "SPEECH_REGION"


speech_synthesis_result = speech_synthesizer.speak_text_async(generated_welcome).get()

while True:   
    time.sleep(1)
    text = voice_recognizer()


    conversation.append({"role": "user", "content": text})

    response = openai.ChatCompletion.create(
      model="gpt-3.5-turbo-16k-0613",
      messages=conversation,
      temperature=1,
      max_tokens=256,
      top_p=1,
      frequency_penalty=0,
      presence_penalty=0
    )
    # Extract the generated response
    generated_text = response["choices"][0]["message"]["content"]


    text = generated_text

    speech_synthesis(speech_synthesizer, text)