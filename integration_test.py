# This is an integration test which analyzes the server's
# ability to respond to a request made by curl
import subprocess
import os
## Global Vars ##
passing = True
port = 3000;
#TODO: add a data strx to hold names of failed tests


## Formatting Functions ##
def line():
    print("----------------------------------------")
def nameTest(name):
    line()
    print("Testing: " + name)
def pas():
    print("PASSED\n")

## Start Server ##
subprocess.call("nohup ./webserver test_config >/dev/null 2>&1 &", shell = True)
pid = subprocess.check_output('ps -a | grep webserver', shell = True).split()[0]
print(pid)


## Testing Process ##
print("\nBeginning Integration Test.\n")
#For Curl: -I sends a HEAD Http Request; -s silences output to the terminal

#try :
nameTest("Connection")
command2 = 'curl -Is localhost:' + str(port) + " | cat"
output=subprocess.check_output(command2, shell=True)
if output != "":
    pas()
    print(output)
    line()
#except CalledProcessError:
else:
    print("ERROR: Cannot connect to server.")
    passing = False


## Kill Server ##
subprocess.call('kill ' + pid, shell = True)


## Conclusion ##
print("Passed all tests?\n" + str(passing) + '\n')
    

## Notes ##
# Todo: add/utilize an http response parser that checks for correct
# status codes, content type, etc.


