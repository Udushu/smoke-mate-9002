from flask import Flask, jsonify
from flask_cors import CORS
import threading
import requests
import time

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
    global status, last_is_running, run_start_time
    
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

                print("Data fetched:", data)
                
                # Check if the controller has transitioned from NOT RUNNING to RUNNING
                current_is_running = data.get('isRunning')
                if last_is_running is not None and not last_is_running and current_is_running:
                    print("Controller transitioned from NOT RUNNING to RUNNING!")
                    run_start_time = time.time()  # Record the start time of the run
                
                last_is_running = current_is_running
                
                # Store the data in the database if the device is running
                if data.get('isRunning'):
                    db.log_status(data)
            else:
                # Handle the case where the API call fails
                print("Failed to fetch data, status code:", response.status_code)
                # Reset the connection status
                status["isConnected"] = False
                
        except Exception as e:
            # Handle any exceptions that occur during the API call
            print("Error fetching data:", e)
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
                print("Config fetched:", config_data)
                # You can add db.log_config(config_data) if you want to log config as well
            else:
                print("Failed to fetch config, status code:", response.status_code)
        except Exception as e:
            print("Error fetching config:", e)
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
    global run_start_time
    if not run_start_time:
        return jsonify({"error": "No running session detected"}), 400
    records = db.get_status_since(run_start_time)
    return jsonify(records)

# Start polling in a background thread
threading.Thread(target=poll_status_api, daemon=True).start()
threading.Thread(target=poll_status_config, daemon=True).start()
threading.Thread(target=cleanup_old_records, daemon=True).start()

if __name__ == '__main__':
    global status
    global config
    global last_is_running
    global run_start_time
    
    status =  {
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
        'isWiFiConnected': False
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
    run_start_time = None
    
    app.run(host='0.0.0.0', port=5000, debug=True)