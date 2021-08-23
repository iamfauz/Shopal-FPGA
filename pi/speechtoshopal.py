from __future__ import division

import re
import sys
import json
import pyrebase

from google.cloud import speech
from google.cloud.speech import enums
from google.cloud.speech import types
import pyaudio
from six.moves import queue

# Audio recording parameters
RATE = 16000
CHUNK = int(RATE / 10)  # 100ms


class MicrophoneStream(object):
    """Opens a recording stream as a generator yielding the audio chunks."""
    def __init__(self, rate, chunk):
        self._rate = rate
        self._chunk = chunk

        # Create a thread-safe buffer of audio data
        self._buff = queue.Queue()
        self.closed = True

    def __enter__(self):
        self._audio_interface = pyaudio.PyAudio()
        self._audio_stream = self._audio_interface.open(
            format=pyaudio.paInt16,
            channels=1, rate=self._rate,
            input=True, frames_per_buffer=self._chunk,
            # Run the audio stream asynchronously to fill the buffer object.
            # This is necessary so that the input device's buffer doesn't
            # overflow while the calling thread makes network requests
            stream_callback=self._fill_buffer,
        )

        self.closed = False

        return self

    def __exit__(self, type, value, traceback):
        self._audio_stream.stop_stream()
        self._audio_stream.close()
        self.closed = True
        # Signal the generator to terminate so that the client's
        # streaming_recognize method will not block the process termination.
        self._buff.put(None)
        self._audio_interface.terminate()

    def _fill_buffer(self, in_data, frame_count, time_info, status_flags):
        """Continuously collect data from the audio stream, into the buffer."""
        self._buff.put(in_data)
        return None, pyaudio.paContinue

    def generator(self):
        while not self.closed:
            # Use a blocking get() to ensure there's at least one chunk of
            # data, and stop iteration if the chunk is None, indicating the
            # end of the audio stream.
            chunk = self._buff.get()
            if chunk is None:
                return
            data = [chunk]

            # Now consume whatever other data's still buffered.
            while True:
                try:
                    chunk = self._buff.get(block=False)
                    if chunk is None:
                        return
                    data.append(chunk)
                except queue.Empty:
                    break

            yield b''.join(data)


def listen_print_loop(responses):
    """Iterates through server responses and prints them.

    The responses passed is a generator that will block until a response
    is provided by the server.

    In this case, responses are provided for interim results as well. If the
    response is an interim one, print a line feed at the end of it, to allow
    the next result to overwrite it, until the response is a final one. For the
    final one, print a newline to preserve the finalized transcription.
    """
    num_chars_printed = 0
    for response in responses:
        if not response.results:
            continue

        # The `results` list is consecutive. For streaming, we only care about
        # the first result being considered, since once it's `is_final`, it
        # moves on to considering the next utterance.
        result = response.results[0]
        if not result.alternatives:
            continue

        # Display the transcription of the top alternative.
        transcript = result.alternatives[0].transcript

        # Display interim results, but with a carriage return at the end of the
        # line, so subsequent lines will overwrite them.
        #
        # If the previous result was longer than this one, we need to print
        # some extra spaces to overwrite the previous result
        overwrite_chars = ' ' * (num_chars_printed - len(transcript))

        if not result.is_final:
            sys.stdout.write(transcript + overwrite_chars + '\r')
            sys.stdout.flush()
            num_chars_printed = len(transcript)

        else:
            #Parse commands said to shopal
            print(overwrite_chars)
            parseCommand(transcript)

            num_chars_printed = 0

#parses commands for items 
def parseCommand(transcript):

    global listening
    transcript = transcript.lower()
    command = transcript.split()
    print(command)
    n = len(command)
    for i in range(n):



        if command[i] in finish:
            print("not listening")
            listening = False
            break

        if command[i] in init:
            listening = True
            print("listening")

        if listening:
            
            if command[i] in add:
                addParser(command[i+1:])
                break

            elif command[i] in clear:
                clearParser()
                break

            elif command[i] in remove:
                removeParser(command[i+1:])
                break

        

            
    
# initialize firebase db
def initDB():
    config = {
      "apiKey": "AIzaSyBMl_4uF7U7DuBEb_DbKwniVLmba_Rah1I",
      "authDomain": "shopal391.firebaseapp.com",
      "databaseURL": "https://shopal391.firebaseio.com/",
      "storageBucket": "shopal391.appspot.com"
    }

    firebase = pyrebase.initialize_app(config)
    db = firebase.database()

    return db

#Parse add commands
def addParser(command):

    pos = 0
    n = len(command)
    items = []
    for j in range(n):
        if (command[j] in connector):
            items.append( " ".join(command[pos:j]))
            pos = j + 1

        elif (command[j] in end):
            items.append( " ".join(command[pos:j]))
            break
        elif (j == (n - 1)):
            items.append( " ".join(command[pos:]))

    
                
    for item in items:
        print(item)
        data = {
          "name": item,
          "quantity": 1
            }
    
        if item:
            db.child("shopal_list").child("items").push(data)

#Remove all items         
def clearParser():
    print("removing all items from the shopping list")
    db.child("shopal_list").child("items").remove()

#Remove specific items
def removeParser(command):

    print("trying to remove")
    pos = 0
    n = len(command)
    items = []
    for j in range(n):
        if (command[j] in connector):
            items.append( (" ".join(command[pos:j])).lower())
            pos = j + 1

        elif (command[j] in end):
            items.append( (" ".join(command[pos:j])).lower())
            break
        elif (j == (n - 1)):
            items.append( (" ".join(command[pos:])).lower())

    if items:
        dbItems = db.child("shopal_list").child("items").get()

        for item in dbItems.each():
            key = item.key()
            name = db.child("shopal_list").child("items").child(str(key)).get().val()["name"]
            if name.lower() in items:
               db.child("shopal_list").child("items").child(str(key)).remove()
               print("removed: " + name)
            

    
def main():
    # See http://g.co/cloud/speech/docs/languages
    # for a list of supported languages.
    language_code = 'en-US'  # a BCP-47 language tag

    client = speech.SpeechClient()
    config = types.RecognitionConfig(
        encoding=enums.RecognitionConfig.AudioEncoding.LINEAR16,
        sample_rate_hertz=RATE,
        language_code=language_code)
    streaming_config = types.StreamingRecognitionConfig(
        config=config,
        interim_results=True)

    
    with MicrophoneStream(RATE, CHUNK) as stream:
        audio_generator = stream.generator()
        requests = (types.StreamingRecognizeRequest(audio_content=content)
                    for content in audio_generator)

        try:
            responses = client.streaming_recognize(streaming_config, requests)

        #Now, put the transcription responses to use.
        except Exception as exception:
            print("stream time out")

        try:
            listen_print_loop(responses)
        except Exception as exception:
            print("stream tried to time out")


db = initDB()

init = {"spell", "chappell", "chappelle", "isabel"}
finish = {"goodbye"}
add = {"add", "ad", "put", "place"}
clear = {"clear", "empty"}
remove = {"delete", "remove"}
end = {"to", "from", "my", "shopping", "list"}
connector = {"and", "plus"}

listening = False


if __name__ == '__main__':
    while(1):
        main()
