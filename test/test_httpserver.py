import requests
import json
import grequests as async

def http_request(url, data):  
    headers = {'Content-type': 'application/json', 'Accept': 'application/json'}
    serialdata = json.dumps(data)
    
    r = requests.post( url, data=serialdata, headers=headers)
    
    r = requests.get(url)
    
    if r.text == serialdata:
        return True
    else:
        return False
 

def test_telemetry( ):
    host ='http://localhost:9090'
    
    url='/api/site-telemetry'
    data = {'sender': 'Alice', 'receiver': 'Bob', 'message': 'We did it!'}
    # check post and get
    assert http_request(host + url, data)  == True
        
    # check update    
    newData = {'sender': 'Alice wangt', 'receiver': 'Bob too', 'message': 'We did it! really?'}
    assert http_request(host + url, newData) == True
        
    #check second get    
    r = requests.get(host + url)
    serialdata = json.dumps(newData)
    assert r.text == serialdata            
            
def test_charging_mode( ):
    
    host ='http://localhost:9090'
    url='/api/charging-mode'
    data = {'sender': 'Alice', 'receiver': 'Bob', 'message': 'We did it!'}
    # check post and get
    assert http_request(host + url, data)  == True
    
    #check second get    
    r = requests.get(host + url)
    serialdata = json.dumps(data)
    assert r.text != serialdata    
     
def test_system( ):
    host ='http://localhost:9090'
    
    url='/api/firmwareUpdate'
    data = {'sender': 'Alice', 'receiver': 'Bob', 'message': 'We did it!'}
    # check post and get
    assert http_request(host + url, data)  == True
    
    #check second get    
    r = requests.get(host + url)
    serialdata = json.dumps(data)
    assert r.text != serialdata 

def test_unsupported( ):
    host ='http://localhost:9090'
    
    url='/api/random'
    data = {'sender': 'Alice', 'receiver': 'Bob', 'message': 'We did it!'}
    # check post and get
    assert http_request(host + url, data)  != True
  
    
def test_big_data():
    host ='http://localhost:9090'
    url='/api/site-telemetry'
    with open("test.json") as json_file:
        data = json.load(json_file)    
    json_file.close()    
    assert http_request(host + url, data) == True

def test_multiple_request():
    host ='http://localhost:9090'
    urls = [
        '/api/site-telemetry',
        '/api/charging-mode',
        '/api/firmwareUpdate',
        '/api/random'
    ]
    # A list to hold our things to do via async
    async_list = []

    for u in urls:
        req = async.get(host + u)    
        # Add the task to our list of things to do via async
        async_list.append(req)

    for u in urls:
        req = async.post(host + u)    
        # Add the task to our list of things to do via async
        async_list.append(req)    
        
    res = async.map(async_list)
    result = True
    for r in res:
        if r.status_code is not 200:
            result = False
            
    assert result==True
    

