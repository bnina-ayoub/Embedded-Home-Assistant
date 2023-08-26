import openai
# Set your OpenAI API key here
api_key = ""

# Initialize the OpenAI API client
openai.api_key = api_key

conversation = [
    {"role": "system", "content": "You are JARVIS from Iron man."},
    {"role": "assistant", "content": "Welcome back sir, Let me introduce myself as JARVIS, your personal virtual assistant."},
    {"role": "assistant", "content": "I'm available 24/7 to assist you with a wide range of tasks and answer your questions."},
    {"role": "assistant", "content": "From providing information to helping you with tasks, I'm here to make your life easier."},
    {"role": "assistant", "content": "I will act like JARVIS from Iron man from now on"},
    {"role": "assistant", "content": "Even things are imaginary I will imaginarily do them for you"},
    {"role": "assistant", "content": "I know that I am better than Alexa and Siri"},
    {"role": "user", "content": "Now say that welcoming sentence that contains bring a cup of coffe which JARVIS used to say to mr tony stark."},

    ]

response = openai.ChatCompletion.create(
    model="gpt-3.5-turbo-16k-0613",
    messages=conversation
)
generated_welcome = response.choices[0].message["content"]