from flask import Flask, jsonify, render_template_string
from flask_cors import CORS
import threading
import requests
import time
import logging
import datetime

from database import Database

BASE_URL = 'http://192.168.2.159:80'
API_STATUS_ENDPOINT = '/status'
API_STATUS_POLL_INTERVAL = 5  # seconds
API_CONFIG_ENDPOINT = '/config'
API_CONFIG_POLL_INTERVAL = 10  # seconds

DATABASE_PATH = 'database.db'

app = Flask(__name__)
CORS(app) 
db = Database(DATABASE_PATH)

def poll_status_api():
    global status, last_is_running, run_start_signature, run_stop_time
    
    while True:
        
        try:
            # Fetch the status from the API
            response = requests.get(BASE_URL + API_STATUS_ENDPOINT)
            # Check if the response is successful
            if response.status_code == 200:
                # Parse the JSON response
                data = response.json()
                # Copy data into the  status variable fied by field
                # This assumes the structure of the data matches the status variable
                status["timestamp"] = datetime.datetime.now().isoformat()
                status["isConnected"] = True
                status["isRunning"] = data.get('isRunning', False)
                status["uuid"] = data.get('uuid', '')
                status["uptime"] = data.get('uptime', 0)
                status["controllerStartMSec"] = data.get('controllerStartMSec', 0)
                status["temperatureSmoker"] = data.get('temperatureSmoker', 0)
                status["temperatureFood"] = data.get('temperatureFood', 0)
                status["temperatureTarget"] = data.get('temperatureTarget', 0)
                status["fanPWM"] = data.get('fanPWM', 0)
                status["doorPosition"] = data.get('doorPosition', 0)
                status["RSSI"] = data.get('RSSI', 0)
                status["bars"] = data.get('bars', 0)
                status["ipAddress"] = data.get('ipAddress', '')
                status["isWiFiConnected"] = data.get('isWiFiConnected', False)
                status["networkName"] = data.get('networkName', '')
                status["temperatureError"] = data.get('temperatureError', 0)
                
                # Check if the controller has transitioned from NOT RUNNING to RUNNING
                current_is_running = data.get('isRunning')
                if (last_is_running is not None) and (not last_is_running) and current_is_running:
                    print("Controller transitioned from NOT RUNNING to RUNNING!")
                    run_start_signature = {}
                    run_start_signature["uptime"]= status["uptime"]
                    run_start_signature["controllerStartMSec"] = status["controllerStartMSec"]
                    print("Run start signature:", run_start_signature)
                    
                
                last_is_running = current_is_running
                print("Last isRunning state:", last_is_running, "at", run_start_signature)
                
                # Store the data in the database if the device is running
                if data.get('isRunning'):
                    db.log_status(data)
            else:
                # Handle the case where the API call fails
                # print("Failed to fetch data, status code:", response.status_code)
                # Reset the connection status
                status["isConnected"] = False
                
        except Exception as e:
            # Handle any exceptions that occur during the API call
            # print("Error fetching data:", e)
            # Update the connection status
            status["isConnected"] = False
        
        # Wait for the specified interval before polling again
        time.sleep(API_STATUS_POLL_INTERVAL)  # Poll every API_STATUS_POLL_INTERVAL seconds
        
def poll_status_config():
    global config
    while True:
        try:
            response = requests.get(BASE_URL + API_CONFIG_ENDPOINT)
            if response.status_code == 200:
                config_data = response.json()                
                config = config_data

                # You can add db.log_config(config_data) if you want to log config as well
            else:
                pass
                # print("Failed to fetch config, status code:", response.status_code)
        except Exception as e:
            pass
            # print("Error fetching config:", e)
        time.sleep(API_CONFIG_POLL_INTERVAL)  # Poll every API_CONFIG_POLL_INTERVAL seconds
        
def cleanup_old_records():
    while True:
        db.cleanup_old_records()
        # Run cleanup once a day
        time.sleep(3600)
            
@app.route('/status', methods=['GET'])
def get_status():
    return jsonify(status)

