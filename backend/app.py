from flask import Flask, jsonify
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
                status = data  # Update global status variable
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
        except Exception as e:
            # Handle any exceptions that occur during the API call
            print("Error fetching data:", e)
        
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
    
    status = {}
    config = {}
    last_is_running = None
    run_start_time = None
    
    app.run(debug=True)