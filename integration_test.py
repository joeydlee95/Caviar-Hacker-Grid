# This is an integration test which analyzes the server's
# ability to respond to a request made by curl
import subprocess
import sys
import os
import telnetlib
import time

## Global Vars ##
passing = True #Variable to check if all tests passed
port = 2020;
failedTests = []

## Formatting Functions ##
def line():
    print("----------------------------------------")
def nameTest(name):
    line()
    print("Testing: " + name)
def pas():
    print("PASSED\n")

## Begin Testing  ##
if __name__ == "__main__":
    print("Starting Integration Test.\n")

    ## Start Server ##
    outputToNull = open(os.devnull, 'w')
    serverProcess = subprocess.Popen(['./webserver', 'test_config'], stdout=outputToNull)
    time.sleep(3)

#####        

    ## Server Status Test ##
    nameTest("Server Status Request")
    statusRawRequest = 'GET /status HTTP/1.1\r\nHost: localhost:2020\r\n\r\n'
    statusSession = telnetlib.Telnet('localhost', port)
    print('Sending Status Request.\n')
    statusSession.write(statusRawRequest.encode('ascii'))
    statusResult = statusSession.read_all().decode('utf-8')
    print(statusResult)
    time.sleep(3)

#####        

    ## Multithreading Test ##
    nameTest('Multithreading')
    blockingRawRequest = 'GET /block HTTP/1.1\r\nHost: localhost:2020\r\n\r\n'
    print('Sending Blocking Request\n')
    blockingSession = telnetlib.Telnet('localhost', port)
    blockingSession.write(blockingRawRequest.encode('ascii'))

    time.sleep(3)

    print('Sending Echo Request after Blocking Request.\n')
    echoRawRequest = 'GET /echo HTTP/1.1\r\nHost: localhost:2020\r\n\r\n' 
    echoSession = telnetlib.Telnet('localhost', port)
    echoSession.write(echoRawRequest.encode('ascii'))
    echoResult = echoSession.read_all().decode('utf-8')
    print("Result of echo request:\n")
    print(echoResult)
    #Echo tested elsewhere, just testing it returns here
    if echoResult != "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nGET /echo HTTP/1.1\r\nHost: localhost:2020\r\n\r\n":
        failedTests.append("Multithreading")
        passing = False

#####        
    
    ## Testing Complete, Kill Server ##
    print("\nKilling Server.\n")
    serverProcess.kill()

#####        
    
    ## Conclusion ##
    print("Passed all tests?\n" + str(passing) + '\n')
    if passing:
        sys.exit(0)
        #return True
    else:
        print("Failed Tests:")
        for test in failedTests:
            print(test, '\n')
        sys.exit(1)
        #return False
        
        ## Notes ##
        # Todo: add/utilize an http response parser that checks for correct
        # status codes, content type, etc.
        