@app.route('/config', methods=['GET'])
def get_config():
    return jsonify(config)

@app.route('/run-status-history', methods=['GET'])
def get_session_status():
    global run_start_signature, run_stop_time
    
    if run_start_signature is None:
        return jsonify({"error": "No running session detected"}), 400
    else:
        records = db.get_status_since(run_start_signature)
        # print(records[-1])
        return jsonify(records)

@app.route('/start', methods=['POST'])
def start_controller():
    try:
        # Forward the POST request to the device's /start endpoint
        response = requests.post(BASE_URL + '/start')
        if response.status_code == 200:
            return jsonify({'success': True, 'message': 'Controller started', 'device_response': response.json()})
        else:
            return jsonify({'success': False, 'error': 'Failed to start controller', 'status_code': response.status_code}), 500
    except Exception as e:
        return jsonify({'success': False, 'error': str(e)}), 500

@app.route('/stop', methods=['POST'])
def stop_controller():
    try:
        # Forward the POST request to the device's /stop endpoint
        response = requests.post(BASE_URL + '/stop')
        if response.status_code == 200:
            return jsonify({'success': True, 'message': 'Controller stopped', 'device_response': response.json()})
        else:
            return jsonify({'success': False, 'error': 'Failed to stop controller', 'status_code': response.status_code}), 500
    except Exception as e:
        return jsonify({'success': False, 'error': str(e)}), 500
    
@app.route('/')
def index():
    # Simple HTML template showing the latest status
    html = """
    <html>
    <head><title>Latest Status</title></head>
    <body>
        <h1>Latest Status</h1>
        <table border="1" cellpadding="5">
            {% for key, value in status.items() %}
            <tr>
                <th>{{ key }}</th>
                <td>{{ value }}</td>
            </tr>
            {% endfor %}
        </table>
    </body>
    </html>
    """
    return render_template_string(html, status=status)

# Start polling in a background thread
threading.Thread(target=poll_status_api, daemon=True).start()
threading.Thread(target=poll_status_config, daemon=True).start()
threading.Thread(target=cleanup_old_records, daemon=True).start()

if __name__ == '__main__':
    global status
    global config
    global last_is_running
    global run_start_signature
    global run_stop_time
    
    
    status =  {
        'timestamp': None,
        'isConnected': False,
        'isRunning': False, 
        'uuid': '', 
        'uptime': 0, 
        'controllerStartMSec': 0, 
        'temperatureSmoker': 0, 
        'temperatureFood': 0, 
        'temperatureTarget': 0, 
        'fanPWM': 0, 
        'doorPosition': 0, 
        'RSSI': 0, 
        'bars': 0, 
        'ipAddress': '192.168.2.159', 
        'isWiFiConnected': False,
        'networkName': '',
        'temperatureError':0
        }
    config = {        
        'temperatureTarget': 0, 
        'temperatureIntervalMSec': 0, 
        'isPIDEnabled': False, 
        'kP': 0, 
        'kI': 0, 
        'kD': 0, 
        'bangBangLowThreshold': 0, 
        'bangBangHighThreshold': 0, 
        'bangBangHysteresis': 0, 
        'bangBangFanSpeed': 0, 
        'doorOpenPosition': 0, 
        'doorClosePosition': 0, 
        'themometerSmokerGain': 0, 
        'themometerSmokerOffset': 0, 
        'themometerFoodGain': 0, 
        'themometerFoodOffset': 0, 
        'isThemometerSimulated': False, 
        'isForcedFanPWM': False, 
        'forcedFanPWM': 0, 
        'isForcedDoorPosition': False, 
        'forcedDoorPosition': 0, 
        'isWiFiEnabled': False, 
        'wifiSSID': 'BELL529'
    }
    last_is_running = None
    run_start_signature = None
    run_stop_time = None
    
    # Disable Flask's default request logging
    log = logging.getLogger('werkzeug')
    log.setLevel(logging.ERROR)

    app.run(host='0.0.0.0', port=5000, debug=True)