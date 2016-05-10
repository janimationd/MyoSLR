import urllib2
import urllib
import json
import spellchecker
	
spellcheck_api_key = "2eb570f4a39a494ba4c4007dd0be68df"
autosuggest_api_key = "0dace54df71443d59efd054289880f22"

def predict_letter(data):
	return "To Be Implemented"
	

def words(data):
	res = []
	req = urllib2.Request("https://bingapis.azure-api.net/api/v5/suggestions/?" + urllib.urlencode({'q': data['txt']}))
	req.add_header('Ocp-Apim-Subscription-Key', autosuggest_api_key)
	resp = urllib2.urlopen(req)
	parsed_json = json.loads(resp.read())
	for suggestion in parsed_json['suggestionGroups'][0]['searchSuggestions']:
		res.append(suggestion['displayText'])
	return res

def spellcheck(data):
	return spellchecker.correct(data.txt)
	

	return "working"