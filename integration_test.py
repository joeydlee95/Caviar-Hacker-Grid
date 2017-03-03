# This is an integration test which analyzes the server's
# ability to respond to a request made by curl
import subprocess
import sys
import os
import telnetlib
import time
## Global Vars ##
passing = True
port = 3000;
#TODO: add a data strx to hold names of failed tests

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
def fail(msg):
    print("FAILED: " + str(msg) + "\n")
    passing = False

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

def TestProxyHandler():
    nameTest("ProxyHandler")
    backend_config = (
            'port 8080;\n'
            'path /echo EchoHandler {}\n'
            'default NotFoundHandler {}\n'
            )
    proxy_config = (
            'port 2020;\n'
            'path /proxy ProxyHandler {\n'
            '  host localhost;\n'
            '  port 8080;\n'
            '}\n'
            'default NotFoundHandler {}\n'
            )
    backend_config_file = 'test_backend_config'
    proxy_config_file = 'test_proxy_config'
    with open(backend_config_file, 'w') as f:
        f.write(backend_config)
    with open(proxy_config_file, 'w') as f:
        f.write(proxy_config)

    WEBSERVER_EXECUTABLE = './webserver'
    backend_process = subprocess.Popen([WEBSERVER_EXECUTABLE, backend_config_file])
    proxy_process = subprocess.Popen([WEBSERVER_EXECUTABLE, proxy_config_file])
    time.sleep(0.1) # give time for webservers to open
    if backend_process.poll():
        return fail("backend webserver exited with status %d" % backend_process.returncode)
    if proxy_process.poll():
        return fail("proxy webserver exited with status %d" % proxy_process.returncode)

    backend_curl = 'curl -Is localhost:8080/echo | cat'
    proxy_curl = 'curl -Is localhost:2020/proxy/echo | cat'

    backend_output = subprocess.check_output(backend_curl, shell=True)
    if backend_output == "":
        backend_process.terminate()
        proxy_process.terminate()
        return fail("backend produced wrong output")
    proxy_output = subprocess.check_output(proxy_curl, shell=True)
    if proxy_output == "":
        backend_process.terminate()
        proxy_process.terminate()
        return fail("proxy produced wrong output")

    if (proxy_output != backend_output):
        fail("Proxy and actual outputs differ!")
    else:
        pas()

    backend_process.terminate() # kill server
    proxy_process.terminate()
    backend_process.wait() # wait for server to die
    proxy_process.wait()

    # delete temporary config files
    os.remove(backend_config_file)
    os.remove(proxy_config_file)

    line()

TestProxyHandler()



## Conclusion ##
print("Passed all tests?\n" + str(passing) + '\n')
    

## Notes ##
# Todo: add/utilize an http response parser that checks for correct
# status codes, content type, etc.
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
        
