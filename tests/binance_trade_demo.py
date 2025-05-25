#!/usr/bin/env python3

import base64
import time
import json
from cryptography.hazmat.primitives.serialization import load_pem_private_key
from websocket import create_connection

# Set up authentication
API_KEY='euHZeWJLF1IO5MNsuKIQYqJH4ZqLfFbtwxp0ychQi41f7CDBRRCrSt2d7nRzXq1T'
PRIVATE_KEY_PATH='/home/qiubinglin/Private_key.txt'

# Load the private key.
# In this example the key is expected to be stored without encryption,
# but we recommend using a strong password for improved security.
with open(PRIVATE_KEY_PATH, 'rb') as f:
    private_key = load_pem_private_key(data=f.read(),
                                       password=None)

# Set up the request parameters
params = {
    'apiKey':        API_KEY,
    'symbol':       'ADAUSDT',
    'side':         'BUY',
    'type':         'LIMIT',
    'timeInForce':  'GTC',
    'quantity':     '10.000000',
    'price':        '0.70'
}

# Timestamp the request
timestamp = int(time.time() * 1000) # UNIX timestamp in milliseconds
params['timestamp'] = timestamp

# Sign the request
payload = '&'.join([f'{param}={value}' for param, value in sorted(params.items())])

signature = base64.b64encode(private_key.sign(payload.encode('ASCII')))
params['signature'] = signature.decode('ASCII')

# Send the request
request = {
    'id': 'my_new_order',
    'method': 'order.place',
    'params': params
}

ws = create_connection("wss://ws-api.binance.com:443/ws-api/v3")
ws.send(json.dumps(request))
result =  ws.recv()
ws.close()

print('payload: {}\n'.format(payload))
print('json: {}\n'.format(json.dumps(request)))
print(result)