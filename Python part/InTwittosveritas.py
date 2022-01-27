#how to install missing python modules ? --> aks google precising the piece of software you are using
import requests, urllib, webbrowser 

#enter your name  
print("who are you ?")
messageauthor=input() #TODO 1 ask the user to enter a string and suppress this comment

#enter your message
print("what is your message ?")
messagecontent=input() #TODO 2 ask the user to enter a string and suppress this comment

#enter your secretkey
print("what is your secret key ?")
secretkey=input() #TODO 3 ask the user to enter a string and suppress this comment

#encode url parameters properly see https://stackoverflow.com/questions/5607551/how-to-urlencode-a-querystring-in-python
urlparameters = urllib.parse.urlencode({ 'author' : messageauthor, 'message' : messagecontent, 'secretkey' : secretkey}) 
print(urlparameters)  #uncomment this line to print the variable value
url = 'https://op-dev.icam.fr/~icebox/createTweeticam.php?' + urlparameters
print(url) #TODO 4 print the URL and suppress this comment

resp = requests.get(url)

print(resp.status_code)   #uncomment this line to print the variable value
print(resp.text) #TODO  5 uncomment this line and suppress this comment

#have a look at https://op-dev.icam.fr/~icebox/listTweeticam.php
webbrowser.open_new("https://op-dev.icam.fr/~icebox/listTweeticam.php") #TODO 6 uncomment this line and suppress this comment
