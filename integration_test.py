# This is an integration test which analyzes the server's
# ability to respond to a request made by curl
import subprocess

## Global Vars ##
port = 8080
passing = True
#TODO: add a data strx to hold names of failed tests


## Formatting Functions ##
def line():
    print("----------------------------------------")
def nameTest(name):
    line()
    print("Testing: " + name)
def pas():
    print("PASSED\n")


## Testing Process ##
print("\nBeginning Integration Test.\n")
#For Curl: -I sends a HEAD Http Request; -s silences output to the terminal
command = 'curl -Is localhost:' + str(port) + " | cat"

try :
    nameTest("Connection")
    output=subprocess.check_output(command, shell=True)
    pas()
    print(output)
    line()
except CalledProcessError:
    print("ERROR: Cannot connect to server.")
    passing = False


## Conclusion ##
print("Passed all tests?\n" + str(passing) + '\n')
    

## Notes ##
# Todo: add/utilize an http response parser that checks for correct
# status codes, content type, etc.

