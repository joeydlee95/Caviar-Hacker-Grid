# This is an integration test which analyzes the server's
# ability to respond to a request made by curl
import subprocess
import os
import time
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
def fail(msg):
    print("FAILED: " + str(msg) + "\n")
    passing = False

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


