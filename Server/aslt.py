from flask import Flask
from flask import request
from flask import jsonify
import translator
import trainer

application = Flask(__name__)

@application.route("/")
def index():
	return "Automatic Sign Language Translator"
#
# Translation Endpoints
#
# {"txt": "Previous Text", "emg": {"avg": [1,2,3,4,5,6,7,8], "sd": [1,2,3,4,5,6,7,8]}}
@application.route("/translator/letter", methods=['POST'])
def translator_letter():
	return translator.letter(request.json)

# {"txt": "Previous Text"}
@application.route("/translator/words", methods=['POST'])
def translator_words():
	return jsonify(suggestions=translator.words(request.json))


# {"txt": "Current Text"}
@application.route("/translator/spellcheck", methods=['POST'])
def translator_spellcheck():
	return jsonify(corrected=translator.spellcheck(request.json))

#
# Trainer Endpoints
#
# {"letter": "a", "emg": [[1,2,3,4,5,6,7,8],[1,2,3,4,5,6,7,8],[1,2,3,4,5,6,7,8]]}
@application.route("/trainer/letter", methods=['POST'])
def trainer_letter():
	return trainer.letter(request.json)

if __name__ == "__main__":
    application.run(host='0.0.0.0')
